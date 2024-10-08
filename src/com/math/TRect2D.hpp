#include <complex>

namespace sf
{

template<typename T>
TRect2D<T>::TRect2D(std::initializer_list<T> list)
{
	// Check the given initializer size.
	if (list.size() != sizeof(_data.array) / sizeof(_data.array[0]))
		throw std::invalid_argument("");
	size_t i = 0;
	for (auto el: list)
	{
		_data.array[i++] = el;
	}
}

template<typename T>
TRect2D<T>::TRect2D(const TRect2D& rect)
{
	_data.rect.left = rect._data.rect.left;
	_data.rect.bottom = rect._data.rect.bottom;
	_data.rect.right = rect._data.rect.right;
	_data.rect.top = rect._data.rect.top;
}

template<typename T>
TRect2D<T>::TRect2D(const TRect2D&& rect)
{
	_data = rect._data;
}

template<typename T>
TRect2D<T>::TRect2D(T left, T bottom, T right, T top)
{
	_data.rect.left = left;
	_data.rect.bottom = bottom;
	_data.rect.right = right;
	_data.rect.top = top;
}

template<typename T>
TRect2D<T>::TRect2D(const TVector2D<T>& loLeft, const TVector2D<T>& upRight)
{
	_data.pt[0] = loLeft._data.coord;
	_data.pt[1] = upRight._data.coord;
}

template<typename T>
void TRect2D<T>::clear()
{
	_data.rect.left = _data.rect.top = _data.rect.right = _data.rect.bottom = 0.0;
}

template<typename T>
TRect2D<T>& TRect2D<T>::assign(const TRect2D& rect)
{
	_data = rect._data;
	return *this;
}

template<typename T>
TRect2D<T>& TRect2D<T>::assign(T left, T bottom, T right, T top)
{
	_data.rect.left = left;
	_data.rect.bottom = bottom;
	_data.rect.right = right;
	_data.rect.top = top;
	return *this;
}

template<typename T>
TRect2D<T>& TRect2D<T>::assign(const TVector2D<T>& loLeft, const TVector2D<T>& upRight)
{
	_data.pt[0] = loLeft._data.coord;
	_data.pt[1] = upRight._data.coord;

	_data.rect.left = loLeft.x();
	_data.rect.bottom = loLeft.y();
	_data.rect.right = upRight.x();
	_data.rect.top = upRight.y();
	return *this;
}

template<typename T>
TRect2D<T>& TRect2D<T>::assignWidthHeight(T left, T bottom, T w, T h)
{
	_data.rect.left = left;
	_data.rect.bottom = bottom;
	_data.rect.right = left + w;
	_data.rect.top = bottom + h;
	return *this;
}

template<typename T>
TRect2D<T>& TRect2D<T>::assignWidthHeight(const TVector2D<T>& bottom_left, const TVector2D<T>& wh)
{
	_data.rect.left = bottom_left.x();
	_data.rect.bottom = bottom_left.y();
	_data.rect.right = bottom_left.x() + wh.x();
	_data.rect.top = bottom_left.y() + wh.y();
	return *this;
}

template<typename T>
inline bool TRect2D<T>::operator==(const TRect2D<T>& rect) const
{
	return isEqual(rect);
}

template<typename T>
bool TRect2D<T>::isEmpty() const
{
	if (std::fabs(_data.rect.left) > tolerance)
		return false;
	if (std::fabs(_data.rect.bottom) > tolerance)
		return false;
	if (std::fabs(_data.rect.right) > tolerance)
		return false;
	if (std::fabs(_data.rect.top) > tolerance)
		return false;
	return true;
}

template<typename T>
bool TRect2D<T>::isEqual(const TRect2D<T>& rect) const
{
	if (std::fabs(_data.rect.left - rect._data.rect.left) > tolerance)
		return false;
	if (std::fabs(_data.rect.bottom - rect._data.rect.bottom) > tolerance)
		return false;
	if (std::fabs(_data.rect.right - rect._data.rect.right) > tolerance)
		return false;
	if (std::fabs(_data.rect.top - rect._data.rect.top) > tolerance)
		return false;
	return true;
}

template<typename T>
bool TRect2D<T>::contains(const TVector2D<T>& point) const
{
	return point.x() >= _data.rect.left &&
		point.x() <= _data.rect.right &&
		point.y() >= _data.rect.bottom &&
		point.y() <= _data.rect.top;
}

template<typename T>
bool TRect2D<T>::contains(const TRect2D<T>& other) const
{
	return other._data.rect.left >= _data.rect.left &&
		other._data.rect.right <= _data.rect.right &&
		other._data.rect.bottom >= _data.rect.bottom &&
		other._data.rect.top <= _data.rect.top;
}

template<typename T>
bool TRect2D<T>::touches(const TRect2D<T>& other) const
{
	return other._data.rect.right >= _data.rect.left &&
		other._data.rect.left <= _data.rect.right &&
		other._data.rect.top >= _data.rect.bottom &&
		other._data.rect.bottom <= _data.rect.top;
}

template<typename T>
TVector2D<T> TRect2D<T>::bottomLeft() const
{
	return {_data.rect.left, _data.rect.bottom};
}

template<typename T>
TVector2D<T> TRect2D<T>::topRight() const
{
	return {_data.rect.right, _data.rect.top};
}

template<typename T>
TVector2D<T> TRect2D<T>::bottomRight() const
{
	return {_data.rect.right, _data.rect.bottom};
}

template<typename T>
TVector2D<T> TRect2D<T>::topLeft() const
{
	return {_data.rect.left, _data.rect.top};
}

template<typename T>
T TRect2D<T>::width() const
{
	return _data.rect.right - _data.rect.left;
}

template<typename T>
T TRect2D<T>::height() const
{
	return _data.rect.top - _data.rect.bottom;
}

template<typename T>
TVector2D<T> TRect2D<T>::Size() const
{
	return {width(), height()};
}

template<typename T>
T TRect2D<T>::area() const
{
	return width() * height();
}

template<typename T>
TVector2D<T> TRect2D<T>::center() const
{
	return {_data.rect.left + width() / 2, _data.rect.bottom + height() / 2};
}

template<typename T>
TRect2D<T>& TRect2D<T>::normalize()
{
	if (_data.rect.left > _data.rect.right)
		std::swap(_data.rect.left, _data.rect.right);
	if (_data.rect.bottom > _data.rect.top)
		std::swap(_data.rect.bottom, _data.rect.top);
	return *this;
}

template<typename T>
TRect2D<T> TRect2D<T>::normalized() const
{
	return {
		std::min(_data.rect.left, _data.rect.right),
		std::min(_data.rect.bottom, _data.rect.top),
		std::max(_data.rect.left, _data.rect.right),
		std::max(_data.rect.bottom, _data.rect.top)
	};
}

template<typename T>
TRect2D<T>& TRect2D<T>::offset(T dx, T dy)
{
	_data.rect.left += dx;
	_data.rect.bottom += dy;
	_data.rect.right += dx;
	_data.rect.top += dy;
	return *this;
}

template<typename T>
inline TRect2D<T> TRect2D<T>::offsetBy(T dx, T dy) const
{
	return TRect2D(*this).offset(dx, dy);
}

template<typename T>
TRect2D<T>& TRect2D<T>::moveTo(T x, T y)
{
	_data.rect.right = x + width();
	_data.rect.top = y + height();
	_data.rect.left = x;
	_data.rect.bottom = y;
	return *this;
}

template<typename T>
TRect2D<T> TRect2D<T>::movedTo(T x, T y)
{
	return TRect2D(*this).moveTo(x, y);
}

template<typename T>
inline TRect2D<T>& TRect2D<T>::operator+=(const TVector2D<T>& delta)
{
	return offset(delta.x(), delta.y());
}

template<typename T>
inline TRect2D<T>& TRect2D<T>::operator-=(const TVector2D<T>& delta)
{
	return offset(-delta.x(), -delta.y());
}

template<typename T>
TRect2D<T>& TRect2D<T>::inflate(T dx, T dy)
{
	_data.rect.left -= dx;
	_data.rect.bottom -= dy;
	_data.rect.top += dy;
	_data.rect.right += dx;
	normalize();
	return *this;
}

template<typename T>
inline TRect2D<T>& TRect2D<T>::inflate(const TVector2D<T>& delta)
{
	return inflate(delta.x(), delta.y());
}

template<typename T>
TRect2D<T> TRect2D<T>::inflatedBy(T dx, T dy) const
{
	return {_data.rect.left - dx, _data.rect.bottom - dy, _data.rect.top + dy, _data.rect.right + dx};
}

template<typename T>
inline TRect2D<T> TRect2D<T>::inflatedBy(const TVector2D<T>& delta) const
{
	return inflatedBy(delta.x(), delta.y());
}

template<typename T>
TRect2D<T>& TRect2D<T>::operator&=(const TRect2D& other)
{
	if (!isEmpty())
	{
		if (other.isEmpty())
			clear();
		else
		{
			_data.rect.left = std::max(_data.rect.left, other._data.rect.left);
			_data.rect.bottom = std::max(_data.rect.bottom, other._data.rect.bottom);
			_data.rect.top = std::min(_data.rect.top, other._data.rect.top);
			_data.rect.right = std::min(_data.rect.right, other._data.rect.right);
		}
	}
	return *this;
}

template<typename T>
TRect2D<T>& TRect2D<T>::operator|=(const TRect2D<T>& other)
{
	// When the passed rectangle is empty ignore it.
	if (!other.isEmpty())
	{
		// When this rectangle instance is empty ignore it and assign the other one.
		if (isEmpty())
		{
			assign(other);
		}
		// Not empty do the heavy lifting.
		else
		{
			_data.rect.left = std::min(_data.rect.left, other._data.rect.left);
			_data.rect.bottom = std::min(_data.rect.bottom, other._data.rect.bottom);
			_data.rect.top = std::max(_data.rect.top, other._data.rect.top);
			_data.rect.right = std::max(_data.rect.right, other._data.rect.right);
		}
	}
	return *this;
}

template<typename T>
TRect2D<T> TRect2D<T>::operator&(const TRect2D& other) const
{
	return (TRect2D(*this) &= other).normalize();
}

template<typename T>
TRect2D<T> TRect2D<T>::operator|(const TRect2D<T>& other) const
{
	return (TRect2D(*this) |= other).normalize();
}

template<typename T>
std::string TRect2D<T>::toString() const
{
	return std::string() + '(' +
		sf::toString<T>(_data.rect.left) + ',' +
		sf::toString<T>(_data.rect.bottom) + ',' +
		sf::toString<T>(_data.rect.right) + ',' +
		sf::toString<T>(_data.rect.top) + ')';
}

template<typename T>
TRect2D<T>& TRect2D<T>::fromString(const std::string& s) noexcept(false)
{
	std::regex re(R"(\(([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?),([+-]?\d*\.?\d+(?:e[+-]?\d+)?)\))", std::regex::icase);
	std::smatch match;
	// Sanity check on the amount of matches which is required.
	if (!std::regex_match(s, match, re) || match.size() != sizeof(_data.array) / sizeof(_data.array[0]) + 1)
	{
		throw std::invalid_argument(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() invalid string '" + s + "' conversion!");
	}
	else
	{
		for (size_t i = 0; i <= sizeof(_data.array) / sizeof(_data.array[0]); i++)
		{
			// First match is the group so skip it (+1).
			_data.array[i] = toNumber<T>(match[i + 1].str());
		}
	}
	return *this;
}

template<typename T>
std::istream& operator>>(std::istream& is, TRect2D<T>& rect)
{
	std::string s;
	auto delimiter = ')';
	std::getline(is, s, delimiter);
	rect.fromString(s.append(1, delimiter));
	return is;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const TRect2D<T>& rect)
{
	return os << rect.toString();
}

}// namespace sf