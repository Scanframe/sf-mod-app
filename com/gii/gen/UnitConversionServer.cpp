#include <iostream>
#include <algorithm>

#include "UnitConversionServer.h"

namespace sf
{

UnitConversionServer::UnitConversionServer()
	:_profile()
{
	// Install handler.
	setUnitConversionHandler(UnitConversionServerClosure().assign(this, &UnitConversionServer::Handler, std::placeholders::_1));
}

UnitConversionServer::~UnitConversionServer()
{
	// Uninstall a possible previous set handler.
	sf::setUnitConversionHandler();
	// Clear the profile.
	_profile.flush();
}

bool UnitConversionServer::load(std::istream& is)
{
	return _profile.read(is);
}

bool UnitConversionServer::save(std::ostream& os)
{
	return !_profile.write(os).fail();
}

void UnitConversionServer::setConversion(UnitConversionEvent& ev)
{
	// When pass though is selected do not write.
	if (_unitSystem != usPassThrough)
	{
		// Select the section.
		if (_profile.setSection(getUnitSystemName(_unitSystem)))
		{
			auto key = std::string(ev._from_unit).append(",").append(itostr(ev._from_precision));
			auto value = std::string(ev._to_unit).append(",").append(trimRight(trimRight(std::to_string(ev._multiplier), "0"), ".")).append(",").append(
			trimRight(trimRight(std::to_string(ev._offset), "0"), ".").append(",").append(std::to_string(ev._to_precision)));
			_profile.setString(key, value);
		}
	}
}


void UnitConversionServer::removeConversion(const std::string& key)
{
	_profile.removeEntry(key);
}

bool UnitConversionServer::Handler(UnitConversionEvent& ev)
{
	// When pass though is selected.
	if (_unitSystem == usPassThrough)
	{
		return false;
	}
	auto section = getUnitSystemName(_unitSystem);
	if (_profile.getSection() != section)
	{
		if (!_profile.setSection(section))
		{
			return false;
		}
	}
	std::string value;
	auto key = std::string(ev._from_unit).append(1, ',').append(itostr(ev._from_precision, 10));
	if (_profile.getString(key, value))
	{
		//std::clog << key << "=" << value << std::endl;
		// Format of the key and value: <from-unit>,<from-precision>=<to-unit>,<multiplier>,<offset>,<to-precision>
		strings sl;
		sl.split(value, ',');
		// Need at least 4 parts.
		if (sl.size() >= 4)
		{
			ev._to_unit = sl[0].empty() ? "??" : sl[0];
			ev._multiplier = toTypeDef<double>(sl[1], 1.0);
			ev._offset = toTypeDef<double>(sl[2], 0.0);
			ev._to_precision = toTypeDef<int>(sl[3], ev._from_precision);
			return true;
		}
	}
	else
	{
		SF_RTTI_NOTIFY(DO_CLOG, "Profile key '" << key << "' does not exist!")
	}
	// Clear all returned values when not found.
	ev._to_unit.clear();
	ev._multiplier = 0.0;
	ev._offset = 0.0;
	ev._to_precision = 0;
	return false;
}

void UnitConversionServer::setUnitSystem(int us)
{
	_unitSystem = (UnitConversionServer::EUnitSystem) us;
	if (_unitSystem != usPassThrough)
	{
		_profile.setSection(getUnitSystemName(_unitSystem));
	}
}

UnitConversionServer::EUnitSystem UnitConversionServer::getUnitSystem() const
{
	return _unitSystem;
}

const std::vector<std::pair<UnitConversionServer::EUnitSystem, const char*>>& UnitConversionServer::getUnitSystemNames()
{
	static auto unitSystemNames = std::vector<UnitSystemPair>{
		{UnitConversionServer::usPassThrough, "PassThrough"},
		{UnitConversionServer::usMetric, "Metric"},
		{UnitConversionServer::usImperial, "Imperial"},
	};
	return unitSystemNames;
}

const char* UnitConversionServer::getUnitSystemName(int us)
{
	auto& names = getUnitSystemNames();
	auto it = std::find_if(names.begin(), names.end(), [us](UnitSystemPair usp) -> bool
	{
		return usp.first == us;
	});
	if (it == names.end())
	{
		it = names.begin();
	}
	return it->second;
}

IniProfile& UnitConversionServer::getProfile()
{
	return _profile;
}

bool UnitConversionServer::isDirty() const
{
	return _profile.isDirty();
}

}
