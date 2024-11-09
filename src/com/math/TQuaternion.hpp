#pragma once

#include <misc/gen/math.h>
#include <misc/gen/string.h>
#include <regex>

namespace sf
{

template<typename T>
TQuaternion<T>::TQuaternion(const TQuaternion& quat)
	: _data(quat._data)
{
}

template<typename T>
TQuaternion<T>::TQuaternion(T real, const TVector3D<T>& imag)
{
	_data.ir.imag(imag);
	_data.ir.real(real);
}

template<typename T>
TQuaternion<T>::TQuaternion(T imag_x, T imag_y, T imag_z, T real)
{
	_data.q.x = imag_x;
	_data.q.y = imag_y;
	_data.q.z = imag_z;
	_data.q.w = real;
}

template<typename T>
TQuaternion<T>::TQuaternion(const TVector3D<T>& axis, T angle)
{
	if (!isZero(angle))
	{
		_data.ir.real = std::cos(angle * T(0.5));
		((std::sin(angle * T(0.5))) * axis.normalized()).copyTo(_data.array);
	}
	else
	{
		_data.ir.real = T(1.0);
		_data.ir.imag = {T(0), T(0), T(0)};
	}
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::assign(const TQuaternion& quat)
{
	_data.ir.imag = quat._data.ir.imag;
	_data.ir.real = quat._data.ir.real;
	return *this;
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::operator=(const TQuaternion& quat)
{
	return assign(quat);
}

template<typename T>
T& TQuaternion<T>::operator[](size_t elem)
{
	if (elem > realR)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[elem];
}

template<typename T>
const T& TQuaternion<T>::operator[](size_t elem) const
{
	if (elem > realR)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[elem];
}

template<typename T>
T TQuaternion<T>::real() const
{
	return _data.ir.real;
}

template<typename T>
T& TQuaternion<T>::real()
{
	return _data.ir.real;
}

template<typename T>
inline TVector3D<T> TQuaternion<T>::imaginary() const
{
	return _data.ir.imag;
}

template<typename T>
inline T TQuaternion<T>::x() const
{
	return _data.ir.imag.x;
}

template<typename T>
inline T& TQuaternion<T>::x()
{
	return _data.ir.imag.x;
}

template<typename T>
inline T TQuaternion<T>::y() const
{
	return _data.ir.imag.y;
}

template<typename T>
inline T& TQuaternion<T>::y()
{
	return _data.ir.imag.y;
}

template<typename T>
inline T TQuaternion<T>::z() const
{
	return _data.ir.imag.z;
}

template<typename T>
inline T& TQuaternion<T>::z()
{
	return _data.ir.imag.z;
}

template<typename T>
bool TQuaternion<T>::isZero(T value)
{
	return std::fabs(value) < tolerance;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::conjugate()
{
	_data.ir.imag.x = -_data.ir.imag.x;
	_data.ir.imag.y = -_data.ir.imag.y;
	_data.ir.imag.z = -_data.ir.imag.z;
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::conjugated() const
{
	return TQuaternion().conjugate();
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::inverse()
{
	conjugate();
	normalize();
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::inversed() const
{
	return TQuaternion(*this).inverse();
}

template<typename T>
void TQuaternion<T>::normalize()
{
	T c = magnitude();
	if (isZero(c))
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() possible division by near zero!");
	}
	_data.q.x /= c;
	_data.q.y /= c;
	_data.q.z /= c;
	_data.q.w /= c;
}

template<typename T>
T TQuaternion<T>::magnitude() const
{
	return std::sqrt(_data.q.x * _data.q.x + _data.q.y * _data.q.y + _data.q.z * _data.q.z + _data.q.w * _data.q.w);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator-() const
{
	return {-_data.q.x, -_data.q.y, -_data.q.z, -_data.q.w};
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator+=(const TQuaternion<T>& quat)
{
	_data.q.x += quat._data.q.x;
	_data.q.y += quat._data.q.y;
	_data.q.z += quat._data.q.z;
	_data.q.w += quat._data.q.w;
	return *this;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator-=(const TQuaternion<T>& quat)
{
	_data.q.x -= quat._data.q.x;
	_data.q.y -= quat._data.q.y;
	_data.q.z -= quat._data.q.z;
	_data.q.w -= quat._data.q.w;
	return *this;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator*=(const TQuaternion<T>& q)
{
	TVector3D<T> mul{
		_data.q.w * q._data.q.x + q._data.q.w * _data.q.x + _data.q.y * q._data.q.z - _data.q.z * q._data.q.y,
		_data.q.w * q._data.q.y + q._data.q.w * _data.q.y + _data.q.z * q._data.q.x - _data.q.x * q._data.q.z,
		_data.q.w * q._data.q.z + q._data.q.w * _data.q.z + _data.q.x * q._data.q.y - _data.q.y * q._data.q.x
	};
	// Do not change the order of these lines! (w needs old value).
	_data.q.w = _data.q.w * q._data.q.w - _data.q.x * q._data.q.x - _data.q.y * q._data.q.y - _data.q.z * q._data.q.z;
	mul.copyTo(_data.array);
	return *this;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator/=(const TQuaternion<T>& quat)
{
	operator*=(quat.inverse());
	return *this;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator^=(const TQuaternion<T>& quat)
{
	assign((quat * log(*this)).exp());
	return *this;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator*=(T c)
{
	_data.q.x *= c;
	_data.q.y *= c;
	_data.q.z *= c;
	_data.q.w *= c;
	return *this;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator/=(T c)
{
	if (isZero(c))
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() possible division by near zero!");
	}
	*this = *this / c;
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator*(T c) const
{
	return {_data.q.x * c, _data.q.y * c, _data.q.z * c, _data.q.w * c};
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator/(T c) const
{
	if (isZero(c))
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() possible division by near zero!");
	}
	return {_data.q.x / c, _data.q.y / c, _data.q.z / c, _data.q.w / c};
}

template<typename T>
bool TQuaternion<T>::operator==(const TQuaternion<T>& quat) const
{
	return (
		isZero(_data.ir.real - quat._data.ir.real) &&
		isZero(_data.ir.imag.x - quat._data.ir.imag.x) &&
		isZero(_data.ir.imag.y - quat._data.ir.imag.y) &&
		isZero(_data.ir.imag.z - quat._data.ir.imag.z)
	);
}

template<typename T>
bool TQuaternion<T>::operator!=(const TQuaternion<T>& quat) const
{
	return !operator==(quat);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::squared()
{
	TVector3D<T> imag{_data.ir.imag.x, _data.ir.imag.y, _data.ir.imag.z};
	return TQuaternion<T>(_data.ir.real * _data.ir.real - imag.lengthSqr(), 2 * _data.ir.real * imag);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::exp()
{
	TQuaternion q(*this);
	auto s = q._data.ir.real;
	auto se = exp(s);
	q._data.ir.real = 0.0;
	auto scale = se;
	auto theta = q.magnitude();
	if (theta > 0.0001)
	{
		scale *= std::sin(theta) / theta;
	}
	q._data.ir.imag *= scale;
	q._data.ir.real = se * std::cos(theta);
	return q;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::log()
{
	TQuaternion q(*this);
	T sl = q.magnitude();
	q.normalize();
	T s = q._data.ir.real;
	q._data.ir.real = 0.0;
	T scale = q.magnitude();
	T theta = atan2(scale, s);
	if (scale > 0.0)
	{
		scale = theta / scale;
	}
	q._data.ir.imag *= scale;
	q._data.ir.real = log(sl);
	return q;
}

template<typename T>
TQuaternion<T> interpolate(const TQuaternion<T>& p, const TQuaternion<T>& q, T t)
{
	TQuaternion<T> qt(q), q1;
	T sp, sq;

	if ((p - q).absolute() > (p + q).absolute())
	{
		q1 = -q;
	}
	else
	{
		q1 = q;
	}

	auto c = p.real() * q1.real() + p.x() * q1.x() + p.y() * q1.y() + p.z() * q1.z();

	if (c > T(-0.9999999999))
	{
		if (c < T(0.9999999999))
		{
			auto o = acos(c);
			auto s = T(1.0) / sin(o);
			sp = sin((1.0 - t) * o) * s;
			sq = sin(t * o) * s;
		}
		else
		{
			sp = T(1.0) - t;
			sq = t;
		}
		qt._real = sp * p._real + sq * q1._real;
		qt.x() = sp * p.x() + sq * q1.x();
		qt.y() = sp * p.y() + sq * q1.y();
		qt.z() = sp * p.z() + sq * q1.z();
	}
	else
	{
		qt.real() = p.z();
		qt.x() = -p.y();
		qt.y() = p.x();
		qt.z() = -p.real();
		sp = std::sin((T(0.5) - t) * numbers::pi_v<T>);
		sq = std::sin(t * numbers::pi_v<T>);
		qt.x() = sp * p.x() + sq * qt.x();
		qt.y() = sp * p.y() + sq * qt.y();
		qt.z() = sp * p.z() + sq * qt.z();
	}
	return qt;
}

template<typename T>
void TQuaternion<T>::toMatrix(TMatrix44<T>& mtx) const
{
	T m[4][4];
	toMatrix(m);
	mtx.setElement(0, 0, m[0][0]);
	mtx.setElement(1, 0, m[1][0]);
	mtx.setElement(2, 0, m[2][0]);
	mtx.setElement(0, 1, m[0][1]);
	mtx.setElement(1, 1, m[1][1]);
	mtx.setElement(2, 1, m[2][1]);
	mtx.setElement(0, 1, m[0][2]);
	mtx.setElement(1, 1, m[1][2]);
	mtx.setElement(2, 1, m[2][2]);
}

template<typename T>
void TQuaternion<T>::toMatrix(T m[4][4]) const
{
	auto x2 = x() + x();
	auto y2 = y() + y();
	auto z2 = z() + z();
	auto wx = real() * x2;
	auto wy = real() * y2;
	auto wz = real() * z2;
	auto xx = x() * x2;
	auto xy = x() * y2;
	auto xz = x() * z2;
	auto yy = y() * y2;
	auto yz = y() * z2;
	auto zz = z() * z2;

	m[0][0] = 1.0 - yy - zz;
	m[1][0] = xy + wz;
	m[2][0] = xz - wy;
	m[0][1] = xy - wz;
	m[1][1] = 1.0 - xx - zz;
	m[2][1] = yz + wx;
	m[0][2] = xz + wy;
	m[1][2] = yz - wx;
	m[2][2] = 1.0 - xx - yy;
}

template<typename T>
TVector3D<T> TQuaternion<T>::getXAxis() const
{
	TMatrix44<T> mat;
	toMatrix(mat);
	return mat.getAxis(TMatrix44<T>::axisX);
}

template<typename T>
TVector3D<T> TQuaternion<T>::getYAxis() const
{
	TMatrix44<T> mat;
	toMatrix(mat);
	return mat.getAxis(TMatrix44<T>::axisY);
}

template<typename T>
TVector3D<T> TQuaternion<T>::getZAxis() const
{
	TMatrix44<T> mat;
	toMatrix(mat);
	return mat.getAxis(TMatrix44<T>::axisZ);
}

template<typename T>
std::string TQuaternion<T>::toString() const
{
	return '(' + sf::toString<T>(_data.q.x) + ',' + sf::toString<T>(_data.q.y) + ',' + sf::toString<T>(_data.q.z) + ',' + sf::toString<T>(_data.q.w) + ')';
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::fromString(const std::string& s) noexcept(false)
{
	constexpr auto sz = sizeof(data_type::array) / sizeof(T);
	std::regex re(R"(^\(([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?)\)$)", std::regex::icase);
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

}// namespace sf
