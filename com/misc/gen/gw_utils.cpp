#include <cstdio>
#include <cstddef>
#include <math.h> // NOLINT(modernize-deprecated-headers)
#include <cerrno>
#include <cfloat>
#include <cctype>
#include <cstring>

namespace gw
{

static const auto ndigitMax = (lrint(ceil(M_LN2 / M_LN10 * DBL_MANT_DIG + 1.0)));

int fcvt_r(double value, int ndigit, int* decpt, int* sign, char* buf, size_t len)
{
	ssize_t n;
	ssize_t i;
	int left;
	if (buf == nullptr)
	{
		errno = EINVAL;
		return -1;
	}
	left = 0;
	if (isfinite(value))
	{
		*sign = signbit(value) != 0;
		if (*sign)
		{
			value = -value;
		}

		if (ndigit < 0)
		{
			/* Rounding to the left of the decimal point.  */
			while (ndigit < 0)
			{
				double new_value = value * 0.1;
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
		/* Value is Inf or NaN.  */
		*sign = 0;
	}

	n = snprintf(buf, len, "%.*lf", std::min<int>(ndigit, ndigitMax),
		value);
/* Check for a too small buffer.  */
	if (n >= (ssize_t) len)
	{
		return -1;
	}
	i = 0;
	while (i < n && isdigit(buf[i]))
	{
		++i;
	}
	*decpt = i;
	if (i == 0)
	{
	/* Value is Inf or NaN.  */
		return 0;
	}
	if (i < n)
	{
		do
		{
			++i;
		}
		while (i < n && !isdigit(buf[i]));

		if (*decpt == 1 && buf[0] == '0' && value != 0.0)
		{
		/* We must not have leading zeroes.  Strip them all out and adjust *DECPT if necessary.  */
			--*decpt;
			while (i < n && buf[i] == '0')
			{
				--*decpt;
				++i;
			}
		}
		memmove(&buf[std::max<int>(*decpt, 0)], &buf[i], n - i);
		buf[n - (i - std::max<int>(*decpt, 0))] = '\0';
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

int ecvt_r(double value, int ndigit, int* decpt, int* sign, char* buf, size_t len)
{
	int exponent = 0;

	if (isfinite(value) && value != 0.0)
	{
		/* Slow code that doesn't require -lm functions.  */
		double d;
		double f = 1.0;
		if (value < 0.0)
		{
			d = -value;
		}
		else
		{
			d = value;
		}
		if (d < 1.0)
		{
			do
			{
				f *= 10.0;
				--exponent;
			}
			while (d * f < 1.0);

			value *= f;
		}
		else if (d >= 10.0)
		{
			do
			{
				f *= 10;
				++exponent;
			}
			while (d >= f * 10.0);

			value /= f;
		}
	}
	else if (value == 0.0)
	{
		/* SUSv2 leaves it unspecified whether *DECPT is 0 or 1 for 0.0.
			 This could be changed to -1 if we want to return 0.  */
		exponent = 0;
	}
	if (ndigit <= 0 && len > 0)
	{
		buf[0] = '\0';
		*decpt = 1;
		*sign = isfinite(value) ? signbit(value) != 0 : 0;
	}
	else if (fcvt_r(value, std::min<int>(ndigit, ndigitMax) - 1, decpt, sign, buf, len))
	{
		return -1;
	}
	*decpt += exponent;
	return 0;
}

}
