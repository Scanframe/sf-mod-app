namespace sf
{

template<typename T>
TVector3D<T>::TVector3D(const TVector3D<T>& v)
	: _data(v._data)
{}

template<typename T>
TVector3D<T>::TVector3D(T xp, T yp, T zp)
	: _data({xp, yp, zp})
{}

template<typename T>
TVector3D<T>& TVector3D<T>::assign(T xp, T yp, T zp)
{
	_data = {xp, yp, zp};
	return *this;
}

template<typename T>
TVector3D<T>& TVector3D<T>::assign(const TVector3D<T>& v)
{
	_data = v._data;
	return *this;
}

template<typename T>
TVector3D<T>& TVector3D<T>::operator=(TVector3D& v) noexcept
{
	_data = v._data;
	return *this;
}

template<typename T>
TVector3D<T>& TVector3D<T>::operator=(TVector3D&& v) noexcept
{
	_data = v._data;
	return *this;
}

template<typename T>
TVector3D<T>& TVector3D<T>::operator*=(const TMatrix44<T>& mtx)
{
	return assign(mtx.transformed(*this));
}

template<typename T>
TVector3D<T> TVector3D<T>::operator*(const TMatrix44<T>& mtx)
{
	return mtx.transformed(*this);
}

template<typename T>
TVector3D<T> TVector3D<T>::operator-() const
{
	return TVector3D(-_data.coord.x, -_data.coord.y, -_data.coord.z);
}

template<typename T>
TVector3D<T>& TVector3D<T>::operator+=(const TVector3D<T>& v)
{
	_data.coord.x += v._data.coord.x;
	_data.coord.y += v._data.coord.y;
	_data.coord.z += v._data.coord.z;
	return *this;
}

template<typename T>
TVector3D<T>& TVector3D<T>::operator-=(const TVector3D<T>& v)
{
	_data.coord.x -= v._data.coord.x;
	_data.coord.y -= v._data.coord.y;
	_data.coord.z -= v._data.coord.z;
	return *this;
}

template<typename T>
TVector3D<T>& TVector3D<T>::operator*=(T c)
{
	_data.coord.x *= c;
	_data.coord.y *= c;
	_data.coord.z *= c;
	return *this;
}

template<typename T>
TVector3D<T>& TVector3D<T>::operator/=(T c)
{
	T inv{1.0};
	inv /= c;
	_data.coord.x *= inv;
	_data.coord.y *= inv;
	_data.coord.z *= inv;
	return *this;
}

template<typename T>
TVector3D<T> TVector3D<T>::operator+(const TVector3D<T>& v) const
{
	return {_data.coord.x + v._data.coord.x, _data.coord.y + v._data.coord.y, _data.coord.z + v._data.coord.z};
}

template<typename T>
TVector3D<T> TVector3D<T>::operator-(const TVector3D<T>& v) const
{
	return {_data.coord.x - v._data.coord.x, _data.coord.y - v._data.coord.y, _data.coord.z - v._data.coord.z};
}

template<typename T>
TVector3D<T> TVector3D<T>::operator/(T c) const
{
	return {_data.coord.x / c, _data.coord.y / c, _data.coord.z / c};
}

template<typename T>
bool TVector3D<T>::isEqual(const TVector3D<T>& v, T tol) const
{
	return sf::isEqual<T>(_data.coord.x, v._data.coord.x, tol) &&
		sf::isEqual<T>(_data.coord.y, v._data.coord.y, tol) &&
		sf::isEqual<T>(_data.coord.z, v._data.coord.z, tol);
}

template<typename T>
inline bool TVector3D<T>::operator==(const TVector3D<T>& v) const
{
	return isEqual(v, tolerance);
}

template<typename T>
inline bool TVector3D<T>::operator!=(const TVector3D<T>& v) const
{
	return !isEqual(v, tolerance);
}

template<typename T>
T& TVector3D<T>::operator[](size_t i)
{
	if (i >= sizeof(data_type::array) / sizeof(T))
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[i];
}

template<typename T>
const T& TVector3D<T>::operator[](size_t i) const
{
	if (i >= sizeof(data_type::array) / sizeof(T))
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() index out of range!");
	}
	return _data.array[i];
}

template<typename T>
inline constexpr TVector3D<T>::operator T*()
{
	return _data.array;
}

template<typename T>
inline TVector3D<T>::operator const T*() const
{
	return _data.array;
}

template<typename T>
inline constexpr T TVector3D<T>::x() const
{
	return _data.coord.x;
}

template<typename T>
inline constexpr T& TVector3D<T>::x()
{
	return _data.coord.x;
}

template<typename T>
inline constexpr T TVector3D<T>::y() const
{
	return _data.coord.y;
}

template<typename T>
inline constexpr T& TVector3D<T>::y()
{
	return _data.coord.y;
}

template<typename T>
inline constexpr T TVector3D<T>::z() const
{
	return _data.coord.z;
}

template<typename T>
inline constexpr T& TVector3D<T>::z()
{
	return _data.coord.z;
}

template<typename T>
T TVector3D<T>::length() const
{
	return std::sqrt(lengthSqr());
}

template<typename T>
T TVector3D<T>::length2D() const
{
	return sqrt(lengthSqr2D());
}

template<typename T>
T TVector3D<T>::lengthSqr() const
{
	return _data.coord.x * _data.coord.x + _data.coord.y * _data.coord.y + _data.coord.z * _data.coord.z;
}

template<typename T>
T TVector3D<T>::lengthSqr2D() const
{
	return _data.coord.x * _data.coord.x + _data.coord.y * _data.coord.y + _data.coord.z * _data.coord.z;
}

template<typename T>
TVector3D<T>& TVector3D<T>::normalize(void)
{
	T len = length();
	if (len != 0.0)
	{
		// avoid SIGFPE
		*this /= (T) len;
	}
	return *this;
}

template<typename T>
TVector3D<T> TVector3D<T>::normalized() const
{
	return TVector3D(_data.coord.x, _data.coord.y, _data.coord.y).normalize();
}

template<typename T>
TVector3D<T>& TVector3D<T>::scale(T factor)
{
	_data.coord.x *= factor;
	_data.coord.y *= factor;
	_data.coord.z *= factor;
	return *this;
}

template<typename T>
TVector3D<T> TVector3D<T>::scaled(T factor) const
{
	return {_data.coord.x * factor, _data.coord.y * factor, _data.coord.z * factor};
}

template<typename T>
TVector3D<T> TVector3D<T>::crossProduct(const TVector3D<T>& v) const
{
	return {
		_data.coord.y * v._data.coord.z - _data.coord.z * v._data.coord.y,
		_data.coord.x * v._data.coord.z - _data.coord.z * v._data.coord.x,
		_data.coord.x * v._data.coord.y - _data.coord.y * v._data.coord.x
	};
}

template<typename T>
T TVector3D<T>::dotProduct(const TVector3D& v) const
{
	return _data.coord.x * v._data.coord.x + _data.coord.y * v._data.coord.y + _data.coord.z * v._data.coord.z;
}

template<typename T>
T TVector3D<T>::dotProduct2D(const TVector3D<T>& v) const
{
	return _data.coord.x * v._data.coord.x + _data.coord.y * v._data.coord.y;
}

template<typename T>
TVector3D<T> TVector3D<T>::operator*(const TVector3D& v) const
{
	return crossProduct(v);
}

template<typename T>
T TVector3D<T>::angle(const TVector3D& v) const
{
	TVector3D v1(*this), v2(v);
	// When for calculations the values are too small enlarge the
	// vector since the result is the same for the angle.
	if (v1.length() < std::numeric_limits<T>::epsilon() * 1E6)
	{
		auto ls = v1.lengthSqr();
		v1 /= ls;
		v2 /= ls;
	}
	if (v2.length() < std::numeric_limits<T>::epsilon() * 1E6)
	{
		auto ls = v2.lengthSqr();
		v1 /= ls;
		v2 /= ls;
	}
	// Quick and dirty to get the angle by using in-product and out-product
	// to get the cos alpha and the sinus alpha.
	value_type len = v1.length() * v2.length();
	if (isZero(len))
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid length/magnitude !");
	}
	auto cp = v1.crossProduct(v2);
	auto dp = v1.dotProduct(v2);
	// When the cross product is zero the 2 vectors are parallel to each other and the angle is therefore zero.
	if (v1.crossProduct(v2) == TVector3D(0, 0, 0))
	{
		return 0.0;
	}
	// Calculation using dot-product.
	return std::acos(dp / len);
}

template<typename T>
T TVector3D<T>::angleNormalized() const
{
	auto rv = angle();
	// When the angle is still negative add 360 degrees in radians to it.
	if (rv < 0.0)
	{
		rv += 2.0 * numbers::pi_v<T>;
	}
	return rv;
}

template<typename T>
T TVector3D<T>::distance(const TVector3D& v) const
{
	return std::sqrt<T>(distanceSqr(v));
}

template<typename T>
T TVector3D<T>::distance2D(const TVector3D<T>& v2) const
{
	return std::sqrt(DistanceSqr2d(v2));
}

template<typename T>
T TVector3D<T>::distanceSqr(const TVector3D<T>& v2) const
{
	return (_data.coord.x - v2._data.coord.x) * (_data.coord.x - v2._data.coord.x) +
		(_data.coord.y - v2._data.coord.y) * (_data.coord.y - v2._data.coord.y) +
		(_data.coord.z - v2._data.coord.z) * (_data.coord.z - v2._data.coord.z);
}

template<typename T>
T TVector3D<T>::distanceSqr2D(const TVector3D<T>& v2) const
{
	return (_data.coord.x - v2._data.coord.x) * (_data.coord.x - v2._data.coord.x) +
		(_data.coord.y - v2._data.coord.y) * (_data.coord.y - v2._data.coord.y);
}

template<typename T>
void TVector3D<T>::updateMin(const TVector3D<T>& vertex)
{
	if (vertex._data.coord.x < _data.coord.x)
	{
		_data.coord.x = vertex._data.coord.x;
	}
	if (vertex._data.coord.y < _data.coord.y)
	{
		_data.coord.y = vertex._data.coord.y;
	}
	if (vertex._data.coord.z < _data.coord.z)
	{
		_data.coord.z = vertex._data.coord.z;
	}
}

template<typename T>
void TVector3D<T>::updateMax(const TVector3D<T>& vertex)
{
	if (vertex._data.coord.x > _data.coord.x)
	{
		_data.coord.x = vertex._data.coord.x;
	}
	if (vertex._data.coord.y > _data.coord.y)
	{
		_data.coord.y = vertex._data.coord.y;
	}
	if (vertex._data.coord.z > _data.coord.z)
	{
		_data.coord.z = vertex._data.coord.z;
	}
}

template<typename T>
std::string TVector3D<T>::toString() const
{
	return '(' +
		sf::toString<T>(isZero(_data.coord.x, tolerance) ? T(0) : _data.coord.x) + ',' +
		sf::toString<T>(isZero(_data.coord.y, tolerance) ? T(0) : _data.coord.y) + ',' +
		sf::toString<T>(isZero(_data.coord.z, tolerance) ? T(0) : _data.coord.z) + ')';
}

template<typename T>
TVector3D<T>& TVector3D<T>::fromString(const std::string& s)
{
	constexpr auto sz = sizeof(data_type::array) / sizeof(T);
	std::regex re(R"(^\(([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?)\)$)", std::regex::icase);
	std::smatch match;
	// Sanity check on the amount of matches.
	if (!std::regex_match(s, match, re) || match.size() != sz + 1)
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid string '" + s + "' conversion!");
	}
	else
	{
		for (size_t i = 0; i < sz; i++)
		{
			// First match is the group so skip it (+1).
			_data.array[i] = toNumber<T>(match[i + 1].str());
		}
	}
	return *this;
}

template<typename T>
int TVector3D<T>::dominantAxis(void)
{
	if (std::fabs(_data.coord.x) > std::fabs(_data.coord.y))
	{
		if (std::fabs(_data.coord.x) > std::fabs(_data.coord.z))
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		if (std::fabs(_data.coord.y) > std::fabs(_data.coord.z))
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
}

template<typename T>
void TVector3D<T>::copyTo(T fa[3]) const
{
	std::memcpy(fa, _data.array, sizeof _data.array);
}

template<typename T>
template<typename F>
const F* TVector3D<T>::floatPtr() const
{
	constexpr size_t sz = sizeof(data_type::array) / sizeof(T);
	// Make the value returned thread safe.
	static thread_local F fa[sz];
	for (size_t i = 0; i < sz; i++)
	{
		fa[i] = static_cast<F>(_data.array[i]);
	}
	return fa;
}

}// namespace sf
