/*
General functions, defines, type definitions and templates to
make C++ programming easier.
*/

#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <string>
#include <memory>
#include <random>
#include <sys/types.h>
#include "TVector.h"
#include "TStrings.h"
#include "TDynamicBuffer.h"
// Import of shared library export defines.
#include "../global.h"

namespace sf
{

/**
 * @brief Calculates the offset for a given range and set point.
 */
template<class T, class S>
inline
S calculateOffset(T value, T min, T max, S len, bool clip)
{
	max -= min;
	value -= min;
	S temp = (max && value) ? (std::numeric_limits<T>::is_iec559 ? len * (value / max) : (len * value) / max) : 0;
	// Clip when required.
	if (clip)
	{
		// When the len is a negative value.
		if (len < 0)
		{
			return ((temp < len) ? len : (temp > S(0)) ? S(0) : temp);
		}
		else
		{
			return ((temp > len) ? len : (temp < S(0)) ? S(0) : temp);
		}
	}
	return temp;
}

/**
 * @brief Return clipped value of v between a and b where a < b.
 */
template<class T>
T clip(const T v, const T a, const T b)
{
	return (v < a) ? a : ((v > b) ? b : v);
}

/**
 * @brief Deletes object and clears pointer
 *
 * Defines and templates for deleting allocated memory and
 * testing for zero and clearing the pointer at the same time.
 */
template<class T>
inline void delete_null(T& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

/**
 * @brief Template function deleting an array previous allocated by 'new[]' when the pointer is non-null and also nulls the passed pointer.
 */
template<class T>
inline void delete_anull(T& p)
{
	if (p)
	{
		delete[] p;
		p = nullptr;
	}
}

/**
 * @brief Template function freeing a pointer previous allocated by 'malloc' when the pointer is non-null and also nulls the passed pointer.
 */
template<class T>
inline
void free_null(T& p)
{
	if (p)
	{
		free(p);
		p = nullptr;
	}
}

/**
 * @brief Swaps the to passed values.
 *
 * @tparam T Type that is swapped
 * @param t1 Parameter 1 of type
 * @param t2 Parameter 2 of type
 */
template<class T>
inline
void swap_it(T& t1, T& t2)
{
	T t(t1);
	t1 = t2;
	t2 = t;
}

/**
 * @brief Deletes the pointer of type T allocated by 'new' when this instance goes out of scope.
 * @tparam T
 */
template<class T>
class scope_delete
{
	public:
		explicit scope_delete(T* p) {P = p;}

		~scope_delete() {delete_null(P);}

		void disable_delete() {P = nullptr;}

	private:
		T* P;
};

/**
 * @brief Frees the pointer of type T allocated by 'malloc' when this instance goes out of scope.
 * @tparam T
 */
template<class T>
class scope_free
{
	public:
		explicit scope_free(T* p) {P = p;}

		~scope_free() {free_null(P);}

		void disable_free() {P = nullptr;}

	private:
		T* P;
};

/**
 * @brief Creates a formatted string and returns it in a string class instance.
 */
_MISC_FUNC std::string stringf(const char* fmt, ...);

/**
 * @brief Better implementation of 'stringf' ?
 */
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	// Extra space for '\0'
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	// We don't want the '\0' inside
	return {buf.get(), buf.get() + size - 1};
}

/**
 * @brief Converts a integer value to bit string '0' and '1' characters where the first character is the first bit
 */
_MISC_FUNC std::string bitToString(unsigned long wrd, size_t count);

/**
 * The gcvt() function converts number to a minimal length std::string.
 * It produces 'digits' significant digits in either printf(3) F format or E format.
 */
_MISC_FUNC std::string gcvtString(double value, int digits);

/**
 * @brief A locale independent version of std::strtold() function which uses locale "C".
 */
_MISC_FUNC long double stold(const char* ptr, char** end_ptr);

/**
 * @brief A locale independent version of std::strtod() function which uses locale "C".
 */
_MISC_FUNC double stod(const char* ptr, char** end_ptr);

/**
 * @brief A locale independent version of std::strtof() function which uses locale "C".
 */
_MISC_FUNC float stof(const char* ptr, char** end_ptr);

/**
 * @brief Returns numeric the value of the passed hexadecimal character
 */
_MISC_FUNC char hexCharValue(char ch);

/**
 * @brief Converts a hexadecimal string to a block of data
 */
_MISC_FUNC size_t stringHex(const char* hexstr, void* buffer, size_t sz);

/**
 * @brief Converts a block of data to a hexadecimal string
 */
_MISC_FUNC std::string hexString(const void* buffer, size_t sz);

/**
 * @brief Escapes all control and non ascii characters.
 *
 * @see unescape()
 * @param str String to be escaped.
 * @param delimiter Character delimiting the escaped string.
 * @return Escaped string
 */
_MISC_FUNC std::string escape(const std::string& str, char delimiter = '\0');

/**
 * @brief Unescapes the passed string escaped by the escape() function.
 *
 * @see escape()
 * @param str Input string
 * @return Unescaped string
 */
_MISC_FUNC std::string unescape(const std::string& str);

/**
 * @brief Filters all characters from the passed string and returns the resulting string.
 */
_MISC_FUNC std::string filter(std::string str, const std::string& filter);

/**
 * @brief Vector of std::strings with additional purks.
 */
typedef TStrings<std::string, std::string::value_type> strings;

/**
 * @brief Return a line from the input stream.
 */
_MISC_FUNC std::string getLine(std::istream& is);

/**
 * @brief Returns OS dependent directory separation character.
 */
_MISC_FUNC std::string::value_type getDirectorySeparator();

/**
 * @brief Returns the working directory.
 */
_MISC_FUNC std::string getWorkingDirectory();

/**
 * @brief Returns the executable filepath.
 */
_MISC_FUNC std::string getExecutableFilepath();

/**
 * @brief Returns the executable directory.
 */
_MISC_FUNC std::string getExecutableDirectory();

/**
 * @brief Returns the executable name.
 */
_MISC_FUNC std::string getExecutableName();

/**
 * @brief Gets the time this application is running.
 *
 * Is used in combination with getTime(false) to get the time since the start of the application.
 */
_MISC_FUNC timespec getTimeRunning();

/**
 * @brief Gets the timespec as function return value as clock_gettime() for the current time.
 * @param realTime When true the real time is returned.
 * 	      When false the time is used for timers and un effected by system time changes.
 */
_MISC_FUNC timespec getTime(bool realTime = false);

/**
 * @brief Returns the unmangled function name returned by type ID.
 */
_MISC_FUNC std::string demangle(const char* name);

/**
 * @brief Compares the 2 times.
 *
 * @return -1, 0, 1 respectively for smaller, equal en larger.
 */
_MISC_FUNC int timespecCompare(const timespec& ts1, const timespec& ts2);

/**
 * @brief Converts the passed string into a lower case one and returns it.
 */
_MISC_FUNC std::string toLower(std::string s);

/**
 * @brief Converts the passed string into a upper case one and returns it.
 */
_MISC_FUNC std::string toUpper(std::string s);

/**
 * @brief Trims a passed string at both sides and returns it.
 */
_MISC_FUNC std::string trim(std::string s, const std::string& t = " ");

/**
 * @brief Trims a passed string left and returns it.
 */
_MISC_FUNC std::string trimLeft(std::string s, const std::string& t = " ");

/**
 * @brief Trims a passed string right and returns it.
 */
_MISC_FUNC std::string trimRight(std::string s, const std::string& t = " ");

/**
 * @brief Returns the same string but now uses a new buffer making the string thread save.
 */
_MISC_FUNC std::string unique(const std::string& s);

/**
 * @brief Generates random numbers within a specified integer range.
 */
template<typename T>
T random(T start, T stop)
{
	static std::default_random_engine generator{}; // NOLINT(cert-msc32-c,cert-msc51-cpp)
	std::uniform_int_distribution<T> distribution(start, stop);
	return distribution(generator);
}

/**
 * @brief Returns the precision of the passed floating point value.
 * This is the amount of characters after the point without the trailing zeros.
 * When the value is 12300.00 the value returned is 3.
 * When the value is 0.0123 the value returned is also 3.
 * @param value Floating point value
 * @return Amount of characters after the point without the trailing zeros.
 */
_MISC_FUNC int precision(double value);

/**
 * @brief Returns the amount of digits which are significant for the value.
 * When the value is 12300.00 the value returned is -2.
 * When the value is 0.0123 the value returned is 4.
 * @param value Double value.
 * @return Amount of significant digits for the passed number.
 */
_MISC_FUNC int digits(double value);

/**
 * @brief Returns the order of magnitude of the passed value.
 * Examples:
 *  magnitude(0.001234) => -2
 *  magnitude(0.123400) =>  0
 *  magnitude(12340.00) =>  6
 */
_MISC_FUNC int magnitude(double value);

/**
 * @brief Gets the amount of required digits needed when drawing a scale.
 *
 * This function is used in combination with #numberString() to provide the 'digits' argument.
 *
 * @param roundVal Value used for rounding or stepping.
 * @param minVal Minimum value on the scale.
 * @param maxVal Maximum value on the scale.
 * @return Amount of digits required.
 */
_MISC_FUNC int requiredDigits(double roundVal, double minVal, double maxVal);

/**
 * Fast integer power-of function.
 * @tparam T Integer type.
 * @param base
 * @param exponent
 * @return
 */
template<typename T>
T ipow(T base, int exponent)
{
	// Initialize the integer.
	T rv = 0;
	if (std::numeric_limits<T>::is_integer)
	{
		rv = 1;
		for (;;)
		{
			// Check if the first bit is set.
			if (exponent & 1)
			{
				rv *= base;
			}
			// Shift the bits right.
			exponent >>= 1;
			// When no bits are standing of the exponent done break th loop.
			if (!exponent)
			{
				break;
			}
			// Squaring the values so far.
			base *= base;
		}
	}
	return rv;
}

/**
 * Rounds the passed value to a multiple of the rnd value.
 */
template<typename T>
T round(T value, T rnd)
{
	return std::numeric_limits<T>::is_integer
		? ((value + (rnd / T(2))) / rnd) * rnd
		: std::floor(value / rnd + T(0.5)) * rnd;
}

/**
 * Creates typed reference to a null pointer.
 * @tparam T
 * @return
 */
template<typename T>
T& null_ref()
{
	return (*(static_cast<T*>(nullptr)));
}

template<typename T>
bool not_ref_null(T& r)
{
	return &r == nullptr;
}

/**
 * Converts an integer to a buffer.<br>
 * itoa converts value to a null-terminated buffer and stores the result
 * in buffer. With itoa, value is an integer.<br>
 * <b>Note:</b> The space allocated for buffer must be large enough to hold
 * the returned buffer, including the terminating null character (\0).
 * itoa can return up to (sizeof(T) * CHAR_BIT + 1) bytes.
 */
template<typename T>
char* itoa(T value, char* buffer, int base)
{
	constexpr size_t buf_size = sizeof(T) * CHAR_BIT;
	// Sanity check.
	if (!value || base < 2 || base > 16)
	{
		buffer[0] = '0';
		buffer[1] = 0;
		return buffer;
	}
	bool neg = false;
	// In standard ltoa(), negative numbers are handled only with base 10.
	// Otherwise, numbers are considered unsigned.
	if (value < 0 && base == 10)
	{
		neg = true;
		value *= -1;
	}
	// Terminate the buffer.
	buffer[buf_size + 1] = 0;
	// Reverse iterate in the character buffer.
	int i;
	for (i = buf_size; value && i; --i)
	{
		buffer[i] = "0123456789abcdef"[value % base];
		value /= base;
	}
	// When negative in base 10 prepend the negative sign.
	if (neg)
	{
		buffer[i--] = '-';
	}
	// Return the pointer of the last written character.
	return &buffer[i + 1];
}

template<typename T>
std::string itostr(T value, int base = 10)
{
	char buf[sizeof(T) * CHAR_BIT + 1];
	return itoa<T>(value, buf, base);
}

/**
 * @brief Converts a std::string to a T value and whe not possible it returns the passed default.
 *
 * @tparam T Any floating point or integer value.
 * @param s String representation of the value.
 * @param def Default value when conversion is not possible.
 * @return Converted or default value.
 */
template<typename T>
T toTypeDef(const std::string& s, T def)
{
	if (!s.empty())
	{
		try
		{
			std::string::size_type idx = std::string::npos;
			if (std::numeric_limits<T>::is_integer)
			{
				if (std::numeric_limits<T>::is_signed)
				{
					auto rv = std::stoll(s, &idx, 0);
					// Must have read to the end of the string.
					if (idx != s.size())
					{
						rv = def;
					}
					return static_cast<T>(rv);
				}
				else
				{
					auto rv = std::stoull(s, &idx, 0);
					// Must have read to the end of the string.
					if (idx != s.size())
					{
						rv = def;
					}
					return static_cast<T>(rv);
				}
			}
			else if (std::numeric_limits<T>::is_iec559)
			{
				auto rv = std::stod(s, &idx);
				// Must have read to the end of the string.
				if (idx != s.size())
				{
					rv = def;
				}
				return static_cast<T>(rv);
			}
		}
		catch (...)
		{
			// Just all of them.
		}
	}
	return def;
}

/**
 * String formats a floating point or integer value using scientific notation where the exponent is always a multiple of 3.
 * @tparam T Type of the template function.
 * @param value Value to be converted to a string.
 * @param digits Resolution in significant digits to represent the value.
 * @param sign_on When true the '+' sign is added.
 * @return String
 */
template<typename T>
std::string numberString(T value, int digits, bool sign_on = true)
{
	std::string rv;
	// Initialize some
	int dec{0}, sign{0};
#if IS_WIN
	rv.resize( _CVTBUFSIZE + 1);
	_ecvt_s(rv.data(), rv.size(), value, std::numeric_limits<double>::digits10, &dec, &sign);
	rv.resize(strlen(rv.c_str()));
#else
	//
	if (std::numeric_limits<T>::is_iec559)
	{
		// Create buffer large enough to hold all digits and signs including exponent 'e' and decimal dot '.'.
		rv.resize(std::numeric_limits<T>::max_digits10 + 1);
		// Check if this is a long double.
		if (sizeof(T) == sizeof(long double))
		{
			qecvt_r(value, std::numeric_limits<T>::digits10, &dec, &sign, rv.data(), rv.size());
		}
		else
		{
			ecvt_r(value, std::numeric_limits<T>::digits10, &dec, &sign, rv.data(), rv.size());
		}
		rv.resize(std::numeric_limits<T>::digits10);
	}
	else
	{
		if (std::numeric_limits<T>::is_integer)
		{
			// Create buffer large enough to hold all digits and signs including exponent 'e' and decimal dot '.'.
			rv.resize(std::numeric_limits<long double>::max_digits10 + 1);
			qecvt_r(value, std::numeric_limits<T>::digits10, &dec, &sign, rv.data(), rv.size());
			rv.resize(std::numeric_limits<long double>::digits10);
		}
	}
#endif
	// Round the integer value up to its required digits.
	auto rnd = ipow(10ull, (int) ((int) rv.length() - digits));
	rv = itostr(round(std::stoull(rv), rnd) / rnd);
	// Check if rounding added a decimal.
	if (rv.length() != digits)
	{
		dec += 1;
		rv.resize(digits);
	}
	// Exponent to be calculated.
	int exp;
	// When the amount decimals needs rounding to multiple of 3.
	if (dec % 3)
	{
		// Calculate the exponent floored to a multiple of 3 when not a multiple of 3.
		exp = (dec - 3) / 3 * 3;
	}
		// When decimals and magnitude are the same.
	else if (dec == magnitude(value))
	{
		exp = 0;
	}
	else
	{
		exp = dec;
	}
	// Correct the decimal position using the calculated exponent.
	dec -= exp;
	// Correction to avoid '0.' at the start when having enough resolution.
	if (exp > 0 && !dec && rv.length() > 3)
	{
		dec += 3;
		exp -= 3;
	}
	// When the floating point before the number string.
	if (dec <= 0)
	{
		rv.insert(0, -dec, '0').insert(0, "0.");
	}
	else
	{
		// When the floating point is within number string.
		if (dec < rv.length())
		{
			rv.insert(dec, ".");
		}
		else if (dec > rv.length())
		{
			rv.append(dec - rv.length(), '0');
		}
	}
	// Only add a exponent value when non zero.
	if (exp)
	{
		rv.append(stringf(sign_on ? "e%+-d" : "e%d", exp));
	}
	// Always prepends the '-' sign but the plus sign only when sign_on is true.
	if (sign || sign_on)
	{
		rv.insert(0, 1, sign ? '-' : '+');
	}
	return rv;
}

/**
 * Modulo function.
 * @tparam T Integer type of the template.
 * @param k Is the dividend.
 * @param n Is the divisor.
 * @return Remainder
 */
template<typename T>
T imodulo(T k, T n)
{
	T r = k % n;
	//if the r is less than zero, add n to put it in the [0, n-1] range if n is positive
	//if the r is greater than zero, add n to put it in the [n-1, 0] range if n is negative
	return ((n > 0 && r < 0) || (n < 0 && r > 0)) ? r + n : r;
}

/**
 * Modulo function
 * @tparam T Floating point type of the template.
 * @param k Is the dividend.
 * @param n Is the divisor.
 * @return Remainder
 */
template<typename T>
T fmodulo(T k, T n)
{
	T r = std::fmod(k, n);
	//if the r is less than zero, add n to put it in the [0, n-1] range if n is positive
	//if the r is greater than zero, add n to put it in the [n-1, 0] range if n is negative
	return ((n > 0 && r < 0) || (n < 0 && r > 0)) ? r + n : r;
}

/*
 * The strncspn() function calculates the length of the initial segment of 's'
 * which consists entirely of characters not in reject up to a maximum 'n'.
 * When no reject chars were found n is returned.
 */
_MISC_FUNC size_t strncspn(const char* s, size_t n, const char* reject);

/*
 * The strncspn() function calculates the length of the initial segment of 's'
 * which consists entirely of characters in accept up to a maximum 'n'.
 */
_MISC_FUNC size_t strnspn(const char* s, size_t n, const char* accept);

/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
_MISC_FUNC const char* strnstr(const char* s, const char* find, size_t n);

/**
 * Matches a string against a wildcard string such as "*.*" or "bl?h.*" etc.
 */
_MISC_FUNC int wildcmp(const char* wild, const char* str, bool case_s);

/**
 * Matches a string against a wildcard string such as "*.*" or "bl?h.*" etc.
 */
inline
int wildcmp(const std::string& wild, const std::string& str, bool case_s)
{
	return wildcmp(wild.c_str(), str.c_str(), case_s);
}

/**
 * Checks if the passed path exist (by doing a access() using F_OK).
 */
_MISC_FUNC bool fileExists(const char* path);

inline
bool fileExists(const std::string& path)
{
	return fileExists(path.c_str());
}

/**
 * Same as basename() but using an std::string.
 */
_MISC_FUNC std::string fileBaseName(const std::string& path);
/**
 * Same as dirname() but using an std::string.
 */
_MISC_FUNC std::string fileDirName(const std::string& path);
/**
 * Same as unlink() but using an std::string.
 */
_MISC_FUNC bool fileUnlink(const std::string& path);
/**
 * Same as rename() but using an std::string.
 */
_MISC_FUNC bool fileRename(const std::string& old_path, const std::string& new_path);

/**
 * @brief Finds the files using the passed wildcard.
 */
_MISC_FUNC bool fileFind(sf::strings& files, const std::string& wildcard);

/**
 * @brief Returns the thread count from the current process.
 */
_MISC_FUNC size_t getThreadCount();



}
