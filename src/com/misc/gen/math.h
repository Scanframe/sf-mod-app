#pragma once

#include "../global.h"
#include <cmath>
#include <random>

namespace sf
{

/**
 * @brief Rounds the passed value to a multiple of the rnd value.
 */
template<typename T>
T round(T value, T rnd)
{
	// Prevent non-integer and non-float types from implementing this template.
	static_assert(std::is_arithmetic<T>::value, "Type T must be a arithmetic type.");
	if constexpr (std::is_integral<T>::value)
		return ((value + (rnd / T(2))) / rnd) * rnd;
	if constexpr (std::is_floating_point<T>::value)
		// Function 'std::floor' has al float types implemented.
		return std::floor(value / rnd + T(0.5)) * rnd;
}

/**
 * @brief Calculates the offset for a given range and set point.
 */
template<class T, class S>
inline S calculateOffset(T value, T min, T max, S len, bool clip)
{
	max -= min;
	value -= min;
	S temp = (max && value) ? (std::is_floating_point<T>::value ? len * (value / max) : (len * value) / max) : 0;
	// Clip when required.
	if (clip)
	{
		// When the len is a negative value.
		if (len < 0)
		{
			return ((temp < len) ? len : (temp > S(0)) ? S(0)
																								 : temp);
		}
		else
		{
			return ((temp > len) ? len : (temp < S(0)) ? S(0)
																								 : temp);
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
 * @brief Generates random numbers within a specified integer range.
 */
template<typename T>
T random(T start, T stop)
{
	static std::default_random_engine generator{};// NOLINT(cert-msc32-c,cert-msc51-cpp)
	std::uniform_int_distribution<T> distribution(start, stop);
	return distribution(generator);
}

/**
 * @brief Modulo function.
 * @tparam T Arithmetic scalar type of the template.
 * @param k Is the dividend.
 * @param n Is the divisor.
 * @return Remainder
 */
template<typename T>
T modulo(T k, T n)
{
	// Only implementations of arithmetic values are allowed.
	static_assert(std::is_arithmetic<T>::value, "Type T must be a arithmetic type.");
	// Distinguish between floats and integers.
	if constexpr (std::is_floating_point<T>::value)
	{
		// Function fmod has implementations for any floating point.
		T r = std::fmod(k, n);
		//if the r is less than zero, add n to put it in the [0, n-1] range if n is positive
		//if the r is greater than zero, add n to put it in the [n-1, 0] range if n is negative
		return ((n > 0 && r < 0) || (n < 0 && r > 0)) ? r + n : r;
	}
	else
	{
		T r = k % n;
		//if the r is less than zero, add n to put it in the [0, n-1] range if n is positive
		//if the r is greater than zero, add n to put it in the [n-1, 0] range if n is negative
		return ((n > 0 && r < 0) || (n < 0 && r > 0)) ? r + n : r;
	}
}

/**
 * Gets the absolute value of a passed scalar value.
 * @tparam T Scalar type only.
 * @param v Scalar value.
 * @return Absolute value.
 */
template<typename T>
T toAbs(T v)
{
	static_assert(std::is_arithmetic<T>::value, "Type T must be a arithmetic type.");
	if constexpr (std::is_same<T, long double>())
		return fabsl(v);
	if constexpr (std::is_same<T, double>())
		return fabs(v);
	if constexpr (std::is_same<T, float>())
		return fabsf(v);
	if constexpr (std::is_integral<T>::value)
	{
		if constexpr (std::numeric_limits<T>::is_signed)
		{
			if constexpr (std::is_same<T, long long int>())
				return std::llabs(v);
			if constexpr (std::is_same<T, long int>())
				return std::labs(v);
			if constexpr (std::is_same<T, int>())
				return std::abs(v);
			// Smaller integers are handled here.
			return static_cast<T>(std::abs(v));
		}
		else
		{
			// Not a signed value return it as is.
			return v;
		}
	}
}

/**
 * @brief Fast integer power-of function.
 * @tparam T Integer type.
 * @param base
 * @param exponent
 * @return
 */
template<typename T>
T ipow(T base, int exponent)
{
	// Prevent non-integer types from implementing this template.
	static_assert(std::is_integral<T>::value, "Type T must be an integer type.");
	// Initialize the integer.
	T rv{1};
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
	return rv;
}

/**
 * @brief Converts degrees to radials.
 */
template<typename T>
inline constexpr T toRadials(T degrees)
{
	return degrees * (std::numbers::pi_v<T> / 180);
}

/**
 * @brief Converts radials to degrees.
 */
template<typename T>
inline constexpr T toDegrees(T radials)
{
	return radials / std::numbers::pi_v<T> * 180;
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

}// namespace sf
