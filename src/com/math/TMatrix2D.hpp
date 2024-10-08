#pragma once

#include <cmath>
#include <misc/gen/dbgutils.h>
#include <misc/gen/string.h>
#include <regex>
#include <sstream>

namespace sf
{

template<typename T>
TMatrix2D<T>::TMatrix2D(std::initializer_list<T> list)
{
	if (list.size() != 4)
		throw std::invalid_argument("");
	size_t i = 0;
	for (auto el: list)
	{
		_data.array[i++] = el;
	}
}

template<typename T>
TMatrix2D<T>::TMatrix2D()
{
	resetOrientation();
}

template<typename T>
TMatrix2D<T>::TMatrix2D(const TMatrix2D& m)
{
	_data = m._data;
}

template<typename T>
TMatrix2D<T>::TMatrix2D(TMatrix2D<T>&& m)
{
	_data = m._data;
}

template<typename T>
inline TMatrix2D<T>::TMatrix2D(T factor_x, T factor_y)
{
	_data.mtx[0][0] = factor_x;
	_data.mtx[0][1] = 0.0;
	_data.mtx[1][0] = 0.0;
	_data.mtx[1][1] = factor_y;
}

template<typename T>
inline TMatrix2D<T>::TMatrix2D(T angle)
{
	setRotation(angle);
}

template<typename T>
TMatrix2D<T>::TMatrix2D(T m00, T m01, T m10, T m11)
{
	_data.mtx[0][0] = m00;
	_data.mtx[0][1] = m01;
	_data.mtx[1][0] = m10;
	_data.mtx[1][1] = m11;
}

template<typename T>
TMatrix2D<T>& TMatrix2D<T>::operator=(TMatrix2D<T>&& m) noexcept
{
	_data = m._data;
	return *this;
}

template<typename T>
TMatrix2D<T>& TMatrix2D<T>::assign(T m00, T m01, T m10, T m11)
{
	_data.array = {m00, m01, m10, m11};

	return *this;
}

template<typename T>
TMatrix2D<T>& TMatrix2D<T>::operator=(const TMatrix2D& mat)
{
	return assign(mat);
}

template<typename T>
inline const TMatrix2D<T> operator*(const TMatrix2D<T>& lhs, const TMatrix2D<T>& rhs)
{
	return TMatrix2D(lhs) *= rhs;
}

template<typename T>
constexpr TMatrix2D<T>::operator T*()
{
	return &_data.mtx[0][0];
}

template<typename T>
constexpr TMatrix2D<T>::operator const T*()
{
	return &_data.mtx[0][0];
}

template<typename T>
TMatrix2D<T>& TMatrix2D<T>::assign(const T mat[2][2])
{
	_data.mtx[0][0] = mat[0][0];
	_data.mtx[0][1] = mat[0][1];
	_data.mtx[1][0] = mat[1][0];
	_data.mtx[1][1] = mat[1][1];
	return *this;
}

template<typename T>
TMatrix2D<T> TMatrix2D<T>::transposed() const
{
	return {_data.mtx[0][0], _data.mtx[1][0], _data.mtx[0][1], _data.mtx[1][1]};
}

template<typename T>
TMatrix2D<T>& TMatrix2D<T>::operator*=(const TMatrix2D<T>& rhs)
{
	*this = {
		rhs._data.mtx[0][0] * _data.mtx[0][0] + rhs._data.mtx[0][1] * _data.mtx[1][0],
		rhs._data.mtx[0][0] * _data.mtx[0][1] + rhs._data.mtx[0][1] * _data.mtx[1][1],
		rhs._data.mtx[1][0] * _data.mtx[0][0] + rhs._data.mtx[1][1] * _data.mtx[1][0],
		rhs._data.mtx[1][0] * _data.mtx[0][1] + rhs._data.mtx[1][1] * _data.mtx[1][1]
	};
	return *this;
}

template<typename T>
inline TVector2D<T> TMatrix2D<T>::operator*(const TVector2D<T>& v) const
{
	return transformed(v);
}

template<typename T>
TVector2D<T> TMatrix2D<T>::transformed(const TVector2D<T>& v) const
{
	return {
		_data.mtx[0][0] * v.x() + _data.mtx[0][1] * v.y(),
		_data.mtx[1][0] * v.x() + _data.mtx[1][1] * v.y()
	};
}

template<typename T>
bool TMatrix2D<T>::operator==(const TMatrix2D<T>& m) const
{
	for (size_t i = 0; i < sizeof(_data.array) / sizeof(_data.array[0]); i++)
	{
		// Use the tolerance when comparing.
		if (std::fabs(_data.array[i] - m._data.array[i]) > tolerance)
		{
			return false;
		}
	}
	return true;
}

template<typename T>
bool TMatrix2D<T>::operator!=(const TMatrix2D<T>& m) const
{
	return !operator==(m);
}

template<typename T>
void TMatrix2D<T>::copyTo(T mat[2][2]) const
{
	std::memcpy(mat, _data.mtx, sizeof(_data));
}

template<typename T>
void TMatrix2D<T>::setRotation(T angle)
{
	T sx = std::sin(angle);
	T cx = std::cos(angle);
	_data.mtx[0][0] = cx;
	_data.mtx[1][0] = sx;
	_data.mtx[0][1] = -sx;
	_data.mtx[1][1] = cx;
}

template<typename T>
T TMatrix2D<T>::getRotation() const
{
	// Check if the matrix is orthogonal
	double det = _data.mtx[0][0] * _data.mtx[1][1] - _data.mtx[0][1] * _data.mtx[1][0];
	if (std::fabs(det - 1.0) > tolerance)
	{
		throw std::domain_error(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() not a square matrix!");
	}
	// Calculate the angle of rotation
	return atan2(_data.mtx[1][0], _data.mtx[0][0]);
}

template<typename T>
TMatrix2D<T>& TMatrix2D<T>::resetOrientation(void)
{
	_data.mtx[1][0] = _data.mtx[0][1] = 0.0;
	_data.mtx[0][0] = _data.mtx[1][1] = 1.0;
	return *this;
}

template<typename T>
std::string TMatrix2D<T>::toString() const
{
	return std::string() + "({" +
		sf::toString<T>(_data.mtx[0][0]) + ',' +
		sf::toString<T>(_data.mtx[0][1]) + "},{" +
		sf::toString<T>(_data.mtx[1][0]) + ',' +
		sf::toString<T>(_data.mtx[1][1]) + "})";
}

template<typename T>
TMatrix2D<T>& TMatrix2D<T>::fromString(const std::string& s) noexcept(false)
{
	std::regex re(R"(\(\{([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?)\},\{([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?)\}\))", std::regex::icase);
	std::smatch match;
	// Sanity check on the amount of matches.
	if (!std::regex_match(s, match, re) || match.size() != 5)
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid string '" + s + "' conversion!");
	}
	else
	{
		for (size_t i = 0; i <= 4; i++)
		{
			// First match is the group so skip it (+1).
			_data.array[i] = toNumber<T>(match[i + 1].str());
		}
	}
	return *this;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const TMatrix2D<T>& mtx)
{
	return os << mtx.toString();
}

template<typename T>
inline std::istream& operator>>(std::istream& is, TMatrix2D<T>& mtx) noexcept(false)
{
	std::string s;
	auto delimiter = ')';
	std::getline(is, s, delimiter);
	mtx.fromString(s.append(1, delimiter));
	return is;
}

}// namespace sf