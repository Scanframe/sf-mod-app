#pragma once

#include <string>

namespace sf
{

// Forward declaration.
template<typename T>
class TVector2D;

/**
 * @brief Generic 2 x 2 matrix template.
 */
template<typename T>
class TMatrix22
{
	public:
		/**
		 * @brief Type accessible when implemented.
		 */
		typedef T value_type;

		/**
		 * @brief Default constructor.
		 */
		TMatrix22();

		/**
		 * @brief Copy constructor.
		 */
		TMatrix22(const TMatrix22<T>& m);

		/**
		 * @brief Move constructor.
		 */
		TMatrix22(TMatrix22<T>&&);

		/**
		 * @brief Initializer list constructor.
		 */
		TMatrix22(std::initializer_list<T> list);

		/**
		 * @brief Initializing constructor.
		 */
		TMatrix22(T m00, T m01, T m10, T m11);

		/**
		 * @brief Scaling constructor.
		 */
		TMatrix22(T scale_x, T scale_y);

		/**
		 * @brief Constructs a rotation matrix passing a single angle.
		 * @param angle angle of rotation.
		 */
		TMatrix22(T angle);

		/**
		 * @brief Gets the transposed version of the matrix.
		 * A transposed rotation matrix inverts the rotation.
		 * @return Transposed matrix.
		 */
		TMatrix22<T> transposed() const;

		/**
		 * @brief Assignment using a 2 by 2 array.
		 * @return The matrix itself.
		 */
		TMatrix22<T>& assign(const T[2][2]);

		/**
		 * @brief Assignment using 4 separate values.
		 * @return The matrix itself.
		 */
		TMatrix22<T>& assign(value_type m00, value_type m01, value_type m10, value_type m11);

		/**
		 * @brief Assignment operator.
		 * @return Matrix itself.
		 */
		TMatrix22<T>& operator=(const TMatrix22<T>& m);

		/**
		 * @brief Assignment move operator.
		 */
		TMatrix22<T>& operator=(TMatrix22<T>&& m) noexcept;

		/**
		 * @brief Vector transformation operator.
		 * @return Matrix itself.
		 */
		TMatrix22<T>& operator*=(const TMatrix22<T>& m);

		/**
		 * @brief Vector transformation method.
		 * @return New transformed vector.
		 */
		TVector2D<T> transformed(const TVector2D<T>&) const;

		/**
		 * @brief Vector transformation operator.
		 * @return New transformed vector.
		 */
		TVector2D<T> operator*(const TVector2D<T>&) const;

		/**
		 * @brief Pointer cast operators.
		 */
		constexpr operator T*();

		/**
		 * @brief Const pointer cast operators.
		 */
		constexpr operator const T*() const;

		/**
		 * @brief Compares the passed matrix within the set tolerance.
		 * @param v Vector to compare with.
		 * @param tol The tolerance when comparing which has a default.
		 * @return True when equal.
		 */
		bool isEqual(const TMatrix22& v, T tol = tolerance) const;

		/**
		 * @brief Compare equal operator using the #tolerance for comparing.
		 * @return True when equal.
		 */
		bool operator==(const TMatrix22<T>&) const;

		/**
		 * @brief Compare unequal operator using the #tolerance for comparing.
		 * @return True when unequal.
		 */
		bool operator!=(const TMatrix22<T>&) const;

		/**
		 * @brief Reset the matrix to the so-called 'identity' matrix where a vector
		 * is not changed during transformation.
		 * @return Matrix itself.
		 */
		TMatrix22<T>& resetOrientation();

		/**
		 * Sets the rotation for the matrix.
		 * @param angle Angle in radians.
		 */
		void setRotation(T angle);

		/**
		 * Gets the rotation of the matrix when it is a square matrix.
		 * @param angle Angle in radians.
		 */
		T getRotation() const;

		/**
		 * @brief Gets the string representation of the matrix formed like '((m00,m01),(m10,m11))'.
		 */
		std::string toString() const;

		/**
		 * @brief Gets 22 matrix values from the string representation formed like '((1.2,4.5),(5.6,7.8))'.
		 */
		TMatrix22<T>& fromString(const std::string& s) noexcept(false);

		/**
		 * @brief Copy to matrix to 2 by 2 value_type array.
		 */
		void copyTo(T[2][2]) const;

		/**
		 * @brief Tolerance for when comparing in the equal operator.
		 *
		 * Used as: std::fabs(x1 - x2) < tolerance
		 */
		static constexpr auto tolerance = TVector2D<T>::tolerance;

	protected:
		/**
		 * @brief Storage union of the 2x2 matrix.
		 */
		union data_type
		{
				T array[2 * 2];
				T mtx[2][2];
		} _data = {0, 0, 0, 0};
};

/**
 * @brief Operator for writing the matrix to an output-stream.
 * @tparam T Base floating point type.
 * @param os Output stream.
 * @param mtx Matrix to stream out.
 * @return The passed output stream.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const TMatrix22<T>& mtx)
{
	return os << mtx.toString();
}

/**
 * @brief Operator for reading the matrix from an input stream.
 * @tparam T Base floating point type.
 * @param os Input stream.
 * @param mtx Matrix to stream.
 * @return The passed input stream.
 */
template<typename T>
inline std::istream& operator>>(std::istream& is, TMatrix22<T>& mtx) noexcept(false)
{
	std::string s;
	auto delimiter = ')';
	std::getline(is, s, delimiter);
	mtx.fromString(s.append(1, delimiter));
	return is;
}

}// namespace sf

#include "TMatrix22.hpp"