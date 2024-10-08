#include "math.h"
#include "gnu_compat.h"
#include "pointer.h"
#include "string.h"

namespace sf
{

int precision(double value)
{
	constexpr size_t sz = 64;
	char buf[sz];
	constexpr int len = std::numeric_limits<double>::digits10;
	int i;
	ecvt_r(value, len, &i, &i, buf, sz);
	i = len;
	while (i--)
	{
		if (buf[i] != '0')
		{
			return i + 1;
		}
	}
	return 0;
}

int digits(double value)
{
	constexpr size_t sz = 64;
	char buf[sz];
	constexpr int len = std::numeric_limits<double>::digits10;
	int dec, sign;
	ecvt_r(value, len, &dec, &sign, buf, sz);
	int i = len;
	while (i--)
	{
		if (buf[i] != '0' || !i)
		{
			return -(dec - i - 1);
		}
	}
	return -(len + dec - 1);
}

int magnitude(double value)
{
	if (value != 0.0)
	{
		constexpr int digits = std::numeric_limits<double>::digits10;
		constexpr size_t buf_sz = 64;
		char buf[buf_sz + 1];
		int dec, sign;
		ecvt_r(value, digits, &dec, &sign, buf, buf_sz);
		return dec;
	}
	return 0;
}

int requiredDigits(double roundVal, double minVal, double maxVal)
{
	int rv, dec, sign;
	std::string s;
	s.reserve(64);
	// Create buffer large enough to hold all digits and signs including exponent 'e' and decimal dot '.'.
	s.resize(std::numeric_limits<double>::max_digits10 + 1, '0');
	ecvt_r(roundVal, std::numeric_limits<double>::digits10, &dec, &sign, s.data(), s.size());
	s.resize(strlen(s.c_str()), '0');
	// Determine the amount of actual digits of the step value.
	rv = static_cast<int>(trimRight(s, "0").length());
	// Get magnitude of the absolute maximum amount of steps that can be made.
	rv += magnitude(std::max(std::abs(maxVal), std::abs(minVal)) / roundVal);
	//
	rv += -1;
	return rv;
}

}// namespace sf