#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cfenv>
#include <sstream>
#include <malloc.h>
#include <values.h>
#include "dynamicbuffer.h"
#include "value.h"
#include "genutils.h"

namespace sf
{

const size_t TValue::_sizeExtra = 1;

const char* TValue::InvalidStr = "n/a";

const char* TValue::TypeNameArray[] =
	{
		"INVALID",
		"UNDEF",
		"INTEGER",
		"FLOAT",
		"STRING",
		"BINARY",
		"SPECIAL"
	};

TValue::TValue(EType type)
{
	SetType(type);
}

TValue::TValue(const TValue& v)
{
	Set(v);
}

TValue::TValue(TValue* v)
{
	Set(vitREFERENCE, v);
}

TValue::TValue(EType type, const void* content, size_t size)
{
	Set(type, content, size);
}

TValue::TValue(double v)
{
	Set(vitFLOAT, &v);
}

TValue::TValue(bool v)
{
	long l = v;
	Set(vitINTEGER, &l);
}

TValue::TValue(int v)
{
	long l = v;
	Set(vitINTEGER, &l);
}

TValue::TValue(unsigned v)
{
	long l = v;
	Set(vitINTEGER, &l);
}

TValue::TValue(long l)
{
	Set(vitINTEGER, &l);
}

TValue::TValue(const char* s)
{
	Set(vitSTRING, s);
}

TValue::TValue(const std::string& s)
{
	Set(vitSTRING, s.c_str(), s.length());
}

#if IS_QT

TValue::TValue(const QString& as)
{
	Set(as);
}

#endif

TValue::TValue(const void* v, size_t size)
{
	_data.Ptr = nullptr;
	Set(vitBINARY, v, size);
}

TValue::~TValue()
{
	// Only delete memory of variable sized value types.
	if (_type >= vitSTRING && _data.Ptr)
	{
		delete[] _data.Ptr;
		_data.Ptr = nullptr;
	}
}

TValue& TValue::Set(int type, const void* content, size_t size)
{
	// Free any memory for this item if any is allocated.
	if (_type >= vitSTRING && _data.Ptr)
	{
		delete[] _data.Ptr;
		_data.Ptr = nullptr;
	}
	_size = 0;
	switch (type)
	{
		case vitUNDEF:
			break;

		case vitINTEGER:
			_size = sizeof(long);
			if (content)
			{
				memcpy(&_data.Int, content, _size);
			}
			else
			{
				memset(&_data.Int, 0, _size);
			}
			break;

		case vitFLOAT:
			_size = sizeof(double);
			if (content)
			{
				memcpy(&_data.Flt, content, _size);
			}
			else
			{
				memset(&_data.Flt, 0, _size);
			}
			break;

		case vitSTRING:
			if (content)
			{
				_size = strlen((char*) content) + 1;
				if (_size > maxSTRING)
				{
					_size = maxSTRING;
				}
				_data.Ptr = new char[_size + _sizeExtra];
				memcpy(_data.Ptr, content, _size);
				_data.Ptr[_size - 1] = '\0';
			}
			else
			{
				if (_size > maxSTRING)
				{
					_size = maxSTRING;
				}
				_data.Ptr = new char[_size + _sizeExtra];
				memset(_data.Ptr, 0, _size);
			}
			break;

		case vitBINARY:
			_size = size;
			if (_size > maxBINARY)
			{
				_size = maxBINARY;
			}
			_data.Ptr = new char[_size + _sizeExtra];
			if (content)
			{
				memcpy(_data.Ptr, content, _size);
			}
			else
			{
				memset(_data.Ptr, 0, _size);
			}
			break;

		case vitSPECIAL:
			_size = size;
			if (_size > maxSPECIAL)
			{
				_size = maxSPECIAL;
			}
			_data.Ptr = new char[_size + _sizeExtra];
			if (content)
			{
				memcpy(_data.Ptr, content, _size);
			}
			else
			{
				memset(_data.Ptr, 0, _size);
			}
			break;

		case vitREFERENCE:
			_data.Ref = (TValue*) content;
			break;

		default:
			_type = vitINVALID;
			return *this;
	} // switch
	_type = (EType) type;
	return *this;
}

TValue& TValue::Assign(const TValue& v)
{
	if (_type == vitREFERENCE)
	{
		_data.Ref->Assign(v);
	}
	else
	{
		// Save current type
		auto type = _type;
		// Set variable using passed value.
		Set(v);
		// Return to previous type if it wasn't the 'invalid' or 'undefined' type.
		if (type != vitUNDEF && type != vitINVALID)
		{
			SetType(type);
		}
	}
	// Return this instance as a reference.
	return *this;
}

bool TValue::IsZero() const
{
	switch (_type)
	{
		case vitINTEGER:
			return _data.Int == 0;

		case vitFLOAT:
			return _data.Flt == 0.0;

		case vitREFERENCE:
			return _data.Ref->IsZero();

		case vitSTRING:
			return strlen(_data.Ptr) == 0;

		case vitBINARY:
		case vitSPECIAL:
			return _size == 0;

		case vitINVALID:
		case vitUNDEF:
		default:
			return true;
	}// switch
}

TValue& TValue::Set(const TValue& v)
{
	// Only when instance are not the same.
	if (&v != this)
	{
		// Delete the current allocated memory.
		if (_type >= vitSTRING && _data.Ptr)
		{
			delete[] _data.Ptr;
		}
		// Assign the new type and size
		_type = v._type;
		_size = v._size;
		// Check if memory needs to be copied and reserved.
		if (_type >= vitSTRING)
		{
			_data.Ptr = new char[_size + _sizeExtra];
			memcpy(_data.Ptr, v._data.Ptr, _size);
		}
		else
		{
			memcpy(&_data, &v._data, _size);
		}
	}
	return *this;
}

long TValue::GetInteger(int* cnverr) const
{
	char* end_ptr = nullptr;
	long rv = 0;
	switch (_type)
	{
		case vitINTEGER:
			rv = _data.Int;
			break;

		case vitFLOAT:
		{
			auto saved_fe = fesetround(FE_TONEAREST);
			rv = std::lround(_data.Flt);
			fesetround(saved_fe);
			break;
		}

		case vitREFERENCE:
			return _data.Ref->GetInteger();

		case vitSTRING:
			rv = strtol(_data.Ptr, &end_ptr, 0);
			if (end_ptr && *end_ptr != '\0' && cnverr)
			{
				(*cnverr)++;
			}
			break;

		case vitBINARY:
		case vitSPECIAL:
		case vitINVALID:
		case vitUNDEF:
		default:
			break;
	}// switch
	return rv;
}

double TValue::GetFloat(int* cnv_err) const
{
	char* end_ptr = nullptr;
	double rv = 0.0;
	switch (_type)
	{
		case vitINTEGER:
			rv = double(_data.Int);
			break;

		case vitFLOAT:
			rv = _data.Flt;
			break;

		case vitREFERENCE:
			return _data.Ref->GetFloat();

		case vitSTRING:
			if (strlen(_data.Ptr))
			{
				rv = strtod(_data.Ptr, &end_ptr);
			}
			if (end_ptr && *end_ptr != '\0' && cnv_err)
			{
				(*cnv_err)++;
			}
			break;

		case vitBINARY:
		case vitSPECIAL:
		case vitINVALID:
		case vitUNDEF:
		default:
			break;
	}// switch
	return rv;
}

std::string TValue::GetString(int precision) const
{
	switch (_type)
	{
		default:
		case vitINVALID:
			return InvalidStr;

		case vitUNDEF:
			return "";

		case vitINTEGER:
		{
			// Create buffer on stack.
			char buf[LONGBITS + 1];
			return ltoa(_data.Int, buf, 10);
		}

		case vitFLOAT:
			if (precision != INT_MAX)
			{
				// Clip the precision
				precision = clip(precision, 0, (DBL_MANT_DIG * 5 / 16 - 1));
				return stringf("%.*lf", precision, _data.Flt);
			}
			else
			{
				char buf[64];
				return gcvt(_data.Flt, (DBL_MANT_DIG * 5 / 16 - 1), buf);
			}

		case vitREFERENCE:
			return _data.Ref->GetString();

		case vitSTRING:
			return _data.Ptr;

		case vitBINARY:
		case vitSPECIAL:
			return hexstring(_data.Ptr, _size);

		case vitLASTENTRY:
			break;
	}// switch
	return InvalidStr;
}

const void* TValue::GetBinary() const
{
	switch (_type)
	{
		case vitINTEGER:
		case vitFLOAT:
			return &_data;

		case vitSTRING:
		case vitBINARY:
		case vitSPECIAL:
			return _data.Ptr;

		case vitREFERENCE:
			return _data.Ref->GetBinary();

		case vitUNDEF:
		case vitINVALID:
		default:
			return nullptr;
	}
}

const char* TValue::GetData() const
{
	if (_type == vitREFERENCE)
	{
		return _data.Ref->GetData();
	}
	return (_type >= vitSTRING) ? _data.Ptr : (char*) &_data;
}

TValue::EType TValue::GetType(const char* type)
{
	for (int i = vitINVALID; i < vitLASTENTRY; i++)
	{
		if (!strcmp(type, TypeNameArray[i]))
		{
			return (TValue::EType) i;
		}
	}
	return vitINVALID;
}

const char* TValue::GetType(TValue::EType type)
{
	return TypeNameArray[(type >= vitLASTENTRY || type < 0) ? vitINVALID : type];
}

void TValue::MakeInvalid()
{
	// When memory was allocated release it.
	if (_type >= vitSTRING)
	{
		delete[] _data.Ptr;
	}
	// Set the type to the invalid value.
	_type = vitINVALID;
}

bool TValue::SetType(EType type)
{
	// Check if a different type is to be set.
	if (_type == type)
	{
		return true;
	}
	// When this instance is a reference root the call to it.
	if (_type == vitREFERENCE)
	{
		return _data.Ref->SetType(type);
	}
	// Cannot set the type of an invalid instance.
	if (_type == vitINVALID)
	{
		return false;
	}
	// Variable to hold type conversion error position
	int cnverr = 0;
	// Convert to the requested type.
	switch (type)
	{
		case vitINVALID:
		default:
			MakeInvalid();
			break;

		case vitUNDEF:
			if (_type >= vitSTRING)
			{
				delete[] _data.Ptr;
			}
			_type = vitUNDEF;
			break;

		case vitINTEGER:
			Set(GetInteger(&cnverr));
			break;

		case vitFLOAT:
			Set(GetFloat(&cnverr));
			break;

		case vitSTRING:
			Set(GetString().c_str());
			break;

		case vitBINARY:
			if (_type == vitSPECIAL)
			{
				_type = type;
				break;
			}// Fall into next

		case vitSPECIAL:
			if (_type == vitBINARY)
			{
				_type = type;
			}
			else
			{// Copy std::string to temporary std::string buffer
				std::string tmp = GetString();
				// Calculate new _size
				_size = tmp.length() / 2;
				// Only delete when type has allocated memory.
				if (_type >= vitSTRING)
				{
					// Delete previous content pointer
					delete[] _data.Ptr;
				}
				// Create new buffer
				_data.Ptr = new char[_size + _sizeExtra];
				// Convert hex std::string to binary data
				if (stringhex(tmp.c_str(), _data.Ptr, _size) == size_t(-1))
				{// Clear all memory after conversion error
					memset(_data.Ptr, '\0', _size);
					cnverr = -1;
				}
				// Assign the type member.
				_type = type;
			}
			break;

	}// switch
	// Return true at success.
	return !cnverr;
}

TValue TValue::Mul(const TValue& v) const
{
	switch (_type)
	{
		case vitINVALID:
		case vitUNDEF:
		case vitSTRING:
		case vitBINARY:
		case vitSPECIAL:
			return *this;

		case vitINTEGER:
			return TValue(_data.Int * v.GetInteger());

		case vitFLOAT:
			return TValue(_data.Flt * v.GetFloat());

		case vitREFERENCE:
			return _data.Ref->Mul(v);

		default:
			return TValue(0L);
	}// switch
}

TValue TValue::Div(const TValue& v) const
{
	switch (_type)
	{
		case vitINVALID:
		case vitUNDEF:
			return *this;

		case vitINTEGER:
		{
			long divider = v.GetInteger();
			// Check if the divider is zero.
			if (!divider)
			{
				divider = 1;
			}
			return TValue(_data.Int / divider);
		}

		case vitFLOAT:
		{
			double divider = v.GetFloat();
			// Check if the divider is zero.
			if (abs(divider) == 0.0)
			{
				divider = 1.0;
			}
			return TValue(_data.Flt / divider);
		}

		case vitREFERENCE:
			return _data.Ref->Div(v);

		case vitSTRING:
		case vitBINARY:
		case vitSPECIAL:
			return *this;

		default:
			return TValue(0L);
	}
}

TValue TValue::Add(const TValue& v) const
{
	switch (_type)
	{
		case vitINVALID:
		case vitUNDEF:
			return v;

		case vitINTEGER:
			return TValue(_data.Int + v.GetInteger());

		case vitFLOAT:
			return TValue(_data.Flt + v.GetFloat());

		case vitSTRING:
			return TValue(_data.Ptr + v.GetString());

		case vitREFERENCE:
			return _data.Ref->Add(v);

		case vitBINARY:
		case vitSPECIAL:
			return *this;

		default:
			return TValue(0L);
	}// switch
}

TValue TValue::Sub(const TValue& v) const
{
	switch (_type)
	{
		case vitINVALID:
		case vitUNDEF:
			return *this;

		case vitINTEGER:
			return TValue(_data.Int - v.GetInteger());

		case vitFLOAT:
			return TValue(_data.Flt - v.GetFloat());

		case vitREFERENCE:
			return _data.Ref->Sub(v);

		case vitSTRING:
		case vitBINARY:
		case vitSPECIAL:
			return *this;

		default:
			return TValue(0L);
	}
}

TValue TValue::Mod(const TValue& v) const
{
	switch (_type)
	{
		case vitINVALID:
		case vitUNDEF:
			return *this;

		case vitINTEGER:
			return TValue(_data.Int % v.GetInteger());

		case vitFLOAT:
			return TValue(fmod(_data.Flt, v.GetFloat()));

		case vitREFERENCE:
			return _data.Ref->Mod(v);

		case vitSTRING:
		case vitBINARY:
		case vitSPECIAL:
			return *this;

		default:
			return TValue(0L);
	}// switch
}

int TValue::Compare(const TValue& v) const
{
	switch (_type)
	{
		case vitINTEGER:
		{
			long l = v.GetInteger();
			int rv = _data.Int != l;
			rv *= (l > _data.Int) ? -1 : 1;
			return rv;
		}

		case vitFLOAT:
		{
			const double& b(_data.Flt);
			double a = v.GetFloat();
			int rv = (a == b) ? 0 : 1;
			if (rv)
			{
				try
				{
					// The difference must be smaller then 1E10 of the operand.
					rv = fabs(a / (a - b)) < 1E10;
				}
				catch (...)
				{
					rv = 1;
				}
			}
			rv *= (a > b) ? -1 : 1;
			return rv;
		}

		case vitSTRING:
		{
			// Case insensitive compare
			return strcasecmp(_data.Ptr, v.GetString().c_str());
		}

		case vitREFERENCE:
			return _data.Ref->Compare(v);

		case vitBINARY:
		case vitSPECIAL:
		{// if the sizes aren't the same the compare criteria is the size
			if (v._size != _size)
			{
				return v._size >= _size;
			}
			// if the compared type isn't the same type it's not equal by default
			if (v._type == vitBINARY || v._type == vitSPECIAL)
			{
				return memcmp(_data.Ptr, v._data.Ptr, _size);
			}
			else
			{
				// return greater then
				return 1;
			}
		}

		case vitINVALID:
		case vitUNDEF:
			// Return greater then when comparing an undefined value.
			return 1;

		default:
			// return INT_MAX if in compareable
			return INT_MAX;
	}// switch
}

TValue& TValue::Round(const TValue& v)
{
	if (!v.IsZero())
	{
		switch (_type)
		{
			case vitINTEGER:
				_data.Int = ((_data.Int + (v.GetInteger() / 2L)) / v.GetInteger()) * v.GetInteger();
				break;

			case vitFLOAT:
				_data.Flt = floor(_data.Flt / v.GetFloat() + 0.5) * v.GetFloat();
				break;

			case vitREFERENCE:
				return _data.Ref->Round(v);

		}// switch
	}
	return *this;
}

#if IS_QT

TValue::operator QString() const
{
	return QString::fromStdString(GetString());
}

#endif

std::ostream& operator<<(std::ostream& os, const TValue& v)
{
	os << '('
		<< TValue::GetType(v.GetType())
		<< ','
		<< '"';
	// Do only std::string conversion when type is STRING.
	if (v.GetType() == TValue::vitSTRING)
	{
		os << escape(v.GetString(), false);
	}
	else
	{
		os << v.GetString();
	}
	os << '"'
		<< ')';
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

std::istream& operator>>(std::istream& is, TValue& v)
{
	char c;
	std::string type, content;
	// Get the first character which is the delimiting one.
	is.get(c);
	read_to_delimiter(is, type, ',') >> c;
	read_to_delimiter(is, content, '"') >> c;
	is.get(c);
	// Get the type value from the string.
	TValue::EType etype = TValue::GetType(type.c_str());
	// When it could not.
	if (etype == TValue::vitINVALID)
	{
		// Make the instance invalid when reading fails.
		v.MakeInvalid();
	}
	else
	{
		// std::string conversion for a STRING type only.
		if (etype == TValue::vitSTRING)
		{
			content = escape(content, true);
		}
		// Set type and content for instance 'v'
		v.Set(content).SetType(etype);
	}
	return is;
}

}// namespace