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
#include "TDynamicBuffer.h"
// Import of shared library export defines.
#include "../global.h"

namespace sf
{

/**
* Calculates the offset for a given range and Set point.
*/
template<class T, class S>
inline
S calc_offset(T value, T min, T max, S len, bool clip)
{
	max -= min;
	value -= min;
	S temp = ((max) && (value)) ? S((value * T(len)) / max) : S(0);
	return (clip) ? ((temp > len) ? len : (temp < S(0)) ? S(0) : temp) : temp;
}

/**
* Return clipped value of v between a and b where a < b.
*/
template<class T>
T clip(const T v, const T a, const T b)
{
	return (v < a) ? a : ((v > b) ? b : v);
}

/**
* Deletes object and clears pointer
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

//
template<class T>
inline void delete_anull(T& p)
{
	if (p)
	{
		delete[] p;
		p = nullptr;
	}
}

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
 * Swaps the to passed values.
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
 * creates a formatted string and returns it in a string class instance.
 */
_MISC_FUNC std::string stringf(const char* fmt, ...);

/**
 * Better implementation of 'stringf' ?
 */
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	// Extra space for '\0'
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	// We don't want the '\0' inside
	return std::string(buf.get(), buf.get() + size - 1);
}

/**
 * converts a integer value to bit string '0' and '1' characters
 * where the first character is the first bit
 */
_MISC_FUNC std::string bittostring(unsigned long wrd, size_t count);

/**
 *  Returns numeric the value of the passed hexadecimal character
 */
_MISC_FUNC char hexcharvalue(char ch);

/**
 * Converts a hexadecimal string to a block of data
 */
_MISC_FUNC size_t stringhex(const char* hexstr, void* buffer, size_t sz);

/**
 * Converts a block of data to a hexadecimal string
 */
_MISC_FUNC std::string hexstring(const void* buffer, size_t sz);

/**
 * Escapes all control and non ascii characters.
 * @see unescape()
 * @param str String to be escaped.
 * @param delimiter Character delimiting the escaped string.
 * @return Escaped string
 */
_MISC_FUNC std::string escape(const std::string& str, char delimiter = '\0');
/**
 * Unescapes the passed string escaped by the escape() function.
 * @see escape()
 * @param str Input string
 * @return Unescaped string
 */
_MISC_FUNC std::string unescape(const std::string& str);

/**
 * Filters all characters from the passed string and returns the resulting string.
 */
_MISC_FUNC std::string filter(std::string str, const std::string& filter);

/**
* Counted vector of strings.
*/
typedef TVector<std::string> strings;

/**
* Counted vector iterator of strings.
*/
typedef TIterator<std::string> strings_iter;
/**
 * Implodes a string list into a single string.
 */
_MISC_FUNC std::string implode(strings strs, std::string glue, bool skip_empty = false);
/**
 * Explodes the passed string into a strings using the separator.
 * When skip_empty is true empty strings are ignored.
 */
_MISC_FUNC strings explode(const std::string& str, const std::string& separator, bool skip_empty = false);

/**
 * Return a line from the input stream.
 */
_MISC_FUNC std::string getline(std::istream& is);

_MISC_FUNC std::string::value_type getDirectorySeparator();
/**
 * Returns the working directory.
 */
_MISC_FUNC std::string getWorkingDirectory();
/**
 * Returns the executable filepath.
 */
_MISC_FUNC std::string getExecutableFilepath();
/**
 * Returns the executable directory.
 */
_MISC_FUNC std::string getExecutableDirectory();
/**
 * Returns the executable name.
 */
_MISC_FUNC std::string getExecutableName();
/**
 * Returns the timespec as function return value as clock_gettime().
 */
_MISC_FUNC timespec gettime();
/**
 * Returns the unmangled function name returned by type ID.
 */
_MISC_FUNC std::string demangle(const char* name);
/**
 * Compares the 2 times.
 * Returns -1, 0, 1 respectively for smaller, equal en larger.
 */
_MISC_FUNC int timespeccmp(const timespec& ts1, const timespec& ts2);
/**
 * Converts the passed string into a lower case one and returns it.
 */
_MISC_FUNC std::string tolower(std::string s);
/**
 * Converts the passed string into a upper case one and returns it.
 */
_MISC_FUNC std::string toupper(std::string s);
/**
 * Trims a passed string at both sides and returns it.
 */
_MISC_FUNC std::string trim(std::string s, const std::string& t = " ");
/**
 * Trims a passed string left and returns it.
 */
_MISC_FUNC std::string trim_left(std::string s, const std::string& t = " ");

/**
 * Trims a passed string right and returns it.
 */
_MISC_FUNC std::string trim_right(std::string s, const std::string& t = " ");

/**
 * Returns the same string but now uses a new buffer making the string thread save.
 */
_MISC_FUNC std::string unique(const std::string& s);

/**
 * Generates random numbers within a specified integer range.
 */
template<typename T>
T random(T start, T stop)
{
	static std::default_random_engine generator;
	std::uniform_int_distribution<T> distribution(start, stop);
	return distribution(generator);
}

/**
 * Returns the precision of the passed floating point value.
 * This is the amount of characters after the point without the trailing zeros.
 * When the value is 12300.00 the value returned is 3.
 * When the value is 0.0123 the value returned is also 3.
 * @tparam T  Floating point type.
 * @param value Floating point value
 * @return Amount of characters after the point without the trailing zeros.
 */
template<typename T>
int precision(T value)
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

/**
 * Returns the amount of digits which are significant for the value.
 * When the value is 12300.00 the value returned is -2.
 * When the value is 0.0123 the value returned is 4.
 * @tparam T Floating point type.
 * @param value
 * @return Amount of significant digits for the passed number.
 */
template<typename T>
int digits(T value)
{
	constexpr int len = std::numeric_limits<T>::digits10;
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

/**
 * Returns the order of magnitude of the passed value.
 * Examples:
 *  magnitude(0.001234) => -2
 *  magnitude(0.123400) =>  0
 *  magnitude(12340.00) =>  6
 */
template<typename T>
int magnitude(T value)
{
	if (value)
	{
		int dec, sign;
		ecvt(value, std::numeric_limits<T>::digits10, &dec, &sign);
		return dec;
	}
	return 0;
}

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
	if (std::numeric_limits<T>::is_integer)
	{
		return ((value + (rnd / T(2))) / rnd) * rnd;
	}
	else
	{
		return std::floor(value / rnd + T(0.5)) * rnd;
	}
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
	// In standard ltoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
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
 * String formats a floating point or integer value using scientific notation where the exponent is always a multiple of 3.
 * @tparam T Type of the template function.
 * @param value Value to be converted to a string.
 * @param digits Resolution in significant digits to represent the value.
 * @param sign_on When true the '+' sign is added.
 * @return String
 */
template<typename T>
std::string numberString(T value, unsigned digits, bool sign_on = true)
{
	std::string rv;
	// Initialize some
	int dec{0}, sign{0};
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
	// Round the integer value up to its required digits.
	auto rnd = ipow(10ull, (int) ((int) rv.length() - digits));
	rv = itostr(round(std::stoull(rv), rnd) / rnd);
	// Calculate the exponent floored to a multiple of 3 when not a multiple of 3.
	int exp = (dec % 3) ? (dec - 3) / 3 * 3 : dec;
	// Correct the decimal position using the new exponent.
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
			rv.append(dec - rv.length() - 1, '0');
		}
	}
	// Only add a exponent value when non zero.
	if (exp)
	{
		rv.append(stringf(sign_on ? "e%+-d" : "e%d", exp));
	}
	// Prepend always the '-' sign but the plus sign only when sign_on is true.
	if (sign || sign_on)
	{
		rv.insert(0, 1, sign ? '-' : '+');
	}
	//
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
 * Checks if the passed path exists (by doing a access() using F_OK).
 */
_MISC_FUNC bool file_exists(const char* path);

inline
bool file_exists(const std::string& path)
{
	return file_exists(path.c_str());
}

/**
 * Same as basename() but using an std::string.
 */
_MISC_FUNC std::string file_basename(const std::string& path);
/**
 * Same as dirname() but using an std::string.
 */
_MISC_FUNC std::string file_dirname(const std::string& path);
/**
 * Same as unlink() but using an std::string.
 */
_MISC_FUNC bool file_unlink(const std::string& path);
/**
 * Same as rename() but using an std::string.
 */
_MISC_FUNC bool file_rename(const std::string& old_path, const std::string& new_path);

/**
 * Finds the files using the passed wildcard.
 */
_MISC_FUNC bool file_find(sf::strings& files, const std::string& wildcard);

} // namespace
