#include "AxesCoord.h"
#include "GmiUtils.h"
#include <misc/gen/TStrings.h>

namespace sf::gmi
{

AxesCoord::AxesCoord(const AxesCoord& ac)
	: _map(ac._map)
{
	std::memcpy(_data, ac._data, sizeof(_data));
}

AxesCoord& AxesCoord::operator=(const AxesCoord& ac)
{
	_map = ac._map;
	std::memcpy(&_data, &ac._data, sizeof(_data));
	return *this;
}

void AxesCoord::clear()
{
	_map._bits = 0;
	memset(&_data, 0, sizeof(_data));
}

AxesCoord::AxesCoord()
{
	clear();
}

AxesCoord::AxesCoord(std::string s)
{
	clear();
	setString(s);
}

double& AxesCoord::getValue(int axis_loc)
{
	return _data[std::abs(axis_loc) % alLAST_ENTRY];
}

double AxesCoord::getValue(int axis_loc) const
{
	return _data[std::abs(axis_loc) % alLAST_ENTRY];
}

AxisValue AxesCoord::operator[](size_t al) const
{
	return _map.contains(al) ? AxisValue(EAxisLocation(al), _data[al]) : AxisValue();
}

bool AxesCoord::isSet(int axis_loc) const
{
	return _map.contains(axis_loc);
}

void AxesCoord::set(int axis_loc, double value)
{
	if (axis_loc < alFIRST_ENTRY || axis_loc >= alLAST_ENTRY)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() Axis location out of scope!");
	}
	_map << axis_loc;
	_data[axis_loc] = value;
}

void AxesCoord::unset(int axis_loc)
{
	if (axis_loc < alFIRST_ENTRY || axis_loc >= alLAST_ENTRY)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() Axis location out of scope!");
	}
	_map >> axis_loc;
}

AxesCoord& AxesCoord::operator<<(const AxisValue& av)
{
	if (av._location >= alFIRST_ENTRY && av._location < alLAST_ENTRY)
	{
		_map << av._location;
		_data[av._location] = av._value;
	}
	return *this;
}

AxesCoord& AxesCoord::operator=(const AxisValue& av)
{
	if (av._location >= alFIRST_ENTRY && av._location < alLAST_ENTRY)
	{
		_map << av._location;
		_data[av._location] = av._value;
	}
	return *this;
}

AxesCoord& AxesCoord::operator-=(const AxisValue& av)
{
	if (_map.contains(av._location))
	{
		_data[av._location] -= av._value;
	}
	return *this;
}

AxesCoord& AxesCoord::operator+=(const AxisValue& av)
{
	if (_map.contains(av._location))
	{
		_data[av._location] += av._value;
	}
	return *this;
}

AxesCoord& AxesCoord::operator>>(int axis_loc)
{
	if (axis_loc < alFIRST_ENTRY || axis_loc >= alLAST_ENTRY)
	{
		throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() Axis location out of scope!");
	}
	_map >> axis_loc;
	_data[axis_loc] = 0.0;
	return *this;
}

bool AxesCoord::operator==(const AxesCoord& ac) const
{
	return std::memcmp(this, &ac, sizeof(ac)) == 0;
}

bool AxesCoord::operator!=(const AxesCoord& ac) const
{
	return std::memcmp(this, &ac, sizeof(ac)) != 0;
}

int AxesCoord::getMap() const
{
	return _map._bits;
}

void AxesCoord::setMap(int map)
{
	_map._bits = map;
}

AxesCoord& AxesCoord::offset(const Vector3D& ofs)
{
	if (isSet(alX))
	{
		getValue(alX) += ofs.x();
	}
	if (isSet(alY))
	{
		getValue(alY) += ofs.y();
	}
	if (isSet(alZ))
	{
		getValue(alZ) += ofs.z();
	}
	return *this;
}

Vector3D AxesCoord::getVector() const
{
	Vector3D rv;
	if (isSet(alX))
	{
		rv.x() = getValue(alX);
	}
	if (isSet(alY))
	{
		rv.y() = getValue(alY);
	}
	if (isSet(alZ))
	{
		rv.z() = getValue(alZ);
	}
	return rv;
}

AxesCoord& AxesCoord::setVector(const Vector3D& vect)
{
	if (isSet(alX))
	{
		getValue(alX) = vect.x();
	}
	if (isSet(alY))
	{
		getValue(alY) = vect.y();
	}
	if (isSet(alZ))
	{
		getValue(alZ) = vect.z();
	}
	return *this;
}

AxesCoord AxesCoord::offsetBy(const Vector3D& ofs) const
{
	AxesCoord ac = *this;
	return ac.offset(ofs);
}

AxesCoord& AxesCoord::operator|=(const AxesCoord& ac)
{
	// Iterate through the axis values.
	for (int j = gmi::alFIRST_ENTRY; j < gmi::alLAST_ENTRY; j++)
	{
		// When the axis is not yet set, set it if a value is available.
		if (ac._map.contains(j) && !_map.contains(j))
		{
			set(j, ac.getValue(j));
		}
	}
	return *this;
}

AxesCoord& AxesCoord::operator&=(const AxesCoord& ac)
{
	// Iterate through the axis values.
	for (int j = gmi::alFIRST_ENTRY; j < gmi::alLAST_ENTRY; j++)
	{
		// When the axis is not yet set, set it if a value is available.
		if (ac._map.contains(j) && _map.contains(j))
		{
			set(j, ac.getValue(j));
		}
	}
	return *this;
}

AxesCoord& AxesCoord::operator-=(const AxesCoord& ac)
{
	// Iterate through the axis values.
	for (int j = gmi::alFIRST_ENTRY; j < gmi::alLAST_ENTRY; j++)
	{
		// When the axis is not yet set, set it if a value is available.
		if (_map.contains(j) && ac._map.contains(j))
		{
			set(j, getValue(j) - ac.getValue(j));
		}
	}
	return *this;
}

AxesCoord& AxesCoord::operator+=(const AxesCoord& ac)
{
	// Iterate through the axis values.
	for (int j = gmi::alFIRST_ENTRY; j < gmi::alLAST_ENTRY; j++)
	{
		// When the axis is not yet set, set it if a value is available.
		if (_map.contains(j) && ac._map.contains(j))
		{
			set(j, getValue(j) + ac.getValue(j));
		}
	}
	return *this;
}

AxesCoord::ECompare AxesCoord::compare(const AxesCoord& ac, const AxesCoord& tolerance) const
{
	return compare(ac, tolerance, AxisLocations() << alC << alD);
}

AxesCoord::ECompare AxesCoord::compare(const AxesCoord& ac, const AxesCoord& tolerances, const AxisLocations rad_unlimited) const
{
	TSet<int> cmp;
	// Get the mapped values which can be compared.
	// Bits must be set in both maps.
	cmp._bits = _map._bits & ac._map._bits;
	//
	for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
	{
		if (cmp.has(i))
		{
			// Get the maximum difference for the next compare of values.
			double tolerance = tolerances._map.has(i) ? tolerances._data[i] : 0.0;
			// Do a fuzzy compare.
			if (compareValue(_data[i], ac._data[i], tolerance, rad_unlimited.has((EAxisLocation) i)))
			{
				// Return immediately when the first value is not equal.
				return crNOT_EQUAL;
			}
		}
	}
	// If both maps are equal the two instances are the same.
	if (_map._bits == ac._map._bits)
	{
		return crEQUAL;
	}
	// Test if all the equal tested values were all the values in this instance.
	// Strip the tested bits from this instance map.
	if ((_map._bits & ~cmp._bits) == 0)
	// All of this instance values where equal compared to the other.
	{
		return crTHIS_EQUAL;
	}
	// Strip the tested bits from other instance map.
	if ((ac._map._bits & ~cmp._bits) == 0)
	{
		return crOTHER_EQUAL;
	}
	// When coming this far in the function bot must be equal.
	return crEQUAL;
}

std::string AxesCoord::getString() const
{
	int prec = 10;
	std::string s;
	char buf[80];
	for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
	{
		if (_map.has(i))
		{
			//int len = sprintf(buf, "%+.*le", prec, FData[i]);
			int len = sprintf(buf, "%.*lf", prec, _data[i]);
			// Strip trailing zero's.
			while (len > 1 && buf[--len] == '0')
			{
				buf[len] = 0;
			}
			// Append the string.
			s += buf;
		}
		// Append a comma.
		if (i < (alLAST_ENTRY - 1))
		{
			s += ',';
		}
	}
	// Return the resulting string.
	return s;
}

bool AxesCoord::setString(std::string str)
{
	bool rv = true;
	// Split the string in using a csv format.
	strings sl;
	sl.split(str, ',', '"');
	auto getField = [sl](strings::size_type i) -> std::string {
		if (i < sl.size())
		{
			return trim(sl[i]);
		}
		return {};
	};
	for (size_t i = 0; i < sl.size(); i++)
	{
		// When the length is zero the value must not be set.
		if (getField(i).length())
		{
			size_t idx;
			// Convert the string to a value.
			_data[i] = sf::toNumber<double>(getField(i), &idx);
			// Set the usage bitmap.
			_map.set(i);
			// Check if there was a conversion error set the return value to false.
			if (idx != getField(i).length())
			{
				rv = false;
			}
		}
		else
		{
			// Unset the usage bitmap.
			_map.unset(i);
			// Clear the field
			_data[i] = 0.0;
		}
	}
	return rv;
}

}// namespace sf::gmi

namespace sf
{

std::ostream& operator<<(std::ostream& os, const gmi::AxesCoord& ac)
{
	return (os << '(' << ac.getString() << ')');
}

std::istream& operator>>(std::istream& is, gmi::AxesCoord& ac)
{
	char c;
	std::string s;
	//  '('
	is >> c;
	sf::read_to_delimiter(is, s, ')');
	//  ')'
	is >> c;
	if (c == ')')
	{
		ac.setString(s);
	}
	return is;
}

}// namespace sf