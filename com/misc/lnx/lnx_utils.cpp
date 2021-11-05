#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fsuid.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "../gen/Exception.h"
#include "../gen/gen_utils.h"
#include "../gen/dbgutils.h"
#include "lnx_utils.h"

namespace sf
{

pid_t gettid() noexcept
{
	return (pid_t)::syscall(SYS_gettid);
}

size_t getThreadCount()
{
	char buf[512];
	auto fd = open("/proc/self/stat", O_RDONLY);
	if (fd == -1)
	{
		return 0;
	}
	size_t count = 0;
	if (read(fd, buf, sizeof(buf)) > 0)
	{
		char* s = strchr(buf, ')');
		if (s != nullptr)
		{
			// Read 18th integer field after the command name
			for (int field = 0; *s != ' ' || ++field < 18; s++) {}
			count = atoi(s + 1);
		}
	}
	close(fd);
	return count;
}

bool kb_hit()
{
	struct timeval tv{};
	fd_set read_fd;
	/* do not wait at all, not even a microsecond */
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	/* must be done first to Initialize read_fd */
	FD_ZERO(&read_fd);
	/* makes select() ask if input is ready :
	* 0 is the file descriptor for stdin
	*/
	FD_SET(0, &read_fd);
	/* the first parameter is the number of the
	* largest file descriptor to check + 1.
	*/
	if (select
		(
			1,
			&read_fd,
			nullptr,    /* NO writes */
			nullptr,    /* NO exceptions */
			&tv
		) == -1)
	{
		return false;
	}      /* An error occurred	*/
	/* read_fd now holds a bitmap of files that are
	* readable. We test the entry for the standard
	* input (file 0).
	*/
	if (FD_ISSET(0, &read_fd))
	{
		/* character pending on stdin */
		return true;
	}
	/* no characters were pending */
	return false;
}

time_t time_mktime(struct tm* tm, bool gmtime)
{
	// When it is local time it's easy.
	if (!gmtime)
	{
		return mktime(tm);
	}
	// Store the current time zone environment.
	const char* tz = getenv("TZ");
	// Set the zone that mktime creates GMT values.
	setenv("TZ", "", 1);
	// InitializeBase the changed time zone.
	tzset();
	// Create the return value.
	time_t ret = mktime(tm);
	// Restore the time zone as it was.
	if (tz)
	{
		setenv("TZ", tz, 1);
	}
	else
	{
		unsetenv("TZ");
	}
	// InitializeBase the changed time zone again.
	tzset();
	// Return the GMT value.
	return ret;
}

time_t time_str2time(const std::string& str, const char* format, bool gmtime)
{
	// When not format has been passed format the XML date.
	if (!format)
	{
		format = "%Y%m%dT%H%M";
	}
	//
	struct tm timeinfo{};
	memset(&timeinfo, 0, sizeof(timeinfo));
	// Returns NULL in case of an error.
	if (::strptime(str.c_str(), format, &timeinfo) == nullptr)
	{
		return -1;
	}
	// set the fields from the time zone which are not Set using strptime().
	{
		struct tm ti{};
		time_t rawtime = 0;
		::time(&rawtime);
		if (gmtime)
		{
			::gmtime_r(&rawtime, &ti);
		}
		else
		{
			::localtime_r(&rawtime, &ti);
		}
		// Assign the missing fields.
		timeinfo.tm_gmtoff = ti.tm_gmtoff;
		timeinfo.tm_isdst = ti.tm_isdst;
		timeinfo.tm_zone = ti.tm_zone;
	}

	// Create the unix time stamp.
	//return mktime(&timeinfo);
	return time_mktime(&timeinfo, gmtime);
}

#if defined(_USE_CRYPT) || true

bool operator==(const md5hash_t& h1, const md5hash_t& h2)
{
	// Use the unions fast compare integers.
	return h1.ints[0] == h2.ints[0] && h1.ints[1] == h2.ints[1];
}

md5hash_t md5(const char* s, size_t sz)
{
	md5hash_t hash;
	MD5((const unsigned char*) s, sz, hash.data);
	return hash;
}

md5hash_t md5(const char* s)
{
	return md5(s, strlen(s));
}

md5hash_t md5(const std::string& s)
{
	md5hash_t hash;
	MD5((const unsigned char*) s.data(), s.length(), hash.data);
	return hash;
}

std::string md5hexstr(const md5hash_t& hash)
{
	return hexString(&hash, sizeof(hash));
}

std::string md5str(const std::string& s)
{
	md5hash_t hash;
	MD5((const unsigned char*) s.data(), s.length(), hash.data);
	return md5hexstr(hash);
}
#endif

void proc_setuid(uid_t uid)
{
	if (setuid(uid))
	{
		throw ExceptionSystemCall("setuid", errno, nullptr, __FUNCTION__);
	}
}

void proc_seteuid(uid_t uid)
{
	if (seteuid(uid))
	{
		throw ExceptionSystemCall("seteuid", errno, nullptr, __FUNCTION__);
	}
}

void proc_setgid(gid_t gid)
{
	if (setgid(gid))
	{
		throw ExceptionSystemCall("setgid", errno, nullptr, __FUNCTION__);
	}
}

void proc_setegid(gid_t gid)
{
	if (setegid(gid))
	{
		throw ExceptionSystemCall("setegid", errno, nullptr, __FUNCTION__);
	}
}

void proc_setfsuid(uid_t uid)
{
	int cuid = setfsuid(uid);
	if (cuid)
	{
		throw ExceptionSystemCall("setfsuid", errno, nullptr, __FUNCTION__);
	}
}

void proc_setfsgid(gid_t gid)
{
	int cgid = setfsgid(gid);
	if (cgid)
	{
		throw ExceptionSystemCall("setfsgid", errno, nullptr, __FUNCTION__);
	}
}

passwd_t::passwd_t() :passwd()
{
	reset();
	// Read the buffer size needed.
	buf_sz = ::sysconf(_SC_GETPW_R_SIZE_MAX);
	// Value was indeterminate
	if (buf_sz == -1)
	{
		// Should be more than enough
		buf_sz = 16384;
	}
	// Allocate the memory.
	buf = (char*) ::malloc(buf_sz);
	// Check for failure.
	if (!buf)
	{
		throw ExceptionSystemCall("malloc", errno, typeid(*this).name(), __FUNCTION__);
	}
}

passwd_t::~passwd_t()
{
	::free(buf);
}

void passwd_t::reset()
{
	valid = false;
	// Reset the passwd_typ part of this instance.
	memset(this, 0, sizeof(passwd_type)); // NOLINT(bugprone-undefined-memory-manipulation)
}

bool proc_getpwnam(std::string name, passwd_t& pwd) // NOLINT(performance-unnecessary-value-param)
{
	passwd_type* _pwd;
	//
	int err = ::getpwnam_r(name.c_str(), &pwd, pwd.buf, pwd.buf_sz, &_pwd);
	// In case of an error or not found reset the content.
	if (err || !_pwd)
	{
		pwd.reset();
	}
	// When an error was found.
	if (err)
	{
		throw ExceptionSystemCall("getpwnam_r", errno, nullptr, __FUNCTION__);
	}
	// Signal success.
	return pwd.valid = (_pwd != nullptr);
}

bool proc_getpwuid(uid_t uid, passwd_t& pwd)
{
	passwd_type* _pwd;
	//
	int err = ::getpwuid_r(uid, &pwd, pwd.buf, pwd.buf_sz, &_pwd);
	// In case of an error or not found reset the content.
	if (err || !_pwd)
	{
		pwd.reset();
	}
	if (err)
	{
		throw ExceptionSystemCall("getpwuid_r", errno, nullptr, __FUNCTION__);
	}
	// Signal success or failure.
	return pwd.valid = (_pwd != nullptr);
}

group_t::group_t()
	:group()
{
	reset();
	// Read the buffer size needed.
	buf_sz = ::sysconf(_SC_GETGR_R_SIZE_MAX);
	// Value was indeterminate
	if (buf_sz == -1)
	{
		// Should be more than enough
		buf_sz = 16384;
	}
	// Allocate the memory.
	buf = (char*) ::malloc(buf_sz);
	// Check for failure.
	if (!buf)
	{
		throw ExceptionSystemCall("malloc", errno, typeid(*this).name(), __FUNCTION__);
	}
}

group_t::~group_t()
{
	::free(buf);
}

void group_t::reset()
{
	valid = false;
	// Reset the passwd_type part of this instance.
	memset(reinterpret_cast<passwd_type*>(this), '\0', sizeof(passwd_type));
}

bool proc_getgrnam(const std::string& name, group_t& grp)
{
	group_type* _grp;
	//
	int err = ::getgrnam_r(name.c_str(), &grp, grp.buf, grp.buf_sz, &_grp);
	// In case of an error or not found reset the content.
	if (err || !_grp)
	{
		grp.reset();
	}
	// When an error was found.
	if (err)
	{
		throw ExceptionSystemCall("getgrnam_r", errno, nullptr, __FUNCTION__);
	}
	// Signal success.
	return grp.valid = (_grp != nullptr);
}

bool proc_getgrgid(gid_t gid, group_t& grp)
{
	group_type* _grp;
	//
	int err = ::getgrgid_r(gid, &grp, grp.buf, grp.buf_sz, &_grp);
	// Incase of an errror or not found reset the content.
	if (err || !_grp)
	{
		grp.reset();
	}
	if (err)
	{
		throw ExceptionSystemCall("getgrgid_r", errno, nullptr, __FUNCTION__);
	}
	// Signal success or failure.
	return grp.valid = (_grp != nullptr);
}

bool file_stat(const std::string& path, stat_t& st)
{
	::memset(&st, 0, sizeof(st));
	if (::stat(path.c_str(), &st) != 0)
	{
		return false;
	}
	return true;
}

bool file_write(const char* path, const void* buf, size_t sz, bool append)
{
	// Set mode to 644
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // NOLINT(hicpp-signed-bitwise)
	int fd = ::open(path, O_CREAT | O_WRONLY | (append ? O_APPEND : O_TRUNC), mode); // NOLINT(hicpp-signed-bitwise)
	if (fd == -1)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "'" << path << "' failed!\n" << strerror(errno))
		return false;
	}
	//
	::write(fd, buf, sz);
	//
	::close(fd);
	return true;
}

std::filesystem::path file_fd_path(int fd)
{
	auto fp = string_format("/proc/%d/fd/%d", getpid(), fd);

	if (!std::filesystem::exists(fp))
	{
		return std::filesystem::path();
	}
	return std::filesystem::read_symlink(fp);
}

std::string time_format(time_t time, const char* format, bool gmtime)
{
	struct tm ti{};
	// When -1 is passed get the time our selfs.
	if (time == -1)
	{
		time = ::time(nullptr);
	}
	// Pass it on to the other function doing the actual work.
	if (gmtime)
	{
		return time_format(::gmtime_r(&time, &ti), format);
	}
	else
	{
		return time_format(::localtime_r(&time, &ti), format);
	}
}

std::string time_format(const struct tm* timeinfo, const char* format)
{
	struct tm ti{};
	// When not format has been passed format the XML date.
	if (!format)
	{
		format = "%Y%m%dT%H%M";
	}
	// When not given get the UTC time.
	if (!timeinfo)
	{
		time_t rawtime;
		::time(&rawtime);
		timeinfo = ::localtime_r(&rawtime, &ti);
	}
	// Format the time.
	char buffer[256];
	//
	::strftime(buffer, sizeof(buffer), format, timeinfo);
	//
	return buffer;
}

bool file_mkdir(const char* path, __mode_t mode)
{
	std::string dir(path);
	// Add terminating slash.
	dir += '/';
	// Read root directory
	size_t ofs = dir.find('/');
	do
	{
		// Read next '/' in the directory.string.
		size_t pos = dir.find('/', ofs + 1);
		// When a directory separator was found.
		if (pos != std::string::npos)
		{
			// Retrieve single subdirectory.
			std::string tmp = dir.substr(0, pos);
			// Check for existence of dir before creating.
			if (!fileExists(tmp.c_str()))
			{
				// Create the sub directory.
				if (::mkdir(tmp.c_str(), mode))
				{
					SF_NORM_NOTIFY(DO_DEFAULT, "Creating direcory '" << tmp << "' failed!\n" << strerror(errno))
					// return false in case of an error.
					return false;
				}
			}
			// Set new value for 'ofs'
			ofs = pos + 1;
			// Check new value for 'ofs'
			if (ofs >= dir.length())
			{
				// Break loop by assigning zero.
				ofs = 0;
			}
		}
		else
		{
			// Break loop by assigning zero.
			ofs = 0;
		}
	}
	while (ofs);
	// In the end check if the directory exists as a whole.
	return fileExists(dir.c_str());
}

}


