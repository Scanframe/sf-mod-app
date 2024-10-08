/*
Purpose:
   The TVector4<T> class is an array that contains 4 floats
      Examples of usage:
        TVector4<T> Vector4A(20, 33, 1);
        TVector4<T> Vector4B(-10, 3, 2);
        cout << Vector4A << " + " << Vector4B<< " = ";
        cout << (Vector4A + Vector4B) << endl;

*/

// Forward definition of template.
template<class T, class T2>
class TVector3;

// Floating point array with length 4
template<class T, class T2>
class TVector4
{
	public:
		typedef T value_type;

		// The array data
		T x;
		T y;
		T z;
		T w;

		// Constructors
		TVector4<T, T2>();
		TVector4<T, T2>(const TVector4<T, T2>&);
		TVector4<T, T2>(T x, T y, T z, T w);

		// Array access operator.
		T& operator[](int);
		const T& operator[](int) const;
		// Cast operator.
		operator T*();
		operator const T*() const;
		// For when the value type is not a float.
		operator const float*() const;

		// assignment
		TVector4<T, T2>& assign(T x, T y, T z, T w);

		// math operators
		TVector4<T, T2> operator-() const;
		TVector4<T, T2>& operator+=(const TVector4<T, T2>&);
		TVector4<T, T2>& operator+=(const TVector3<T, T2>&);
		TVector4<T, T2>& operator-=(const TVector4<T, T2>&);
		TVector4<T, T2>& operator-=(const TVector3<T, T2>&);
		TVector4<T, T2>& operator*=(T);
		TVector4<T, T2>& operator/=(T);

		TVector4<T, T2> operator+(const TVector4<T, T2>&) const;
		TVector4<T, T2> operator+(const TVector3<T, T2>&) const;
		TVector4<T, T2> operator-(const TVector4<T, T2>&) const;
		TVector4<T, T2> operator-(const TVector3<T, T2>&) const;
		TVector4<T, T2> operator/(T) const;

		bool operator==(const TVector4<T, T2>&) const;
		bool operator!=(const TVector4<T, T2>&) const;

		// conversion
		void copyTo(T[4]) const;
		// Dumps data to stream.
		ostream& Write(ostream& os) const;
		// vector4 stuff
		// operations
		T2 length(void) const;
		T lengthSqr(void) const;
		T2 length3d(void) const;
		T lengthSqr3d(void) const;
		void normalize(void);

		// conversions
		TVector3<T, T2> ToVector3(void) const;

		// dot product
		T dot(const TVector4<T, T2>&) const;
		T dot3d(const TVector4<T, T2>&) const;

		// point
		// distance
		T distanceSqr(const TVector4<T, T2>&);
		T2 distance(const TVector4<T, T2>&);
		T distanceSqr3d(const TVector4<T, T2>&);
		T2 distance3d(const TVector4<T, T2>&);
};
//---------------------------------------------------------------------------
//
inline template<class T, class T2>
ostream& operator<<(ostream& os, const TVector4<T, T2>& v)
{
	return v.Dump(os);
}
//---------------------------------------------------------------------------
//
inline template<class T, class T2>
TVector4<T, T2>::operator const T*() const
{
	return &(*this)[0];
}
//---------------------------------------------------------------------------
//
inline template<class T, class T2>
TVector4<T, T2>::operator T*()
{
	return &(*this)[0];
}
//---------------------------------------------------------------------------
//
template<class T, class T2>
TVector4<T, T2>::operator const float*() const
{
	static __thread float flts[4];
	flts[0] = x;
	flts[1] = y;
	flts[2] = z;
	flts[3] = w;
	return flts;
}
//---------------------------------------------------------------------------
//
// Default Constructor
//
inline template<class T, class T2>
TVector4<T, T2>::TVector4()
	: x(0)
	, y(0)
	, z(0)
	, w(0)
{
}
//---------------------------------------------------------------------------
//
// Copy Constructor
//
inline template<class T, class T2>
TVector4<T, T2>::TVector4(const TVector4<T, T2>& v)
	: x(v.x)
	, y(v.y)
	, z(v.z)
	, w(v.w)
{
}
//---------------------------------------------------------------------------
//
// Element Constructor
//
inline template<class T, class T2>
TVector4<T, T2>::TVector4(T ix, T iy, T iz, T iw)
	: x(ix)
	, y(iy)
	, z(iz)
	, w(iw)
{
}
//---------------------------------------------------------------------------
//
// Array Access
//
template<class T, class T2>
T& TVector4<T, T2>::operator[](int i)
{
	switch (i)
	{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
	}
	assert(false);
	return x;
}
//
//
template<class T, class T2>
const T& TVector4<T, T2>::operator[](int i) const
{
	assert(i >= 0 && i <= 3);
	switch (i)
	{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
	}
	assert(false);
	return x;
}
//---------------------------------------------------------------------------
//
// Assignment
//
inline template<class T, class T2>
TVector4<T, T2>& TVector4<T, T2>::assign(T ix, T iy, T iz, T iw)
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
	return *this;
}
//
#if 0
// assignment operator slows things down considerably !!
// e.g. a = b + c; is then much slower, so we do not define this operator
//---------------------------------------------------------------------------
//
// TVector4<T,T2> = TVector4<T,T2>
//
inline
template <class T, class T2>
TVector4<T,T2>& TVector4<T,T2>::operator =(const TVector4<T,T2>& v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  w = v.w;
  return *this;
}
#endif
//---------------------------------------------------------------------------
// Math Operators
//---------------------------------------------------------------------------
//
// - TVector4<T,T2>
//
inline template<class T, class T2>
TVector4<T, T2> TVector4<T, T2>::operator-() const
{
	return TVector4<T, T2>(-x, -y, -z, -w);
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> += TVector4<T,T2>
//
inline template<class T, class T2>
TVector4<T, T2>& TVector4<T, T2>::operator+=(const TVector4<T, T2>& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> -= TVector4<T,T2>
//
inline template<class T, class T2>
TVector4<T, T2>& TVector4<T, T2>::operator-=(const TVector4<T, T2>& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> += Vector3
//
inline template<class T, class T2>
TVector4<T, T2>& TVector4<T, T2>::operator+=(const TVector3<T, T2>& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> -= Vector3
//
inline template<class T, class T2>
TVector4<T, T2>& TVector4<T, T2>::operator-=(const TVector3<T, T2>& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> *= T
//
inline template<class T, class T2>
TVector4<T, T2>& TVector4<T, T2>::operator*=(T c)
{
	x *= c;
	y *= c;
	z *= c;
	w *= c;
	return *this;
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> /= T
//
inline template<class T, class T2>
TVector4<T, T2>& TVector4<T, T2>::operator/=(T c)
{
	x /= c;
	y /= c;
	z /= c;
	w /= c;
	return *this;
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> + TVector4<T,T2>
//
inline template<class T, class T2>
TVector4<T, T2> TVector4<T, T2>::operator+(const TVector4<T, T2>& v) const
{
	return TVector4<T, T2>(x + v.x, y + v.y, z + v.z, w + v.w);
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> - TVector4<T,T2>
//
inline template<class T, class T2>
TVector4<T, T2> TVector4<T, T2>::operator-(const TVector4<T, T2>& v) const
{
	return TVector4<T, T2>(x - v.x, y - v.y, z - v.z, w - v.w);
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> + Vector3
//
inline template<class T, class T2>
TVector4<T, T2> TVector4<T, T2>::operator+(const TVector3<T, T2>& v) const
{
	return TVector4<T, T2>(x + v.x, y + v.y, z + v.z, w);
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> - Vector3
//
inline template<class T, class T2>
TVector4<T, T2> TVector4<T, T2>::operator-(const TVector3<T, T2>& v) const
{
	return TVector4<T, T2>(x - v.x, y - v.y, z - v.z, w);
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> / T
//
inline template<class T, class T2>
TVector4<T, T2> TVector4<T, T2>::operator/(T c) const
{
	//  assert(!IsZero(c));
	return TVector4<T, T2>(x / c, y / c, z / c, w / c);
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> == TVector4<T,T2>
//
inline template<class T, class T2>
bool TVector4<T, T2>::operator==(const TVector4<T, T2>& v) const
{
	return ((x == v.x) && (y == v.y) && (z == v.z) && (w == v.w));
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> != TVector4<T,T2>
//
inline template<class T, class T2>
bool TVector4<T, T2>::operator!=(const TVector4<T, T2>& v) const
{
	return (!(*this == v));
}
//---------------------------------------------------------------------------
//
// T * TVector4<T,T2>
//
inline template<class T, class T2>
TVector4<T, T2> operator*(T c, TVector4<T, T2> const& v)
{
	return TVector4<T, T2>(v.x * c, v.y * c, v.z * c, v.w * c);
}
//---------------------------------------------------------------------------
//
// TVector4<T,T2> * T
//
inline template<class T, class T2>
TVector4<T, T2> operator*(TVector4<T, T2> const& v, T c)
{
	return TVector4<T, T2>(v.x * c, v.y * c, v.z * c, v.w * c);
}
//---------------------------------------------------------------------------
//
// copyTo
//
inline template<class T, class T2>
void TVector4<T, T2>::copyTo(T f[4]) const
{
	f[0] = x;
	f[1] = y;
	f[2] = z;
	f[3] = w;
}

inline template<class T, class T2>
ostream& TVector4<T, T2>::Write(ostream& os) const
{
	return os << '(' << x << ',' << y << ',' << z << ',' << w << ')';
}
//---------------------------------------------------------------------------
//
// length : returns the length of the vector4 as a double, note that
//          this has to return a double because it uses the sqrt function
//
inline template<class T, class T2>
T2 TVector4<T, T2>::length() const
{
	return sqrt(lengthSqr());
}
//---------------------------------------------------------------------------
//
// lengthSqr : returns the squared length of the vector4
//
inline template<class T, class T2>
T TVector4<T, T2>::lengthSqr() const
{
	return x * x + y * y + z * z + w * w;
}
//---------------------------------------------------------------------------
//
// length3d : returns the length of the vector4 as a double, note that
//          this has to return a double because it uses the sqrt function
//
inline template<class T, class T2>
T2 TVector4<T, T2>::length3d() const
{
	return sqrt(lengthSqr3d());
}
//---------------------------------------------------------------------------
//
// lengthSqr3d : returns the squared 3d length of the vector4
//
inline template<class T, class T2>
T TVector4<T, T2>::lengthSqr3d() const
{
	return x * x + y * y + z * z;
}
//---------------------------------------------------------------------------
//
// normalize : will normalize the vector4, make it of length 1
//
inline template<class T, class T2>
void TVector4<T, T2>::normalize(void)
{
	T2 len = length();
	if (len != 0.0)
		*this /= ((T) len);// avoid SIGFPE
}
//
// ToVector3
//
inline template<class T, class T2>
TVector3<T, T2> TVector4<T, T2>::ToVector3(void) const
{
	return TVector3<T, T2>(x, y, z);
}
//---------------------------------------------------------------------------
//
// dot : returns the dot product of 2 vectors
//
inline template<class T, class T2>
T TVector4<T, T2>::dot(const TVector4<T, T2>& v2) const
{
	return x * v2.x + y * v2.y + z * v2.z + w * v2.w;
}
//---------------------------------------------------------------------------
//
// dot3d : returns the dot product of 2 vector4s for 3d only
//

inline template<class T, class T2>
T TVector4<T, T2>::dot3d(const TVector4<T, T2>& v2) const
{
	return x * v2.x + y * v2.y + z * v2.z;
}
//---------------------------------------------------------------------------
//
// distanceSqr : return the squared distance between 2 given points which
//               avoids taking an expensive sqrt call. Usefull when comparing
//               different distances.
//
inline template<class T, class T2>
T TVector4<T, T2>::distanceSqr(const TVector4<T, T2>& v2)
{
	return (x - v2.x) * (x - v2.x) +
		(y - v2.y) * (y - v2.y) +
		(z - v2.z) * (z - v2.z) +
		(w - v2.w) * (w - v2.w);
}
//---------------------------------------------------------------------------
//
// distance : return the distance between 2 given points, note that
//            this has to return a double because it uses the sqrt function
//
inline template<class T, class T2>
T2 TVector4<T, T2>::distance(const TVector4<T, T2>& v2)
{
	return sqrt(distanceSqr(v2));
}
//---------------------------------------------------------------------------
//
// distanceSqr3d : return the squared distance between 2 given points in 3d
//                 only (z is ignored)
//
inline template<class T, class T2>
T TVector4<T, T2>::distanceSqr3d(const TVector4<T, T2>& v2)
{
	return (x - v2.x) * (x - v2.x) +
		(y - v2.y) * (y - v2.y) +
		(z - v2.z) * (z - v2.z);
}
//---------------------------------------------------------------------------
//
// distance : return the distance between 2 given points, note that
//            this has to return a double because it uses the sqrt function
//
inline template<class T, class T2>
T2 TVector4<T, T2>::distance3d(const TVector4<T, T2>& v2)
{
	return sqrt(distanceSqr3d(v2));
}
//---------------------------------------------------------------------------
#endif// MISC_VECTOR4_H
//---------------------------------------------------------------------------

/*
$Log$
Revision 1.3  2004/03/26 02:24:13  avo
Converted into a templates.

*/
