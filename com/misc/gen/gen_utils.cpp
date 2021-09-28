
#include <cstdarg>
#include <ctime>
#include <utility>
#include <cxxabi.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include "Exception.h"
#include "dbgutils.h"
#include "gen_utils.h"
#include "target.h"
#include "TimeSpec.h"

#if IS_WIN
#include <windows.h>
#endif

namespace sf
{

std::string stringf(const char* fmt, ...)
{
	const size_t sz = 4096;
	auto buf = static_cast<char*>(malloc(sz));
	scope_free<char> sf(buf);
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(buf, sz, fmt, argptr);
	va_end(argptr);
	return &buf[0];
}

std::string bitToString(unsigned long wrd, size_t count)
{
	std::string ret;
	for (size_t i = 0; i < count; i++)
	{
		ret.append(1, (wrd & (1L << i)) ? '1' : '0');
	}
	return ret;
}

char hexCharValue(char ch)
{
	ch = (char) ((isalpha(ch)) ? ch & (~(1 << 5)) : ch);
	return (char) ((isalpha(ch)) ? ch - 'A' + 10 : ch - '0');
}

std::string hexString(const void* array, size_t size)
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

size_t stringHex(const char* hexstr, void* buffer, size_t size)
{
	// Check valid pointers.
	if (!hexstr || !buffer)
	{
		return -1;
	}
	// Get the amount of bytes to convert.
	unsigned chars = strlen(hexstr);
	// Check string length.
	if (!chars)
	{
		return 0;
	}
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
			((char*) buffer)[i / 2] = (char) ((hexCharValue(hexstr[i]) << 4) + hexCharValue(hexstr[i + 1]));
		}
	}
	// Return the number of returned bytes.
	return chars / 2;
}

std::string doEscaping(const std::string& str, bool reverse = false, char delimiter = '\0')
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
			// Check if the delimiter has been Set and the current is one.
			if (delimiter && ch == delimiter)
			{
				// Add the delimiter character as a hexadecimal 2 character code.
				rv += '\\';
				rv += 'x';
				rv += hexString(str.c_str() + i, 1);
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
						rv += hexString(str.c_str() + i, 1);
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
							if (stringHex(str.c_str() + i, &ch, 1))
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
	return doEscaping(str, false, delimiter);
}

std::string unescape(const std::string& str)
{
	return doEscaping(str, true);
}

std::string filter(std::string s, const std::string& filter)
{
	// Remove the unwanted characters from the string.
	size_t p;
	do
	{
		p = s.find_first_of(filter);
		if (p != std::string::npos)
		{
			s.erase(p, 1);
		}
	}
	while (p != std::string::npos);
	return s;
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

std::string getLine(std::istream& is)
{
	const size_t sz = 4096;
	char* buf = (char*) malloc(sz);
	scope_free<char> sf(buf);
	is.getline(buf, sz - 1);
	return &buf[0];
}

std::string getWorkingDirectory()
{
#if IS_WIN
	const size_t sz = 4096;
	char* buf = (char*) malloc(sz);
	scope_free<char> sf(buf);
	return getcwd(buf, sz - 1);
#else
	auto dir = get_current_dir_name();
	std::string rv(dir);
	free(dir);
	return rv;
#endif
}

std::string::value_type getDirectorySeparator()
{
#if IS_WIN
	return '\\';
#else
	return '/';
#endif
}

std::string getExecutableFilepath()
{
#if IS_WIN
	std::string rv(PATH_MAX, '\0');
	rv.resize(::GetModuleFileNameA(NULL, rv.data(), rv.capacity()));
	return rv;
#else
	std::string rv(PATH_MAX, '\0');
	rv.resize(::readlink("/proc/self/exe", rv.data(), rv.capacity()));
	return rv;
#endif
}

std::string getExecutableDirectory()
{
	auto rv = getExecutableFilepath();
	auto pos = rv.find_last_of(getDirectorySeparator());
	return rv.erase((pos != std::string::npos && pos > 0) ? pos : 0);
}

std::string getExecutableName()
{
	auto rv = getExecutableFilepath();
	auto pos = rv.find_last_of(getDirectorySeparator());
	return rv.erase(0, (pos != std::string::npos && pos > 0) ? pos + 1 : rv.length());
}

std::string demangle(const char* name)
{
	int status;
	char* nm = abi::__cxa_demangle(name, nullptr, nullptr, &status);
	std::string rv(nm);
	free(nm);
	return rv;
}

namespace
{

timespec startTime{0, 0};

__attribute__((constructor)) void initializeStartTime()
{
#if IS_WIN
	clock_gettime(CLOCK_MONOTONIC, &startTime);
#else
	clock_gettime(CLOCK_MONOTONIC_COARSE, &startTime);
#endif
}

}

timespec getTimeRunning()
{
	return TimeSpec(getTime()).sub(startTime);
}

timespec getTime(bool realTime)
{
	timespec ct{};
#if IS_WIN
	if (clock_gettime(realTime ? CLOCK_REALTIME :	CLOCK_MONOTONIC, &ct))
#else
	if (clock_gettime(realTime ? CLOCK_REALTIME :	CLOCK_MONOTONIC_COARSE, &ct))
#endif
	{
		throw ExceptionSystemCall("clock_gettime", errno, nullptr, __FUNCTION__);
	}
	return ct;
}

int timespecCompare(const timespec& ts1, const timespec& ts2)
{
	if (ts1.tv_sec > ts2.tv_sec)
	{  // NOLINT(bugprone-branch-clone)
		return 1;
	}
	else if (ts1.tv_sec < ts2.tv_sec)
	{  // NOLINT(bugprone-branch-clone)
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

std::string toLower(std::string s)
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

std::string toUpper(std::string s)
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

std::string trimRight(std::string s, const std::string& t)
{
	return s.erase(s.find_last_not_of(t) + 1);
}

std::string trimLeft(std::string s, const std::string& t)
{
	return s.erase(0, s.find_first_not_of(t));
}

std::string trim(std::string s, const std::string& t)
{
	return trimLeft(trimRight(std::move(s), t), t);
}

std::string unique(const std::string& s)
{
	return {s.c_str(), s.length()};
}

int precision(double value)
{
	constexpr int len = std::numeric_limits<double>::digits10;
	int i;
	char* s = ecvt(value, len, &i, &i);
	i = len;
	while (i--)
	{
		if (s[i] != '0')
		{
			return i + 1;
		}
	}
	return 0;
}

int digits(double value)
{
	constexpr int len = std::numeric_limits<double>::digits10;
	int dec, sign;
	char* s = ecvt(value, len, &dec, &sign);
	int i = len;
	while (i--)
	{
		if (s[i] != '0' || !i)
		{
			return -(dec - i - 1);
		}
	}
	return -(len + dec - 1);
}

int magnitude(double value)
{
	if (value)
	{
		int dec, sign;
		ecvt(value, std::numeric_limits<double>::digits10, &dec, &sign);
		return dec;
	}
	return 0;
}

int requiredDigits(double roundVal, double minVal, double maxVal)
{
	int rv, dec, sign;
	std::string s;
#if IS_WIN
	rv.resize( _CVTBUFSIZE + 1, '0');
	_ecvt_s(s.data(), s.size(), stepVal, std::numeric_limits<double>::digits10, &dec, &sign);
	s.resize(strlen(s.c_str()));
	rv = static_cast<int>(trimRight(s, "0").length());
#else
	// Create buffer large enough to hold all digits and signs including exponent 'e' and decimal dot '.'.
	s.resize(std::numeric_limits<double>::max_digits10 + 1, '0');
	ecvt_r(roundVal, std::numeric_limits<double>::digits10, &dec, &sign, s.data(), s.size());
	s.resize(strlen(s.c_str()), '0');
#endif
	// Determine the amount of actual digits of the step value.
	rv = static_cast<int>(trimRight(s, "0").length());
	// Get magnitude of the absolute maximum amount of steps that can be made.
	rv += magnitude(std::max(std::abs(maxVal), std::abs(minVal)) / roundVal);
	//
	rv += -1;
	return rv;
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

bool getFiles(strings& files, std::string directory, std::string wildcard) // NOLINT(performance-unnecessary-value-param)
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
		if (wildcmp(wildcard.c_str(), dirp->d_name, true))
		{
			files.push_back(std::string(dirp->d_name));
		}
	}
	//
	::closedir(dp);
	//
	return true;
}

std::string fileBaseName(const std::string& path)
{
	return ::basename(const_cast<char*>(sf::unique(path).c_str()));
}

std::string fileDirName(const std::string& path)
{
	return ::dirname(const_cast<char*>(sf::unique(path).c_str()));
}

bool fileUnlink(const std::string& path)
{
	if (::unlink(path.c_str()) == -1)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "of '" << path << "' failed!\n" << strerror(errno))
		return false;
	}
	return true;
}

bool fileRename(const std::string& old_path, const std::string& new_path)
{
	if (std::rename(old_path.c_str(), new_path.c_str()) == -1)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "from '" << old_path << "' to '" << new_path << "' failed!\n" << strerror(errno))
		return false;
	}
	return true;
}

bool fileExists(const char* path)
{
	return !::access(path, F_OK);
}

bool fileFind(sf::strings& files, const std::string& path)
{
	return sf::getFiles(files, fileDirName(path), fileBaseName(path));
}

}
