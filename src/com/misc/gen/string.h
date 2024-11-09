#pragma once

#include "../global.h"
#include "Locale.h"
#include "math.h"
#include <cstring>
#include <cxxabi.h>
#include <limits>
#include <memory>
#include <string>

namespace sf
{

/**
 * @brief  Replaces a decimal comma for a point.
 * This instead of changing the global locale.
 * Used by template functions.
 * @param buffer
 * @param len
 * @return The passed buffer pointer.
 */
_MISC_FUNC char* decimal_separator_fix(char* buffer, size_t len);

/**
 * @brief The strncspn() function calculates the length of the initial segment of 's'
 * which consists entirely of characters not in reject up to a maximum 'n'.
 * When no reject chars were found n is returned.
 */
_MISC_FUNC size_t strncspn(const char* s, size_t n, const char* reject);

/**
 * @brief The strncspn() function calculates the length of the initial segment of 's'
 * which consists entirely of characters in accept up to a maximum 'n'.
 */
_MISC_FUNC size_t strnspn(const char* s, size_t n, const char* accept);

/**
 * @brief Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
_MISC_FUNC const char* strnstr(const char* s, const char* find, size_t n);

/**
 * @brief Returns the unmangled function name returned by type ID.
 */
_MISC_FUNC std::string demangle(const char* name);

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
 * @brief Converts an integer value to bit string '0' and '1' characters where the first character is the first bit
 */
_MISC_FUNC std::string bitToString(unsigned long wrd, size_t count);

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
 * @brief Converts the passed string into a lower case one and returns it.
 */
_MISC_FUNC std::string toLower(std::string s);

/**
 * @brief Converts the passed string into an upper case one and returns it.
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
 * @brief Matches a string against a wildcard string such as "*.*" or "bl?h.*" etc.
 */
_MISC_FUNC int wildcmp(const char* wild, const char* str, bool case_s);

/**
 * @brief Matches a string against a wildcard string such as "*.*" or "bl?h.*" etc.
 */
inline int wildcmp(const std::string& wild, const std::string& str, bool case_s)
{
	return wildcmp(wild.c_str(), str.c_str(), case_s);
}

/**
 * @brief Thread safe version strerror_r() or strerror_s() when cross compiling for Windows.
 */
_MISC_FUNC std::string error_string(int error_num);

/**
 * @brief Creates a formatted string and returns it in a string class instance.
 */
_MISC_FUNC std::string stringf(const char* fmt, ...);

/**
 * @brief Better implementation of 'stringf' ?
 */
template<typename... Args>
std::string string_format(const std::string& format, Args&&... args)
{
	// Extra space for '\0'
	size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args...);
	// We don't want the '\0' inside
	return {buf.get(), buf.get() + size - 1};
}

/**
 * @brief Converts a floating-point number to a string always using a decimal point.
 * It produces 'digits' significant digits in either printf(3) F format or E (scientific) format.
 * Always returns a decimal point without setting the global locale.
 * @tparam T Floating point type.
 * @param value Floating point value to convert.
 * @param ndigits Amount of digits.
 * @param buf Buffer pointer.
 * @param len Length of the buffer.
 * @return Pointer to passed buffer.
 */
template<typename T>
char* gcvt(T value, int ndigits, char* buf, size_t len)
{
	// Only implemented for floating point values.
	static_assert(std::is_floating_point<T>::value, "Type T must be a floating point type.");
	if constexpr (std::is_same<T, long double>())
	{
		std::snprintf(buf, len, "%.*Lg", std::min(ndigits, std::numeric_limits<T>::max_digits10), value);
	}
	if constexpr (std::is_same<T, double>())
	{
		std::snprintf(buf, len, "%.*lg", std::min(ndigits, std::numeric_limits<T>::max_digits10), value);
	}
	if constexpr (std::is_same<T, float>())
	{
		std::snprintf(buf, len, "%.*g", std::min(ndigits, std::numeric_limits<T>::max_digits10), value);
	}
	return decimal_separator_fix(buf, len);
}

/**
 * @brief Converts a floating-point number to a string always using a decimal point.
 * @see sf::gcvt()
 * @tparam T Floating point type.
 * @param value Floating point value to convert.
 * @param ndigits Amount of digits default to 0 which is means the minimum amount to represent the value.
 * @return String containing converted value.
 */
template<typename T>
std::string gcvtString(T value, int ndigits = 0)
{
	// Only implemented for floating point values.
	static_assert(std::is_floating_point<T>::value, "Type T must be a floating point type.");
	// Buffer large enough to hold the string.
	char buf[std::numeric_limits<T>::max_digits10 + std::numeric_limits<decltype(value)>::max_exponent10 + 5];
	// When zero digits is given use the maximum.
	ndigits = clip<int>(ndigits, 0, std::numeric_limits<T>::digits10);
	if (ndigits <= 0)
	{
		ndigits = std::numeric_limits<T>::digits10;
	}
	// Convert the string.
	return gcvt<T>(value, ndigits, buf, sizeof(buf));
}

/**
 * @brief Converts an integer type value to a buffer.
 *
 * The itoa function converts value to a null-terminated buffer and stores the result in buffer.
 * With itoa, value is an integer.
 * **Note:** The space allocated for buffer must be large enough to hold
 * the returned buffer, including the terminating null character (\0).
 * itoa can return up to (sizeof(T) * CHAR_BIT + 1) bytes.
 *
 * @tparam T Integer type.
 * @param value Integer value.
 * @param buffer Receiving buffer pointer.
 * @param base  Base of number system or radix defaulting to 10.
 * @return Null terminated pointer buffer. (Not the same as the buffer pointer.)
 */
template<typename T>
char* itoa(T value, char* buffer, int base = 10)
{
	// Prevent non-integer types from implementing this template.
	static_assert(std::is_integral<T>::value, "Type T must be an integer type.");
	// The buffer size is set to the amount of bits the type occupies.
	constexpr size_t buf_size = std::numeric_limits<T>::digits;
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

/**
 * @brief Converts an integer type value to a std::string.
 * @tparam T Integer type.
 * @param value Integer value.
 * @param base  Base of number system or radix defaulting to 10.
 * @return Null terminated pointer buffer. (Not the same as the buffer pointer.)
 */
template<typename T>
std::string itostr(T value, int base = 10)
{
	// Prevent non-integer types from implementing this template.
	static_assert(std::is_integral<T>::value, "Type T must be an integer type.");
	char buf[std::numeric_limits<T>::digits + 1];
	return itoa<T>(value, buf, base);
}

/**
 * @brief A locale independent template version for std::strtof(), std::strtod() and std::strtold() functions which uses locale "C".
 * @tparam T Floating point type.
 * @param ptr String buffer pointer to start.
 * @param end_ptr Optional pointer to pointer where conversion stopped reading.
 * @return The floating point value.
 */
template<typename T>
T toFloat(const char* ptr, char** end_ptr = nullptr)
{
	// Prevent non-integer and non-float types from implementing this template.
	static_assert(std::is_floating_point<T>::value, "Type T must be a floating point type.");
	{
		// Set locale to 'C' to get the required decimal point for this function.
		Locale locale;
		if constexpr (std::is_same<T, float>())
		{
			return std::strtof(ptr, end_ptr);
		}
		if constexpr (std::is_same<T, double>())
		{
			return std::strtod(ptr, end_ptr);
		}
		if constexpr (std::is_same<T, long double>())
		{
			return std::strtold(ptr, end_ptr);
		}
	}
}

/**
 * @brief Converts a std::string to a T value and whe not possible it returns the passed default.
 *
 * @tparam T Any floating point or integer value.
 * @param s String representation of the value.
 * @param idx Position where the conversion stopped in string 's'.
 * @return Converted value.
 */
template<typename T, typename S = std::string>
T toNumber(const S& s, size_t* idx = nullptr)
{
	if (!s.empty())
	{
		try
		{
			if constexpr (std::is_integral<T>::value)
			{
				if constexpr (std::numeric_limits<T>::is_signed)
				{
					return static_cast<T>(std::stoll(s, idx, 0));
				}
				else
				{
					return static_cast<T>(std::stoull(s, idx, 0));
				}
			}
			else if constexpr (std::is_floating_point<T>::value)
			{
				T rv;
				// Set locale to 'C' to get always the needed decimal point.
				Locale locale;
				if constexpr (std::is_same<T, float>())
				{
					rv = std::stof(s, idx);
				}
				if constexpr (std::is_same<T, double>())
				{
					rv = std::stod(s, idx);
				}
				if constexpr (std::is_same<T, long double>())
				{
					rv = std::stold(s, idx);
				}
				return rv;
			}
		}
		catch (...)
		{
			// Just all of them.
		}
	}
	return {};
}

/**
 * @brief Converts a std::string to a T value and when not possible it returns the passed default.
 *
 * @tparam T Any floating point or integer value.
 * @param s String representation of the value.
 * @param def Default value when conversion is not possible.
 * @return Converted or default value.
 */
template<typename T, typename S = std::string>
T toNumberDefault(const S& s, T def)
{
	static_assert(std::is_arithmetic<T>::value, "Type T must be a arithmetic type.");
	static_assert(std::is_same<S, std::string>() || std::is_same<S, std::wstring>(), "Type T must be of std::string or std::wstring.");
	size_t idx;
	auto rv = toNumber<T>(s, &idx);
	// Must have read to the end of the string for a good conversion
	// otherwise return the default value instead.
	if (idx != s.size())
	{
		rv = def;
	}
	return rv;
}

/**
 * @brief The function converts number to a minimal length.
 * It produces 'digits' significant digits in either printf(3) F format or E (scientific) format.
 * Always returns a decimal point without setting the global locale.
 * In case of more than 3 trailing zeros with large numbers a scientific notation is formed instead.
 * @param value Value to be converted to a string.
 * @param digits Amount of digits of resolution in case of a floating point.
 */
template<typename T>
std::string toString(T value, int digits = 0)
{
	static_assert(std::is_arithmetic<T>::value, "Type T must be a arithmetic type.");
	std::string rv;
	if constexpr (std::is_integral<T>::value)
	{
		return itostr<T>(value);
	}
	else
	{
		rv = gcvtString(value, digits);
		// Check for trailing zeros and convert to scientific notation if necessary.
		if (rv.find('e') == std::string::npos && rv.find('.') == std::string::npos)
		{
			int trailing_zeros = 0;
			for (int i = rv.length() - 1; i >= 0 && rv[i] == '0'; i--)
			{
				trailing_zeros++;
			}
			// Only when larger than 3 zeros.
			if (trailing_zeros > 3)
			{
				rv.erase(rv.length() - trailing_zeros, trailing_zeros).append("e").append(std::to_string(trailing_zeros));
			}
		}
		return rv;
	}
}

/**
 * @brief Coverts any floating point value in to a string with a given precision.
 * The default precision shows a .
 * @param value Floating point value.
 * @param prec Amount of float decimals when not in scientific notation.
 * @return sprintf string formated.
 */
template<typename T>
std::string toStringPrecision(T value, int prec = std::numeric_limits<int>::max())
{
	// Buffer large enough to hold the string.
	constexpr auto sz = std::numeric_limits<T>::max_digits10 + std::numeric_limits<T>::max_exponent10 + 16;
	char buf[sz];
	if constexpr (std::is_same<T, long double>())
	{
		if (prec == std::numeric_limits<int>::max())
		{
			std::snprintf(buf, sz, "%Lf", value);
		}
		else
		{
			std::snprintf(buf, sz, "%.*Lf", clip(prec, 0, std::numeric_limits<T>::digits10), value);
		}
	}
	if constexpr (std::is_same<T, double>())
	{
		if (prec == std::numeric_limits<int>::max())
		{
			std::snprintf(buf, sz, "%lf", value);
		}
		else
		{
			std::snprintf(buf, sz, "%.*lf", clip(prec, 0, std::numeric_limits<T>::digits10), value);
		}
	}
	if constexpr (std::is_same<T, float>())
	{
		if (prec == std::numeric_limits<int>::max())
		{
			std::snprintf(buf, sz, "%f", value);
		}
		else
		{
			std::snprintf(buf, sz, "%.*f", clip(prec, 0, std::numeric_limits<T>::digits10), value);
		}
	}
	return decimal_separator_fix(buf, sz);
}

/**
 * @brief String formats a floating point or integer value using scientific notation.
 * where the exponent is always a multiple of 3.
 * @param value Value to be converted to a string.
 * @param digits Resolution in significant digits to represent the value.
 * @param sign_on When true the '+' sign is added.
 * @return String
 */
_MISC_FUNC std::string numberString(double value, int digits, bool sign_on = true);

/**
 * @brief Read a stream until a given delimiter.
 * @param is Input stream.
 * @param s String being filled.
 * @param delimiter Delimiting character.
 * @return Passed input stream.
 */
_MISC_FUNC std::istream& read_to_delimiter(std::istream& is, std::string& s, char delimiter);

/**
 * @brief Function identical to ecvt(), except that 'ndigits' specifies the number of digits after the decimal point.
 *
 * @tparam T Floating point type.
 * @param value Floating point value to convert.
 * @param ndigit Amount of digits.
 * @param decpt Decimal point.
 * @param sign Non-zero when the value is negative.
 * @param buf Receiving character buffer pointer.
 * @param len Receiving buffer size.
 * @return 0 on success, and -1 otherwise.
 *
 * @note Copied from glibc and modified for C++.
 */
template<typename T>
int fcvt_r(T value, int ndigit, int* decpt, int* sign, char* buf, size_t len)
{
	// Only implemented for floating point values.
	static_assert(std::is_floating_point<T>::value, "Type T must be a floating point type.");
	ssize_t n, i;
	int left;
	if (buf == nullptr)
	{
		errno = (EINVAL);
		return -1;
	}
	left = 0;
	if (std::isfinite(value))
	{
		*sign = std::signbit(value) != 0;
		if (*sign)
		{
			value = -value;
		}
		if (ndigit < 0)
		{
			// Rounding to the left of the decimal point.
			while (ndigit < 0)
			{
				T new_value = value * 0.1;
				if (new_value < 1.0)
				{
					ndigit = 0;
					break;
				}
				value = new_value;
				++left;
				++ndigit;
			}
		}
	}
	else
	{
		// Value is Inf or NaN.
		*sign = 0;
	}
	if constexpr (std::is_same<T, long double>())
	{
		n = snprintf(buf, len, "%.*Lf", std::min(ndigit, std::numeric_limits<T>::max_digits10), value);
	}
	if constexpr (std::is_same<T, double>())
	{
		n = snprintf(buf, len, "%.*lf", std::min(ndigit, std::numeric_limits<T>::max_digits10), value);
	}
	if constexpr (std::is_same<T, float>())
	{
		n = snprintf(buf, len, "%.*f", std::min(ndigit, std::numeric_limits<T>::max_digits10), value);
	}
	// Check for a too small buffer.
	if (n >= (ssize_t) len)
	{
		return -1;
	}
	i = 0;
	while (i < n && std::isdigit(buf[i]))
	{
		++i;
	}
	*decpt = i;
	if (i == 0)
	{
		// Value is Inf or NaN.
		return 0;
	}
	if (i < n)
	{
		do
		{
			++i;
		} while (i < n && !isdigit(buf[i]));

		if (*decpt == 1 && buf[0] == '0' && value != 0.0)
		{
			// We must not have leading zeroes.  Strip them all out and adjust *DECPT if necessary.
			--*decpt;
			while (i < n && buf[i] == '0')
			{
				--*decpt;
				++i;
			}
		}
		memmove(&buf[std::max(*decpt, 0)], &buf[i], n - i);
		buf[n - (i - std::max(*decpt, 0))] = '\0';
	}

	if (left)
	{
		*decpt += left;
		if ((ssize_t) --len > n)
		{
			while (left-- > 0 && n < (ssize_t) len)
			{
				buf[n++] = '0';
			}
			buf[n] = '\0';
		}
	}
	return 0;
}

/**
 * @brief Converts number to a null-terminated string of 'ndigits' digits.
 * where 'ndigits' is reduced to a system-specific limit determined by the precision of a double.
 * The high-order digit is nonzero, unless number is zero.
 * The low order digit is rounded. The string itself does not contain a decimal point; however,
 * the position of the decimal point relative to the start of the string is stored in *decpt.
 * A negative value for *decpt means that the decimal point is to the left of the start of the string.
 * If the sign of number is negative, *sign is set to a nonzero value, otherwise it is set to 0.
 * If number is zero, it is unspecified whether *decpt is 0 or 1.
 *
 * @tparam T Floating point type.
 * @param value Floating point value to convert.
 * @param ndigit Amount of digits.
 * @param decpt Decimal point.
 * @param sign Non-zero when the value is negative.
 * @param buf Receiving character buffer pointer.
 * @param len Receiving buffer size.
 * @return 0 on success, and -1 otherwise.
 *
 * @note Copied from glibc and modified for C++.
 */
template<typename T>
int ecvt_r(T value, int ndigit, int* decpt, int* sign, char* buf, size_t len)
{
	// Only implemented for floating point values.
	static_assert(std::is_floating_point<T>::value, "Type T must be a floating point type.");
	// Type constexpr is not possible since std::pow so a static is used.
	static const T min_10_norm = std::pow(static_cast<T>(10), std::numeric_limits<T>::min_exponent10);
	int exponent = 0;
	if (std::isfinite(value) && value != 0.0)
	{
		// Slow code that doesn't require -lm functions.
		T d;
		T f = 1.0;
		if (value < 0.0)
		{
			d = -value;
		}
		else
		{
			d = value;
		}
		// For denormalized numbers the d < 1.0 case below won't work, as f can overflow to +Inf.
		if (d < min_10_norm)
		{
			value /= min_10_norm;
			if (value < 0.0)
			{
				d = -value;
			}
			else
			{
				d = value;
			}
			exponent += std::numeric_limits<T>::min_exponent10;
		}
		if (d < 1.0)
		{
			do
			{
				f *= 10.0;
				--exponent;
			} while (d * f < 1.0);
			value *= f;
		}
		else if (d >= 10.0)
		{
			do
			{
				f *= 10;
				++exponent;
			} while (d >= f * 10.0);
			value /= f;
		}
	}
	else if (value == 0.0)
	{
		// SUSv2 leaves it unspecified whether *DECPT is 0 or 1 for 0.0.
		// This could be changed to -1 if we want to return 0.
		exponent = 0;
	}
	if (ndigit <= 0 && len > 0)
	{
		buf[0] = '\0';
		*decpt = 1;
		*sign = std::isfinite(value) ? std::signbit(value) != 0 : 0;
	}
	else if (fcvt_r<T>(value, std::min(ndigit, std::numeric_limits<T>::max_digits10) - 1, decpt, sign, buf, len))
	{
		return -1;
	}
	*decpt += exponent;
	return 0;
}

}// namespace sf
