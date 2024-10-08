#include "Value.h"
#include "TDynamicBuffer.h"
#include "gnu_compat.h"
#include "pointer.h"
#include "string.h"
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <sstream>

namespace sf
{

constexpr size_t Value::_sizeExtra = 1;

const char* Value::_invalidStr = "n/a";

const char* Value::_typeNames[] =
	{
		"INVALID",
		"UNDEF",
		"INTEGER",
		"FLOAT",
		"STRING",
		"BINARY",
		"CUSTOM"
};

Value& Value::operator=(Value&& v) noexcept
{
	_data = v._data;
	_size = v._size;
	_type = v._type;
	v._type = vitInvalid;
	v._data._ptr = nullptr;
	v._size = 0;
	return *this;
}

Value::Value()
{
	(void) this;
}

Value::Value(Value&& v) noexcept
	: _data(v._data)
{
	_size = v._size;
	_type = v._type;
	v._type = vitInvalid;
	v._data._ptr = nullptr;
	v._size = 0;
}

Value::Value(const Value& v)
{
	set(v);
}

Value::Value(EType type)
{
	setType(type);
}

Value::Value(const Value* v)
{
	set(vitReference, v);
}

Value::Value(EType type, const void* content, size_t size)
{
	set(type, content, size);
}

Value::Value(flt_type v)
{
	set(vitFloat, &v);
}

Value::Value(bool v)
{
	int_type ll = v;
	set(vitInteger, &ll);
}

Value::Value(int v)
{
	int_type ll = v;
	set(vitInteger, &ll);
}

#if IS_WIN
Value::Value(long v)
{
	int_type ll = v;
	set(vitInteger, &ll);
}
#endif

Value::Value(unsigned v)
{
	int_type ll = v;
	set(vitInteger, &ll);
}

Value::Value(int_type v)
{
	set(vitInteger, &v);
}

Value::Value(const char* v)
{
	set(vitString, v);
}

Value::Value(const std::string& v)
{
	set(vitString, v.c_str(), v.length());
}

#if IS_QT

Value::Value(const QString& v)
{
	set(v);
}

#endif

Value::Value(const void* v, size_t size)
{
	_data._ptr = nullptr;
	set(vitBinary, v, size);
}

Value::~Value()
{
	// Only delete memory of variable sized value types.
	if (_type >= vitString && _data._ptr)
	{
		free_null(_data._ptr);
	}
}

Value& Value::set(int type, const void* content, size_t size)
{
	// Free any memory for this item if any is allocated.
	if (_type >= vitString && _data._ptr)
	{
		free_null(_data._ptr);
	}
	_size = 0;
	switch (type)
	{
		case vitUndefined:
			break;

		case vitInteger:
			_size = sizeof(_data._int);
			if (content)
			{
#if IS_WIN
				memcpy_s(&_data._int, _size, content, _size);
#else
				memcpy(&_data._int, content, _size);
#endif
			}
			else
			{
				memset(&_data._int, 0, _size);
			}
			break;

		case vitFloat:
			_size = sizeof(flt_type);
			if (content)
			{
#if IS_WIN
				memcpy_s(&_data._flt, _size, content, _size);
#else
				memcpy(&_data._flt, content, _size);
#endif
			}
			else
			{
				memset(&_data._flt, 0, _size);
			}
			break;

		case vitString:
			if (content)
			{
				_size = strlen(static_cast<char*>(const_cast<void*>(content))) + 1;
				if (_size > maxString)
				{
					_size = maxString;
				}
				_data._ptr = static_cast<char*>(malloc(_size + _sizeExtra));
#if IS_WIN
				memcpy_s(_data._ptr, _size, content, _size);
#else
				memcpy(_data._ptr, content, _size);
#endif
				_data._ptr[_size - 1] = '\0';
			}
			else
			{
				_data._ptr = static_cast<char*>(malloc(_size + _sizeExtra));
				memset(_data._ptr, 0, _size);
			}
			break;

		case vitBinary:
			_size = size;
			if (_size > maxBinary)
			{
				_size = maxBinary;
			}
			_data._ptr = static_cast<char*>(malloc(_size + _sizeExtra));
			if (content)
			{
#if IS_WIN
				memcpy_s(_data._ptr, _size, content, _size);
#else
				memcpy(_data._ptr, content, _size);
#endif
			}
			else
			{
				memset(_data._ptr, 0, _size);
			}
			break;

		case vitCustom:
			_size = size;
			if (_size > maxCustom)
			{
				_size = maxCustom;
			}
			_data._ptr = static_cast<char*>(malloc(_size + _sizeExtra));
			if (content)
			{
#if IS_WIN
				memcpy_s(_data._ptr, _size, content, _size);
#else
				memcpy(_data._ptr, content, _size);
#endif
			}
			else
			{
				memset(_data._ptr, 0, _size);
			}
			break;

		case vitReference:
			_data._ref = static_cast<Value*>(const_cast<void*>(content));
			break;

		default:
			_type = vitInvalid;
			return *this;
	}// switch
	_type = (EType) type;
	return *this;
}

Value& Value::assign(const Value& v)
{
	if (_type == vitReference)
	{
		_data._ref->assign(v);
	}
	else
	{
		// Save current type
		auto type = _type;
		// Set variable using passed value.
		set(v);
		// Return to previous type if it wasn't the 'invalid' or 'undefined' type.
		if (type != vitUndefined && type != vitInvalid)
		{
			setType(type);
		}
	}
	// Return this instance as a reference.
	return *this;
}

bool Value::isZero() const
{
	switch (_type)
	{
		case vitInteger:
			return _data._int == std::numeric_limits<int_type>::denorm_min();

		case vitFloat:
			return std::fabs(_data._flt) <= std::numeric_limits<flt_type>::denorm_min();

		case vitReference:
			return _data._ref->isZero();

		case vitString:
			return !strlen(_data._ptr);

		case vitBinary:
		case vitCustom:
			return _size == 0;

		case vitInvalid:
		case vitUndefined:
		default:
			return true;
	}// switch
}

Value& Value::set(const Value& v)
{
	// Only when instance are not the same.
	if (&v != this)
	{
		// Delete the current allocated memory.
		if (_type >= vitString && _data._ptr)
		{
			free_null(_data._ptr);
		}
		// Assign the new type and size
		_type = v._type;
		_size = v._size;
		// Check if memory needs to be copied and reserved.
		if (_type >= vitString)
		{
			_data._ptr = static_cast<char*>(malloc(_size + _sizeExtra));
#if IS_WIN
			memcpy_s(_data._ptr, _size, v._data._ptr, _size);
#else
			memcpy(_data._ptr, v._data._ptr, _size);
#endif
		}
		else
		{
#if IS_WIN
			memcpy_s(&_data, _size, &v._data, _size);
#else
			memcpy(&_data, &v._data, _size);
#endif
		}
	}
	return *this;
}

Value::int_type Value::getInteger(int* cnv_err) const
{
	char* end_ptr = nullptr;
	int_type rv = 0;
	switch (_type)
	{
		case vitInteger:
			rv = _data._int;
			break;

		case vitFloat: {
			auto saved_fe = fesetround(FE_TONEAREST);
			rv = std::lround(_data._flt);
			fesetround(saved_fe);
			break;
		}

		case vitReference:
			return _data._ref->getInteger(nullptr);

		case vitString:
			rv = strtol(_data._ptr, &end_ptr, 0);
			if (end_ptr && *end_ptr != '\0' && cnv_err)
			{
				(*cnv_err)++;
			}
			break;

		case vitBinary:
		case vitCustom:
		case vitInvalid:
		case vitUndefined:
		default:
			break;
	}
	return rv;
}

Value::flt_type Value::getFloat(int* cnv_err) const
{
	char* end_ptr = nullptr;
	flt_type rv = 0.0;
	switch (_type)
	{
		case vitInteger:
			rv = flt_type(_data._int);
			break;

		case vitFloat:
			rv = _data._flt;
			break;

		case vitReference:
			return _data._ref->getFloat();

		case vitString:
			if (strlen(_data._ptr))
			{
				rv = sf::stod<flt_type>(_data._ptr, &end_ptr);
			}
			if (end_ptr && *end_ptr != '\0' && cnv_err)
			{
				(*cnv_err)++;
			}
			break;

		case vitBinary:
		case vitCustom:
		case vitInvalid:
		case vitUndefined:
		default:
			break;
	}
	return rv;
}

std::string Value::getString(int precision) const
{
	switch (_type)
	{
		case vitInvalid:
			return _invalidStr;

		case vitUndefined:
			return "";

		case vitInteger: {
			return toString<int_type>(_data._int);
		}

		case vitFloat:
			if (precision != std::numeric_limits<int>::max())
			{
				return toStringPrecision(_data._flt, precision);
			}
			else
			{
				return gcvtString(_data._flt, std::numeric_limits<flt_type>::digits10);
			}

		case vitReference:
			return _data._ref->getString();

		case vitString:
			return _data._ptr;

		case vitBinary:
		case vitCustom:
			return hexString(_data._ptr, _size);
	}
	return _invalidStr;
}

const void* Value::getBinary() const
{
	switch (_type)
	{
		case vitInteger:
		case vitFloat:
			return &_data;

		case vitString:
		case vitBinary:
		case vitCustom:
			return _data._ptr;

		case vitReference:
			return _data._ref->getBinary();

		case vitUndefined:
		case vitInvalid:
		default:
			return nullptr;
	}
}

const char* Value::getData() const
{
	if (_type == vitReference)
	{
		return _data._ref->getData();
	}
	else if (_type >= vitString)
	{
		return _data._ptr;
	}
	return reinterpret_cast<const char*>(&_data);
}

Value::EType Value::getType(const char* type)
{
	int i = 0;
	for (const char* n: _typeNames)
	{
		if (!std::strcmp(type, n))
		{
			return (Value::EType) i;
		}
		i++;
	}
	return vitInvalid;
}

const char* Value::getType(Value::EType type)
{
	constexpr auto max = sizeof(_typeNames) / sizeof(_typeNames[0]);
	return _typeNames[(type >= max || type < 0) ? vitInvalid : type];
}

void Value::makeInvalid()
{
	// When memory was allocated release it.
	if (_type >= vitString)
	{
		free_null(_data._ptr);
	}
	// Set the type to the invalid value.
	_type = vitInvalid;
}

bool Value::setType(EType type)
{
	// Check if a different type is to be Set.
	if (_type == type)
	{
		return true;
	}
	// When this instance is a reference root the call to it.
	if (_type == vitReference)
	{
		return _data._ref->setType(type);
	}
	// Cannot set the type of invalid instances.
	if (_type == vitInvalid)
	{
		return false;
	}
	// Variable to hold type conversion error position
	int cnv_err = 0;
	// Convert to the requested type.
	switch (type)
	{
		case vitInvalid:
		default:
			makeInvalid();
			break;

		case vitUndefined:
			if (_type >= vitString)
			{
				free_null(_data._ptr);
			}
			_type = vitUndefined;
			break;

		case vitInteger:
			set(getInteger(&cnv_err));
			break;

		case vitFloat:
			set(getFloat(&cnv_err));
			break;

		case vitString:
			set(getString().c_str());
			break;

		case vitBinary:
			if (_type == vitCustom)
			{
				_type = type;
				break;
			}// Fall into next

		case vitCustom:
			if (_type == vitBinary)
			{
				_type = type;
			}
			else
			{
				// Copy std::string to temporary std::string buffer
				std::string tmp = getString();
				// Calculate new size
				_size = tmp.length() / 2;
				// Only delete when type has allocated memory.
				if (_type >= vitString)
				{
					// Delete previous content pointer
					free_null(_data._ptr);
				}
				// Create new buffer
				_data._ptr = static_cast<char*>(malloc(_size + _sizeExtra));
				// Convert hex std::string to binary data
				if (stringHex(tmp.c_str(), _data._ptr, _size) == size_t(-1))
				{
					// Clear all memory after conversion error
					std::memset(_data._ptr, '\0', _size);
					cnv_err = -1;
				}
				// Assign the type member.
				_type = type;
			}
			break;

	}// switch
	// Return true at success.
	return !cnv_err;
}

Value Value::mul(const Value& v) const
{
	switch (_type)
	{
		case vitInvalid:
		case vitUndefined:
		case vitString:
		case vitBinary:
		case vitCustom:
			return *this;

		case vitInteger:
			return Value(_data._int * v.getInteger(nullptr));

		case vitFloat:
			return Value(_data._flt * v.getFloat(nullptr));

		case vitReference:
			return _data._ref->mul(v);

		default:
			return Value(0);
	}// switch
}

Value Value::div(const Value& v) const
{
	switch (_type)
	{
		case vitInvalid:
		case vitUndefined:
			return *this;

		case vitInteger: {
			int_type divider = v.getInteger(nullptr);
			// Check if the divider is zero.
			if (!divider)
			{
				divider = 1;
			}
			return Value(_data._int / divider);
		}

		case vitFloat: {
			flt_type divider = v.getFloat();
			// Check if the divider is zero.
			if (abs(divider) == 0.0)
			{
				divider = 1.0;
			}
			return Value(_data._flt / divider);
		}

		case vitReference:
			return _data._ref->div(v);

		case vitString:
		case vitBinary:
		case vitCustom:
			return *this;

		default:
			return Value(0);
	}
}

Value Value::add(const Value& v) const
{
	switch (_type)
	{
		case vitInvalid:
		case vitUndefined:
			return v;

		case vitInteger:
			return Value(_data._int + v.getInteger(nullptr));

		case vitFloat:
			return Value(_data._flt + v.getFloat(nullptr));

		case vitString:
			return Value(_data._ptr + v.getString());

		case vitReference:
			return _data._ref->add(v);

		case vitBinary:
		case vitCustom:
			return *this;

		default:
			return Value(0);
	}
}

Value Value::sub(const Value& v) const
{
	switch (_type)
	{
		case vitInteger:
			return Value(_data._int - v.getInteger(nullptr));

		case vitFloat:
			return Value(_data._flt - v.getFloat(nullptr));

		case vitReference:
			return _data._ref->sub(v);

		case vitInvalid:
		case vitUndefined:
		case vitString:
		case vitBinary:
		case vitCustom:
			return *this;

		default:
			return Value(0);
	}
}

Value Value::mod(const Value& v) const
{
	switch (_type)
	{
		case vitInteger:
			return Value(modulo<int_type>(_data._int, v.getInteger(nullptr)));

		case vitFloat:
			return Value(modulo<flt_type>(_data._flt, v.getFloat(nullptr)));

		case vitReference:
			return _data._ref->mod(v);

		case vitInvalid:
		case vitUndefined:
		case vitString:
		case vitBinary:
		case vitCustom:
			return *this;

		default:
			return Value(0);
	}// switch
}

int Value::compare(const Value& v) const
{
	switch (_type)
	{
		case vitInteger: {
			int_type l = v.getInteger(nullptr);
			int rv = _data._int != l;
			rv *= (l > _data._int) ? -1 : 1;
			return rv;
		}

		case vitFloat: {
			const flt_type& b(_data._flt);
			// Covert the passed instance to the Float type.
			flt_type a = v.getFloat();
			// Get a preliminary return value.
			int rv = (a == b) ? 0 : 1;
			// When not equal bit wise check a gain.
			if (rv)
			{
				try
				{
					// The difference must be smaller than the next representable value.
					rv = std::abs(a - b) > std::numeric_limits<flt_type>::epsilon();
				}
				catch (...)
				{
					rv = 1;
				}
			}
			rv *= (a > b) ? -1 : 1;
			return rv;
		}

		case vitString: {
			// Case sensitive compare
			return std::basic_string_view(_data._ptr, _size - 1).compare(v.getString());
		}

		case vitReference:
			return _data._ref->compare(v);

		case vitBinary:
		case vitCustom: {// if the sizes aren't the same the compare criteria is the size
			if (v._size != _size)
			{
				return v._size >= _size;
			}
			// if the compared type isn't the same type it's not equal by default
			if (v._type == vitBinary || v._type == vitCustom)
			{
				return memcmp(_data._ptr, v._data._ptr, _size);
			}
			else
			{
				// return greater then
				return 1;
			}
		}

		case vitInvalid:
		case vitUndefined:
			// Return greater than when comparing an undefined value.
			return 1;

		default:
			// Return max integer when incomparable.
			return std::numeric_limits<int>::max();
	}// switch
}

Value& Value::round(const Value& v)
{
	if (!v.isZero())
	{
		switch (_type)
		{
			case vitInteger: {
				//_data._int = sf::round(_data._int, v.GetInteger(nullptr));
				int_type iv = v.getInteger(nullptr);
				_data._int = ((_data._int + (iv / 2L)) / iv) * iv;
				break;
			}

			case vitFloat: {
				_data._flt = sf::round(_data._flt, v.getFloat(nullptr));
				break;
			}

			case vitReference:
				return _data._ref->round(v);

			default:
				break;
		}// switch
	}
	return *this;
}

#if IS_QT

Value::operator QString() const
{
	return QString::fromStdString(getString());
}

#endif

std::ostream& operator<<(std::ostream& os, const Value& v)
{
	auto delimiter = '"';
	os << '(' << Value::getType(v.getType()) << ',' << delimiter;
	// Do only std::string conversion when type is STRING.
	if (v.getType() == Value::vitString)
	{
		os << escape(v.getString(), delimiter);
	}
	else
	{
		os << v.getString();
	}
	os << delimiter << ')';
	return os;
}

std::istream& operator>>(std::istream& is, Value& v)
{
	char c;
	std::string type, content;
	// Get the first character which is the delimiting one.
	is.get(c);
	read_to_delimiter(is, type, ',') >> c;
	read_to_delimiter(is, content, '"') >> c;
	is.get(c);
	// Get the type value from the string.
	Value::EType t = Value::getType(type.c_str());
	// When it could not.
	if (t == Value::vitInvalid)
	{
		// Make the instance invalid when reading fails.
		v.makeInvalid();
	}
	else
	{
		// std::string conversion for a STRING type only.
		if (t == Value::vitString)
		{
			content = unescape(content);
		}
		// Set type and content for instance 'v'.
		v.set(content).setType(t);
	}
	return is;
}

Value Value::calculateOffset(Value value, Value min, Value max, const Value& len, bool clip)
{
	Value z(0);
	// Sync the types.
	z.setType(len.getType());
	min.setType(value.getType());
	max.setType(value.getType());
	max -= min;
	value -= min;
	Value temp = (max && value) ? (value * Value(len) / max) : z;
	if (clip)
	{
		// Check upper boundary.
		if (temp > len)
			return len;
		// Check lower boundary.
		if (temp < z)
			return z;
	}
	return temp;
}

}// namespace sf
