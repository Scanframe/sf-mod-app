/*
Description:
 The Matrix44 class is a matrix of 4x4 floating point values.

 Note:
   Beware of OpenGL matrices (their translation vector is row 4 instead of col4)
   Functions are provided to convert to and from GL matrices (TransposeCopyTo
   and TransposeAssign).

 Example of usage:
   Matrix44 A(...), B(...);
   Matrix44 C = A*B;

 Note:
    A * B is not equal to resulting matrix of B * A.
    When A * B then matrix A is applied on the axes of B.
    When A *= B the matrix B is applied on the axes of A.
*/

#pragma once
#include "TVector3D.h"
#include <string>

namespace sf
{

// Forward definition.
template<typename T>
class TQuaternion;

/**
 * @brief Generic 4 x 4 matrix template.
 */
template<typename T>
class TMatrix44
{
	public:
		/**
		 * @brief Type accessible when implemented.
		 */
		typedef T value_type;

		/**
		 * @brief Default constructor which is by default a unit-matrix.
		 */
		TMatrix44();

		/**
		 * @brief Initialization constructor for 16 single floating point values.
		 */
		TMatrix44(
			T m00, T m01, T m02, T m03,
			T m10, T m11, T m12, T m13,
			T m20, T m21, T m22, T m23,
			T m30, T m31, T m32, T m33
		);

		/**
		 * @brief Initialization constructor for 4x4 floating point values.
		 */
		explicit TMatrix44(const T[4][4]);

		/**
		 * @brief Construct from a translation vector.
		 * Construct a translation transformation from a given 3D Vector.
		 */
		explicit TMatrix44(const TVector3D<T>& tv);

		/**
		 * @brief Construct from rotations.
		 * @see setTiltPanRoll()
		 */
		TMatrix44(T tilt, T pan, T roll);

		/**
		 * @brief Construct from quaternion rotation.
		 */
		explicit TMatrix44(const TQuaternion<T>& q);

		/**
		 * @brief Copy constructor.
		 */
		TMatrix44(const TMatrix44&);

		/**
		 * @brief Move constructor.
		 */
		TMatrix44(TMatrix44&&);

		/**
		 * @brief Makes this a unit-matrix.
		 * A unit matrix, also known as the identity matrix, is a square matrix in which all the
		 * elements along the main diagonal (from the top-left to the bottom-right) are 1,
		 * and all other elements are 0. It acts as the multiplicative identity in matrix algebra,
		 * meaning that multiplying any matrix by the identity matrix results in the original matrix itself.
		 */
		void unit();

		/**
		 * @brief Inverts matrix44 and gets determinant only when the determinant != 0.
		 * @return The matrix determinant.
		 */
		T invert();

		/**
		 * @brief Get a transposed matrix of this instance.
		 * @return Transposed matrix.
		 */
		TMatrix44 transposed() const;

		/**
		 * @brief Transpose this matrix.
		 * @return This matrix
		 */
		TMatrix44& transpose();

		/**
		 * @brief Assigns the 4x4 array structure to this matrix.
		 * @return
		 */
		TMatrix44& assign(const T[4][4]);

		/**
		 * @brief Assigns the transposed 4x4 array structure to this matrix.
		 * @return This matrix.
		 */
		TMatrix44& transposeAssign(const T[4][4]);

		/**
		 * @brief Assignment operator.
		 */
		TMatrix44& operator=(const TMatrix44&);
		/**
		 * @brief Assignment move operator.
		 */
		TMatrix44& operator=(TMatrix44&& m) noexcept;

		/**
		 * @brief Applies passed matrix on the axes of this matrix.
		 * @return This modified matrix.
		 */
		TMatrix44& operator*=(const TMatrix44<T>&);

		/**
		 * @brief Applies matrix on the passed 3D vector.
		 * @return Modified 3D-vector.
		 */
		TVector3D<T> operator*(const TVector3D<T>&) const;

		/**
		 * @brief Cast operator to pointer of type 'T'.
		 * @return Pointer.
		 */
		operator T*();

		/**
		 * @brief Cast operator to pointer of type 'T'.
		 * @return Const pointer.
		 */
		operator const T*() const;

		/**
		 * @brief Compare equal operator using the #tolerance when comparing.
		 * @return True when equal.
		 */
		bool operator==(const TMatrix44&) const;

		/**
		 * @brief Compare unequal operator using the #tolerance when comparing.
		 * @return True when unequal.
		 */
		bool operator!=(const TMatrix44&) const;

		/**
		 * @brief Copies the matrix to a 4x4 array.
		 */
		void copyTo(T[4][4]) const;

		/**
		 * @brief Transposed copy to a 4x4 array for compatibility with OpenGL.
		 */
		void transposeCopyTo(T[4][4]) const;

		/**
		 * @brief Sets/resets rotation part of the matrix according to tilt pan roll.
		 * @param tilt Tilt is a rotation around the horizontal (x-axis).
		 * @param pan Pan is a rotation around the vertical (y-axis).
		 * @param roll Roll is a rotation around the forward (z-axis).
		 * @return This matrix.
		 */
		TMatrix44& setTiltPanRoll(T tilt, T pan, T roll);

		/**
		 * @brief Applies rotation on the current matrix.
		 * @param tilt Tilt is a rotation around the horizontal (x-axis).
		 * @param pan Pan is a rotation around the vertical (y-axis).
		 * @param roll Roll is a rotation around the forward (z-axis).
		 * @return This matrix.
		 */
		TMatrix44& rotate(T tilt, T pan, T roll);

		/**
		 * @brief Translates rotation part of matrix to tilt, pan and roll values.
		 * @note Only valid for rotation matrices without scaling info!
		 * @param tilt Tilt angle in radians.
		 * @param pan Pan angle in radians.
		 * @param roll Roll angle in radians.
		 */
		void getTiltPanRoll(T& tilt, T& pan, T& roll) const;

		/**
		 * @brief
		 * @param near_plane
		 * @param far_plane
		 * @param fov
		 */
		void setProjectionMatrix(T near_plane, T far_plane, T fov);

		/**
		 * @brief Multiplies this matrix with the passed one so that the passed matrix is applied on the matrix axes.
		 * @return this matrix.
		*/
		TMatrix44& multiply(const TMatrix44& m);

		/**
		 * @brief Zero's or clears translation part of this instance.
		 */
		void clearPos();

		/**
		 * @brief Sets the translation vector of this instance using a 3D vector.
		 */
		void setPos(const TVector3D<T>& v);

		/**
		 * @brief Sets the translation vector of this instance using individual
		 */
		void setPos(T x, T y, T z);

		/**
		 * @brief Gets translation part of matrix.
		 * @return Translation vector.
		 */
		TVector3D<T> getPos() const;

		/**
		 * @brief Enumerate for identifying a matrix axis.
		 */
		enum EAxis : unsigned int
		{
			/** @brief X-axis of the matrix. */
			axisX = 0,
			/** @brief Y-axis of the matrix. */
			axisY = 1,
			/** @brief Z-axis of the matrix. */
			axisZ = 2,
			/** @brief Translation axis of the matrix. */
			axisT = 3
		};
		/**
		 * @brief Gets given axis of matrix as a 3D vector.
		 * @param axis Requested axis.
		 * @return 3D-vector.
		 */
		TVector3D<T> getAxis(EAxis axis) const;

		/**
		 * @brief Gets transformation of rotation around local x- and y-axis of current matrix.
		 * @param horizontal
		 * @param vertical
		 * @return
		 */
		TMatrix44 orbit(T horizontal, T vertical) const;

		/**
		 * @brief Gets the orientation part of matrix.
		 */
		TMatrix44 orientation(void) const;

		/**
		 * @brief Sets the element specified by the row and column.
		 * @param row Row number between 0 and 3.
		 * @param column` Column number between 0 and 3.
		 */
		void setElement(unsigned int row, unsigned int column, T value);

		/**
		 * @brief Convert rotation matrix part to unit quaternion.
		 * @return Quaternion
		 */
		TQuaternion<T> getQuaternion(void) const;

		/**
		 * @brief Resets the orientation and leaves the translation as is.
		 */
		void resetOrientation(void);

		/**
		 * @brief Set orientation using 2 direction vectors.
		 * y-axis direction dominates x-axis (x-axis is corrected if x-axis and y-axis not perpendicular)
		 * @param x_axis
		 * @param y_axis
		 * @return true on success.
		 */
		bool setOrientationXY(const TVector3D<T>& x_axis, const TVector3D<T>& y_axis);

		/**
		 * @brief Set orientation using 2 direction vectors.
		 * The z-axis direction dominates y-axis (y-axis is corrected if y & z not perpendicular)
		 * @param z_axis
		 * @param y_axis
		 * @return true on success.
		 */
		bool setOrientationZY(const TVector3D<T>& z_axis, const TVector3D<T>& y_axis);

		/**
		 * @brief Another way of applying rotation to the matrix.
		 * @param rx x Radians
		 * @param ry
		 * @param rz
		 */
		void insertGIGRotXYZ(T rx, T ry, T rz);

		/**
		 * @brief Gets the string representation of the matrix formed like '({m00,m01,m02,m03},{m10,m11,m12,m13},{m20,m21,m22,m23},{m30,m31,m32,m33})'.
		 */
		std::string toString() const;

		/**
		 * @brief Gets matrix values from the string representation formed like '({1,2,3,4},{11,12,13,14},{21,22,23,24},{31,32,33,34})'.
		 */
		TMatrix44& fromString(const std::string& s) noexcept(false);

		/**
		 * @brief Tolerance for when comparing in the equal operator.
		 *
		 * Used as: std::fabs(x1 - x2) < tolerance
		 */
		static constexpr auto tolerance = TVector3D<T>::tolerance;

	protected:
		/**
		 * @brief Storage union of the 4x4 matrix as array and 4 x4 array.
		 * It is initialized as a unit-matrix.
		 */
		union data_type
		{
				T array[4 * 4];
				T mtx[4][4];
		} _data = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
};

/**
 * @brief Multiplies to matrices into a single one.
 * @tparam T Base floating point type.
 * @param lm Left matrix of multiplication.
 * @param rm Right matrix of multiplication.
 * @return New matrix.
 */
template<typename T>
inline TMatrix44<T> operator*(const TMatrix44<T>& lm, const TMatrix44<T>& rm)
{
	return TMatrix44<T>(rm).multiply(lm);
}

/**
 * @brief Operator for writing the matrix to an output-stream.
 * @tparam T Base floating point type.
 * @param os Output stream.
 * @param mtx Matrix to stream out.
 * @return The passed output stream.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const TMatrix44<T>& mtx)
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
inline std::istream& operator>>(std::istream& is, TMatrix44<T>& mtx) noexcept(false)
{
	std::string s;
	auto delimiter = ')';
	std::getline(is, s, delimiter);
	mtx.fromString(s.append(1, delimiter));
	return is;
}

}// namespace sf

#include "TMatrix44.hpp"