#pragma once

#include "TVector3.h"
#include <complex>
#include <istream>
#include <misc/gen/dbgutils.h>

namespace sf
{

#pragma option push pack(1)

/**
 * @brief Generic type Quaternion
 * @tparam T
 * @tparam T2
 */
template<class T, class T2>
class Quaterion
{
	public:
		typedef T value_type;
		/**
		 * @brief The quaterion data imaginary part.
		 */
		TVector3<T, T2> v;
		/**
		 * @brief The quaterion data real part.
		 */
		T w;
		/**
		 * @brief Default Constructor
		 */
		Quaterion();
		/**
		 * @brief Copy Constructor
		 */
		Quaterion(const Quaterion<T, T2>&);
		/**
		 * @brief
		 */
		// This constructor is used in operators and sets the member variables
		// directly
		Quaterion(T qw, const TVector3<T, T2>& qv);
		/**
		 * @brief This constructor is useful for creating rotations by specifying the
		 * angle of rotation and the axis around which you want to rotate, this axis does not have to be normalized
		 */
		Quaterion(const TVector3<T, T2>& axis, T angle);
		/**
		 * @brief 4 Element Constructor.
		 */
		Quaterion(T x, T y, T z, T w);
		/**
		 * @brief Array access operator.
		 */
		T& operator[](int);
		/**
		 * @brief Array access const operator.
		 */
		const T& operator[](int) const;

		// Assignment
		/**
		 * @brief
		 */
		Quaterion& operator=(const Quaterion& q);

		/**
		 * @brief
		 */
		T Real() const;
		/**
		 * @brief
		 */
		T X() const;
		/**
		 * @brief
		 */
		T Y() const;
		/**
		 * @brief
		 */
		T Z() const;

		/**
		 * @brief
		 */
		bool IsZero(T2 f);

		/**
		 * @brief
		 */
		TVector3<T, T2> Imaginary() const;
		/**
		 * @brief
		 */
		double Abs() const;
		/**
		 * @brief
		 */
		void Conj();
		/**
		 * @brief
		 */
		Quaterion<T, T2> Inverse() const;
		/**
		 * @brief
		 */
		void Normalize();
		/**
		 * @brief
		 */
		T Magnitude() const;
		/**
		 * @brief
		 */
		void InsertInMatrix(T[4][4]) const;
		//
		/**
		 * @brief
		 */
		TVector3<T, T2> GetXAxis() const;
		/**
		 * @brief
		 */
		TVector3<T, T2> GetYAxis() const;
		/**
		 * @brief
		 */
		TVector3<T, T2> GetZAxis() const;
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2> operator-() const;
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2>& operator+=(const Quaterion<T, T2>& q);
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2>& operator-=(const Quaterion<T, T2>& q);
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2>& operator*=(const Quaterion<T, T2>& q);
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2>& operator/=(const Quaterion<T, T2>& q);
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2>& operator^=(const Quaterion<T, T2>& q);
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2>& operator*=(T c);
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2>& operator/=(T c);
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2> operator*(T) const;
		/**
		 * @brief operator.
		 */
		Quaterion<T, T2> operator/(T) const;
		/**
		 * @brief operator.
		 */
		bool operator==(const Quaterion<T, T2>& q) const;
		/**
		 * @brief operator.
		 */
		bool operator!=(const Quaterion<T, T2>& q) const;

		/**
		 * @brief Writes content to an output stream.
		 */
		std::ostream& Write(std::ostream& os) const;

		// Friend functions.
		friend Quaterion<T, T2> operator*(T d, const Quaterion<T, T2>& q1);
		friend Quaterion<T, T2> Square(const Quaterion<T, T2>&);
		friend Quaterion<T, T2> Exp(const Quaterion<T, T2>&);
		friend Quaterion<T, T2> Log(const Quaterion<T, T2>&);
		// Spherical interpolation between 2 quaternions
		friend Quaterion<T, T2> Interpolate(const Quaterion<T, T2>&, const Quaterion<T, T2>&, double t);
};
// Restore packing
#pragma option pop

template<class T, class T2>
const Quaterion<T, T2> operator+(const Quaterion<T, T2>& lhs, const Quaterion<T, T2>& rhs)
{
	return Quaterion(lhs) += rhs;
}

template<class T, class T2>
const Quaterion<T, T2> operator-(const Quaterion<T, T2>& lhs, const Quaterion<T, T2>& rhs)
{
	return Quaterion(lhs) -= rhs;
}

template<class T, class T2>
const Quaterion<T, T2> operator*(const Quaterion<T, T2>& lhs, const Quaterion<T, T2>& rhs)
{
	return Quaterion<T, T2>(lhs) *= rhs;
}

template<class T, class T2>
const Quaterion<T, T2> operator/(const Quaterion<T, T2>& lhs, const Quaterion<T, T2>& rhs)
{
	return Quaterion<T, T2>(lhs) /= rhs;
}

template<class T, class T2>
const Quaterion<T, T2> operator^(const Quaterion<T, T2>& lhs, const Quaterion<T, T2>& rhs)
{
	return Quaterion<T, T2>(lhs) ^= rhs;
}

template<class T, class T2>
bool Quaterion<T, T2>::IsZero(T2 f)
{
	T epsilon = 0.00000000001;
	return (f < epsilon && f > -epsilon);
}

template<class T, class T2>
Quaterion<T, T2>::Quaterion()
	: v(Vector3())
	, w(1)
{
}

template<class T, class T2>
Quaterion<T, T2>::Quaterion(const Quaterion& q)
	: v(q.v)
	, w(q.w)
{
}

// This constructor is used in operators and sets the member variables directly.
template<class T, class T2>
Quaterion<T, T2>::Quaterion(T qw, const TVector3<T, T2>& qv)
	: v(qv)
	, w(qw)
{
}

// This constructor is useful for creating rotations by specifying the
// angle of rotation and the axis around which you want to rotate,
// this axis does not have to be normalized
template<class T, class T2>
Quaterion<T, T2>::Quaterion(const TVector3<T, T2>& axis, T angle)
{
	if (!IsZero((T2) angle))
	{
		w = cos(0.5 * angle);
		v = ((T) -sin(0.5 * angle)) * axis;
	}
	else
	{
		w = 1.0;
		v = Vector3(0.0, 0.0, 0.0);
	}
}

template<class T, class T2>
Quaterion<T, T2>::Quaterion(T qx, T qy, T qz, T qw)
	: v(qx, qy, qz)
	, w(qw)
{
}

template<class T, class T2>
T& Quaterion<T, T2>::operator[](int i)
{
	if (i < 0 || i > 3)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	switch (i)
	{
		case 0:
			return v.x;
		case 1:
			return v.y;
		case 2:
			return v.z;
		case 3:
			return w;
	}
	return w;
}

template<class T, class T2>
const T& Quaterion<T, T2>::operator[](int i) const
{
	if (i < 0 || i > 3)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	switch (i)
	{
		case 0:
			return v.x;
		case 1:
			return v.y;
		case 2:
			return v.z;
		case 3:
			return w;
	}
	return w;
}

template<class T, class T2>
T Quaterion<T, T2>::Real() const
{
	return w;
}

template<class T, class T2>
TVector3<T, T2> Quaterion<T, T2>::Imaginary() const
{
	return v;
}
template<class T, class T2>
T Quaterion<T, T2>::X() const
{
	return v.x;
}

template<class T, class T2>
T Quaterion<T, T2>::Y() const
{
	return v.y;
}

template<class T, class T2>
T Quaterion<T, T2>::Z() const
{
	return v.z;
}

template<class T, class T2>
double Quaterion<T, T2>::Abs() const
{
	return std::sqrt(Magnitude());
}

template<class T, class T2>
void Quaterion<T, T2>::Conj()
{
	v = -v;
}

template<class T, class T2>
Quaterion<T, T2> Quaterion<T, T2>::Inverse() const
{
	Quaterion q = *this;
	q.Conj();
	q.Normalize();
	return q;
}

template<class T, class T2>
void Quaterion<T, T2>::Normalize()
{
	T c = Abs();
	if (IsZero(c))
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() possible division by near zero!");
	}
	w /= c;
	v /= c;
}

template<class T, class T2>
T Quaterion<T, T2>::Magnitude(void) const
{
	return v.LengthSqr() + w * w;
}

template<class T, class T2>
Quaterion<T, T2> Quaterion<T, T2>::operator-() const
{
	return Quaterion(-w, -v);
}

template<class T, class T2>
Quaterion<T, T2>& Quaterion<T, T2>::operator+=(const Quaterion<T, T2>& q)
{
	w += q.w;
	v += q.v;
	return *this;
}

template<class T, class T2>
Quaterion<T, T2>& Quaterion<T, T2>::operator-=(const Quaterion<T, T2>& q)
{
	w -= q.w;
	v -= q.v;
	return *this;
}

template<class T, class T2>
Quaterion<T, T2>& Quaterion<T, T2>::operator*=(const Quaterion<T, T2>& q)
{
	Vector3 mul(
		w * q.X() + q.w * X() + Y() * q.Z() - Z() * q.Y(),
		w * q.Y() + q.w * Y() + Z() * q.X() - X() * q.Z(),
		w * q.Z() + q.w * Z() + X() * q.Y() - Y() * q.X()
	);
	w = w * q.w - X() * q.X() - Y() * q.Y() - Z() * q.Z();
	v = mul;// do not change the order of these lines! (w needs old v)
	return *this;
}

template<class T, class T2>
Quaterion<T, T2>& Quaterion<T, T2>::operator/=(const Quaterion<T, T2>& q)
{
	*this *= q.Inverse();
	return *this;
}

template<class T, class T2>
Quaterion<T, T2>& Quaterion<T, T2>::operator^=(const Quaterion<T, T2>& q)
{
	*this = Exp(q * Log(*this));
	return *this;
}

template<class T, class T2>
Quaterion<T, T2>& Quaterion<T, T2>::operator*=(T c)
{
	w *= c;
	v *= c;
	return *this;
}

template<class T, class T2>
Quaterion<T, T2>& Quaterion<T, T2>::operator/=(T c)
{
	if (IsZero(c))
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() possible division by near zero!");
	}
	*this = *this / c;
	return *this;
}

template<class T, class T2>
Quaterion<T, T2> Quaterion<T, T2>::operator*(T c) const
{
	return Quaterion<T, T2>(c * w, c * v);
}

template<class T, class T2>
Quaterion<T, T2> Quaterion<T, T2>::operator/(T c) const
{
	if (IsZero(c))
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() possible division by near zero!");
	}
	return Quaterion<T, T2>(w / c, v / c);
}

template<class T, class T2>
bool Quaterion<T, T2>::operator==(const Quaterion<T, T2>& q) const
{
	return ((w == q.w) && (v == q.v));
}

template<class T, class T2>
bool Quaterion<T, T2>::operator!=(const Quaterion<T, T2>& q) const
{
	return (!(*this == q));
}

template<class T, class T2>
Quaterion<T, T2> operator*(T c, const Quaterion<T, T2>& q)
{
	return Quaterion<T, T2>(c * q.w, c * q.v);
}

template<class T, class T2>
Quaterion<T, T2> Square(const Quaterion<T, T2>& q)
{
	return Quaterion<T, T2>(q.w * q.w - q.v.LengthSqr(), 2 * q.w * q.v);
}

template<class T, class T2>
Quaterion<T, T2> Exp(const Quaterion<T, T2>& q1)
{
	Quaterion q = q1;

	T s = q.w;
	T se = exp(s);
	q.w = 0.0;
	T scale = se;
	T theta;
	if ((theta = q.Abs()) > 0.0001)
		scale *= sin(theta) / theta;
	q.v *= scale;
	q.w = se * cos(theta);
	return q;
}

template<class T, class T2>
Quaterion<T, T2> Log(const Quaterion<T, T2>& q1)
{
	Quaterion<T, T2> q = q1;

	T sl = q.Abs();
	q.Normalize();
	T s = q.w;
	q.w = 0.0;
	T scale = q.Abs();
	T theta = atan2(scale, s);
	if (scale > 0.0)
		scale = theta / scale;
	q.v *= scale;
	q.w = log(sl);
	return q;
}

template<class T, class T2>
Quaterion<T, T2> Interpolate(
	const Quaterion<T, T2>& p,
	const Quaterion<T, T2>& q,
	double t
)
{
	Quaterion<T, T2> qt, q1;
	double sp, sq;

	if ((p - q).Abs() > (p + q).Abs())
		q1 = -q;
	else
		q1 = q;

	double c = (double) (p.w * q1.w + p.v[0] * q1.v[0] + p.v[1] * q1.v[1] + p.v[2] * q1.v[2]);

	if (c > -0.9999999999)
	{
		if (c < 0.9999999999)
		{
			double o = acos(c);
			double s = 1.0 / sin(o);
			sp = sin((1.0 - t) * o) * s;
			sq = sin(t * o) * s;
		}
		else
		{
			sp = 1.0 - t;
			sq = t;
		}
		qt.w = sp * p.w + sq * q1.w;
		qt.v[0] = sp * p.v[0] + sq * q1.v[0];
		qt.v[1] = sp * p.v[1] + sq * q1.v[1];
		qt.v[2] = sp * p.v[2] + sq * q1.v[2];
	}
	else
	{
		qt.w = p.v[2];
		qt.v[0] = -p.v[1];
		qt.v[1] = p.v[0];
		qt.v[2] = -p.w;
		sp = sin((0.5 - t) * M_PI);
		sq = sin(t * M_PI);
		qt.v[0] = sp * p.v[0] + sq * qt.v[0];
		qt.v[1] = sp * p.v[1] + sq * qt.v[1];
		qt.v[2] = sp * p.v[2] + sq * qt.v[2];
	}
	return qt;
}

template<class T, class T2>
void Quaterion<T, T2>::InsertInMatrix(T m[4][4]) const
{
	T x2 = X() + X();
	T y2 = Y() + Y();
	T z2 = Z() + Z();
	T wx = w * x2;
	T wy = w * y2;
	T wz = w * z2;
	T xx = X() * x2;
	T xy = X() * y2;
	T xz = X() * z2;
	T yy = Y() * y2;
	T yz = Y() * z2;
	T zz = Z() * z2;
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

// TODO: could be optimized to forget about non used axis values
template<class T, class T2>
TVector3<T, T2> Quaterion<T, T2>::GetXAxis() const
{
	Matrix44 mat;
	InsertInMatrix(mat._data_mtx);
	return mat.GetAxis(0);
}

// TODO: could be optimized to forget about non used axis values
template<class T, class T2>
TVector3<T, T2> Quaterion<T, T2>::GetYAxis() const
{
	Matrix44 mat;
	InsertInMatrix(mat._data_mtx);
	return mat.GetAxis(1);
}

// TODO: could be optimized to forget about non used axis values
template<class T, class T2>
TVector3<T, T2> Quaterion<T, T2>::GetZAxis() const
{
	Matrix44 mat;
	InsertInMatrix(mat._data_mtx);
	return mat.GetAxis(2);
}

template<class T, class T2>
std::ostream& Quaterion<T, T2>::Write(std::ostream& os) const
{
	os << "QUAT with w = " << w << " and vector is: ";
	return v.Write(os);
}

}// namespace sf
