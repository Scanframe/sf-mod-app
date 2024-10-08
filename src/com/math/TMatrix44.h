/*
Description:
 The Matrix44 class is a matrix of 4x4 floats

 Note:
   Beware of OpenGL matrices (their translation vector is row 4 instead of col4)
   Functions are provided to convert to and from GL matrices (TransposeCopyTo
   and TransposeAssign).

 Example of usage:
   Matrix44 A(...), B(...);
   Matrix44 C = A*B;

 Note:
    A * B is not eaqual to resulting matrix of B * A.
    When A * B then matrix A is applied on the axes of B.
    When A *= B the matrix B is applied on the axes of A.
*/

#pragma once
#include "Vector3D.h"
#include "Vector4D.h"
#include <algorithm>
#include <assert.h>

// Forward definition.
template<class T, class T2>
class TQuat;
//---------------------------------------------------------------------------
//
// Generic float 4 x 4 matrix
//
template<class T, class T2>
class TMatrix44
{
	public:
		typedef T value_type;
		// Default constructor
		TMatrix44();
		// Construct from a translation vector.
		TMatrix44(const TVector3<T, T2>& a);
		// Construct from rotations.
		TMatrix44(T tilt, T pan, T roll);
		// Construct from quaternion: rotation
		TMatrix44(const TQuat<T, T2>& q);
		// Copy constructor.
		TMatrix44(const TMatrix44&);
		// Init constructor.
		TMatrix44(
			T m00, T m01, T m02, T m03,
			T m10, T m11, T m12, T m13,
			T m20, T m21, T m22, T m23,
			T m30, T m31, T m32, T m33
		);

		// Returns determinant and inverts matrix44 (not if det == 0)
		T Invert();
		TMatrix44 Transpose();
		// Construct a unitmatrix.
		void Unit();

		// Assignment functions.
		TMatrix44& Assign(const T[4][4]);
		TMatrix44& TransposeAssign(const T[4][4]);

#if 0
    // assignment operator slows things down considerably !!
    // e.g. a = b + c; is then much slower, so we do not define this operator
    TMatrix44& operator = (const TMatrix44&);
#endif

		// The passed matrix is applied on the axes of this matrix.
		TMatrix44<T, T2>& operator*=(const TMatrix44<T, T2>& m);
		// This matrix is applied on the passed vector.
		TVector3<T, T2> operator*(const TVector3<T, T2>& v) const;
		// This matrix is applied on the passed vector.
		TVector4<T, T2> operator*(const TVector4<T, T2>& v) const;
		// Cast operators.
		operator T*() { return &m[0][0]; }
		operator const T*() { return &m[0][0]; }
		//
		// A Normal vector transforms differently as well because
		// mathematically it can better be interpreted as a plane
		// perpendicular to the vector, so the transformation for a normal
		// vector is like that of the transformation of a perpendicular plane.
		// Normal vectors are transformed by the inverse transpose of the
		// transformation that transforms points.
		//
		bool operator==(const TMatrix44<T, T2>&) const;
		bool operator!=(const TMatrix44<T, T2>&) const;

		// Conversions
		void CopyTo(T[4][4]) const;
		// Transposed copy for compatibility with Open GL.
		void TransposeCopyTo(T[4][4]) const;
		// Overrides rotation part of matrix according to tilt pan roll,
		// tilt pan roll in radials.
		TMatrix44<T, T2>& SetTiltPanRoll(T x, T y, T z);
		// Rotates the axes of the current matrix.
		// Returns this matrix as a reference.
		TMatrix44<T, T2>& Rotate(T x, T y, T z);
		// Translates rotation part of matrix to tilt pan roll values,
		// tilt pan roll in radials.
		// Only valid for rotation matrices without scaling info!
		void GetTiltPanRoll(T& tilt, T& pan, T& roll) const;
		void SetProjectionMatrix(T near_plane, T far_plane, T fov);

		// Overrides translation part of matrix according to given position.
		void SetPos(const TVector3<T, T2>& pos);
		void SetPos(T x, T y, T z);

		// Multiplies this matrix with the passed one so that the passed matrix
		// is applied on the this matrix axes.
		// Returns this matrix as a reference.
		TMatrix44<T, T2>& Mult(const TMatrix44<T, T2>& lhs);

		// Overrides translation part with zero vector.
		void ClrPos();
		// Get translation part of matrix.
		TVector3<T, T2> GetPos(void) const;

		// Get given axis of matrix as a 3D array.
		TVector3<T, T2> GetAxis(int columnnr) const;
		// Get transformation of rotation around local x and y axis of current matrix.
		TMatrix44<T, T2> Orbit(T horizontal, T vertical) const;
		// Return orientation part of matrix.
		TMatrix44<T, T2> Ori(void) const;
		// Convert rotation matrix part to unit quaternion
		TQuat<T, T2> GetQuat(void) const;

		void ResetOrientation(void);
		// Set orientation using 2 direction vectors
		// y-axis direction dominates xaxis (x-axis is corrected if x & y
		// not perpendicular)
		// Return true on success.
		bool SetOrientationXY(const TVector3<T, T2>& xaxis, const TVector3<T, T2>& yaxis);
		// set orientation using 2 direction vectors
		// z-axis direction dominates y-axis (y-axis is corrected if y & z
		// not perpendicular)
		// Return true on success.
		bool SetOrientationZY(const TVector3<T, T2>& zaxis, const TVector3<T, T2>& yaxis);
		void InsertGIGRotXYZ(T rotx, T roty, T rotz);
		//
		ostream& Write(ostream& os) const;
		istream& Read(istream& is);
		istream& getdouble(istream& is, T& d, char delim);

	public:
		// NOTE: This seems to be m[col][row], is this correct???? - Bram
		// the matrix data
		T m[4][4];
};
//---------------------------------------------------------------------------
// Passed matrix lhs is applied on axes of this matrix.
inline template<class T, class T2>
TMatrix44<T, T2>& TMatrix44<T, T2>::operator*=(const TMatrix44<T, T2>& lhs)
{
	return Mult(lhs);
}
//---------------------------------------------------------------------------
// Passed matrix lhs is applied on axes of matrix rhs.
inline template<class T, class T2>
const TMatrix44<T, T2> operator*(const TMatrix44<T, T2>& lhs, const TMatrix44<T, T2>& rhs)
{
	return TMatrix44<T, T2>(rhs).Mult(lhs);
}
//---------------------------------------------------------------------------

inline template<class T, class T2>
ostream& operator<<(ostream& os, const TMatrix44<T, T2>& m)
{
	return m.Write(os);
}
//---------------------------------------------------------------------------

inline template<class T, class T2>
istream& operator>>(istream& is, TMatrix44<T, T2>& m)
{
	return m.Read(is);
}
//---------------------------------------------------------------------------
//
// Default Constructor: construct a unitmatrix
//
inline template<class T, class T2>
TMatrix44<T, T2>::TMatrix44()
{
	Unit();
}
//---------------------------------------------------------------------------
inline template<class T, class T2>
TMatrix44<T, T2>::TMatrix44(T tilt, T pan, T roll)
{
	Unit();
	SetTiltPanRoll(tilt, pan, roll);
}
//---------------------------------------------------------------------------
//
// Conversion Constructor: construct a translation transformation from
// a given 3D Vector
//
template<class T, class T2>
TMatrix44<T, T2>::TMatrix44(const TVector3<T, T2>& a)
{
	m[1][0] = m[2][0] = m[0][3] =
		m[0][1] = m[2][1] = m[1][3] =
			m[0][2] = m[1][2] = m[2][3] = 0;
	m[3][0] = a.x;
	m[3][1] = a.y;
	m[3][2] = a.z;
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
}
//---------------------------------------------------------------------------
//
// Conversion Constructor: construct a rotation matrix from a quaternion
//
template<class T, class T2>
TMatrix44<T, T2>::TMatrix44(const TQuat<T, T2>& q)
{
	q.InsertInMatrix(m);
	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[0][3] = 0.0;
	m[1][3] = 0.0;
	m[2][3] = 0.0;
	m[3][3] = 1.0;
}
//---------------------------------------------------------------------------
//
// Copy Constructor
//
template<class T, class T2>
TMatrix44<T, T2>::TMatrix44(const TMatrix44& rhs)
{
	m[0][0] = rhs.m[0][0];
	m[0][1] = rhs.m[0][1];
	m[0][2] = rhs.m[0][2];
	m[0][3] = rhs.m[0][3];
	m[1][0] = rhs.m[1][0];
	m[1][1] = rhs.m[1][1];
	m[1][2] = rhs.m[1][2];
	m[1][3] = rhs.m[1][3];
	m[2][0] = rhs.m[2][0];
	m[2][1] = rhs.m[2][1];
	m[2][2] = rhs.m[2][2];
	m[2][3] = rhs.m[2][3];
	m[3][0] = rhs.m[3][0];
	m[3][1] = rhs.m[3][1];
	m[3][2] = rhs.m[3][2];
	m[3][3] = rhs.m[3][3];
}
//---------------------------------------------------------------------------
//
// Elements Constructor
//
template<class T, class T2>
TMatrix44<T, T2>::TMatrix44(
	T m00, T m01, T m02, T m03,
	T m10, T m11, T m12, T m13,
	T m20, T m21, T m22, T m23,
	T m30, T m31, T m32, T m33
)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;
	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}
//---------------------------------------------------------------------------
//
// Assignment
//
// normal conversion
//
template<class T, class T2>
TMatrix44<T, T2>& TMatrix44<T, T2>::Assign(const T mat[4][4])
{
	m[0][0] = mat[0][0];
	m[0][1] = mat[0][1];
	m[0][2] = mat[0][2];
	m[0][3] = mat[0][3];
	m[1][0] = mat[1][0];
	m[1][1] = mat[1][1];
	m[1][2] = mat[1][2];
	m[1][3] = mat[1][3];
	m[2][0] = mat[2][0];
	m[2][1] = mat[2][1];
	m[2][2] = mat[2][2];
	m[2][3] = mat[2][3];
	m[3][0] = mat[3][0];
	m[3][1] = mat[3][1];
	m[3][2] = mat[3][2];
	m[3][3] = mat[3][3];
	return *this;
}
//---------------------------------------------------------------------------
//
// Assignment
//
// transposed conversion
//
template<class T, class T2>
TMatrix44<T, T2>& TMatrix44<T, T2>::TransposeAssign(const T mat[4][4])
{
	m[0][0] = mat[0][0];
	m[1][0] = mat[0][1];
	m[2][0] = mat[0][2];
	m[3][0] = mat[0][3];
	m[0][1] = mat[1][0];
	m[1][1] = mat[1][1];
	m[2][1] = mat[1][2];
	m[3][1] = mat[1][3];
	m[0][2] = mat[2][0];
	m[1][2] = mat[2][1];
	m[2][2] = mat[2][2];
	m[3][2] = mat[2][3];
	m[0][3] = mat[3][0];
	m[1][3] = mat[3][1];
	m[2][3] = mat[3][2];
	m[3][3] = mat[3][3];
	return *this;
}
//---------------------------------------------------------------------------
//
// determinant = TMatrix44.Invert()
//
// returns determinant and inverts matrix44 (not if det == 0)
//
template<class T, class T2>
T TMatrix44<T, T2>::Invert()
{
	int indxc[4], indxr[4], ipiv[4];
	int i, icol, irow, j, k, l, ll;
	T big, dum, pivinv;

	icol = irow = 0;
	ipiv[0] = ipiv[1] = ipiv[2] = ipiv[3] = 0;

	for (i = 0; i < 4; i++)// compile with loop unrolling
	{
		big = 0;
		for (j = 0; j < 4; j++)
		{
			if (ipiv[j] != 1)
			{
				for (k = 0; k < 4; k++)
				{
					if (!ipiv[k])
					{
						if ((dum = fabs(m[j][k])) >= big)
						{
							big = dum;
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1)
						return 0;
				}
			}
		}
		++ipiv[icol];
		if (irow != icol)
		{
			for (l = 0; l < 4; l++)
			{
				// AVO: Needs other function.
				// Swap(m[irow][l], m[icol][l]);
				swap(m[irow][l], m[icol][l]);
			}
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if ((dum = m[icol][icol]) == 0)
			return 0;
		pivinv = 1 / dum;
		m[icol][icol] = 1;
		for (l = 0; l < 4; l++)
			m[icol][l] *= pivinv;
		for (ll = 0; ll < 4; ll++)
		{
			if (ll != icol)
			{
				dum = m[ll][icol];
				m[ll][icol] = 0;
				for (l = 0; l < 4; l++)
					m[ll][l] -= m[icol][l] * dum;
			}
		}
	}
	for (l = 3; l >= 0; l--)
	{
		if (indxr[l] != indxc[l])
		{
			for (k = 0; k < 4; k++)
			{
				// AVO: Needs other function.
				// Swap(m[k][indxr[l]], m[k][indxc[l]]);
				swap(m[k][indxr[l]], m[k][indxc[l]]);
			}
		}
	}
	return 1;
}
//---------------------------------------------------------------------------
//
// TMatrix44.Transpose()
//
template<class T, class T2>
TMatrix44<T, T2> TMatrix44<T, T2>::Transpose()
{
	return TMatrix44(
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]
	);
}
//---------------------------------------------------------------------------
//
// Unit: construct a unitmatrix
//
template<class T, class T2>
void TMatrix44<T, T2>::Unit()
{
	m[1][0] = m[2][0] = m[3][0] =
		m[0][1] = m[2][1] = m[3][1] =
			m[0][2] = m[1][2] = m[3][2] =
				m[0][3] = m[1][3] = m[2][3] = 0.0;

	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
}
//---------------------------------------------------------------------------
//
// SetProjectionMatrix
//
template<class T, class T2>
void TMatrix44<T, T2>::SetProjectionMatrix(
	T near_plane,
	T far_plane,
	T fov
)
{
	T c = (T) cos(fov * 0.5);
	T s = (T) sin(fov * 0.5);
	T Q = s / (1.0 - near_plane / far_plane);
	m[0][0] = c;
	m[0][1] = 0.0;
	m[0][2] = 0.0;
	m[0][3] = 0.0;
	m[1][0] = 0.0;
	m[1][1] = c;
	m[1][2] = 0.0;
	m[1][3] = 0.0;
	m[2][0] = 0.0;
	m[2][1] = 0.0;
	m[2][2] = Q;
	m[2][3] = s;
	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = -Q * near_plane;
	m[3][3] = 0.0;
}
//---------------------------------------------------------------------------
// Math Operators
//---------------------------------------------------------------------------
//
// Multiply matrices.
//
template<class T, class T2>
TMatrix44<T, T2>& TMatrix44<T, T2>::Mult(const TMatrix44& lhs)
{
	// AVO: Reversed operants for this one from the original.
	// So it more logical then before.
	// Now the passed matrix is applied on the axes of this matrix.
	TMatrix44& rhs(*this);
	*this = TMatrix44<T, T2>(
		rhs.m[0][0] * lhs.m[0][0] +
			rhs.m[0][1] * lhs.m[1][0] +
			rhs.m[0][2] * lhs.m[2][0] +
			rhs.m[0][3] * lhs.m[3][0],

		rhs.m[0][0] * lhs.m[0][1] +
			rhs.m[0][1] * lhs.m[1][1] +
			rhs.m[0][2] * lhs.m[2][1] +
			rhs.m[0][3] * lhs.m[3][1],

		rhs.m[0][0] * lhs.m[0][2] +
			rhs.m[0][1] * lhs.m[1][2] +
			rhs.m[0][2] * lhs.m[2][2] +
			rhs.m[0][3] * lhs.m[3][2],

		rhs.m[0][0] * lhs.m[0][3] +
			rhs.m[0][1] * lhs.m[1][3] +
			rhs.m[0][2] * lhs.m[2][3] +
			rhs.m[0][3] * lhs.m[3][3],

		rhs.m[1][0] * lhs.m[0][0] +
			rhs.m[1][1] * lhs.m[1][0] +
			rhs.m[1][2] * lhs.m[2][0] +
			rhs.m[1][3] * lhs.m[3][0],

		rhs.m[1][0] * lhs.m[0][1] +
			rhs.m[1][1] * lhs.m[1][1] +
			rhs.m[1][2] * lhs.m[2][1] +
			rhs.m[1][3] * lhs.m[3][1],

		rhs.m[1][0] * lhs.m[0][2] +
			rhs.m[1][1] * lhs.m[1][2] +
			rhs.m[1][2] * lhs.m[2][2] +
			rhs.m[1][3] * lhs.m[3][2],

		rhs.m[1][0] * lhs.m[0][3] +
			rhs.m[1][1] * lhs.m[1][3] +
			rhs.m[1][2] * lhs.m[2][3] +
			rhs.m[1][3] * lhs.m[3][3],

		rhs.m[2][0] * lhs.m[0][0] +
			rhs.m[2][1] * lhs.m[1][0] +
			rhs.m[2][2] * lhs.m[2][0] +
			rhs.m[2][3] * lhs.m[3][0],

		rhs.m[2][0] * lhs.m[0][1] +
			rhs.m[2][1] * lhs.m[1][1] +
			rhs.m[2][2] * lhs.m[2][1] +
			rhs.m[2][3] * lhs.m[3][1],

		rhs.m[2][0] * lhs.m[0][2] +
			rhs.m[2][1] * lhs.m[1][2] +
			rhs.m[2][2] * lhs.m[2][2] +
			rhs.m[2][3] * lhs.m[3][2],

		rhs.m[2][0] * lhs.m[0][3] +
			rhs.m[2][1] * lhs.m[1][3] +
			rhs.m[2][2] * lhs.m[2][3] +
			rhs.m[2][3] * lhs.m[3][3],

		rhs.m[3][0] * lhs.m[0][0] +
			rhs.m[3][1] * lhs.m[1][0] +
			rhs.m[3][2] * lhs.m[2][0] +
			rhs.m[3][3] * lhs.m[3][0],

		rhs.m[3][0] * lhs.m[0][1] +
			rhs.m[3][1] * lhs.m[1][1] +
			rhs.m[3][2] * lhs.m[2][1] +
			rhs.m[3][3] * lhs.m[3][1],

		rhs.m[3][0] * lhs.m[0][2] +
			rhs.m[3][1] * lhs.m[1][2] +
			rhs.m[3][2] * lhs.m[2][2] +
			rhs.m[3][3] * lhs.m[3][2],

		rhs.m[3][0] * lhs.m[0][3] +
			rhs.m[3][1] * lhs.m[1][3] +
			rhs.m[3][2] * lhs.m[2][3] +
			rhs.m[3][3] * lhs.m[3][3]
	);
	return *this;
}
//---------------------------------------------------------------------------
//
// TMatrix44 * Vector4
//
template<class T, class T2>
TVector4<T, T2> TMatrix44<T, T2>::operator*(const TVector4<T, T2>& a) const
{
	return Vector4(
		m[0][0] * a.x + m[1][0] * a.y + m[2][0] * a.z + m[3][0] * a.w,
		m[0][1] * a.x + m[1][1] * a.y + m[2][1] * a.z + m[3][1] * a.w,
		m[0][2] * a.x + m[1][2] * a.y + m[2][2] * a.z + m[3][2] * a.w,
		m[0][3] * a.x + m[1][3] * a.y + m[2][3] * a.z + m[3][3] * a.w
	);
}
//---------------------------------------------------------------------------
//
// TMatrix44 * Vector3
//
template<class T, class T2>
TVector3<T, T2> TMatrix44<T, T2>::operator*(const TVector3<T, T2>& a) const
{
	return TVector3<T, T2>(
		m[0][0] * a.x + m[1][0] * a.y + m[2][0] * a.z,
		m[0][1] * a.x + m[1][1] * a.y + m[2][1] * a.z,
		m[0][2] * a.x + m[1][2] * a.y + m[2][2] * a.z
	);
}
//---------------------------------------------------------------------------
//
// TMatrix44 == TMatrix44
//
template<class T, class T2>
bool TMatrix44<T, T2>::operator==(const TMatrix44<T, T2>& rhs) const
{
	return ((m[0][0] == rhs.m[0][0]) && (m[0][1] == rhs.m[0][1]) && (m[0][2] == rhs.m[0][2]) && (m[0][3] == rhs.m[0][3]) && (m[1][0] == rhs.m[1][0]) && (m[1][1] == rhs.m[1][1]) && (m[1][2] == rhs.m[1][2]) && (m[1][3] == rhs.m[1][3]) && (m[2][0] == rhs.m[2][0]) && (m[2][1] == rhs.m[2][1]) && (m[2][2] == rhs.m[2][2]) && (m[2][3] == rhs.m[2][3]) && (m[3][0] == rhs.m[3][0]) && (m[3][1] == rhs.m[3][1]) && (m[3][2] == rhs.m[3][2]) && (m[3][3] == rhs.m[3][3]));
}
//---------------------------------------------------------------------------
//
// TMatrix44 != TMatrix44
//
inline template<class T, class T2>
bool TMatrix44<T, T2>::operator!=(const TMatrix44<T, T2>& m) const
{
	return (!(*this == m));
}
//---------------------------------------------------------------------------
//
// TMatrix44.CopyTo(m)
//
template<class T, class T2>
void TMatrix44<T, T2>::CopyTo(T mat[4][4]) const
{
	mat[0][0] = m[0][0];
	mat[0][1] = m[0][1];
	mat[0][2] = m[0][2];
	mat[0][3] = m[0][3];
	mat[1][0] = m[1][0];
	mat[1][1] = m[1][1];
	mat[1][2] = m[1][2];
	mat[1][3] = m[1][3];
	mat[2][0] = m[2][0];
	mat[2][1] = m[2][1];
	mat[2][2] = m[2][2];
	mat[2][3] = m[2][3];
	mat[3][0] = m[3][0];
	mat[3][1] = m[3][1];
	mat[3][2] = m[3][2];
	mat[3][3] = m[3][3];
}
//---------------------------------------------------------------------------
//
// TMatrix44.TransposeCopyTo(m)
//
// transposed copy
//
template<class T, class T2>
void TMatrix44<T, T2>::TransposeCopyTo(T mat[4][4]) const
{
	mat[0][0] = m[0][0];
	mat[1][0] = m[0][1];
	mat[2][0] = m[0][2];
	mat[3][0] = m[0][3];
	mat[0][1] = m[1][0];
	mat[1][1] = m[1][1];
	mat[2][1] = m[1][2];
	mat[3][1] = m[1][3];
	mat[0][2] = m[2][0];
	mat[1][2] = m[2][1];
	mat[2][2] = m[2][2];
	mat[3][2] = m[2][3];
	mat[0][3] = m[3][0];
	mat[1][3] = m[3][1];
	mat[2][3] = m[3][2];
	mat[3][3] = m[3][3];
}
//---------------------------------------------------------------------------
//
// TMatrix44.SetTiltPanRoll
//  overrides rotation part of matrix according to tilt pan roll,
//  tilt pan roll in degrees
//
template<class T, class T2>
TMatrix44<T, T2>& TMatrix44<T, T2>::SetTiltPanRoll(T tilt, T pan, T roll)
{
	T ta = tilt;
	T pa = pan;
	T ra = roll;
	T sx = sin(ta);
	T sy = sin(pa);
	T sz = sin(ra);
	T cx = cos(ta);
	T cy = cos(pa);
	T cz = cos(ra);

	m[0][0] = cy * cz + sy * sx * sz;
	m[1][0] = sy * sx * cz - cy * sz;
	m[2][0] = sy * cx;
	m[0][1] = cx * sz;
	m[1][1] = cx * cz;
	m[2][1] = -sx;
	m[0][2] = cy * sx * sz - sy * cz;
	m[1][2] = cy * sx * cz + sy * sz;
	m[2][2] = cy * cx;

	return *this;
}
//---------------------------------------------------------------------------
//
// TMatrix44.GetTiltPanRoll
//  translates rotation part of matrix to tilt pan roll values,
//  tilt pan roll in radials
//  Only valid for rotation matrices without scaling info!
//
template<class T, class T2>
void TMatrix44<T, T2>::GetTiltPanRoll(
	T& tilt,
	T& pan,
	T& roll
) const
{
	T sx = -m[2][1];
	T cx = sqrt(1.0 - sx * sx);
	if (cx < 1.0e-12)
	{
		tilt = sx * 2.0 * M_PI;
		pan = 0.0f;
		roll = atan2(-m[1][0], m[0][0]);
	}
	else
	{
		tilt = atan2(sx, cx);
		pan = atan2(m[2][0], m[2][2]);
		roll = atan2(m[0][1], m[1][1]);
	}
}
//---------------------------------------------------------------------------
//
// TMatrix44.Rotate
//  Rotates the current matrix.
//
inline template<class T, class T2>
TMatrix44<T, T2>& TMatrix44<T, T2>::Rotate(T x, T y, T z)
{
	return Mult(TMatrix44<T, T2>().SetTiltPanRoll(x, y, z));
}
//---------------------------------------------------------------------------
//
// TMatrix44.SetPos
//  overrides translation part of matrix according to given position
//
inline template<class T, class T2>
void TMatrix44<T, T2>::SetPos(const TVector3<T, T2>& pos)
{
	m[3][0] = pos.x;
	m[3][1] = pos.y;
	m[3][2] = pos.z;
}
//
inline template<class T, class T2>
void TMatrix44<T, T2>::SetPos(T x, T y, T z)
{
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}
//---------------------------------------------------------------------------
//
// TMatrix44.ClrPos
//  overrides translation part of matrix to zero.
//
inline template<class T, class T2>
void TMatrix44<T, T2>::ClrPos()
{
	m[3][0] = m[3][1] = m[3][2] = 0.0;
}
//---------------------------------------------------------------------------
//
// TMatrix44.GetPos
//  Get translation part of matrix
//
inline template<class T, class T2>
TVector3<T, T2> TMatrix44<T, T2>::GetPos(void) const
{
	return TVector3<T, T2>(m[3][0], m[3][1], m[3][2]);
}
//---------------------------------------------------------------------------
//
// TMatrix44.GetAxis
//  Get given axis of matrix as a 3D array
//
inline template<class T, class T2>
TVector3<T, T2> TMatrix44<T, T2>::GetAxis(int nr) const
{
	assert(nr >= 0 && nr <= 3);
	return TVector3<T, T2>(m[nr][0], m[nr][1], m[nr][2]);
}
//---------------------------------------------------------------------------
//
// TMatrix44.Orbit
//  Get transformation of rotation around local x and y axis of current matrix
//
template<class T, class T2>
TMatrix44<T, T2> TMatrix44<T, T2>::Orbit(T horizontal, T vertical) const
{
	TVector3<T, T2> x_axis = GetAxis(0);
	TVector3<T, T2> y_axis = GetAxis(1);

	TQuat<T, T2> horizontalq(y_axis, horizontal);
	TQuat<T, T2> verticalq(x_axis, vertical);

	TQuat<T, T2> qmul = verticalq * horizontalq;
	qmul.Normalize();// Was left out?

	return TMatrix44<T, T2>(qmul);
}
//---------------------------------------------------------------------------
//
// TMatrix44.Ori
//  return orientation part of matrix
//
template<class T, class T2>
TMatrix44<T, T2> TMatrix44<T, T2>::Ori() const
{
	return TMatrix44<T, T2>(
		m[0][0], m[0][1], m[0][2], 0.0,
		m[1][0], m[1][1], m[1][2], 0.0,
		m[2][0], m[2][1], m[2][2], 0.0,
		0.0, 0.0, 0.0, 1.0
	);
}
//---------------------------------------------------------------------------
//
// GetQuat
//  convert rotation matrix part to unit quaternion
//
template<class T, class T2>
TQuat<T, T2> TMatrix44<T, T2>::GetQuat() const
{
	int next_idx[3] = {1, 2, 0};
	TQuat<T, T2> q;
	T2 tr = m[0][0] + m[1][1] + m[2][2];
	if (tr > 0.0)
	{
		T2 s = sqrt(tr + 1.0);
		q.w = 0.5 * s;
		s = 0.5 / s;
		q.v.x = (m[2][1] - m[1][2]) * s;
		q.v.y = (m[0][2] - m[2][0]) * s;
		q.v.z = (m[1][0] - m[0][1]) * s;
	}
	else
	{
		int i = 0;
		if (m[1][1] > m[0][0])
			i = 1;
		if (m[2][2] > m[i][i])
			i = 2;
		int j = next_idx[i];
		int k = next_idx[j];
		T2 s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
		q[i] = 0.5 * s;
		s = 0.5 / s;
		q.w = (m[k][j] - m[j][k]) * s;
		q[j] = (m[j][i] + m[i][j]) * s;
		q[k] = (m[k][i] + m[i][k]) * s;
	}
	q.Normalize();
	return q;
}
//---------------------------------------------------------------------------
//
// SetOrientationXY
//  set orientation using 2 direction vectors
//  yaxis direction dominates xaxis (xaxis is corrected if x & y
//  not perpendicular)
//  return true on success
//
template<class T, class T2>
bool TMatrix44<T, T2>::SetOrientationXY(
	const TVector3<T, T2>& ixaxis,
	const TVector3<T, T2>& iyaxis
)
{
	Vector3 xaxis(ixaxis);
	Vector3 yaxis(iyaxis);

	// check whether axes are valid for orientation
	// by examining the cross product

	xaxis.Normalize();
	yaxis.Normalize();
	Vector3 zaxis = xaxis * yaxis;
	if (zaxis.x == 0.0 &&
			zaxis.y == 0.0 &&
			zaxis.z == 0.0)
	{
		return false;// invalid axes, no effect
	}
	zaxis.Normalize();

	xaxis = yaxis * zaxis;
	xaxis.Normalize();

	m[0][0] = xaxis.x;
	m[0][1] = xaxis.y;
	m[0][2] = xaxis.z;
	m[1][0] = yaxis.x;
	m[1][1] = yaxis.y;
	m[1][2] = yaxis.z;
	m[2][0] = zaxis.x;
	m[2][1] = zaxis.y;
	m[2][2] = zaxis.z;
	return true;
}
//---------------------------------------------------------------------------
//
// SetOrientationZY
//  set orientation using 2 direction vectors
//  zaxis direction dominates yaxis (yaxis is corrected if y & z
//  not perpendicular)
//  return true on success
//
template<class T, class T2>
bool TMatrix44<T, T2>::SetOrientationZY(
	const TVector3<T, T2>& izaxis,
	const TVector3<T, T2>& iyaxis
)
{
	TVector3<T, T2> zaxis(izaxis);
	TVector3<T, T2> yaxis(iyaxis);

	// check whether axes are valid for orientation
	// by examining the cross product

	yaxis.Normalize();
	zaxis.Normalize();
	Vector3 xaxis = yaxis * zaxis;
	if (xaxis.x == 0.0 &&
			xaxis.y == 0.0 &&
			xaxis.z == 0.0)
	{
		return false;// invalid axes, no effect
	}
	xaxis.Normalize();

	yaxis = zaxis * xaxis;
	yaxis.Normalize();

	m[0][0] = xaxis.x;
	m[0][1] = xaxis.y;
	m[0][2] = xaxis.z;
	m[1][0] = yaxis.x;
	m[1][1] = yaxis.y;
	m[1][2] = yaxis.z;
	m[2][0] = zaxis.x;
	m[2][1] = zaxis.y;
	m[2][2] = zaxis.z;
	return true;
}
//---------------------------------------------------------------------------
//
template<class T, class T2>
void TMatrix44<T, T2>::ResetOrientation(void)
{
	m[1][0] = m[2][0] = m[0][3] =
		m[0][1] = m[2][1] = m[1][3] =
			m[0][2] = m[1][2] = m[2][3] = 0;
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
}
//---------------------------------------------------------------------------
//
template<class T, class T2>
void TMatrix44<T, T2>::InsertGIGRotXYZ(T rotx, T roty, T rotz)
{
	T sx = sin(rotx);
	T cx = cos(rotx);
	T sy = sin(roty);
	T cy = cos(roty);
	T sz = sin(rotz);
	T cz = cos(rotz);
	m[0][0] = cy * cz;
	m[2][0] = -cy * sz;
	m[1][0] = sy;
	m[0][2] = cx * sz + sx * sy * cz;
	m[2][2] = cx * cz - sx * sy * sz;
	m[1][2] = -sx * cy;
	m[0][1] = sx * sz - cx * sy * cz;
	m[2][1] = sx * cz + cx * sy * sz;
	m[1][1] = cx * cy;
}
//---------------------------------------------------------------------------
//
template<class T, class T2>
ostream& TMatrix44<T, T2>::Write(ostream& os) const
{
	os << '(' << m[0][0] << ',' << m[0][1] << ',' << m[0][2] << ',' << m[0][3] << ')';
	os << '(' << m[1][0] << ',' << m[1][1] << ',' << m[1][2] << ',' << m[1][3] << ')';
	os << '(' << m[2][0] << ',' << m[2][1] << ',' << m[2][2] << ',' << m[2][3] << ')';
	os << '(' << m[3][0] << ',' << m[3][1] << ',' << m[3][2] << ',' << m[3][3] << ')';
	return os;
}
//---------------------------------------------------------------------------
//
template<class T, class T2>
istream& TMatrix44<T, T2>::getdouble(istream& is, T& d, char delim)
{
	const len = 30;
	char buf[len + 1];
	is.getline(buf, len, delim);
	if (is.good())
		d = strtod(buf, NULL);
	return is;
}
//---------------------------------------------------------------------------
//
template<class T, class T2>
istream& TMatrix44<T, T2>::Read(istream& is)
{
	char c;
	is >> c;
	getdouble(is, m[0][0], ',');// >> c;
	getdouble(is, m[0][1], ',');// >> c;
	getdouble(is, m[0][2], ',');// >> c;
	getdouble(is, m[0][3], ')');// >> c;
	is >> c;
	getdouble(is, m[1][0], ',');// >> c;
	getdouble(is, m[1][1], ',');// >> c;
	getdouble(is, m[1][2], ',');// >> c;
	getdouble(is, m[1][3], ')');// >> c;
	is >> c;
	getdouble(is, m[2][0], ',');// >> c;
	getdouble(is, m[2][1], ',');// >> c;
	getdouble(is, m[2][2], ',');// >> c;
	getdouble(is, m[2][3], ')');// >> c;
	is >> c;
	getdouble(is, m[3][0], ',');// >> c;
	getdouble(is, m[3][1], ',');// >> c;
	getdouble(is, m[3][2], ',');// >> c;
	getdouble(is, m[3][3], ')');// >> c;
	return is;
}
