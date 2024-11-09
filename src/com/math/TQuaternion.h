#pragma once

#include "TVector3D.h"
#include <complex>
#include <istream>
#include <misc/gen/dbgutils.h>

namespace sf
{

#pragma option push pack(1)

/**
 * @brief Generic type Quaternion
 */
template<typename T>
class TQuaternion
{
	public:
		/**
		 * @brief Type declaration of the coordinate storage values.
		 */
		typedef T value_type;

		/**
		 * @brief Default Constructor
		 */
		TQuaternion() = default;

		/**
		 * @brief Copy Constructor
		 */
		TQuaternion(const TQuaternion&);

		/**
		 * @brief Initializing constructor is used in operators and sets the member variables directly.
		 */
		TQuaternion(T qw, const TVector3D<T>& qv);

		/**
		 * @brief Constructor for creating rotations by specifying the angle of rotation and the axis
		 * around which to rotate, this axis does not have to be normalized.
		 */
		TQuaternion(const TVector3D<T>& axis, T angle);

		/**
		 * @brief Initializing constructor for 4 element parts.
		 */
		TQuaternion(T imag_x, T imag_y, T imag_z, T real);

		/**
		 * @brief Assigns of another instance elements.
		 */
		TQuaternion& assign(const TQuaternion&);

		/**
		 * @brief Assignment operator.
		 */
		TQuaternion& operator=(const TQuaternion&);

		/**
		 * @brief Enumerate for identifying a matrix axis.
		 */
		enum EElement : size_t
		{
			/** @brief X imaginary of the quaternion. */
			imagX = 0,
			/** @brief Y-axis of the matrix. */
			imagY = 1,
			/** @brief Z-axis of the matrix. */
			imagZ = 2,
			/** @brief Translation axis of the matrix. */
			realR = 3
		};

		/**
		 * @brief Array access operator.
		 */
		T& operator[](size_t);

		/**
		 * @brief Array access const operator to the elements.
		 */
		const T& operator[](size_t) const;

		/**
		 * @brief Gets the real part of this instance.
		 */
		T real() const;

		/**
		 * @brief Gets the real part reference of this instance.
		 */
		T& real();

		/**
		 * @brief Get the imaginary X-part of this instance.
		 */
		T x() const;

		/**
		 * @brief Get the imaginary X-part reference of this instance.
		 */
		T& x();

		T y() const;

		/**
		 * @brief Get the imaginary Y-part reference of this instance.
		 */
		T& y();

		/**
		 * @brief Get the imaginary Z-part of this instance.
		 */
		T z() const;

		/**
		 * @brief Get the imaginary Z-part reference of this instance.
		 */
		T& z();

		/**
		 * @brief Check if the passed value is zero or near zero according the #tolerance.
		 */
		bool isZero(T);

		/**
		 * @brief Get the imaginary part as a 3D-vector.
		 */
		TVector3D<T> imaginary() const;

		/**
		 * @brief Inverts (or negates) the imaginary (vector) part of a quaternion.
		 */
		TQuaternion& conjugate();

		/**
		 * @brief Gets a copy of this instance with inverted (or negated) the imaginary (vector) part of a quaternion.
		 */
		TQuaternion conjugated() const;

		/**
		 * @brief Inverse the imaginary part of this instance.
		 */
		TQuaternion& inverse();

		/**
		 * @brief Gets an inversed copy of this instance.
		 */
		TQuaternion inversed() const;

		/**
		 * @brief
		 */
		void normalize();

		/**
		 * @brief Gets the magnitude of this quaternion.
		 * The magnitude (or norm) of a quaternion q=a+bi+cj+dk, where a, b, c, and d are real numbers,
		 * is calculated similarly to the magnitude of a 4-dimensional vector.
		 * All element values squared and added together.
		 */
		T magnitude() const;

		/**
		 * @brief Sets the orientation part of the passed matrix value array.
		 */
		void toMatrix(T[4][4]) const;

		/**
		 * @brief Sets the orientation part of the passed TMatrix instance.
		 */
		void toMatrix(TMatrix44<T>&) const;

		/**
		 * @brief
		 */
		TVector3D<T> getXAxis() const;

		/**
		 * @brief
		 */
		TVector3D<T> getYAxis() const;

		/**
		 * @brief
		 */
		TVector3D<T> getZAxis() const;

		/**
		 * @brief operator.
		 */
		TQuaternion operator-() const;

		/**
		 * @brief operator.
		 */
		TQuaternion& operator+=(const TQuaternion&);

		/**
		 * @brief operator.
		 */
		TQuaternion& operator-=(const TQuaternion&);

		/**
		 * @brief operator.
		 */
		TQuaternion& operator*=(const TQuaternion&);

		/**
		 * @brief operator.
		 */
		TQuaternion& operator/=(const TQuaternion&);

		/**
		 * @brief operator.
		 */
		TQuaternion& operator^=(const TQuaternion&);

		/**
		 * @brief operator.
		 */
		TQuaternion& operator*=(T c);

		/**
		 * @brief operator.
		 */
		TQuaternion& operator/=(T c);

		/**
		 * @brief operator.
		 */
		TQuaternion operator*(T) const;

		/**
		 * @brief operator.
		 */
		TQuaternion operator/(T) const;

		/**
		 * @brief operator.
		 */
		bool operator==(const TQuaternion&) const;

		/**
		 * @brief operator.
		 */
		bool operator!=(const TQuaternion&) const;

		TQuaternion squared();

		TQuaternion exp();

		TQuaternion log();

		/**
		 * @brief Gets the string representation of the quaternion formed like '(x,y,z,r)'.
		 */
		std::string toString() const;

		/**
		 * @brief Gets the quaternion values from the string representation formed like '(1,2,3,4)'.
		 */
		TQuaternion& fromString(const std::string& s) noexcept(false);

		/**
		 * @brief Tolerance for when comparing in the equal operator.
		 *
		 * Used as: std::fabs(x1 - x2) < tolerance
		 */
		static constexpr auto tolerance = TVector3D<T>::tolerance;

	protected:
		/**
		 * @brief Storage union of the 3D coordinate making the x,y,z,w accessible as an array.
		 */
		union data_type
		{
				T array[4];
				struct imag_real_type
				{
						struct point_type
						{
								T x;
								T y;
								T z;
						} imag;
						T real;
				} ir;
				struct quat_type
				{
						T x;
						T y;
						T z;
						T w;
				} q;
		} _data{0, 0, 0, 1.0};
};

template<typename T>
inline const TQuaternion<T> operator+(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return Quaterion(lhs) += rhs;
}

template<typename T>
inline const TQuaternion<T> operator-(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return Quaterion(lhs) -= rhs;
}

template<typename T>
inline const TQuaternion<T> operator*(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return TQuaternion<T>(lhs) *= rhs;
}

template<typename T>
inline const TQuaternion<T> operator/(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return TQuaternion<T>(lhs) /= rhs;
}

template<typename T>
inline const TQuaternion<T> operator^(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return TQuaternion<T>(lhs) ^= rhs;
}

template<typename T>
TQuaternion<T> operator*(T c, const TQuaternion<T>& quat)
{
	return TQuaternion<T>(c * quat.real(), c * quat.imag());
}

/**
 * @brief Operator for writing the quaternion to an output-stream.
 * @tparam T Base floating point type.
 * @param os Output stream.
 * @param mtx Matrix to stream out.
 * @return The passed output stream.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const TQuaternion<T>& quat)
{
	return os << quat.toString();
}

/**
 * @brief Operator for reading the quaternion from an input stream.
 * @tparam T Base floating point type.
 * @param os Input stream.
 * @param mtx Matrix to stream.
 * @return The passed input stream.
 */
template<typename T>
inline std::istream& operator>>(std::istream& is, TQuaternion<T>& quat) noexcept(false)
{
	std::string s;
	auto delimiter = ')';
	std::getline(is, s, delimiter);
	quat.fromString(s.append(1, delimiter));
	return is;
}

}// namespace sf

#include "TQuaternion.hpp"