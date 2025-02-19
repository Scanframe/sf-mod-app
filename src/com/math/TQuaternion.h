#pragma once
#include <math/TVector3D.h>

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
		TQuaternion(T real, const TVector3D<T>& imag);

		/**
		 * @brief Constructor for creating rotations by specifying the angle of rotation and the axis
		 * around which to rotate, this axis does not have to be normalized.
		 */
		TQuaternion(const TVector3D<T>& axis, T angle);

		/**
		 * @brief Initializing constructor for 4 element parts.
		 */
		TQuaternion(T real_w, T imag_x, T imag_y, T imag_z);

		/**
		 * @brief Assigns of another instance elements.
		 */
		TQuaternion& assign(T real_w, T imag_x, T imag_y, T imag_z);
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
			/** @brief Translation axis of the matrix. */
			realW = 0,
			/** @brief X imaginary part of the quaternion. */
			imagX = 1,
			/** @brief Y imaginary part of the quaternion. */
			imagY = 2,
			/** @brief Z imaginary part of the quaternion. */
			imagZ = 3
		};

		/**
		 * @brief Array access operator.
		 * @throw std::out_of_range
		 */
		T& operator[](size_t);

		/**
		 * @brief Array access const operator to the elements.
		 * @throw std::out_of_range
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
		 * @brief Get the imaginary part as a 3D-vector.
		 */
		TVector3D<T> imaginary() const;

		/**
		 * @brief Gets the w or real part of this instance.
		 * Same as #real() method.
		 */
		T w() const;

		/**
		 * @brief Gets the w or real part reference of this instance.
		 * Same as #real() method.
		 */
		T& w();

		/**
		 * @brief Get the imaginary X-part of this instance.
		 */
		T x() const;

		/**
		 * @brief Get the imaginary X-part reference of this instance.
		 */
		T& x();

		/**
		 * @brief Get the imaginary Y-part of this instance.
		 */
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
		 * @brief Gets a copy of this instance where the imaginary (vector) part of a quaternion is negated.
		 * The conjugate of a quaternion is a simple operation that reflects the imaginary components of
		 * the quaternion while leaving the real component unchanged. It is often used in quaternion
		 * operations such as finding the inverse or performing transformations.
		 */
		TQuaternion conjugate() const;

		/**
		 * @brief Gets the inverse of the current instance.
		 * The inverse of a quaternion is a quaternion that "reverses" the effect of the original quaternion
		 * when combined through quaternion multiplication.
		 * For unit quaternions, the inverse undoes the rotation described by the quaternion.
		 * This property is analogous to the inverse of a matrix or a number.
		 */
		TQuaternion inverse() const;

		/**
		 * @brief Turns this instance into it inverse.
		 * @see inverse()
		 */
		TQuaternion& invert();

		/**
		 * @brief Normalizes this instance.
		 */
		TQuaternion& normalize();

		/**
		 * @brief Get a normalizes copy of this instance.
		 */
		TQuaternion normalized() const;

		/**
		 * @brief Gets the squared magnitude which is used in computations and the #magnitude() method for the actual magnitude.
		 * @return Square root of the magnitude.
		 */
		T magnitudeSqr() const;

		/**
		 * @brief Gets the magnitude of this quaternion.
		 * The magnitude (or norm) of a quaternion q=a+bi+cj+dk, where a, b, c, and d are real numbers,
		 * is calculated similarly to the magnitude of a 4-dimensional vector.
		 * All element values squared and added together.
		 */
		T magnitude() const;

		/**
		 * @brief Sets the orientation part of the passed TMatrix instance.
		 */
		TMatrix44<T> toMatrix(TMatrix44<T>&) const;

		/**
		 * @brief Get the matrix having only the orientation part set using this instance normalized version.
		 * Before the matrix is calculated the quaternion is normalized.
		 */
		TMatrix44<T> toMatrix() const;

		/**
		 * @brief Transforms a rotation matrix to this Quaternion.
		 * Throws an exception when the passed matrix is not a rotation matrix.
		 * @throw std::invalid_argument
		 */
		TQuaternion& fromMatrix(const T[4][4]);
		/**
		 * @brief Transforms a 3D-vector not using the quaternion directly instead of a matrix.
		 */
		TVector3D<T> transform(TVector3D<T> v) const;

		/**
		 * @brief Negation operator.
		 */
		TQuaternion operator-() const;

		/**
		 * @brief Addition assignment operator.
		 */
		TQuaternion& operator+=(const TQuaternion&);

		/**
		 * @brief Subtraction assignment operator.
		 */
		TQuaternion& operator-=(const TQuaternion&);

		/**
		 * @brief Multiplication assignment operator.
		 */
		TQuaternion& operator*=(const TQuaternion&);

		/**
		 * @brief Division assignment operator.
		 */
		TQuaternion& operator/=(const TQuaternion&);

		/**
		 * @brief operator.
		 */
		TQuaternion& operator^=(const TQuaternion&);

		/**
		 * @brief Multiplication assignment operator.
		 */
		TQuaternion& operator*=(T c);

		/**
		 * @brief Division operator.
		 */
		TQuaternion& operator/=(T c);

		/**
		 * @brief Pointer cast operator.
		 */
		TQuaternion operator*(T) const;

		/**
		 * @brief operator.
		 */
		TQuaternion operator/(T) const;

		/**
		 * @brief Compares the passed quaternion within the set tolerance.
		 * @param quat Vector to compare with.
		 * @param tol The tolerance when comparing which has a default.
		 * @return True when equal.
		 */
		bool isEqual(const TQuaternion& quat, T tol = tolerance) const;

		/**
		 * @brief operator.
		 */
		bool operator==(const TQuaternion&) const;

		/**
		 * @brief operator.
		 */
		bool operator!=(const TQuaternion&) const;

		/**
		 * @brief Squares this instance.
		 * If the quaternion is a unit quaternion (meaning it has magnitude 1),
		 * squaring it produces another quaternion with the same magnitude.
		 * When a unit quaternion represents a rotation, squaring the quaternion effectively
		 * doubles the rotation angle while keeping the axis the same.
		 * @return Squared quaternion.
		 */
		TQuaternion squared() const;

		/**
		 * @brief Gets the logarithm form of this instance.
		 * The logarithm log(q) of a quaternion is a mathematical operation that maps the quaternion
		 * from its rotational form to a form that can be linearly interpolated in logarithmic space.
		 * This is particularly useful for spherical blending or spline interpolation of rotations,
		 * where transformations are more natural in logarithmic form.
		 * To move back from the logarithmic form to a quaternion, use the quaternion exponential #exp(),
		 * which reconstructs the quaternion from its angle-axis form.
		 * By converting a quaternion to its logarithmic form, it allows linearly interpolate between two rotations.
		 * This is helpful in spline-based interpolation techniques for smooth rotational blending.
		 * Before the logarithm is calculated the quaternion is normalized and result is scaled back accordingly.
		 * @return Logarithm quaternion.
		 */
		TQuaternion log() const;

		/**
		 * @brief Gets the exponential of a quaternion which is the inverse of #log().
		 * Moves back from the logarithmic form to a quaternion, which reconstructs the quaternion from its angle-axis form.
		 * @return Regular quaternion.
		 */
		TQuaternion exp() const;

		/**
		 * @brief Spherical Linear Interpolation (SLERP) function.
		 * Used to smoothly blending or transitioning between two quaternions, typically representing rotations in 3D space.
		 * Interpolation is used in animation that require smooth rotational transitions,
		 * as it avoids issues like gimbal lock and provides a smooth, natural path between orientations.
		 * @param q Quaternion to transition to.
		 * @param t Transition ranges from 0 to 1.
		 * @return Intermediate quaternion.
		 */
		TQuaternion interpolate(const TQuaternion& q, T t) const;

		/**
		 * @brief Implements a logarithmic interpolation function.
		 * @param q Targeted quaternion value.
		 * @param t Value from 0.0 to 1.0
		 * @return
		 */
		TQuaternion interpolateLogarithmic(const TQuaternion& q, T t) const;

		/**
		 * @brief Gets the string representation of the quaternion formed like '(x,y,z,r)'.
		 */
		std::string toString() const;

		/**
		 * @brief Gets the quaternion values from the string representation formed like '(1,2,3,4)'.
		 * Throws an exception when the string is not in the correct format.
		 * @throw std::invalid_argument
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
				struct quat_type
				{
						T w;
						T x;
						T y;
						T z;
				} q;
		} _data{1, 0, 0, 0};
};

/**
 * @brief Adds two quaternions.
 * @tparam T Base floating point type.
 * @param lhs Left value of the addition.
 * @param rhs Right value of the addition.
 * @return New matrix.
 */
template<typename T>
TQuaternion<T> operator+(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return TQuaternion<T>(lhs) += rhs;
}

/**
 * @brief Subtracts two quaternions.
 * @tparam T Base floating point type.
 * @param lhs Left value of the subtraction.
 * @param rhs Right value of the subtraction.
 * @return New matrix.
 */
template<typename T>
TQuaternion<T> operator-(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return TQuaternion<T>(lhs) -= rhs;
}

/**
 * @brief Multiplies two quaternions.
 * @tparam T Base floating point type.
 * @param lhs Left value of the multiplication.
 * @param rhs Right value of the multiplication.
 * @return New quaternion instance.
 */
template<typename T>
TQuaternion<T> operator*(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return TQuaternion<T>(lhs) *= rhs;
}

/**
 * @brief Divides two quaternions.
 * @tparam T Base floating point type.
 * @param lhs Left value of the division.
 * @param rhs Right value of the division.
 * @return New quaternion instance.
 */
template<typename T>
TQuaternion<T> operator/(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs)
{
	return TQuaternion<T>(lhs) /= rhs;
}

/**
 * @brief Multiplies a scalar value with a quaternion.
 * @tparam T Base floating point type.
 * @param c Scalar left value of the multiplication.
 * @param quat Quaternion right value of the multiplication.
 * @return New quaternion instance.
 */
template<typename T>
TQuaternion<T> operator*(T c, const TQuaternion<T>& quat)
{
	return {c * quat.w(), c * quat.x(), c * quat.y(), c * quat.z()};
}

/**
 * @brief Operator for writing the #sf::TQuaternion to an output-stream.
 * Calls #TQuaternion::toString() for the operator.
 * @tparam T Base floating point type.
 * @param os Output stream.
 * @param quat Matrix to stream out.
 * @return The passed output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const TQuaternion<T>& quat)
{
	return os << quat.toString();
}

/**
 * @brief Operator for reading the #sf::TQuaternion from an input stream.
 * Calls #TQuaternion<T>::fromString() to perform this operator.
 * @tparam T Base floating point type.
 * @param is Input stream.
 * @param quat Matrix to stream.
 * @return The passed input stream.
 */
template<typename T>
std::istream& operator>>(std::istream& is, TQuaternion<T>& quat) noexcept(false)
{
	std::string s;
	auto delimiter = ')';
	std::getline(is, s, delimiter);
	quat.fromString(s.append(1, delimiter));
	return is;
}

}// namespace sf

#include <math/TQuaternion.hpp>
