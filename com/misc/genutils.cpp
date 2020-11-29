// Import of type va_list and its functions.
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <dirent.h>
#include <cxxabi.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <libgen.h>
#include <fcntl.h>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <sys/select.h>
#include <sys/fsuid.h>
// Import of SYS_xxx
#include <sys/syscall.h>
#endif

// Import of TExceptionSystemCall.
#include "exception.h"
// Import debug utilities.
#include "dbgutils.h"
// Import the files header file.
#include "genutils.h"

std::string stringf(const char* fmt, ...)
{
	const size_t sz = 4096;
	char* buf = (char*) malloc(sz);
	scope_free<char> sf(buf);
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(buf, sz, fmt, argptr);
	va_end(argptr);
	return &buf[0];
}

//
std::string bittostring(unsigned long wrd, size_t count)
{
	std::string ret;
	for (size_t i = 0; i < count; i++)
	{
		ret.append(1, (wrd & (1L << i)) ? '1' : '0');
	}
	return ret;
}

//
char hexcharvalue(char ch)
{
	ch = (char) ((isalpha(ch)) ? ch & (~(1 << 5)) : ch);
	return (char) ((isalpha(ch)) ? ch - 'A' + 10 : ch - '0');
}

//
std::string hexstring(const void* array, size_t size)
{
	// look up table for hex characters
	const char* hexval = "0123456789abcdef";
	// create mstring type buffer of the correct size
	std::string buffer(size * 2, ' ');
	// fill the hex string buffer.
	for (size_t i = 0; i < size; i++)
	{
		buffer[i * 2] = hexval[(((char*) array)[i] >> 4) & 0x0F];
		buffer[i * 2 + 1] = hexval[((char*) array)[i] & 0x0F];
	}
	// Return the filled buffer
	return buffer;
}

//
size_t stringhex(const char* hexstr, void* buffer, size_t size)
{
	// check valid pointers
	if (!hexstr || !buffer)
	{
		return -1;
	}
	// get the amount of bytes to convert
	unsigned chars = strlen(hexstr);
	// check mstring length
	if (!chars)
	{return 0;}
	// check for odd number of characters to convert
	if (chars % 2)
	{
		chars--;
	}
	// adjust amount of bytes to convert to the size of the buffer
	if (chars / 2 > size)
	{
		chars = size * 2;
	}
	if (chars)
	{
		for (unsigned i = 0; i < chars; i += 2)
		{
			((char*) buffer)[i / 2] = (char) ((hexcharvalue(hexstr[i]) << 4) + hexcharvalue(hexstr[i + 1]));
		}
	}
	// return the number of returned bytes
	return chars / 2;
}

#ifndef WIN32

pid_t gettid() noexcept
{
	return ::syscall(SYS_gettid);
}

//
bool kbhit()
{
	struct timeval tv{};
	fd_set read_fd;
	/* do not wait at all, not even a microsecond */
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	/* must be done first to initilize read_fd */
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
					NULL,    /* NO writes */
					NULL,    /* NO exceptions */
					&tv
				) == -1)
	{
		return false;
	}      /* An error occured	*/
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

#endif // WIN32

namespace misc
{
using namespace std;

string implode(strings strs, string glue, bool skip_empty)
{
	string retval;
	strings_iter i(strs);
	while (i)
	{
		if (retval.length() || !skip_empty)
		{
			retval += glue;
		}
		retval += (i++);
	}
	return retval;
}

strings explode(string str, string separator, bool skip_empty)
{
	strings result;
	size_t ofs = 0, found = str.find_first_of(separator, ofs);
	if (found != string::npos)
	{
		do
		{
			result.push_back(str.substr(ofs, found - ofs));
			//
			ofs = skip_empty ? str.find_first_not_of(separator, found) : (found + 1);
			if (ofs == string::npos)
			{
				break;
			}
			//
			found = str.find_first_of(separator, ofs);
		}
		while (found != string::npos);
	}
	//
	if (str.length() > ofs)
	{
		result.push_back(str.substr(ofs, str.length()));
	}
	//
	return result;
}

#if defined(_USE_CRYPT) && !defined(WIN32)

bool operator==(const md5hash_t& h1, const md5hash_t& h2)
{
	// Use the unions fast compare integers.
	return h1.ints[0] == h2.ints[0] && h1.ints[1] == h2.ints[1];
	//return !::memcmp(&h1, &h2, sizeof(md5hash_t));
}

md5hash_t md5(const char* s)
{
	md5hash_t hash;
	MD5((const unsigned char*) s, strlen(s), hash.data);
	return hash;
}

md5hash_t md5(string s)
{
	md5hash_t hash;
	MD5((const unsigned char*) s.data(), s.length(), hash.data);
	return hash;
}

string md5hexstr(const md5hash_t& hash)
{
	return hexstring(&hash, sizeof(hash));
}

string md5str(string s)
{
	md5hash_t hash;
	MD5((const unsigned char*) s.data(), s.length(), hash.data);
	return md5hexstr(hash);
}
#endif

//
string getline(istream& is)
{
	const size_t sz = 4096;
	char* buf = (char*) malloc(sz);
	scope_free<char> sf(buf);
	is.getline(buf, sz - 1);
	return &buf[0];
}

//
string getcwdstr()
{
	const size_t sz = 4096;
	char* buf = (char*) malloc(sz);
	scope_free<char> sf(buf);
	return getcwd(buf, sz - 1);
}

//
string demangle(const char* name)
{
	int status;
	char* nm = abi::__cxa_demangle(name, 0, nullptr, &status);
	string retval(nm);
	free(nm);
	return retval;
}

//
timespec gettime()
{
	timespec ct{};
	if (clock_gettime(CLOCK_REALTIME, &ct))
	{
		throw TExceptionSystemCall("epoll_wait", errno, nullptr, __FUNCTION__);
	}
	return ct;
}

//
int timespeccmp(const timespec& ts1, const timespec& ts2)
{
	if (ts1.tv_sec > ts2.tv_sec)
	{
		return 1;
	}
	else if (ts1.tv_sec < ts2.tv_sec)
	{
		return -1;
	}
	else if (ts1.tv_nsec > ts2.tv_nsec)
	{
		return 1;
	}
	else if (ts1.tv_nsec < ts2.tv_nsec)
	{
		return -1;
	}
	// Finally it is equal.
	return 0;
}

//
string tolower(string s)
{
	// Make the data pointer accessible.
	char* p = const_cast<char*>(s.data());
	for (string::size_type i = 0; i <= s.length(); i++)
	{
		if (isalpha(p[i]))
		{
			p[i] = (char) std::tolower(p[i]);
		}
	}
	return s;
}

string toupper(string s)
{
	// Make the data pointer accessible.
	char* p = const_cast<char*>(s.data());
	for (string::size_type i = 0; i <= s.length(); i++)
	{
		if (isalpha(p[i]))
		{
			p[i] = (char) std::toupper(p[i]);
		}
	}
	return s;
}

string trim_right(string s, const string& t)
{
	return s.erase(s.find_last_not_of(t) + 1);
}

string trim_left(string s, const string& t)
{
	return s.erase(0, s.find_first_not_of(t));
}

//
string trim(string s, const string& t)
{
	return trim_left(trim_right(s, t), t);
}

//
string unique(const string& s)
{
	return string(s.c_str(), s.length());
}

//
size_t strncspn(const char* s, size_t n, const char* reject)
{
	// Iterate from zero to the maximum.
	size_t i = 0;
	for (; i < n; i++)
	{
		// Check if the character is in the rejects.
		if (strchr(reject, s[i]))
		{
			break;
		}
	}
	// Return the current position.
	return i;
}

//
size_t strnspn(const char* s, size_t n, const char* accept)
{
	// Iterate from zero to the maximum.
	size_t i = 0;
	for (; i < n; i++)
	{
		// Check if the character is in the rejects.
		if (!strchr(accept, s[i]))
		{
			break;
		}
	}
	// Return the current position.
	return i;
}

//
const char* strnstr(const char* s, const char* find, size_t n)
{
	char c, sc;
	size_t len;
	//
	if ((c = *find++) != '\0')
	{
		len = strlen(find);
		do
		{
			do
			{
				if ((sc = *s++) == '\0' || n-- < 1)
				{
					return (NULL);
				}
			}
			while (sc != c);
			if (len > n)
			{
				return NULL;
			}
		}
		while (strncmp(s, find, len) != 0);
		s--;
	}
	return (char*) s;
}

//
char* itoa(int value, char* string, int radix)
{
	if (!value)
	{
		string[0] = '0';
		string[1] = 0;
		return string;
	}
	string[32] = 0;
	int i = 31;
	for (; value && i; --i, value /= radix)
	{
		string[i] = "0123456789abcdef"[value % radix];
	}
	//
	return &string[i + 1];
}

//
int wildcmp(const char* wild, const char* str, bool case_s)
{
	if (!wild || !str)
	{
		return 0;
	}
	//
	const char* cp;
	const char* mp;
	//
	while ((*str) && (*wild != '*'))
	{
		if ((case_s ? (*wild != *str) : (std::toupper(*wild) != std::toupper(*str)))
				&& (*wild != '?'))
		{
			return 0;
		}
		//
		wild++;
		str++;
	}
	// Test if it is not the end of the string.
	while (*str)
	{
		if (*wild == '*')
		{
			if (!*++wild)
			{
				return 1;
			}
			mp = wild;
			cp = str + 1;
		}
		else if ((case_s ? (*wild == *str) : (std::toupper(*wild) == std::toupper(*str)))
						 || (*wild == '?'))
		{
			wild++;
			str++;
		}
		else
		{
			wild = mp;
			str = cp++;
		}
	}
	while (*wild == '*')
	{
		wild++;
	}
	//
	return !*wild;
}

//
bool getfiles(strings& files, string
directory,
	string wildcard
)
{
	DIR* dp;
	dirent* dirp;
// Prevent errors from printing by checking the access.
	if (
		access(directory
				.
					c_str(),
			F_OK | X_OK))
	{
		return false;
	}
//
	if ((
				dp = ::opendir(directory.c_str())
			) == NULL)
	{
		cerr << "Error(" << errno << ") " <<
				 strerror(errno)
				 << " " << directory <<
				 endl;
		return false;
	}
//
	while ((
					 dirp = readdir(dp)
				 ) != NULL)
	{
		if (
			wildcmp(wildcard
				.
					c_str(), dirp
				->d_name, true))
		{
			files.
				push_back(string(dirp->d_name)
			);
		}
	}
//
	::closedir(dp);
//
	return true;
}

} /* namespace std */

//
std::string file_basename(const std::string& path)
{
	return ::basename(const_cast<char*>(misc::unique(path).c_str()));
}

//
std::string file_dirname(const std::string& path)
{
	return ::dirname(const_cast<char*>(misc::unique(path).c_str()));
}

//
bool file_exists(const char* path)
{
	return !access(path, F_OK);
}

bool file_unlink(const std::string& path)
{
	if (::unlink(path.c_str()) == -1)
	{
		_NORM_NOTIFY(DO_DEFAULT, "of '" << path << "' failed!\n" << strerror(errno))
		return false;
	}
	return true;
}

bool file_rename(const std::string& oldpath, const std::string& newpath)
{
	if (::rename(oldpath.c_str(), newpath.c_str()) == -1)
	{
		_NORM_NOTIFY(DO_DEFAULT, "from '" << oldpath << "' to '" << newpath << "' failed!\n" << strerror(errno))
		return false;
	}
	return true;
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

#ifndef WIN32

bool file_mkdir(const char* path, __mode_t mode)
{
	std::string dir(path);
	// Add terminating slash.
	dir += '/';
	// Get root directory
	size_t ofs = dir.find('/');
	do
	{
		// Get next '/' in the directory.string.
		size_t pos = dir.find('/', ofs + 1);
		// When a directory separator was found.
		if (pos != std::string::npos)
		{
			// Retrieve single subdirectory.
			std::string tmp = dir.substr(0, pos);
			// Check for existence of dir before creating.
			if (!file_exists(tmp.c_str()))
			{
				// Create the sub directory.
				if (mkdir(tmp.c_str(), mode))
				{
					_NORM_NOTIFY(DO_DEFAULT, "Creating direcory '" << tmp << "' failed!\n" << strerror(errno))
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
	// In the end check if the odirectory exists as a whole.
	return file_exists(dir.c_str());
}

#endif // WIN32

bool file_find(misc::strings& files,
	const std::string& path
)
{
	return
		misc::getfiles(files, file_dirname(path), file_basename(path)
		);
}

bool file_write(const char* path, const void* buf, size_t sz, bool append)
{
	// Set mode to 644
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int fd = ::open(path, O_CREAT | O_WRONLY | (append ? O_APPEND : O_TRUNC), mode);
	if (fd == -1)
	{
		_NORM_NOTIFY(DO_DEFAULT, "'" << path << "' failed!\n" << strerror(errno))
		return false;
	}
	//
	::write(fd, buf, sz);
	//
	::close(fd);
	return true;
}


std::string time_format(time_t time, const char* format, bool gmtime)
{
	struct tm ti;
	// When -1 is passed get the time ourselfs.
	if (time == -1)
	{
		time = ::time(NULL);
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
	struct tm ti;
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

#ifndef WIN32

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
	// Initialize the changed time zone.
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
	// Initialize the changed time zone again.
	tzset();
	// Return the GMT value.
	return ret;
}

time_t time_str2time(std::string str, const char* format, bool gmtime)
{
	// When not format has been passed format the XML date.
	if (!format)
	{
		format = "%Y%m%dT%H%M";
	}
	//
	struct tm timeinfo;
	memset(&timeinfo, 0, sizeof(timeinfo));
	// Returns NULL in case of an error.
	if (::strptime(str.c_str(), format, &timeinfo) == NULL)
	{
		return -1;
	}
	// Set the fields from the time zone which are not set using strptime().
	{
		struct tm ti;
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

void proc_setuid(uid_t uid)
{
	if (setuid(uid))
	{
		throw TExceptionSystemCall("setuid", errno, NULL, __FUNCTION__);
	}
}

void proc_seteuid(uid_t uid)
{
	if (seteuid(uid))
	{
		throw TExceptionSystemCall("seteuid", errno, NULL, __FUNCTION__);
	}
}

void proc_setgid(gid_t gid)
{
	if (setgid(gid))
	{
		throw TExceptionSystemCall("setgid", errno, NULL, __FUNCTION__);
	}
}

void proc_setegid(gid_t gid)
{
	if (setegid(gid))
	{
		throw TExceptionSystemCall("setegid", errno, NULL, __FUNCTION__);
	}
}

void proc_setfsuid(uid_t uid)
{
	int cuid = setfsuid(uid);
	if (cuid)
	{
		throw TExceptionSystemCall("setfsuid", errno, NULL, __FUNCTION__);
	}
}

void proc_setfsgid(gid_t gid)
{
	int cgid = setfsgid(gid);
	if (cgid)
	{
		throw TExceptionSystemCall("setfsgid", errno, NULL, __FUNCTION__);
	}
}

passwd_t::passwd_t()
{
	reset();
	// Get the buffer size needed.
	bufsz = ::sysconf(_SC_GETPW_R_SIZE_MAX);
	// Value was indeterminate
	if (bufsz == -1)
	{
		// Should be more than enough
		bufsz = 16384;
	}
	// Allocate the memory.
	buf = (char*) ::malloc(bufsz);
	// Check for failure.
	if (!buf)
	{
		throw TExceptionSystemCall("malloc", errno, typeid(*this).name(), __FUNCTION__);
	}
}

passwd_t::~passwd_t()
{
	::free(buf);
}

void passwd_t::reset()
{
	valid = false;
	// Clear the passwd_typ part of this instance.
	memset(this, 0, sizeof(passwd_type));
}

bool proc_getpwnam(std::string name, passwd_t& pwd)
{
	passwd_type* __pwd;
	//
	int err = ::getpwnam_r(name.c_str(), &pwd, pwd.buf, pwd.bufsz, &__pwd);
	// In case of an error or not found reset the content.
	if (err || !__pwd)
	{
		pwd.reset();
	}
	// When an error was found.
	if (err)
	{
		throw TExceptionSystemCall("getpwnam_r", errno, NULL, __FUNCTION__);
	}
	// Signal success.
	return pwd.valid = (__pwd != NULL);
}

bool proc_getpwuid(uid_t uid, passwd_t& pwd)
{
	passwd_type* __pwd;
	//
	int err = ::getpwuid_r(uid, &pwd, pwd.buf, pwd.bufsz, &__pwd);
	// Incase of an errror or not found reset the content.
	if (err || !__pwd)
	{
		pwd.reset();
	}
	if (err)
	{
		throw TExceptionSystemCall("getpwuid_r", errno, NULL, __FUNCTION__);
	}
	// Signal success or failure.
	return pwd.valid = (__pwd != NULL);
}

group_t::group_t()
{
	reset();
	// Get the buffer size needed.
	bufsz = ::sysconf(_SC_GETGR_R_SIZE_MAX);
	// Value was indeterminate
	if (bufsz == -1)
	{
		// Should be more than enough
		bufsz = 16384;
	}
	// Allocate the memory.
	buf = (char*) ::malloc(bufsz);
	// Check for failure.
	if (!buf)
	{
		throw TExceptionSystemCall("malloc", errno, typeid(*this).name(), __FUNCTION__);
	}
}

group_t::~group_t()
{
	::free(buf);
}

void group_t::reset()
{
	valid = false;
	// Clear the passwd_typ part of this instance.
	memset(this, 0, sizeof(passwd_type));
}

bool proc_getgrnam(std::string name, group_t& grp)
{
	group_type* __grp;
	//
	int err = ::getgrnam_r(name.c_str(), &grp, grp.buf, grp.bufsz, &__grp);
	// In case of an error or not found reset the content.
	if (err || !__grp)
	{
		grp.reset();
	}
	// When an error was found.
	if (err)
	{
		throw TExceptionSystemCall("getgrnam_r", errno, NULL, __FUNCTION__);
	}
	// Signal success.
	return grp.valid = (__grp != NULL);
}

bool proc_getgrgid(gid_t gid, group_t& grp)
{
	group_type* __grp;
	//
	int err = ::getgrgid_r(gid, &grp, grp.buf, grp.bufsz, &__grp);
	// Incase of an errror or not found reset the content.
	if (err || !__grp)
	{
		grp.reset();
	}
	if (err)
	{
		throw TExceptionSystemCall("getgrgid_r", errno, NULL, __FUNCTION__);
	}
	// Signal success or failure.
	return grp.valid = (__grp != NULL);
}

#endif

