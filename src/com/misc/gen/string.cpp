#include "string.h"
#include "TDynamicBuffer.h"
#include "pointer.h"
#include <cstdarg>

namespace sf
{

std::string demangle(const char* name)
{
#if IS_GNU
	std::string rv;
	// Check if not null.
	if (name != nullptr)
	{
		int status;
		char* nm = abi::__cxa_demangle(name, nullptr, nullptr, &status);
		if (status == 0)
		{
			rv.assign(nm);
			free(nm);
		}
		else
		{
			rv += "?";
			rv += name;
			rv += "?";
		}
	}
	else
	{
		rv = "<nullptr>";
	}
	return rv;
#else
	return name;
#endif
}

char hexCharValue(char ch)
{
	ch = (char) ((isalpha(ch)) ? ch & (~(1 << 5)) : ch);
	return (char) ((isalpha(ch)) ? ch - 'A' + 10 : ch - '0');
}

size_t stringHex(const char* hexstr, void* buffer, size_t size)
{
	// Check valid pointers.
	if (!hexstr || !buffer)
	{
		return -1;
	}
	// Get the amount of bytes to convert.
	size_t chars = strlen(hexstr);
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

std::string bitToString(unsigned long wrd, size_t count)
{
	std::string ret;
	for (size_t i = 0; i < count; i++)
	{
		ret.append(1, (wrd & (1L << i)) ? '1' : '0');
	}
	return ret;
}

std::string doEscaping(const std::string& str, bool reverse = false, char delimiter = '\0')
{
	std::string rv;
	// create conversion table
	static struct
	{
			char code;
			char ch;
	} table[] =
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
					// All characters below the space are control characters and should hex.
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
		}// for
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
				// Increase index and check validity of the same index.
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
		}// for
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
	} while (p != std::string::npos);
	return s;
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

std::string trim(std::string s, const std::string& t)
{
	return trimLeft(trimRight(std::move(s), t), t);
}

std::string trimRight(std::string s, const std::string& t)
{
	return s.erase(s.find_last_not_of(t) + 1);
}

std::string trimLeft(std::string s, const std::string& t)
{
	return s.erase(0, s.find_first_not_of(t));
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
		if ((case_s ? (*wild != *str) : (std::toupper(*wild) != std::toupper(*str))) && (*wild != '?'))
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
		else if ((case_s ? (*wild == *str) : (std::toupper(*wild) == std::toupper(*str))) || (*wild == '?'))
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

std::string error_string(int error_num)
{
	char buffer[BUFSIZ];
#if IS_WIN
	if (::strerror_s(buffer, BUFSIZ, error_num))
	{
		::snprintf(buffer, BUFSIZ, "Unknown error %d", error_num);
	}
	return buffer;
#else
	auto rv = ::strerror_r(error_num, buffer, BUFSIZ);
	return rv;
#endif
}

std::string stringf(const char* fmt, ...)
{
	const size_t sz = 4096;
	auto* buf = static_cast<char*>(malloc(sz));
	scope_free<char> sf(buf);
	va_list argptr;
	va_start(argptr, fmt);
	(void) vsnprintf(buf, sz, fmt, argptr);
	va_end(argptr);
	return &buf[0];
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
			} while (sc != c);
			if (len > n)
			{
				return nullptr;
			}
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return (char*) s;
}

char* decimal_separator_fix(char* buffer, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		char& c(buffer[i]);
		if (c == 0)
			break;
		if (c == ',')
		{
			c = '.';
			// Break on first encounter.
			break;
		}
	}
	return buffer;
}

std::string gcvtString(double value, int digits)
{
	// Buffer large enough to hold the string.
	char buf[std::numeric_limits<decltype(value)>::max_digits10 + std::numeric_limits<decltype(value)>::max_exponent10 + 5];
	// When zero digits is given use the maximum.
	digits = clip(digits, 0, std::numeric_limits<decltype(value)>::digits10);
	if (digits <= 0)
	{
		digits = std::numeric_limits<decltype(value)>::digits10;
	}
#if IS_WIN
	::_gcvt_s(buf, sizeof(buf), value, digits);
#else
	// Convert the string.
	::gcvt(value, digits, buf);
#endif
#if IS_WIN
	// Only needed for Windows since it adds a trailing '.' even when not required.
	return trimRight(decimal_separator_fix(buf, sizeof(buf)), ".");
#else
	// Get the value fixed decimal separator string.
	return decimal_separator_fix(buf, sizeof(buf));
#endif
}

std::string qgcvtString(long double value, int digits)
{
	// Buffer large enough to hold the string.
	char buf[std::numeric_limits<decltype(value)>::max_digits10 + std::numeric_limits<decltype(value)>::max_exponent10 + 5];
	// When zero digits is given use the maximum for the float type.
	if (digits <= 0)
	{
		digits = std::numeric_limits<decltype(value)>::digits10;
	}
#if IS_WIN
	// FIXME: Windowqs does not have a equivalent qgcvt() function.
	::_gcvt_s(buf, sizeof(buf), value, digits);
#else
	// Convert the string.
	::qgcvt(value, digits, buf);
#endif
#if IS_WIN
	// Only needed for Windows since it adds a trailing '.' even when not required.
	return trimRight(decimal_separator_fix(buf, sizeof(buf)), ".");
#else
	// Get the value fixed decimal separator string.
	return decimal_separator_fix(buf, sizeof(buf));
#endif
}

std::string toStringPrecision(double value, int precision)
{
	// Buffer large enough to hold the string.
	constexpr auto sz = std::numeric_limits<decltype(value)>::max_digits10 + std::numeric_limits<decltype(value)>::max_exponent10 + 16;
	char buf[sz];
	snprintf(buf, sz, "%.*lf", precision, value);
	return decimal_separator_fix(buf, sz);
}

std::string toStringPrecision(long double value, int precision)
{
	// Buffer large enough to hold the string.
	constexpr auto sz = std::numeric_limits<decltype(value)>::max_digits10 + std::numeric_limits<decltype(value)>::max_exponent10 + 16;
	char buf[sz];
	snprintf(buf, sz, "%.*Lf", precision, value);
	return buf;
}

std::string numberString(double value, int digits, bool sign_on)
{
	std::string rv;
	// Initialize some
	int dec{0}, sign{0};
	// When zero digits is given use the maximum for the float type.
	if (digits <= 0)
	{
		digits = std::numeric_limits<decltype(value)>::digits10;
	}
#if IS_WIN
	rv.resize(_CVTBUFSIZE + 1);
	_ecvt_s(rv.data(), rv.size(), value, std::numeric_limits<decltype(value)>::digits10, &dec, &sign);
	rv.resize(strlen(rv.c_str()));
#else
	// Create buffer large enough to hold all digits and signs including exponent 'e' and decimal dot '.'.
	rv.resize(std::numeric_limits<decltype(value)>::max_digits10 + 1);
	ecvt_r(value, std::numeric_limits<decltype(value)>::digits10, &dec, &sign, rv.data(), rv.size());
	rv.resize(std::numeric_limits<decltype(value)>::digits10);
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
	int exp{0};
	// When the amount decimals needs rounding to multiple of 3.
	if (dec % 3)
	{
		// Calculate the exponent floored to a multiple of 3 when not a multiple of 3.
		exp = (dec - 3) / 3 * 3;
	}
	else
	{
		exp = dec;
	}
	// Correct the decimal position using the calculated exponent.
	dec -= exp;
	// Correction to avoid '0.' at the start e.g. when having enough resolution.
	if (dec != 0)
	{
		if (exp != 0)
		{
			// Increment of decimal point placement.
			auto inc = (dec / 3) * -3;
			dec += inc;
			exp -= inc;
		}
	}
	else
	{
		if (std::abs(exp) > 0 && digits >= 3)
		{
			// Increment of decimal point placement.
			auto inc = (digits / 3) * 3;
			// Prevent an exponent part when not needed when there are enough digits.
			if (exp > 0 && exp < inc)
				inc = exp;
			dec += inc;
			exp -= inc;
		}
	}
	// When the floating point before the number string.
	if (dec <= 0)
	{
		rv.insert(0, -dec, '0').insert(0, "0.");
	}
	else
	{
		// When the floating point is within number string.
		if (dec < digits)
		{
			rv.insert(dec, ".");
		}
		else if (dec > digits)
		{
			rv.append(dec - digits, '0');
		}
	}
	// Only add a exponent value when non zero.
	if (exp)
	{
		if (sign_on && exp > 0)
			rv.append("e+").append(itostr(exp));
		else
			rv.append("e").append(itostr(exp));
	}
	// Always prepends the '-' sign but the plus sign only when sign_on is true.
	if (sign || sign_on)
	{
		rv.insert(0, 1, sign ? '-' : '+');
	}
	return rv;
}

std::istream& read_to_delimiter(std::istream& is, std::string& s, char delimiter)
{
	DynamicBuffer buf;
	const size_t block_size = 1024;
	size_t rc = 0;
	// skip first character if this is a delimiter
	if (is.peek() == delimiter)
	{
		is.get();
	}
	while (is.good() && is.peek() != delimiter)
	{
		buf.resize(block_size + rc);
		is.get(buf.ptr<char>(rc), block_size, delimiter);
		rc += is.gcount();
	}
	// Resize the buffer to the actual needed size and reserve space for the terminating nul.
	buf.resize(rc);
	// Replace the content of the passed string.
	s.assign(buf.c_str(), buf.size());
	//
	return is;
}

}// namespace sf
