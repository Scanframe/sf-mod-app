#pragma once
#include <misc/gen/dbgutils.h>
#include <misc/gen/string.h>
#include <regex>

namespace sf
{

template<typename T>
TVector2D<T>::TVector2D(const TVector2D& v)
	: _data(v._data)
{}
template<typename T>
TVector2D<T>::TVector2D(TVector2D&&) noexcept
{}

template<typename T>
TVector2D<T>::TVector2D(T xp, T yp)
	: _data({xp, yp})
{}

template<typename T>
TVector2D<T>::operator const T*() const
{
	return _data.array;
}

template<typename T>
constexpr TVector2D<T>::operator T*()
{
	return _data.array;
}

template<typename T>
T TVector2D<T>::length() const
{
	return std::sqrt(lengthSqr());
}

template<typename T>
T TVector2D<T>::lengthSqr() const
{
	return _data.coord.x * _data.coord.x + _data.coord.y * _data.coord.y;
}

template<typename T>
T TVector2D<T>::distanceSqr(const TVector2D& v) const
{
	return (_data.coord.x - v._data.coord.x) * (_data.coord.x - v._data.coord.x) + (_data.coord.y - v._data.coord.y) * (_data.coord.y - v._data.coord.y);
}

template<typename T>
T TVector2D<T>::distance(const TVector2D& v) const
{
	return std::sqrt<T>(distanceSqr(v));
}

template<typename T>
TVector2D<T>& TVector2D<T>::normalize()
{
	auto len = length();
	// Prevent division by near zero.
	if (len != std::numeric_limits<T>::epsilon())
	{
		*this /= len;
	}
	return *this;
}

template<typename T>
TVector2D<T> TVector2D<T>::normalized() const
{
	return TVector2D(_data.coord.x, _data.coord.y).normalize();
}

template<typename T>
TVector2D<T>& TVector2D<T>::scale(T factor)
{
	_data.coord.x *= factor;
	_data.coord.y *= factor;
	return *this;
}

template<typename T>
TVector2D<T> TVector2D<T>::scaled(T factor) const
{
	return {_data.coord.x * factor, _data.coord.y * factor};
}

template<typename T>
T TVector2D<T>::crossProduct(const TVector2D& v) const
{
	return _data.coord.x * v._data.coord.x - _data.coord.y * v._data.coord.y;
}

template<typename T>
T TVector2D<T>::dotProduct(const TVector2D& v) const
{
	return _data.coord.x * v._data.coord.x + _data.coord.y * v._data.coord.y;
}

template<typename T>
T TVector2D<T>::operator*(const TVector2D& v) const
{
	return crossProduct(v);
}

template<typename T>
T TVector2D<T>::angle() const
{
	value_type rv = 0.0;
	TVector2D v1(*this);
	// Check for a very small length and increment it since it does not change the outcome.
	if (isZero(v1.length(), tolerance * 1E5))
	{
		v1 /= v1.lengthSqr();
	}
	// Avoid an almost division by zero.
	if (isZero(v1._data.coord.x, tolerance))
	{
		if (v1._data.coord.x < 0.0)
		{
			if (v1._data.coord.y < 0.0)
			{
				rv = numbers::pi_v<T> / 2;
			}
			else
			{
				rv = -numbers::pi_v<T> / 2;
			}
		}
		else
		{
			if (v1._data.coord.y < 0.0)
			{
				rv = -numbers::pi_v<T> / 2;
			}
			else
			{
				rv = numbers::pi_v<T> / 2;
			}
		}
	}
	else
	{
		// Function atan2 takes location in quadrants in to account when returning value.
		rv = std::atan2(v1._data.coord.y, v1._data.coord.x);
	}
	// Return a non-normalized angle.
	return rv;
}

template<typename T>
T TVector2D<T>::angleNormalized() const
{
	auto rv = angle();
	// When the angle is still negative add 360 degrees in radials to it.
	if (rv < 0.0)
	{
		rv += 2.0 * numbers::pi_v<T>;
	}
	return rv;
}

template<typename T>
T TVector2D<T>::angle(const TVector2D& v) const
{
	TVector2D v1(*this), v2(v);
	// When for calculations the values are too small enlarge the
	// vector since the result is the same for the angle.
	if (v1.length() < std::numeric_limits<T>::epsilon() * 1E6)
	{
		auto ls = v1.lengthSqr();
		v1 /= ls;
		v2 /= ls;
	}
	if (v2.length() < std::numeric_limits<T>::epsilon() * 1E6)
	{
		auto ls = v2.lengthSqr();
		v1 /= ls;
		v2 /= ls;
	}
	// Quick and dirty to get the angle by using in-product and out-product
	// to get the cos alpha and the sinus alpha.
	auto len = v1.length() * v2.length();
	if (len < std::numeric_limits<T>::epsilon())
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid length/magnitude !");
	}
	auto dp = v1.dotProduct(v2);
	// When the cross product is zero the 2 vectors are parallel to each other and the angle is therefore zero.
	if (isZero(v1.crossProduct(v2), tolerance))
	{
		// TODO: Is the result 0.0 not always the case here?
		return (dp > 0) ? 0.0 : numbers::pi_v<T>;
	}
	// Calculation using dot-product.
	return std::acos(dp / len);
}

template<typename T>
T TVector2D<T>::slope() const
{
	// Check for a possible division by zero.
	if (std::abs(_data.coord.x) < std::numeric_limits<T>::epsilon())
	{
		return std::numeric_limits<T>::max();
	}
	return _data.coord.y / _data.coord.x;
}

template<typename T>
constexpr T TVector2D<T>::x() const
{
	return _data.coord.x;
}

template<typename T>
constexpr T& TVector2D<T>::x()
{
	return _data.coord.x;
}

template<typename T>
constexpr T TVector2D<T>::y() const
{
	return _data.coord.y;
}

template<typename T>
constexpr T& TVector2D<T>::y()
{
	return _data.coord.y;
}

template<typename T>
T& TVector2D<T>::operator[](size_t i)
{
	constexpr auto sz = sizeof(data_type::array) / sizeof(T);
	if (i >= sz)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[i];
}

template<typename T>
const T& TVector2D<T>::operator[](size_t i) const
{
	constexpr auto sz = sizeof(data_type::array) / sizeof(T);
	if (i >= sz)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[i];
}

template<typename T>
TVector2D<T>& TVector2D<T>::assign(T xp, T yp)
{
	_data.coord.x = xp;
	_data.coord.y = yp;
	return *this;
}

template<typename T>
TVector2D<T>& TVector2D<T>::assign(const TVector2D& v)
{
	_data = v._data;
	return *this;
}

template<typename T>
TVector2D<T>& TVector2D<T>::operator=(TVector2D& v) noexcept
{
	_data = v._data;
	return *this;
}

template<typename T>
TVector2D<T>& TVector2D<T>::operator=(TVector2D&& v) noexcept
{
	_data = v._data;
	return *this;
}

template<typename T>
TVector2D<T>& TVector2D<T>::operator*=(const TMatrix22<T>& mtx)
{
	return assign(mtx.transformed(*this));
}

template<typename T>
TVector2D<T> TVector2D<T>::operator-() const
{
	return TVector2D(-_data.coord.x, -_data.coord.y);
}

template<typename T>
TVector2D<T>& TVector2D<T>::operator+=(const TVector2D& v)
{
	_data.coord.x += v._data.coord.x;
	_data.coord.y += v._data.coord.y;
	return *this;
}

template<typename T>
TVector2D<T>& TVector2D<T>::operator-=(const TVector2D& v)
{
	_data.coord.x -= v._data.coord.x;
	_data.coord.y -= v._data.coord.y;
	return *this;
}

template<typename T>
TVector2D<T>& TVector2D<T>::operator*=(T c)
{
	return scale(c);
}

template<typename T>
TVector2D<T>& TVector2D<T>::operator/=(T c)
{
	T inv = 1.0 / c;
	return scale(inv);
}

template<typename T>
TVector2D<T> TVector2D<T>::operator+(const TVector2D& v) const
{
	return TVector2D(_data.coord.x + v._data.coord.x, _data.coord.y + v._data.coord.y);
}

template<typename T>
TVector2D<T> TVector2D<T>::operator-(const TVector2D& v) const
{
	return TVector2D(_data.coord.x - v._data.coord.x, _data.coord.y - v._data.coord.y);
}

template<typename T>
TVector2D<T> TVector2D<T>::operator/(T c) const
{
	if (std::abs(c) < std::numeric_limits<T>::epsilon())
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid denominator!");
	}
	return TVector2D(_data.coord.x / c, _data.coord.y / c);
}

template<typename T>
bool TVector2D<T>::isEqual(const TVector2D& v, T tol) const
{
	return sf::isEqual<T>(_data.coord.x, v._data.coord.x, tol) && sf::isEqual<T>(_data.coord.y, v._data.coord.y, tol);
}

template<typename T>
bool TVector2D<T>::operator==(const TVector2D& v) const
{
	return isEqual(v, tolerance);
}

template<typename T>
bool TVector2D<T>::operator!=(const TVector2D& v) const
{
	return !isEqual(v, tolerance);
}

template<typename T>
void TVector2D<T>::updateMin(const TVector2D& vertex)
{
	if (vertex._data.coord.x < _data.coord.x)
	{
		_data.coord.x = vertex._data.coord.x;
	}
	if (vertex._data.coord.y < _data.coord.y)
	{
		_data.coord.y = vertex._data.coord.y;
	}
}

template<typename T>
void TVector2D<T>::updateMax(const TVector2D& vertex)
{
	if (vertex._data.coord.x > _data.coord.x)
	{
		_data.coord.x = vertex._data.coord.x;
	}
	if (vertex._data.coord.y > _data.coord.y)
	{
		_data.coord.y = vertex._data.coord.y;
	}
}

template<typename T>
std::string TVector2D<T>::toString() const
{
	return '(' + sf::toString<T>(isZero(_data.coord.x, tolerance) ? T(0) : _data.coord.x) + ',' +
		sf::toString<T>(isZero(_data.coord.y, tolerance) ? T(0) : _data.coord.y) + ')';
}

template<typename T>
TVector2D<T>& TVector2D<T>::fromString(const std::string& s)
{
	constexpr auto sz = sizeof(data_type::array) / sizeof(T);
	std::regex re(R"(^\(([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?)\)$)", std::regex::icase);
	std::smatch match;
	// Sanity check on the amount of matches.
	if (!std::regex_match(s, match, re) || match.size() != sz + 1)
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid string '" + s + "' conversion!");
	}
	else
	{
		for (size_t i = 0; i < sz; i++)
		{
			// First match is the group so skip it (+1).
			_data.array[i] = toNumber<T>(match[i + 1].str());
		}
	}
	return *this;
}

template<typename T>
int TVector2D<T>::areOnSameSide(const TVector2D& lp1, const TVector2D& lp2, const TVector2D& p1, const TVector2D& p2)
{
	T dx = lp2._data.coord.x - lp1._data.coord.x;
	T dy = lp2._data.coord.y - lp1._data.coord.y;
	T dx1 = p1._data.coord.x - lp1._data.coord.x;
	T dy1 = p1._data.coord.y - lp1._data.coord.y;
	T dx2 = p2._data.coord.x - lp2._data.coord.x;
	T dy2 = p2._data.coord.y - lp2._data.coord.y;
	dx = (dx * dy1 - dy * dx1) * (dx * dy2 - dy * dx2);
	//
	if (std::abs(dx) < std::numeric_limits<T>::epsilon())
	{
		return 0;
	}
	else if (dx < 0)
	{
		return -1;
	}
	return 1;
}

}// namespace sf
