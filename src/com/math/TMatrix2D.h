#pragma once

#include <string>

namespace sf
{

// Forward declaration.
template<typename T>
class TVector2D;

/**
 * @brief Generic 2 x 2 matrix.
 */
template<typename T>
class TMatrix2D
{
	public:
		/**
		 * @brief Type accessible when implemented.
		 */
		typedef T value_type;

		/**
		 * @brief Default constructor.
		 */
		TMatrix2D();

		/**
		 * @brief Copy constructor.
		 */
		TMatrix2D(const TMatrix2D<T>& m);

		/**
		 * @brief Move constructor.
		 */
		TMatrix2D(TMatrix2D<T>&&);

		/**
		 * @brief Initializer list constructor.
		 */
		TMatrix2D(std::initializer_list<T> list);

		/**
		 * @brief Initializing constructor.
		 */
		TMatrix2D(T m00, T m01, T m10, T m11);

		/**
		 * @brief Scaling constructor.
		 */
		TMatrix2D(T scale_x, T scale_y);

		/**
		 * @brief Constructs a rotation matrix passing a single angle.
		 * @param angle angle of rotation.
		 */
		TMatrix2D(T angle);

		/**
		 * @brief Gets the transposed version of the matrix.
		 * A transposed rotation matrix inverts the rotation.
		 * @return Transposed matrix.
		 */
		TMatrix2D<T> transposed() const;

		/**
		 * @brief Assignment using a 2 by 2 array.
		 * @return The matrix itself.
		 */
		TMatrix2D<T>& assign(const T[2][2]);

		/**
		 * @brief Assignment using 4 separate values.
		 * @return The matrix itself.
		 */
		TMatrix2D<T>& assign(value_type m00, value_type m01, value_type m10, value_type m11);

		/**
		 * @brief Assignment move operator.
		 */
		TMatrix2D<T>& operator=(TMatrix2D<T>&& m) noexcept;

		/**
		 * @brief Assignment operator slows things down considerably !!
		 * @return Matrix itself.
		 */
		TMatrix2D<T>& operator=(const TMatrix2D<T>& m);

		/**
		 * @brief Vector transformation operator.
		 * @return Matrix itself.
		 */
		TMatrix2D<T>& operator*=(const TMatrix2D<T>& m);

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
		constexpr operator const T*();

		/*
		 * A Normal vector transforms differently as well because
		 * mathematically it can better be interpreted as a plane
		 * perpendicular to the vector, so the transformation for a normal
		 * vector is like that of the transformation of a perpendicular plane.
		 * Normal vectors are transformed by the inverse transpose of the
		 * transformation that transforms points.
		 */

		/**
		 * @brief Compare equal operator.
		 * @return True when equal.
		 */
		bool operator==(const TMatrix2D<T>&) const;

		/**
		 * @brief Compare unequal operator.
		 * @return True when unequal.
		 */
		bool operator!=(const TMatrix2D<T>&) const;

		/**
		 * @brief Reset the matrix to the so-called 'identity' matrix where a vector
		 * is not changed during transformation.
		 * @return Matrix itself.
		 */
		TMatrix2D<T>& resetOrientation();

		/**
		 * Sets the rotation for the matrix.
		 * @param angle Angle in radials.
		 */
		void setRotation(T angle);

		/**
		 * Gets the rotation of the matrix when it is a square matrix.
		 * @param angle Angle in radials.
		 */
		T getRotation() const;

		/**
		 * @brief Gets the string representation of the matrix formed like '((m00,m01),(m10,m11))'.
		 */
		std::string toString() const;

		/**
		 * @brief Gets 2D vector value from the string representation formed like '((1.2,4.5),(5.6,7.8))'.
		 */
		TMatrix2D<T>& fromString(const std::string& s) noexcept(false);

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
		 * @brief Storage union of the 2D matrix.
		 */
		union
		{
				T array[4];
				T mtx[2][2];
		} _data = {0, 0, 0, 0};
};

}// namespace sf

#include "TMatrix2D.hpp"