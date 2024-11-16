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
	_data.q.w = real;
	_data.q.x = imag.x();
	_data.q.y = imag.y();
	_data.q.z = imag.z();
}

template<typename T>
TQuaternion<T>::TQuaternion(T real_w, T imag_x, T imag_y, T imag_z)
{
	_data.q.w = real_w;
	_data.q.x = imag_x;
	_data.q.y = imag_y;
	_data.q.z = imag_z;
}

template<typename T>
TQuaternion<T>::TQuaternion(const TVector3D<T>& axis, T angle)
{
	if (!isZero(angle))
	{
		_data.q.w = std::cos(angle * T(0.5));
		((std::sin(angle * T(0.5))) * axis.normalized()).copyTo(&_data.array[imagX]);
	}
	else
	{
		_data.q = {T(0), T(0), T(0), T(1.0)};
	}
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::assign(T real_w, T imag_x, T imag_y, T imag_z)
{
	_data = {real_w, imag_x, imag_y, imag_z};
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::assign(const TQuaternion& quat)
{
	_data.ir.imag = quat._data.ir.imag;
	_data.q.w = quat._data.q.w;
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
	if (elem > imagZ)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[elem];
}

template<typename T>
const T& TQuaternion<T>::operator[](size_t elem) const
{
	if (elem > imagZ)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[elem];
}

template<typename T>
T TQuaternion<T>::real() const
{
	return _data.q.w;
}

template<typename T>
T& TQuaternion<T>::real()
{
	return _data.q.w;
}

template<typename T>
inline TVector3D<T> TQuaternion<T>::imaginary() const
{
	return _data.ir.imag;
}

template<typename T>
inline T TQuaternion<T>::w() const
{
	return _data.q.w;
}

template<typename T>
inline T& TQuaternion<T>::w()
{
	return _data.q.w;
}

template<typename T>
inline T TQuaternion<T>::x() const
{
	return _data.q.x;
}

template<typename T>
inline T& TQuaternion<T>::x()
{
	return _data.q.x;
}

template<typename T>
inline T TQuaternion<T>::y() const
{
	return _data.q.y;
}

template<typename T>
inline T& TQuaternion<T>::y()
{
	return _data.q.y;
}

template<typename T>
inline T TQuaternion<T>::z() const
{
	return _data.q.z;
}

template<typename T>
inline T& TQuaternion<T>::z()
{
	return _data.q.z;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::conjugate() const
{
	return {_data.q.w, -_data.q.x, -_data.q.y, -_data.q.z};
}

template<typename T>
TQuaternion<T> TQuaternion<T>::inverse() const
{
	// Step 1: Compute the Conjugate of the Quaternion.
	// Step 2: Compute the Norm (Magnitude) of the Quaternion.
	// Step 3: Divide the Conjugate by the Norm Squared
	// Simplified where magnitudeSqr() from this instance is the same as the conjugate one.
	return conjugate() / magnitudeSqr();
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::normalize()
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
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::normalized() const
{
	return TQuaternion(*this).normalize();
}

template<typename T>
T TQuaternion<T>::magnitudeSqr() const
{
	return _data.q.x * _data.q.x +
		_data.q.y * _data.q.y +
		_data.q.z * _data.q.z +
		_data.q.w * _data.q.w;
}

template<typename T>
inline T TQuaternion<T>::magnitude() const
{
	return std::sqrt(magnitudeSqr());
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator-() const
{
	return {-_data.q.w, -_data.q.x, -_data.q.y, -_data.q.z};
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator+=(const TQuaternion<T>& quat)
{
	_data.q.w += quat._data.q.w;
	_data.q.x += quat._data.q.x;
	_data.q.y += quat._data.q.y;
	_data.q.z += quat._data.q.z;
	return *this;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator-=(const TQuaternion<T>& quat)
{
	_data.q.w -= quat._data.q.w;
	_data.q.x -= quat._data.q.x;
	_data.q.y -= quat._data.q.y;
	_data.q.z -= quat._data.q.z;
	return *this;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator*=(const TQuaternion<T>& q)
{
	auto x = _data.q.w * q._data.q.x + q._data.q.w * _data.q.x + _data.q.y * q._data.q.z - _data.q.z * q._data.q.y;
	auto y = _data.q.w * q._data.q.y + q._data.q.w * _data.q.y + _data.q.z * q._data.q.x - _data.q.x * q._data.q.z;
	auto z = _data.q.w * q._data.q.z + q._data.q.w * _data.q.z + _data.q.x * q._data.q.y - _data.q.y * q._data.q.x;
	// Do not change the order of these lines! (w needs old value).
	_data.q.w = _data.q.w * q._data.q.w - _data.q.x * q._data.q.x - _data.q.y * q._data.q.y - _data.q.z * q._data.q.z;
	_data.q.x = x;
	_data.q.y = y;
	_data.q.z = z;
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
	return assign((quat * log()).exp());
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
	_data.q.x /= c;
	_data.q.y /= c;
	_data.q.z /= c;
	_data.q.w /= c;
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator*(T c) const
{
	return {_data.q.w * c, _data.q.x * c, _data.q.y * c, _data.q.z * c};
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator/(T c) const
{
	if (isZero(c))
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() possible division by near zero!");
	}
	return {_data.q.w / c, _data.q.x / c, _data.q.y / c, _data.q.z / c};
}

template<typename T>
bool TQuaternion<T>::isEqual(const TQuaternion& quat, T tol) const
{
	return sf::isEqual<T>(_data.q.w, quat._data.q.w, tol) &&
		sf::isEqual<T>(_data.q.x, quat._data.q.x, tol) &&
		sf::isEqual<T>(_data.q.y, quat._data.q.y, tol) &&
		sf::isEqual<T>(_data.q.z, quat._data.q.z, tol);
}

template<typename T>
inline bool TQuaternion<T>::operator==(const TQuaternion<T>& quat) const
{
	return isEqual(quat);
}

template<typename T>
inline bool TQuaternion<T>::operator!=(const TQuaternion<T>& quat) const
{
	return !isEqual(quat);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::squared() const
{
	TVector3D<T> imag{_data.q.x, _data.q.y, _data.q.z};
	return TQuaternion<T>(_data.q.w * _data.q.w - imag.lengthSqr(), T(2.0) * _data.q.w * imag);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::exp() const
{
	TQuaternion q(*this);
	// Save the real part to scale back afterward.
	auto se = std::exp(q._data.q.w);
	auto scale = se;
	// Zero the real part for the magnitude calculation.
	q._data.q.w = 0.0;
	auto theta = q.magnitude();
	// Check if theta is near zero.
	if (!isZero(theta, tolerance))
	{
		scale *= std::sin(theta) / theta;
	}
	q._data.q.w = se * std::cos(theta);
	q._data.q.x *= scale;
	q._data.q.y *= scale;
	q._data.q.z *= scale;
	return q;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::log() const
{
	TQuaternion q(*this);
	T sl = q.magnitude();
	q.normalize();
	T s = q._data.q.w;
	q._data.q.w = 0.0;
	T scale = q.magnitude();
	T theta = atan2(scale, s);
	if (!isZero(scale, tolerance))
	{
		scale = theta / scale;
	}
	q._data.q.w = std::log(sl);
	q._data.q.x *= scale;
	q._data.q.y *= scale;
	q._data.q.z *= scale;
	return q;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::interpolateLogarithmic(const TQuaternion<T>& q2, T t) const
{
	return (t * (q2 * conjugate()).log()).exp() * (*this);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::interpolate(const TQuaternion<T>& q, T t) const
{
	const TQuaternion& p(*this);
	TQuaternion<T> qt(q), q1;
	T sp, sq;
	if ((p - q).magnitude() > (p + q).magnitude())
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
			auto o = std::acos(c);
			auto s = T(1.0) / std::sin(o);
			sp = std::sin((1.0 - t) * o) * s;
			sq = std::sin(t * o) * s;
		}
		else
		{
			sp = T(1.0) - t;
			sq = t;
		}
		qt.real() = sp * p.real() + sq * q1.real();
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
TMatrix44<T> TQuaternion<T>::toMatrix(TMatrix44<T>& mtx) const
{
	auto x2 = _data.q.x * _data.q.x;
	auto y2 = _data.q.y * _data.q.y;
	auto z2 = _data.q.z * _data.q.z;
	auto wx = _data.q.w * _data.q.x;
	auto wy = _data.q.w * _data.q.y;
	auto wz = _data.q.w * _data.q.z;
	auto xy = _data.q.x * _data.q.y;
	auto xz = _data.q.x * _data.q.z;
	auto yz = _data.q.y * _data.q.z;

	mtx.setElement(0, 0, T(1.0) - T(2.0) * (y2 + z2));
	mtx.setElement(0, 1, T(2.0) * (xy - wz));
	mtx.setElement(0, 2, T(2.0) * (xz + wy));

	mtx.setElement(1, 0, T(2.0) * (xy + wz));
	mtx.setElement(1, 1, T(1.0) - T(2.0) * (x2 + z2));
	mtx.setElement(1, 2, T(2.0) * (yz - wx));

	mtx.setElement(2, 0, T(2.0) * (xz - wy));
	mtx.setElement(2, 1, T(2.0) * (yz + wx));
	mtx.setElement(2, 2, T(1.0) - T(2.0) * (x2 + y2));
	return mtx;
}

template<typename T>
inline TMatrix44<T> TQuaternion<T>::toMatrix() const
{
	TMatrix44<T> mtx;
	return normalized().toMatrix(mtx);
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::fromMatrix(const T mtx[4][4])
{
	// Check if the matrix passed is a rotation matrix.
	if (!TMatrix44<T>(mtx).isRotational())
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() matrix is not rotational!");
	}
	// Calculate the Trace of the matrix where trace(R)=R11+R22+R33 .
	auto trace = mtx[0][0] + mtx[1][1] + mtx[2][2];
	// Based on the Trace when the Trace is > 0.
	if (trace > 0)
	{
		auto s = T(2) * (trace + 1);
		_data.q.w = s / 4;
		_data.q.x = (mtx[2][1] - mtx[1][2]) / s;
		_data.q.y = (mtx[0][2] - mtx[2][0]) / s;
		_data.q.z = (mtx[1][0] - mtx[0][1]) / s;
	}
	else
	{
		auto index = sf::maxArgumentIndex(mtx[0][0], mtx[1][1], mtx[2][2]);
		// Value of mtx[0][0] is the largest.
		if (index == 0)
		{
			auto s = T(2) * std::sqrt(T(1) + mtx[0][0] - mtx[1][1] - mtx[2][2]);
			_data.q.w = (mtx[2][1] - mtx[2][1]) / s;
			_data.q.x = s / 4;
			_data.q.y = (mtx[0][1] + mtx[1][0]) / s;
			_data.q.z = (mtx[0][2] + mtx[2][0]) / s;
		}
		// Value of mtx[1][1] is the largest.
		else if (index == 1)
		{
			auto s = T(2) * std::sqrt(T(1) + mtx[1][1] - mtx[0][0] - mtx[2][2]);
			_data.q.w = (mtx[0][2] - mtx[2][0]) / s;
			_data.q.x = (mtx[0][1] + mtx[1][0]) / s;
			_data.q.y = s / 4;
			_data.q.z = (mtx[1][2] + mtx[2][1]) / s;
		}
		// Value of mtx[2][2] is the largest.
		else
		{
			auto s = T(2) * std::sqrt(T(1) + mtx[2][2] - mtx[0][0] - mtx[1][1]);
			_data.q.w = (mtx[1][0] - mtx[0][1]) / s;
			_data.q.x = (mtx[0][2] + mtx[2][0]) / s;
			_data.q.y = (mtx[1][2] + mtx[2][1]) / s;
			_data.q.z = s / 4;
		}
	}
	return *this;
}

template<typename T>
TVector3D<T> TQuaternion<T>::transform(TVector3D<T> v) const
{
	auto x2 = _data.q.x + _data.q.x;
	auto y2 = _data.q.y + _data.q.y;
	auto z2 = _data.q.z + _data.q.z;
	auto wx2 = _data.q.w * x2;
	auto wy2 = _data.q.w * y2;
	auto wz2 = _data.q.w * z2;
	auto xx2 = _data.q.x * x2;
	auto xy2 = _data.q.x * y2;
	auto xz2 = _data.q.x * z2;
	auto yy2 = _data.q.y * y2;
	auto yz2 = _data.q.y * z2;
	auto zz2 = _data.q.z * z2;
	auto xp = ((v.x() * ((T(1) - yy2) - zz2)) + (v.y() * (xy2 - wz2))) + (v.z() * (xz2 + wy2));
	auto yp = ((v.x() * (xy2 + wz2)) + (v.y() * ((T(1) - xx2) - zz2))) + (v.z() * (yz2 - wx2));
	auto zp = ((v.x() * (xz2 - wy2)) + (v.y() * (yz2 + wx2))) + (v.z() * ((T(1) - xx2) - yy2));
	return {xp, yp, zp};
}

template<typename T>
std::string TQuaternion<T>::toString() const
{
	return '(' + sf::toString<T>(isZero(_data.q.w, tolerance) ? T(0) : _data.q.w) + ',' +
		sf::toString<T>(isZero(_data.q.x, tolerance) ? T(0) : _data.q.x) + ',' +
		sf::toString<T>(isZero(_data.q.y, tolerance) ? T(0) : _data.q.y) + ',' +
		sf::toString<T>(isZero(_data.q.z, tolerance) ? T(0) : _data.q.z) + ')';
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::fromString(const std::string& s) noexcept(false)
{
	constexpr auto sz = sizeof(data_type::array) / sizeof(T);
	auto res = R"(^\(([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?)\)$)";
	std::regex re(res, std::regex::icase);
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
