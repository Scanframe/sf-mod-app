#include <iostream>
#include <misc/gen/math.h>
#include <misc/gen/string.h>
#include <sstream>
#include <valarray>

namespace sf
{

// Forward definition.
template<class T, class T2>
class TMatrix44;

/**
 * Template of 3D vector for floating point values also accessible as an array.
 * @code
 * TVector3<double> Vector3A(20, 33, 1), Vector3B(-10, 3, 2);
 * std::cout << Vector3A << " + " << Vector3B<< " = ";
 * std::cout << (Vector3A + Vector3B) << endl;
 * @endcode
*/
template<class T, class T2 = T>
class TVector3
{
	private:
		// Make the values x,y and z also accessible as an array.
		union __attribute__((packed))
		{
				T array[3];
				struct
				{
						T x;
						T y;
						T z;
				};
		} _data{0, 0, 0};

	public:
		typedef T value_type;

		// constructors
		TVector3();
		TVector3(const TVector3&);
		TVector3(T x, T y, T z);

		// array access
		T& operator[](int);
		const T& operator[](int) const;

		// assignment
		TVector3& Assign(T x, T y, T z);
		/**
		 * @brief Assignment operator.
		 *
		 * The assignment operator slows things down considerably !
		 * e.g. a = b + c; is then much slower.
		 */
		//
		TVector3& operator=(const TVector3& v);

		// math operators
		TVector3<T, T2> operator-() const;
		TVector3<T, T2>& operator+=(const TVector3&);
		TVector3<T, T2>& operator-=(const TVector3&);
		TVector3<T, T2>& operator*=(T);
		TVector3<T, T2>& operator/=(T);
		TVector3<T, T2> operator+(const TVector3&) const;
		TVector3<T, T2> operator-(const TVector3&) const;
		TVector3<T, T2> operator/(T) const;

		bool operator==(const TVector3&) const;
		bool operator!=(const TVector3&) const;

		// Compares the passed vector within the set accuracy
		bool IsEqual(const TVector3&, T accuracy) const;
		// conversion
		void CopyTo(T[3]) const;
		std::ostream& Write(std::ostream& os) const;
		std::istream& Read(std::istream& is);
		// Strings functions.
		std::string GetString() const;
		void SetString(std::string s);

		// Cast operators.
		operator T*();
		operator const T*() const;
		// For when the value type is not a float.
		operator const float*() const;

		// Returns the length of the vector3 as a double, note that
		// this has to return a double because it uses the sqrt function.
		T2 Length() const;
		// Returns the squared length of the vector3
		T LengthSqr() const;
		// Returns the length of the vector3 as a double, note that
		// this has to return a double because it uses the sqrt function.
		T2 Length2d() const;
		// Returns the squared 2d length of the vector3.
		T LengthSqr2d() const;
		// Will normalize the vector3, make it of length 1
		TVector3& Normalize();
		// Returns a normalized copy.
		TVector3 Normalized() const;
		// Returns the dominant axis index (x=0, y=1, z=2)
		int DominantAxis();

		// Returns the cross (out) product of 2 vectors.
		TVector3<T, T2> operator*(const TVector3&) const;
		TVector3<T, T2> operator*(const TMatrix44<T, T2>&) const;
		TVector3<T, T2>& operator*=(const TMatrix44<T, T2>&);

		// Returns the dot (in) product of 2 vectors
		T Dot(const TVector3<T, T2>&) const;
		// Returns the dot (in) product of 2 vectors for 2d only
		T Dot2d(const TVector3<T, T2>&) const;
		// Returns the amgle between the two vectors.
		// Angle can be always between -180 and +180 degrees (in radials of course)
		T2 Angle(const TVector3<T, T2>& v) const;

		// Returns the distance between 2 given points, note that
		// this has to return a double because it uses the sqrt function.
		T2 Distance(const TVector3<T, T2>&) const;
		// Returns the squared distance between 2 given points which
		// avoids taking an expensive sqrt call.
		// Useful when comparing different distances.
		T DistanceSqr(const TVector3<T, T2>&) const;
		// Returns the squared distance between 2 given points in
		// 2d only (z is ignored).
		T DistanceSqr2d(const TVector3<T, T2>&) const;

		// Returns the distance between 2 given points, note that
		// this has to return a double because it uses the sqrt function
		T2 Distance2d(const TVector3<T, T2>&) const;

		// Copy only those values of x,y or z which are smaller.
		void UpdateMin(const TVector3<T, T2>& vertex);
		// Copy only those values of x,y or z which are larger.
		void UpdateMax(const TVector3<T, T2>& vertex);

		// Reads a double value from the stream.
		std::istream& getdouble(std::istream& is, T& d, char delim);
};

template<class T, class T2>
std::ostream& operator<<(std::ostream& os, const TVector3<T, T2>& v)
{
	return v.Write(os);
}

template<class T, class T2>
TVector3<T, T2>::TVector3()
{
}

template<class T, class T2>
TVector3<T, T2>::operator const T*() const
{
	return &(*this)[0];
}

template<class T, class T2>
TVector3<T, T2>::operator T*()
{
	return &(*this)[0];
}

template<class T, class T2>
TVector3<T, T2>::operator const float*() const
{
	static __thread float flts[3];
	flts[0] = _data.x;
	flts[1] = _data.y;
	flts[2] = _data.z;
	return flts;
}

template<class T, class T2>
TVector3<T, T2>::TVector3(const TVector3<T, T2>& v)
	: _data(v._data)
{
}

template<class T, class T2>
TVector3<T, T2>::TVector3(T ix, T iy, T iz)
{
	_data.array = {ix, iy, iz};
}

template<class T, class T2>
T& TVector3<T, T2>::operator[](int i)
{
	if (i < 0 || i >= 3)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[i];
}

template<class T, class T2>
const T& TVector3<T, T2>::operator[](int i) const
{
	if (i < 0 || i >= 3)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[i];
}

template<class T, class T2>
TVector3<T, T2>& TVector3<T, T2>::Assign(T ix, T iy, T iz)
{
	_data.array = {ix, iy, iz};
	return *this;
}

template<class T, class T2>
TVector3<T, T2>& TVector3<T, T2>::operator=(const TVector3<T, T2>& v)
{
	_data = v._data;
	return *this;
}

template<class T, class T2>
TVector3<T, T2> TVector3<T, T2>::operator-() const
{
	return TVector3<T, T2>(-_data.x, -_data.y, -_data.z);
}

template<class T, class T2>
TVector3<T, T2>& TVector3<T, T2>::operator+=(const TVector3<T, T2>& v)
{
	_data.x += v._data.x;
	_data.y += v._data.y;
	_data.z += v._data.z;
	return *this;
}

template<class T, class T2>
TVector3<T, T2>& TVector3<T, T2>::operator-=(const TVector3<T, T2>& v)
{
	_data.x -= v._data.x;
	_data.y -= v._data.y;
	_data.z -= v._data.z;
	return *this;
}

template<class T, class T2>
TVector3<T, T2>& TVector3<T, T2>::operator*=(T c)
{
	_data.x *= c;
	_data.y *= c;
	_data.z *= c;
	return *this;
}

template<class T, class T2>
TVector3<T, T2> TVector3<T, T2>::operator*(const TMatrix44<T, T2>& mtx) const
{
	return mtx * *this;
}

template<class T, class T2>
TVector3<T, T2>& TVector3<T, T2>::operator*=(const TMatrix44<T, T2>& mtx)
{
	return *this = (mtx * *this);
}

template<class T, class T2>
TVector3<T, T2>& TVector3<T, T2>::operator/=(T c)
{
	T inv{1.0};
	inv /= c;
	_data.x *= inv;
	_data.y *= inv;
	_data.z *= inv;
	return *this;
}

template<class T, class T2>
TVector3<T, T2> TVector3<T, T2>::operator+(const TVector3<T, T2>& v) const
{
	return {_data.x + v._data.x, _data.y + v._data.y, _data.z + v._data.z};
}

template<class T, class T2>
TVector3<T, T2> TVector3<T, T2>::operator-(const TVector3<T, T2>& v) const
{
	return {_data.x - v._data.x, _data.y - v._data.y, _data.z - v._data.z};
}

template<class T, class T2>
TVector3<T, T2> TVector3<T, T2>::operator/(T c) const
{
	return {_data.x / c, _data.y / c, _data.z / c};
}

template<class T, class T2>
bool TVector3<T, T2>::operator==(const TVector3<T, T2>& v) const
{
	// Probably useful with a floating point value.
	return (
		(_data.x == v._data.x) &&
		(_data.y == v._data.y) &&
		(_data.z == v._data.z)
	);
}

template<class T, class T2>
bool TVector3<T, T2>::operator!=(const TVector3<T, T2>& v) const
{
	// Probably useful with a floating point value.
	return !(
		(_data.x == v._data.x) &&
		(_data.y == v._data.y) &&
		(_data.z == v._data.z)
	);
}

template<class T, class T2>
bool TVector3<T, T2>::IsEqual(const TVector3<T, T2>& v, T accuracy) const
{
	return (
		toAbs(_data.x - v._data.x) < accuracy &&
		toAbs(_data.y - v._data.y) < accuracy &&
		toAbs(_data.z - v._data.z) < accuracy
	);
}

template<class T, class T2>
TVector3<T, T2> operator*(T c, TVector3<T, T2> const& v)
{
	return {v._data.x * c, v._data.y * c, v._data.z * c};
}

template<class T, class T2>
TVector3<T, T2> operator*(TVector3<T, T2> const& v, T c)
{
	return {v._data.x * c, v._data.y * c, v._data.z * c};
}

template<class T, class T2>
void TVector3<T, T2>::CopyTo(T f[3]) const
{
	_data.array = f;
}

template<class T, class T2>
std::istream& TVector3<T, T2>::getdouble(std::istream& is, T& d, char delim)
{
	const auto len = 30;
	char buf[len + 1];
	is.getline(buf, len, delim);
	if (is.good())
		d = strtod(buf, NULL);
	return is;
}

template<class T, class T2>
std::ostream& TVector3<T, T2>::Write(std::ostream& os) const
{
	return os << '(' << _data.x << ',' << _data.y << ',' << _data.z << ')';
}

template<class T, class T2>
std::istream& TVector3<T, T2>::Read(std::istream& is)
{
	char c;
	is >> c;
	getdouble(is, _data.x, ',');
	getdouble(is, _data.y, ',');
	getdouble(is, _data.z, ')');
	return is;
}

template<class T, class T2>
std::string TVector3<T, T2>::GetString() const
{
	return std::stringstream() << toString<T>(_data.x) << ',' << toString<T>(_data.y) << ',' << toString<T>(_data.z).str();
}

template<class T, class T2>
void TVector3<T, T2>::SetString(std::string s)
{
	std::string::size_type pos = 0;
	_data.x = toNumber<T>(s, &pos);
	s = s.substr(pos + 1);
	_data.y = toNumber<T>(s, &pos);
	s = s.substr(pos + 1);
	_data.z = toNumber<T>(s, &pos);
}

template<class T, class T2>
T2 TVector3<T, T2>::Length() const
{
	return sqrt(LengthSqr());
}

template<class T, class T2>
T TVector3<T, T2>::LengthSqr() const
{
	return _data.x * _data.x + _data.y * _data.y + _data.z * _data.z;
}

template<class T, class T2>
T2 TVector3<T, T2>::Length2d() const
{
	return sqrt(LengthSqr2d());
}

template<class T, class T2>
T TVector3<T, T2>::LengthSqr2d() const
{
	return _data.x * _data.x + _data.y * _data.y;
}

template<class T, class T2>
TVector3<T, T2>& TVector3<T, T2>::Normalize(void)
{
	T2 len = Length();
	if (len != 0.0)
	{
		// avoid SIGFPE
		*this /= (T) len;
	}
	return *this;
}

template<class T, class T2>
TVector3<T, T2> TVector3<T, T2>::Normalized() const
{
	return TVector3<T, T2>(_data.array).Normalize();
}

template<class T, class T2>
int TVector3<T, T2>::DominantAxis(void)
{
	if (toAbs(_data.x) > toAbs(_data.y))
	{
		if (toAbs(_data.x) > toAbs(_data.z))
			return 0;
		else
			return 2;
	}
	else
	{
		if (toAbs(_data.y) > toAbs(_data.z))
			return 1;
		else
			return 2;
	}
}

template<class T, class T2>
T TVector3<T, T2>::Dot(const TVector3<T, T2>& v2) const
{
	return _data.x * v2._data.x + _data.y * v2._data.y + _data.z * v2._data.z;
}

template<class T, class T2>
T2 TVector3<T, T2>::Angle(const TVector3<T, T2>& v) const
{
	// Get sine and cosine of the angle between the vectors.
	// Note: the haven't both been divided by the absolute length
	// between the vectors yet because it is not significant.
	T2 cos_alpha = Dot(v);
	T2 sin_alpha = ((*this) * v).length();
	T2 retval = 0.0;
	try
	{
		// Avoid division by zero.
		if (fabs(cos_alpha) <= std::numeric_limits<T>::min())
		{
			if (cos_alpha < 0.0)
			{
				if (_data.y < 0.0)
					retval = M_PI_2;
				else
					retval = -M_PI_2;
			}
			else
			{
				if (sin_alpha < 0.0)
					retval = -M_PI_2;
				else
					retval = M_PI_2;
			}
		}
		else
		{
			// atan2 takes location in quadrants in to account when returning value.
			retval = atan2(sin_alpha, cos_alpha);
		}
	}
	catch (...)
	{
		retval = 0.0;
		std::cout << "TVector3<T,T2>: Error calculating the angle!";
	}
	// Return a normalized angle.
	return retval;
}

template<class T, class T2>
T TVector3<T, T2>::Dot2d(const TVector3<T, T2>& v2) const
{
	return _data.x * v2._data.x + _data.y * v2._data.y;
}

template<class T, class T2>
TVector3<T, T2> TVector3<T, T2>::operator*(const TVector3<T, T2>& v) const
{
	return {
		_data.y * v._data.z - _data.z * v._data.y,
		-_data.x * v._data.z + _data.z * v._data.x,
		_data.x * v._data.y - _data.y * v._data.x
	};
}

template<class T, class T2>
T TVector3<T, T2>::DistanceSqr(const TVector3<T, T2>& v2) const
{
	return (_data.x - v2._data.x) * (_data.x - v2._data.x) +
		(_data.y - v2._data.y) * (_data.y - v2._data.y) +
		(_data.z - v2._data.z) * (_data.z - v2._data.z);
}

template<class T, class T2>
T2 TVector3<T, T2>::Distance(const TVector3<T, T2>& v2) const
{
	return std::sqrt(DistanceSqr(v2));
}

template<class T, class T2>
T TVector3<T, T2>::DistanceSqr2d(const TVector3<T, T2>& v2) const
{
	return (_data.x - v2._data.x) * (_data.x - v2._data.x) + (_data.y - v2._data.y) * (_data.y - v2._data.y);
}

template<class T, class T2>
T2 TVector3<T, T2>::Distance2d(const TVector3<T, T2>& v2) const
{
	return std::sqrt(DistanceSqr2d(v2));
}

template<class T, class T2>
void TVector3<T, T2>::UpdateMin(const TVector3<T, T2>& vertex)
{
	if (vertex._data.x < _data.x)
		_data.x = vertex._data.x;
	if (vertex._data.y < _data.y)
		_data.y = vertex._data.y;
	if (vertex._data.z < _data.z)
		_data.z = vertex._data.z;
}

template<class T, class T2>
void TVector3<T, T2>::UpdateMax(const TVector3<T, T2>& vertex)
{
	if (vertex._data.x > _data.x)
		_data.x = vertex._data.x;
	if (vertex._data.y > _data.y)
		_data.y = vertex._data.y;
	if (vertex._data.z > _data.z)
		_data.z = vertex._data.z;
}

}// namespace sf
