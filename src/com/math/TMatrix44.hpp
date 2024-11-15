#pragma once

#include <cmath>
#include <misc/gen/dbgutils.h>
#include <misc/gen/string.h>
#include <regex>
#include <sstream>

namespace sf
{

template<typename T>
inline TMatrix44<T>::TMatrix44()
{
	unit();
}

template<typename T>
inline TMatrix44<T>::TMatrix44(T tilt, T pan, T roll)
{
	unit();
	setTiltPanRoll(tilt, pan, roll);
}

template<typename T>
TMatrix44<T>::TMatrix44(const TVector3D<T>& tv)
{
	_data.mtx[1][0] = _data.mtx[2][0] = _data.mtx[0][3] =
		_data.mtx[0][1] = _data.mtx[2][1] = _data.mtx[1][3] =
			_data.mtx[0][2] = _data.mtx[1][2] = _data.mtx[2][3] = 0;
	_data.mtx[3][0] = tv.x;
	_data.mtx[3][1] = tv.y;
	_data.mtx[3][2] = tv.z;
	_data.mtx[0][0] = _data.mtx[1][1] = _data.mtx[2][2] = _data.mtx[3][3] = 1;
}

// Conversion Constructor: construct a rotation matrix from a quaternion
template<typename T>
TMatrix44<T>::TMatrix44(const TQuaternion<T>& q)
{
	q.InsertInMatrix(_data.mtx);
	_data.mtx[3][0] = 0.0;
	_data.mtx[3][1] = 0.0;
	_data.mtx[3][2] = 0.0;
	_data.mtx[0][3] = 0.0;
	_data.mtx[1][3] = 0.0;
	_data.mtx[2][3] = 0.0;
	_data.mtx[3][3] = 1.0;
}

template<typename T>
TMatrix44<T>::TMatrix44(TMatrix44&& m)
{
	_data = m._data;
}

// Copy Constructor
template<typename T>
TMatrix44<T>::TMatrix44(const TMatrix44& m)
{
	_data = m._data;
}

template<typename T>
TMatrix44<T>::TMatrix44(
	T m00, T m01, T m02, T m03,
	T m10, T m11, T m12, T m13,
	T m20, T m21, T m22, T m23,
	T m30, T m31, T m32, T m33
)
{
	_data = {
		m00, m01, m02, m03,
		m10, m11, m12, m13,
		m20, m21, m22, m23,
		m30, m31, m32, m33
	};
}

template<typename T>
TMatrix44<T>::TMatrix44(const T arr[4][4])
{
	std::memcpy(_data.mtx, arr, sizeof(_data));
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::assign(const T arr[4][4])
{
	_data.array = arr;
	return *this;
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::transposeAssign(const T arr[4][4])
{
	_data.array = arr;
	transpose();
	return *this;
}

template<typename T>
T TMatrix44<T>::determinant() const
{
	return _data.mtx[0][0] * (_data.mtx[1][1] * _data.mtx[2][2] - _data.mtx[1][2] * _data.mtx[2][1]) -
		_data.mtx[0][1] * (_data.mtx[1][0] * _data.mtx[2][2] - _data.mtx[1][2] * _data.mtx[2][0]) +
		_data.mtx[0][2] * (_data.mtx[1][0] * _data.mtx[2][1] - _data.mtx[1][1] * _data.mtx[2][0]);
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::invert()
{
	int indxc[4], indxr[4], ipiv[4];
	int i, icol, irow, j, k, l, ll;
	T big, dum, pivinv;
	icol = irow = 0;
	ipiv[0] = ipiv[1] = ipiv[2] = ipiv[3] = 0;

	for (i = 0; i < 4; i++)
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
						if ((dum = std::fabs(_data.mtx[j][k])) >= big)
						{
							big = dum;
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1)
					{
						throw std::range_error(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid matrix!");
					}
				}
			}
		}
		++ipiv[icol];
		if (irow != icol)
		{
			for (l = 0; l < 4; l++)
			{
				std::swap(_data.mtx[irow][l], _data.mtx[icol][l]);
			}
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if ((dum = _data.mtx[icol][icol]) == 0)
		{
			throw std::range_error(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid matrix!");
		}
		pivinv = 1 / dum;
		_data.mtx[icol][icol] = 1;
		for (l = 0; l < 4; l++)
		{
			_data.mtx[icol][l] *= pivinv;
		}
		for (ll = 0; ll < 4; ll++)
		{
			if (ll != icol)
			{
				dum = _data.mtx[ll][icol];
				_data.mtx[ll][icol] = 0;
				for (l = 0; l < 4; l++)
				{
					_data.mtx[ll][l] -= _data.mtx[icol][l] * dum;
				}
			}
		}
	}
	for (l = 3; l >= 0; l--)
	{
		if (indxr[l] != indxc[l])
		{
			for (k = 0; k < 4; k++)
			{
				std::swap(_data.mtx[k][indxr[l]], _data.mtx[k][indxc[l]]);
			}
		}
	}
	return *this;
}

template<typename T>
inline TMatrix44<T> TMatrix44<T>::inverted() const
{
	return TMatrix44(*this).invert();
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::transpose()
{
	_data = {
		_data.mtx[0][0], _data.mtx[1][0], _data.mtx[2][0], _data.mtx[3][0],
		_data.mtx[0][1], _data.mtx[1][1], _data.mtx[2][1], _data.mtx[3][1],
		_data.mtx[0][2], _data.mtx[1][2], _data.mtx[2][2], _data.mtx[3][2],
		_data.mtx[0][3], _data.mtx[1][3], _data.mtx[2][3], _data.mtx[3][3]
	};
	return *this;
}

template<typename T>
TMatrix44<T> TMatrix44<T>::transposed() const
{
	return TMatrix44(*this).transpose();
}

// Unit: construct a unit-matrix
template<typename T>
void TMatrix44<T>::unit()
{
	//_data.array = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	_data.mtx[1][0] = _data.mtx[2][0] = _data.mtx[3][0] =
		_data.mtx[0][1] = _data.mtx[2][1] = _data.mtx[3][1] =
			_data.mtx[0][2] = _data.mtx[1][2] = _data.mtx[3][2] =
				_data.mtx[0][3] = _data.mtx[1][3] = _data.mtx[2][3] = 0.0;
	_data.mtx[0][0] = _data.mtx[1][1] = _data.mtx[2][2] = _data.mtx[3][3] = 1.0;
}

// SetProjectionMatrix
template<typename T>
void TMatrix44<T>::setProjectionMatrix(
	T near_plane,
	T far_plane,
	T fov
)
{
	auto c = std::cos(fov * 0.5);
	auto s = std::sin(fov * 0.5);
	T Q = s / (1.0 - near_plane / far_plane);
	_data.mtx[0][0] = c;
	_data.mtx[0][1] = 0.0;
	_data.mtx[0][2] = 0.0;
	_data.mtx[0][3] = 0.0;
	_data.mtx[1][0] = 0.0;
	_data.mtx[1][1] = c;
	_data.mtx[1][2] = 0.0;
	_data.mtx[1][3] = 0.0;
	_data.mtx[2][0] = 0.0;
	_data.mtx[2][1] = 0.0;
	_data.mtx[2][2] = Q;
	_data.mtx[2][3] = s;
	_data.mtx[3][0] = 0.0;
	_data.mtx[3][1] = 0.0;
	_data.mtx[3][2] = -Q * near_plane;
	_data.mtx[3][3] = 0.0;
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::multiply(const TMatrix44& m)
{
	_data = {
		_data.mtx[0][0] * m._data.mtx[0][0] +
			_data.mtx[0][1] * m._data.mtx[1][0] +
			_data.mtx[0][2] * m._data.mtx[2][0] +
			_data.mtx[0][3] * m._data.mtx[3][0],

		_data.mtx[0][0] * m._data.mtx[0][1] +
			_data.mtx[0][1] * m._data.mtx[1][1] +
			_data.mtx[0][2] * m._data.mtx[2][1] +
			_data.mtx[0][3] * m._data.mtx[3][1],

		_data.mtx[0][0] * m._data.mtx[0][2] +
			_data.mtx[0][1] * m._data.mtx[1][2] +
			_data.mtx[0][2] * m._data.mtx[2][2] +
			_data.mtx[0][3] * m._data.mtx[3][2],

		_data.mtx[0][0] * m._data.mtx[0][3] +
			_data.mtx[0][1] * m._data.mtx[1][3] +
			_data.mtx[0][2] * m._data.mtx[2][3] +
			_data.mtx[0][3] * m._data.mtx[3][3],

		_data.mtx[1][0] * m._data.mtx[0][0] +
			_data.mtx[1][1] * m._data.mtx[1][0] +
			_data.mtx[1][2] * m._data.mtx[2][0] +
			_data.mtx[1][3] * m._data.mtx[3][0],

		_data.mtx[1][0] * m._data.mtx[0][1] +
			_data.mtx[1][1] * m._data.mtx[1][1] +
			_data.mtx[1][2] * m._data.mtx[2][1] +
			_data.mtx[1][3] * m._data.mtx[3][1],

		_data.mtx[1][0] * m._data.mtx[0][2] +
			_data.mtx[1][1] * m._data.mtx[1][2] +
			_data.mtx[1][2] * m._data.mtx[2][2] +
			_data.mtx[1][3] * m._data.mtx[3][2],

		_data.mtx[1][0] * m._data.mtx[0][3] +
			_data.mtx[1][1] * m._data.mtx[1][3] +
			_data.mtx[1][2] * m._data.mtx[2][3] +
			_data.mtx[1][3] * m._data.mtx[3][3],

		_data.mtx[2][0] * m._data.mtx[0][0] +
			_data.mtx[2][1] * m._data.mtx[1][0] +
			_data.mtx[2][2] * m._data.mtx[2][0] +
			_data.mtx[2][3] * m._data.mtx[3][0],

		_data.mtx[2][0] * m._data.mtx[0][1] +
			_data.mtx[2][1] * m._data.mtx[1][1] +
			_data.mtx[2][2] * m._data.mtx[2][1] +
			_data.mtx[2][3] * m._data.mtx[3][1],

		_data.mtx[2][0] * m._data.mtx[0][2] +
			_data.mtx[2][1] * m._data.mtx[1][2] +
			_data.mtx[2][2] * m._data.mtx[2][2] +
			_data.mtx[2][3] * m._data.mtx[3][2],

		_data.mtx[2][0] * m._data.mtx[0][3] +
			_data.mtx[2][1] * m._data.mtx[1][3] +
			_data.mtx[2][2] * m._data.mtx[2][3] +
			_data.mtx[2][3] * m._data.mtx[3][3],

		_data.mtx[3][0] * m._data.mtx[0][0] +
			_data.mtx[3][1] * m._data.mtx[1][0] +
			_data.mtx[3][2] * m._data.mtx[2][0] +
			_data.mtx[3][3] * m._data.mtx[3][0],

		_data.mtx[3][0] * m._data.mtx[0][1] +
			_data.mtx[3][1] * m._data.mtx[1][1] +
			_data.mtx[3][2] * m._data.mtx[2][1] +
			_data.mtx[3][3] * m._data.mtx[3][1],

		_data.mtx[3][0] * m._data.mtx[0][2] +
			_data.mtx[3][1] * m._data.mtx[1][2] +
			_data.mtx[3][2] * m._data.mtx[2][2] +
			_data.mtx[3][3] * m._data.mtx[3][2],

		_data.mtx[3][0] * m._data.mtx[0][3] +
			_data.mtx[3][1] * m._data.mtx[1][3] +
			_data.mtx[3][2] * m._data.mtx[2][3] +
			_data.mtx[3][3] * m._data.mtx[3][3]
	};
	return *this;
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::operator=(const TMatrix44& m)
{
	return assign(m);
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::operator=(TMatrix44<T>&& m) noexcept
{
	_data = m._data;
	return *this;
}

template<typename T>
inline TMatrix44<T>& TMatrix44<T>::operator*=(const TMatrix44<T>& lhs)
{
	return multiply(lhs);
}

template<typename T>
TVector3D<T> TMatrix44<T>::operator*(const TVector3D<T>& v) const
{
	// Get a local copy of X, Y and Z for fast access.
	T vt[3];
	v.copyTo(vt);
	return {
		_data.mtx[0][0] * vt[0] + _data.mtx[0][1] * vt[1] + _data.mtx[0][2] * vt[2],
		_data.mtx[1][0] * vt[0] + _data.mtx[1][1] * vt[1] + _data.mtx[1][2] * vt[2],
		_data.mtx[2][0] * vt[0] + _data.mtx[2][1] * vt[1] + _data.mtx[2][2] * vt[2]
	};
}

template<typename T>
inline TMatrix44<T>::operator T*()
{
	return &_data.array[0];
}

template<typename T>
inline TMatrix44<T>::operator const T*() const
{
	return &_data.array[0];
}

template<typename T>
bool TMatrix44<T>::isEqual(const TMatrix44& m, T tol) const
{
	// Iterate through the array which makes up the matrix.
	for (size_t i = 0; i < sizeof(_data.array) / sizeof(_data.array[0]); i++)
	{
		// Use the tolerance when comparing.
		if (!sf::isEqual<T>(_data.array[i], m._data.array[i], tol))
		{
			// Bailout on first inequality entry.
			return false;
		}
	}
	return true;
}

template<typename T>
bool TMatrix44<T>::isRotational() const
{
	// A rotation matrix has a determinant of 1.
	if (!sf::isEqual<T>(determinant(), 1.0, tolerance))
	{
		return false;
	}
	// Ensure the passed matrix is a valid rotation matrix by checking the matrix inverse multiplication is an identiy matrix.
	if ((*this) * inverted() != TMatrix44())
	{
		return false;
	}
	return true;
}

template<typename T>
inline bool TMatrix44<T>::operator==(const TMatrix44& m) const
{
	return isEqual(m);
}

template<typename T>
inline bool TMatrix44<T>::operator!=(const TMatrix44& m) const
{
	return !isEqual(m);
}

template<typename T>
void TMatrix44<T>::copyTo(T arr[4][4]) const
{
	std::memcpy(arr, _data.mtx, sizeof(_data));
}

template<typename T>
void TMatrix44<T>::transposeCopyTo(T mat[4][4]) const
{
	mat[0][0] = _data.mtx[0][0];
	mat[1][0] = _data.mtx[0][1];
	mat[2][0] = _data.mtx[0][2];
	mat[3][0] = _data.mtx[0][3];
	mat[0][1] = _data.mtx[1][0];
	mat[1][1] = _data.mtx[1][1];
	mat[2][1] = _data.mtx[1][2];
	mat[3][1] = _data.mtx[1][3];
	mat[0][2] = _data.mtx[2][0];
	mat[1][2] = _data.mtx[2][1];
	mat[2][2] = _data.mtx[2][2];
	mat[3][2] = _data.mtx[2][3];
	mat[0][3] = _data.mtx[3][0];
	mat[1][3] = _data.mtx[3][1];
	mat[2][3] = _data.mtx[3][2];
	mat[3][3] = _data.mtx[3][3];
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::setTiltPanRoll(T tilt, T pan, T roll)
{
	auto sx = std::sin(tilt);
	auto sy = std::sin(pan);
	auto sz = std::sin(roll);
	auto cx = std::cos(tilt);
	auto cy = std::cos(pan);
	auto cz = std::cos(roll);
	_data.mtx[0][0] = cy * cz + sy * sx * sz;
	_data.mtx[1][0] = sy * sx * cz - cy * sz;
	_data.mtx[2][0] = sy * cx;
	_data.mtx[0][1] = cx * sz;
	_data.mtx[1][1] = cx * cz;
	_data.mtx[2][1] = -sx;
	_data.mtx[0][2] = cy * sx * sz - sy * cz;
	_data.mtx[1][2] = cy * sx * cz + sy * sz;
	_data.mtx[2][2] = cy * cx;
	return *this;
}

template<typename T>
void TMatrix44<T>::getTiltPanRoll(T& tilt, T& pan, T& roll) const
{
	T sx = -_data.mtx[2][1];
	T cx = sqrt(1.0 - sx * sx);
	// Check for a small value of cx.
	if (isZero(cx, tolerance))
	{
		tilt = sx * 2.0 * numbers::pi_v<T>;
		pan = 0.0f;
		roll = std::atan2(-_data.mtx[1][0], _data.mtx[0][0]);
	}
	else
	{
		tilt = std::atan2(sx, cx);
		pan = std::atan2(_data.mtx[2][0], _data.mtx[2][2]);
		roll = std::atan2(_data.mtx[0][1], _data.mtx[1][1]);
	}
}

template<typename T>
inline TMatrix44<T>& TMatrix44<T>::rotate(T x, T y, T z)
{
	return multiply(TMatrix44<T>().setTiltPanRoll(x, y, z));
}

template<typename T>
inline void TMatrix44<T>::setPos(const TVector3D<T>& v)
{
	_data.mtx[3][0] = v.x();
	_data.mtx[3][1] = v.y();
	_data.mtx[3][2] = v.z();
}

template<typename T>
inline void TMatrix44<T>::setPos(T x, T y, T z)
{
	_data.mtx[3][0] = x;
	_data.mtx[3][1] = y;
	_data.mtx[3][2] = z;
}

// TMatrix44.ClrPos
//  overrides translation part of matrix to zero.
template<typename T>
inline void TMatrix44<T>::clearPos()
{
	_data.mtx[3][0] = _data.mtx[3][1] = _data.mtx[3][2] = 0.0;
}

template<typename T>
inline TVector3D<T> TMatrix44<T>::getPos(void) const
{
	return {_data.mtx[3][0], _data.mtx[3][1], _data.mtx[3][2]};
}

template<typename T>
inline TVector3D<T> TMatrix44<T>::getAxis(EAxis axis) const
{
	if (axis >= 4)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return {_data.mtx[axis][0], _data.mtx[axis][1], _data.mtx[axis][2]};
}

template<typename T>
TMatrix44<T> TMatrix44<T>::orbit(T horizontal, T vertical) const
{
	auto x_axis = getAxis(axisX);
	auto y_axis = getAxis(axisY);
	TQuaternion<T> h_q(y_axis, horizontal);
	TQuaternion<T> v_q(x_axis, vertical);
	TQuaternion<T> qmul = v_q * h_q;
	// Was left out?
	qmul.Normalize();
	return TMatrix44<T>(qmul);
}

template<typename T>
TMatrix44<T> TMatrix44<T>::orientation() const
{
	return TMatrix44<T>(
		_data.mtx[0][0], _data.mtx[0][1], _data.mtx[0][2], 0.0,
		_data.mtx[1][0], _data.mtx[1][1], _data.mtx[1][2], 0.0,
		_data.mtx[2][0], _data.mtx[2][1], _data.mtx[2][2], 0.0,
		0.0, 0.0, 0.0, 1.0
	);
}

template<typename T>
inline void TMatrix44<T>::setElement(unsigned int row, unsigned int column, T value)
{
	if (row >= 4 || column >= 4)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	_data.mtx[row][column] = value;
}

template<typename T>
inline void TMatrix44<T>::element(unsigned int row, unsigned int column, T value) const
{
	if (row >= 4 || column >= 4)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.mtx[row][column];
}

template<typename T>
TQuaternion<T> TMatrix44<T>::quaternion() const
{
	return TQuaternion<T>().fromMatrix(_data.mtx);
}

template<typename T>
bool TMatrix44<T>::setOrientationXY(const TVector3D<T>& ixaxis, const TVector3D<T>& iyaxis)
{
	auto xaxis(ixaxis);
	auto yaxis(iyaxis);
	// Check whether axes are valid for orientation by examining the cross product.
	xaxis.normalize();
	yaxis.normalize();
	auto zaxis = xaxis * yaxis;
	if (zaxis.x == 0.0 &&
			zaxis.y == 0.0 &&
			zaxis.z == 0.0)
	{
		// Invalid axes, no effect
		return false;
	}
	zaxis.normalize();
	xaxis = yaxis * zaxis;
	xaxis.normalize();
	_data.mtx[0][0] = xaxis.x;
	_data.mtx[0][1] = xaxis.y;
	_data.mtx[0][2] = xaxis.z;
	_data.mtx[1][0] = yaxis.x;
	_data.mtx[1][1] = yaxis.y;
	_data.mtx[1][2] = yaxis.z;
	_data.mtx[2][0] = zaxis.x;
	_data.mtx[2][1] = zaxis.y;
	_data.mtx[2][2] = zaxis.z;
	return true;
}

template<typename T>
bool TMatrix44<T>::setOrientationZY(const TVector3D<T>& izaxis, const TVector3D<T>& iyaxis)
{
	auto zaxis(izaxis);
	auto yaxis(iyaxis);
	// Check whether axes are valid for orientation by examining the cross product
	yaxis.normalize();
	zaxis.normalize();
	// Cross-product.
	auto xaxis = yaxis * zaxis;
	// Check if the vector is zero or near zero according the tolerance.
	if (xaxis.isEqual(TVector3D<T>(0, 0, 0), tolerance))
	{
		// invalid axes, no effect
		return false;
	}
	xaxis.normalize();
	yaxis = zaxis * xaxis;
	yaxis.normalize();
	//
	_data.mtx[0][0] = xaxis.x();
	_data.mtx[0][1] = xaxis.y();
	_data.mtx[0][2] = xaxis.z();
	_data.mtx[1][0] = yaxis.x();
	_data.mtx[1][1] = yaxis.y();
	_data.mtx[1][2] = yaxis.z();
	_data.mtx[2][0] = zaxis.x();
	_data.mtx[2][1] = zaxis.y();
	_data.mtx[2][2] = zaxis.z();
	return true;
}

template<typename T>
void TMatrix44<T>::resetOrientation(void)
{
	_data.mtx[1][0] = _data.mtx[2][0] = _data.mtx[0][3] =
		_data.mtx[0][1] = _data.mtx[2][1] = _data.mtx[1][3] =
			_data.mtx[0][2] = _data.mtx[1][2] = _data.mtx[2][3] = 0;
	_data.mtx[0][0] = _data.mtx[1][1] = _data.mtx[2][2] = _data.mtx[3][3] = 1;
}

template<typename T>
void TMatrix44<T>::insertGIGRotXYZ(T rx, T ry, T rz)
{
	auto sx = std::sin(rx);
	auto cx = std::cos(rx);
	auto sy = std::sin(ry);
	auto cy = std::cos(ry);
	auto sz = std::sin(rz);
	auto cz = std::cos(rz);
	_data.mtx[0][0] = cy * cz;
	_data.mtx[2][0] = -cy * sz;
	_data.mtx[1][0] = sy;
	_data.mtx[0][2] = cx * sz + sx * sy * cz;
	_data.mtx[2][2] = cx * cz - sx * sy * sz;
	_data.mtx[1][2] = -sx * cy;
	_data.mtx[0][1] = sx * sz - cx * sy * cz;
	_data.mtx[2][1] = sx * cz + cx * sy * sz;
	_data.mtx[1][1] = cx * cy;
}

template<typename T>
std::string TMatrix44<T>::toString() const
{
	// Lambda function to get the string of each row.
	auto row = [this](int row) -> std::string {
		return std::string() + "{" +
			sf::toString<T>(isZero(_data.mtx[row][0], tolerance) ? T(0) : _data.mtx[row][0]) + ',' +
			sf::toString<T>(isZero(_data.mtx[row][1], tolerance) ? T(0) : _data.mtx[row][1]) + ',' +
			sf::toString<T>(isZero(_data.mtx[row][2], tolerance) ? T(0) : _data.mtx[row][2]) + ',' +
			sf::toString<T>(isZero(_data.mtx[row][3], tolerance) ? T(0) : _data.mtx[row][3]) + '}';
	};
	return '(' + row(0) + ',' + row(1) + ',' + row(2) + ',' + row(3) + ')';
}

template<typename T>
TMatrix44<T>& TMatrix44<T>::fromString(const std::string& s) noexcept(false)
{
	std::string row(R"(\{([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?)\})");
	std::regex re("^\\(" + row + ',' + row + ',' + row + ',' + row + "\\)$", std::regex::icase);
	std::smatch match;
	// Sanity check on the amount of matches.
	if (!std::regex_match(s, match, re) || match.size() != 17)
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid string '" + s + "' conversion!");
	}
	else
	{
		for (size_t i = 0; i <= 16; i++)
		{
			// First match is the group so skip it (+1).
			_data.array[i] = toNumber<T>(match[i + 1].str());
		}
	}
	return *this;
}

}// namespace sf
