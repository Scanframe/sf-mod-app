#include "AxesCoord.h"
#include "GmiUtils.h"
#include <misc/gen/TStrings.h>

namespace sf::gmi
{

AxesCoord::AxesCoord(const AxesCoord& ac)
{
	FMap = ac.FMap;
	std::memcpy(&FData, &ac.FData, sizeof(FData));
}

AxesCoord& AxesCoord::operator=(const AxesCoord& ac)
{
	FMap = ac.FMap;
	std::memcpy(&FData, &ac.FData, sizeof(FData));
	return *this;
}

void AxesCoord::Clear()
{
	FMap._bits = 0;
	memset(&FData, 0, sizeof(FData));
}

AxesCoord::AxesCoord()
{
	Clear();
}

AxesCoord::AxesCoord(std::string s)
{
	Clear();
	SetString(s);
}

double& AxesCoord::Value(unsigned int axis_loc)
{
	return FData[axis_loc % alLAST_ENTRY];
}

double AxesCoord::Value(unsigned int axis_loc) const
{
	return FData[axis_loc % alLAST_ENTRY];
}

AxisValue AxesCoord::operator[](size_t al) const
{
	return FMap.contains(al) ? AxisValue(EAxisLocation(al), FData[al]) : AxisValue();
}

bool AxesCoord::IsSet(int axis_loc) const
{
	return FMap.contains(axis_loc);
}

void AxesCoord::Set(unsigned int axis_loc, double value)
{
	FMap << axis_loc;
	FData[axis_loc] = value;
}

void AxesCoord::Unset(int axis_loc)
{
	FMap >> axis_loc;
}

AxesCoord& AxesCoord::operator<<(const AxisValue& av)
{
	FMap << av._location;
	if (av._location >= 0)
		FData[av._location] = av._value;
	return *this;
}

AxesCoord& AxesCoord::operator=(const AxisValue& av)
{
	FMap << av._location;
	if (av._location >= 0)
	{
		FData[av._location] = av._value;
	}
	return *this;
}

AxesCoord& AxesCoord::operator-=(const AxisValue& av)
{
	if (FMap.contains(av._location))
	{
		FData[av._location] -= av._value;
	}
	return *this;
}

AxesCoord& AxesCoord::operator+=(const AxisValue& av)
{
	if (FMap.contains(av._location))
	{
		FData[av._location] += av._value;
	}
	return *this;
}

AxesCoord& AxesCoord::operator>>(unsigned int axis_loc)
{
	FMap >> axis_loc;
	if (axis_loc >= 0)
	{
		FData[axis_loc] = 0.0;
	}
	return *this;
}

bool AxesCoord::operator==(const AxesCoord& ac) const
{
	int cmp = std::memcmp(this, &ac, sizeof(ac));
	return cmp ? false : true;
}

bool AxesCoord::operator!=(const AxesCoord& ac) const
{
	int cmp = std::memcmp(this, &ac, sizeof(ac));
	return cmp ? true : false;
}

int AxesCoord::GetMap() const
{
	return FMap._bits;
}

void AxesCoord::SetMap(int map)
{
	FMap._bits = map;
}

AxesCoord& AxesCoord::Offset(const Vector3D& ofs)
{
	if (IsSet(alX))
		Value(alX) += ofs.x();
	if (IsSet(alY))
		Value(alY) += ofs.y();
	if (IsSet(alZ))
		Value(alZ) += ofs.z();
	return *this;
}

Vector3D AxesCoord::GetVector() const
{
	Vector3D rv;
	if (IsSet(alX))
		rv.x() = Value(alX);
	if (IsSet(alY))
		rv.y() = Value(alY);
	if (IsSet(alZ))
		rv.z() = Value(alZ);
	return rv;
}

AxesCoord& AxesCoord::SetVector(const Vector3D& vect)
{
	if (IsSet(alX))
		Value(alX) = vect.x();
	if (IsSet(alY))
		Value(alY) = vect.y();
	if (IsSet(alZ))
		Value(alZ) = vect.z();
	return *this;
}

AxesCoord AxesCoord::OffsetBy(const Vector3D& ofs) const
{
	AxesCoord ac = *this;
	return ac.Offset(ofs);
}

AxesCoord& AxesCoord::operator|=(const AxesCoord& ac)
{
	// Iterate through the axis values.
	for (unsigned int j = gmi::alFirst; j < gmi::alLAST_ENTRY; j++)
	{
		// When the axis is not yet set, set it if a value is available.
		if (ac.FMap.contains(j) && !FMap.contains(j))
		{
			Set(j, ac.Value(j));
		}
	}
	return *this;
}

AxesCoord& AxesCoord::operator&=(const AxesCoord& ac)
{
	// Iterate through the axis values.
	for (unsigned int j = gmi::alFirst; j < gmi::alLAST_ENTRY; j++)
	{
		// When the axis is not yet set, set it if a value is available.
		if (ac.FMap.contains(j) && FMap.contains(j))
		{
			Set(j, ac.Value(j));
		}
	}
	return *this;
}

AxesCoord& AxesCoord::operator-=(const AxesCoord& ac)
{
	// Iterate through the axis values.
	for (unsigned int j = gmi::alFirst; j < gmi::alLAST_ENTRY; j++)
	{
		// When the axis is not yet set, set it if a value is available.
		if (FMap.contains(j) && ac.FMap.contains(j))
		{
			Set(j, Value(j) - ac.Value(j));
		}
	}
	return *this;
}

AxesCoord& AxesCoord::operator+=(const AxesCoord& ac)
{
	// Iterate through the axis values.
	for (unsigned int j = gmi::alFirst; j < gmi::alLAST_ENTRY; j++)
	{
		// When the axis is not yet set, set it if a value is available.
		if (FMap.contains(j) && ac.FMap.contains(j))
		{
			Set(j, Value(j) + ac.Value(j));
		}
	}
	return *this;
}

AxesCoord::ECompare AxesCoord::Compare(const AxesCoord& ac, const AxesCoord& tolerance) const
{
	return Compare(ac, tolerance, AxisLocations() << alC << alD);
}

AxesCoord::ECompare AxesCoord::Compare(const AxesCoord& ac, const AxesCoord& tolerances, const AxisLocations rad_unlimited) const
{
	TSet<int> cmp;
	// Get the mapped values which can be compared.
	// Bits must be set in both maps.
	cmp._bits = FMap._bits & ac.FMap._bits;
	//
	for (int i = alFirst; i < alLAST_ENTRY; i++)
	{
		if (cmp.has(i))
		{
			// Get the maximum difference for the next compare of values.
			double tolerance = tolerances.FMap.has(i) ? tolerances.FData[i] : 0.0;
			// Do a fuzzy compare.
			if (compareValue(FData[i], ac.FData[i], tolerance, rad_unlimited.has((EAxisLocation) i)))
				// Return immediately when the first value is not equal.
				return crNOT_EQUAL;
		}
	}
	// If both maps are equal the two instances are the same.
	if (FMap._bits == ac.FMap._bits)
	{
		return crEQUAL;
	}
	// Test if all the equal tested values were all the values in this instance.
	// Strip the tested bits from this instance map.
	if ((FMap._bits & ~cmp._bits) == 0)
	// All of this instance values where equal compared to the other.
	{
		return crTHIS_EQUAL;
	}
	// Strip the tested bits from other instance map.
	if ((ac.FMap._bits & ~cmp._bits) == 0)
	{
		return crOTHER_EQUAL;
	}
	// When coming this far in the function bot must be equal.
	return crEQUAL;
}

std::string AxesCoord::GetString() const
{
	int prec = 10;
	std::string s;
	char buf[80];
	for (int i = alFirst; i < alLAST_ENTRY; i++)
	{
		if (FMap.has(i))
		{
			//int len = sprintf(buf, "%+.*le", prec, FData[i]);
			int len = sprintf(buf, "%.*lf", prec, FData[i]);
			// Strip trailing zero's.
			while (len > 1 && buf[--len] == '0')
				buf[len] = 0;
			// Append the string.
			s += buf;
		}
		// Append a comma.
		if (i < (alLAST_ENTRY - 1))
			s += ',';
	}
	// Return the resulting string.
	return s;
}

bool AxesCoord::SetString(std::string str)
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
			FData[i] = sf::toNumber<double>(getField(i), &idx);
			// Set the usage bitmap.
			FMap.set(i);
			// Check if there was a conversion error set the return value to false.
			if (idx != getField(i).length())
				rv = false;
		}
		else
		{
			// Unset the usage bitmap.
			FMap.unset(i);
			// Clear the field
			FData[i] = 0.0;
		}
	}
	return rv;
}

}// namespace sf::gmi

std::ostream& operator<<(std::ostream& os, const sf::gmi::AxesCoord& ac)
{
	return (os << '(' << ac.GetString() << ')');
}

std::istream& operator>>(std::istream& is, sf::gmi::AxesCoord& ac)
{
	char c;
	std::string s;
	//  '('
	is >> c;
	sf::read_to_delimiter(is, s, ')');
	//  ')'
	is >> c;
	if (c == ')')
		ac.SetString(s);
	return is;
}
