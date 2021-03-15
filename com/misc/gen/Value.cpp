#include <cmath>
#include <cstdlib>
#include <cfenv>
#include <sstream>
#include <malloc.h>
#include <climits>
#include "TDynamicBuffer.h"
#include "Value.h"
#include "gen_utils.h"

namespace sf
{

const size_t Value::_sizeExtra = 1;

const char* Value::_invalidStr = "n/a";

const char* Value::_typeNames[] =
	{
		"INVALID",
		"UNDEF",
		"INTEGER",
		"FLOAT",
		"STRING",
		"BINARY",
		"SPECIAL"
	};

Value::Value(EType type)
{
	setType(type);
}

Value::Value(const Value& v)
{
	set(v);
}

Value::Value(Value* v)
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

Value::Value(unsigned v)
{
	int_type ll = v;
	set(vitInteger, &ll);
}

Value::Value(long v)
{
	int_type ll(v);
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
		delete[] _data._ptr;
		_data._ptr = nullptr;
	}
}

Value& Value::set(int type, const void* content, size_t size)
{
	// Free any memory for this item if any is allocated.
	if (_type >= vitString && _data._ptr)
	{
		delete[] _data._ptr;
		_data._ptr = nullptr;
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
				memcpy(&_data._int, content, _size);
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
				memcpy(&_data._flt, content, _size);
			}
			else
			{
				memset(&_data._flt, 0, _size);
			}
			break;

		case vitString:
			if (content)
			{
				_size = strlen((char*) content) + 1;
				if (_size > maxSTRING)
				{
					_size = maxSTRING;
				}
				_data._ptr = new char[_size + _sizeExtra];
				memcpy(_data._ptr, content, _size);
				_data._ptr[_size - 1] = '\0';
			}
			else
			{
				if (_size > maxSTRING)
				{
					_size = maxSTRING;
				}
				_data._ptr = new char[_size + _sizeExtra];
				memset(_data._ptr, 0, _size);
			}
			break;

		case vitBinary:
			_size = size;
			if (_size > maxBINARY)
			{
				_size = maxBINARY;
			}
			_data._ptr = new char[_size + _sizeExtra];
			if (content)
			{
				memcpy(_data._ptr, content, _size);
			}
			else
			{
				memset(_data._ptr, 0, _size);
			}
			break;

		case vitCustom:
			_size = size;
			if (_size > maxSPECIAL)
			{
				_size = maxSPECIAL;
			}
			_data._ptr = new char[_size + _sizeExtra];
			if (content)
			{
				memcpy(_data._ptr, content, _size);
			}
			else
			{
				memset(_data._ptr, 0, _size);
			}
			break;

		case vitReference:
			_data._ref = (Value*) content;
			break;

		default:
			_type = vitInvalid;
			return *this;
	} // switch
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
			delete[] _data._ptr;
		}
		// Assign the new type and size
		_type = v._type;
		_size = v._size;
		// Check if memory needs to be copied and reserved.
		if (_type >= vitString)
		{
			_data._ptr = new char[_size + _sizeExtra];
			memcpy(_data._ptr, v._data._ptr, _size);
		}
		else
		{
			memcpy(&_data, &v._data, _size);
		}
	}
	return *this;
}

Value::int_type Value::getInteger(int* cnverr) const
{
	char* end_ptr = nullptr;
	int_type rv = 0;
	switch (_type)
	{
		case vitInteger:
			rv = _data._int;
			break;

		case vitFloat:
		{
			auto saved_fe = fesetround(FE_TONEAREST);
			rv = std::lround(_data._flt);
			fesetround(saved_fe);
			break;
		}

		case vitReference:
			return _data._ref->getInteger(nullptr);

		case vitString:
			rv = strtol(_data._ptr, &end_ptr, 0);
			if (end_ptr && *end_ptr != '\0' && cnverr)
			{
				(*cnverr)++;
			}
			break;

		case vitBinary:
		case vitCustom:
		case vitInvalid:
		case vitUndefined:
		default:
			break;
	}// switch
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
				rv = strtod(_data._ptr, &end_ptr);
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
	}// switch
	return rv;
}

std::string Value::getString(int precision) const
{
	switch (_type)
	{
		default:
		case vitInvalid:
			return _invalidStr;

		case vitUndefined:
			return "";

		case vitInteger:
		{
			// Create buffer on stack.
			char buf[std::numeric_limits<int_type>::max_digits10 + 1];
			return itoa(_data._int, buf, 10);
		}

		case vitFloat:
			if (precision != INT_MAX)
			{
				// Clip the precision
				precision = clip(precision, 0, std::numeric_limits<flt_type>::digits10);
				return stringf("%.*lf", precision, _data._flt);
			}
			else
			{
				// Create buffer large enough to hold all digits and signs including exponent 'e' and decimal dot '.'.
				char buf[std::numeric_limits<flt_type>::max_digits10 + std::numeric_limits<flt_type>::max_exponent10 + 5];
				// It seems the last digit is not reliable so the 'max_digits10 - 1' is given.
				std::string s(gcvt(_data._flt, std::numeric_limits<flt_type>::digits10, buf));
				// Only needed for Windows since it adds a trailing '.' even when not required.
				return s.erase(s.find_last_not_of('.') + 1);
			}

		case vitReference:
			return _data._ref->getString();

		case vitString:
			return _data._ptr;

		case vitBinary:
		case vitCustom:
			return hexstring(_data._ptr, _size);

		case vitLastEntry:
			break;
	}// switch
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
	return (_type >= vitString) ? _data._ptr : (char*) &_data;
}

Value::EType Value::getType(const char* type)
{
	for (int i = vitInvalid; i < vitLastEntry; i++)
	{
		if (!strcmp(type, _typeNames[i]))
		{
			return (Value::EType) i;
		}
	}
	return vitInvalid;
}

const char* Value::getType(Value::EType type)
{
	return _typeNames[(type >= vitLastEntry || type < 0) ? vitInvalid : type];
}

void Value::makeInvalid()
{
	// When memory was allocated release it.
	if (_type >= vitString)
	{
		delete[] _data._ptr;
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
	// Cannot Set the type of an invalid instance.
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
				delete[] _data._ptr;
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
					delete[] _data._ptr;
				}
				// Create new buffer
				_data._ptr = new char[_size + _sizeExtra];
				// Convert hex std::string to binary data
				if (stringhex(tmp.c_str(), _data._ptr, _size) == size_t(-1))
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
			return Value(0L);
	}// switch
}

Value Value::div(const Value& v) const
{
	switch (_type)
	{
		case vitInvalid:
		case vitUndefined:
			return *this;

		case vitInteger:
		{
			int_type divider = v.getInteger(nullptr);
			// Check if the divider is zero.
			if (!divider)
			{
				divider = 1;
			}
			return Value(_data._int / divider);
		}

		case vitFloat:
		{
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
			return Value(0L);
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
			return Value(0L);
	}// switch
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
			return Value(0L);
	}
}

Value Value::mod(const Value& v) const
{
	switch (_type)
	{
		case vitInteger:
			return Value(imodulo<int_type>(_data._int, v.getInteger(nullptr)));

		case vitFloat:
			return Value(fmodulo<flt_type>(_data._flt, v.getFloat(nullptr)));

		case vitReference:
			return _data._ref->mod(v);

		case vitInvalid:
		case vitUndefined:
		case vitString:
		case vitBinary:
		case vitCustom:
			return *this;

		default:
			return Value(0L);
	}// switch
}

int Value::compare(const Value& v) const
{
	switch (_type)
	{
		case vitInteger:
		{
			int_type l = v.getInteger(nullptr);
			int_type rv = _data._int != l;
			rv *= (l > _data._int) ? -1 : 1;
			return rv;
		}

		case vitFloat:
		{
			const flt_type& b(_data._flt);
			flt_type a = v.getFloat();
			int rv = (a == b) ? 0 : 1;
			if (rv)
			{
				try
				{
					// The difference must be smaller then 1E10 of the operand.
					rv = std::fabs(a / (a - b)) < 1E10;
				}
				catch (...)
				{
					rv = 1;
				}
			}
			rv *= (a > b) ? -1 : 1;
			return rv;
		}

		case vitString:
		{
			// Case sensitive compare
			return std::basic_string_view(_data._ptr, _size - 1).compare(v.getString());
		}

		case vitReference:
			return _data._ref->compare(v);

		case vitBinary:
		case vitCustom:
		{// if the sizes aren't the same the compare criteria is the size
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
			// Return greater then when comparing an undefined value.
			return 1;

		default:
			// return INT_MAX if in comparable.
			return INT_MAX;
	}// switch
}

Value& Value::round(const Value& v)
{
	if (!v.isZero())
	{
		switch (_type)
		{
			case vitInteger:
			{
//				_data._int = sf::round(_data._int, v.GetInteger(nullptr));
				int_type iv = v.getInteger(nullptr);
				_data._int = ((_data._int + (iv / 2L)) / iv) * iv;
				break;
			}

			case vitFloat:
			{
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
	auto delim = '"';
	os << '('	<< Value::getType(v.getType())	<< ',' << delim;
	// Do only std::string conversion when type is STRING.
	if (v.getType() == Value::vitString)
	{
		os << escape(v.getString(), delim);
	}
	else
	{
		os << v.getString();
	}
	os << delim	<< ')';
	return os;
}

std::istream& read_to_delimiter(std::istream& is, std::string& s, char delimiter)
{
	DynamicBuffer buf;
	const size_t block_size = 1024;
	size_t rc = 0;
	// skip first character if this is a delimiter
	if (is.peek() == delimiter)
	{
		is.get();
	}
	while (is.good() && is.peek() != delimiter)
	{
		buf.resize(block_size + rc);
		is.get(buf.ptr<char>(rc), block_size, delimiter);
		rc += is.gcount();
	}
	// Resize the buffer to the actual needed size and reserve space for the terminating nul.
	buf.resize(rc);
	// Replace the content of the passed string.
	s.assign(buf.c_str(), buf.size());
	//
	return is;
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
		// Set type and content for instance 'v'
		v.set(content).setType(t);
	}
	return is;
}

}// namespace
