// Import the files header file.
#include "genutils.h"

#include <cstdarg>
#include <ctime>
#include <utility>
#include <cxxabi.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
// When not Windows.
#if !IS_WIN
#include <sys/select.h>
#include <sys/fsuid.h>
// Import of SYS_xxx
#include <sys/syscall.h>
#endif

// Import of ExceptionSystemCall.
#include "exception.h"
// Import debug utilities.
#include "dbgutils.h"

namespace sf
{

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

std::string bittostring(unsigned long wrd, size_t count)
{
	std::string ret;
	for (size_t i = 0; i < count; i++)
	{
		ret.append(1, (wrd & (1L << i)) ? '1' : '0');
	}
	return ret;
}

char hexcharvalue(char ch)
{
	ch = (char) ((isalpha(ch)) ? ch & (~(1 << 5)) : ch);
	return (char) ((isalpha(ch)) ? ch - 'A' + 10 : ch - '0');
}

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

size_t stringhex(const char* hexstr, void* buffer, size_t size)
{
	// Check valid pointers.
	if (!hexstr || !buffer)
	{
		return -1;
	}
	// Get the amount of bytes to convert.
	unsigned chars = strlen(hexstr);
	// Check mstring length.
	if (!chars) {return 0;}
	// check for odd number of characters to convert.
	if (chars % 2)
	{
		chars--;
	}
	// Adjust amount of bytes to convert to the size of the buffer
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
	// Return the number of returned bytes.
	return chars / 2;
}

std::string do_escaping(const std::string& str, bool reverse = false, char delimiter = '\0')
{
	std::string rv;
	// create conversion table
	static struct
	{
		char code;
		char ch;
	}
		table[] =
		{
			// single quote
			{'\'', '\''},
			// double quote
			{'\"', '"'},
			// bell
			{'\a', 'a'},
			// backspace
			{'\b', 'b'},
			// form feed / new page
			{'\f', 'f'},
			// new line / line feed
			{'\n', 'n'},
			// carriage return
			{'\r', 'r'},
			// horizontal tab
			{'\t', 't'},
			// vertical tab
			{'\v', 'v'},
			// '\' character
			{'\\', '\\'},
		};
	// When escaping.
	if (!reverse)
	{
		// Convert std::string character by character.
		for (size_t i = 0; i < str.length(); i++)
		{
			// get the current character 'ch'
			char ch = str[i];
			// Check if the delimiter has been set and the current is one.
			if (delimiter && ch == delimiter)
			{
				// Add the delimiter character as a hexadecimal 2 character code.
				rv += '\\';
				rv += 'x';
				rv += hexstring(str.c_str() + i, 1);
			}
			else
			{
				bool escaped = false;
				// Iterate though the table and assemble escapable characters.
				for (auto entry: table)
				{
					// When the character is to be escaped.
					if (ch == entry.code)
					{
						// Add the escaped character.
						rv += '\\';
						rv += entry.ch;
						// Break the loop
						escaped = true;
						break;
					}
				}
				// Check if code above did the conversion.
				if (!escaped)
				{
					// Al characters below the space are control characters and should hex.
					if (ch < ' ')
					{
						// Add the character as a hexadecimal 2 character code.
						rv += '\\';
						rv += 'x';
						rv += hexstring(str.c_str() + i, 1);
					}
					else
					{
						// Just add the character.
						rv += ch;
					}
				}
			}
		} // for
	}
	// In reverse (unescape).
	else
	{
		// Convert std::string character by character.
		for (size_t i = 0; i < str.length(); i++)
		{
			// Check for '\' character.
			if (str[i] == '\\')
			{
				bool found = false;
				// Increase index an check validity of the same index.
				if (++i < str.length())
				{
					// Iterate though the table and assemble escapable characters.
					for (auto entry: table)
					{
						// Check if the escaped character is convertible.
						if (entry.ch == str[i])
						{
							// Add character code.
							rv += entry.code;
							found = true;
							// Break from while loop.
							break;
						}
					}
				}
				// Check if code above did the conversion.
				if (!found)
				{
					// Check if the next character is 'x'.
					if (::tolower(str[i]) == 'x')
					{
						// Increase index an check validity of the same index.
						if (i += 2 < str.length())
						{
							// convert next 2 characters in the std::string to a binary character.
							char ch;
							if (stringhex(str.c_str() + i, &ch, 1))
							{
								// On success add the character
								rv += ch;
								// Increase index
								i++;
							}
						}
					}
					// Not a hex value.
					else
					{
						// Increase index and check validity of the same index.
						if (i < str.length())
						{
							// if next char is '\' add '\' to the returning std::string.
							if (str[i] == '\\')
							{
								rv += '\\';
							}
							else
							{
								rv += str[i];
							}
						}
					}
				}
			}
			// Not a control character.
			else
			{
				// Just add the character.
				rv += str[i];
			}
		} // for
	}
	return rv;
}

std::string escape(const std::string& str, char delimiter)
{
	return  do_escaping(str, false, delimiter);
}

std::string unescape(const std::string& str)
{
	return do_escaping(str, true);
}

std::string filter(std::string s, const std::string& filter)
{
	// Remove the unwanted characters from the string.
	size_t p;
	do
	{
		p = s.find_first_of(filter);
		if (p != std::string::npos)
			s.erase(p, 1);
	}
	while (p != std::string::npos);
	return s;
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
	/* must be done first to initialize read_fd */
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

std::string implode(strings strs, std::string glue, bool skip_empty) // NOLINT(performance-unnecessary-value-param)
{
	std::string retval;
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

strings explode(std::string str, std::string separator, bool skip_empty) // NOLINT(performance-unnecessary-value-param)
{
	strings result;
	size_t ofs = 0, found = str.find_first_of(separator, ofs);
	if (found != std::string::npos)
	{
		do
		{
			result.push_back(str.substr(ofs, found - ofs));
			//
			ofs = skip_empty ? str.find_first_not_of(separator, found) : (found + 1);
			if (ofs == std::string::npos)
			{
				break;
			}
			//
			found = str.find_first_of(separator, ofs);
		}
		while (found != std::string::npos);
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
std::string getline(std::istream& is)
{
	const size_t sz = 4096;
	char* buf = (char*) malloc(sz);
	scope_free<char> sf(buf);
	is.getline(buf, sz - 1);
	return &buf[0];
}

//
std::string getcwdstr()
{
	const size_t sz = 4096;
	char* buf = (char*) malloc(sz);
	scope_free<char> sf(buf);
	return getcwd(buf, sz - 1);
}

//
std::string demangle(const char* name)
{
	int status;
	char* nm = abi::__cxa_demangle(name, nullptr, nullptr, &status);
	std::string rv(nm);
	free(nm);
	return rv;
}

//
timespec gettime()
{
	timespec ct{};
	if (clock_gettime(CLOCK_REALTIME, &ct))
	{
		throw ExceptionSystemCall("epoll_wait", errno, nullptr, __FUNCTION__);
	}
	return ct;
}

//
int timespeccmp(const timespec& ts1, const timespec& ts2)
{
	if (ts1.tv_sec > ts2.tv_sec)
	{ // NOLINT(bugprone-branch-clone)
		return 1;
	}
	else if (ts1.tv_sec < ts2.tv_sec)
	{ // NOLINT(bugprone-branch-clone)
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
std::string tolower(std::string s)
{
	// Make the data pointer accessible.
	char* p = const_cast<char*>(s.data());
	for (std::string::size_type i = 0; i <= s.length(); i++)
	{
		if (isalpha(p[i]))
		{
			p[i] = (char) std::tolower(p[i]);
		}
	}
	return s;
}

std::string toupper(std::string s)
{
	// Make the data pointer accessible.
	char* p = const_cast<char*>(s.data());
	for (std::string::size_type i = 0; i <= s.length(); i++)
	{
		if (isalpha(p[i]))
		{
			p[i] = (char) std::toupper(p[i]);
		}
	}
	return s;
}


std::string trim_right(std::string s, const std::string& t)
{
	return s.erase(s.find_last_not_of(t) + 1);
}

std::string trim_left(std::string s, const std::string& t)
{
	return s.erase(0, s.find_first_not_of(t));
}

std::string trim(std::string s, const std::string& t)
{
	return trim_left(trim_right(std::move(s), t), t);
}

std::string unique(const std::string& s)
{
	return std::string(s.c_str(), s.length());
}

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
					return nullptr;
				}
			}
			while (sc != c);
			if (len > n)
			{
				return nullptr;
			}
		}
		while (strncmp(s, find, len) != 0);
		s--;
	}
	return (char*) s;
}

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

bool
getfiles(strings& files, std::string directory, std::string wildcard) // NOLINT(performance-unnecessary-value-param)
{
	DIR* dp;
	dirent* dirp;
	// Prevent errors from printing by checking the access.
	if (access(directory.c_str(), F_OK | X_OK))
	{
		return false;
	}
	//
	if ((dp = ::opendir(directory.c_str())) == nullptr)
	{
		std::cerr << "Error(" << errno << ") " << strerror(errno) << " " << directory << std::endl;
		return false;
	}
	//
	while ((dirp = readdir(dp)) != nullptr)
	{
		if (
			wildcmp(wildcard
				.
					c_str(), dirp
				->d_name, true))
		{
			files.
				push_back(std::string(dirp->d_name)
			);
		}
	}
	//
	::closedir(dp);
	//
	return true;
}

std::string file_basename(const std::string& path)
{
	return ::basename(const_cast<char*>(sf::unique(path).c_str()));
}

std::string file_dirname(const std::string& path)
{
	return ::dirname(const_cast<char*>(sf::unique(path).c_str()));
}

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

// When not windows.
#if !IS_WIN

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
	// In the end check if the directory exists as a whole.
	return file_exists(dir.c_str());
}

#endif // WIN32

bool file_find(sf::strings& files, const std::string& path)
{
	return sf::getfiles(files, file_dirname(path), file_basename(path));
}

bool file_write(const char* path, const void* buf, size_t sz, bool append)
{
	// Set mode to 644
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // NOLINT(hicpp-signed-bitwise)
	int fd = ::open(path, O_CREAT | O_WRONLY | (append ? O_APPEND : O_TRUNC), mode); // NOLINT(hicpp-signed-bitwise)
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
	struct tm ti{};
	// When -1 is passed get the time ourselfs.
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
	// Set the fields from the time zone which are not set using strptime().
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

bool proc_getgrnam(std::string name, group_t& grp)
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

#endif // IS_WIN

} // namespace
