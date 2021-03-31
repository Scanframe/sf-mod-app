#include <iostream>

#include "UnitConversionServer.h"

namespace sf
{

UnitConversionServer::UnitConversionServer(const std::string& ini_filepath)
	:_profile(ini_filepath)
{
	// Install handler.
	sf::setUnitConversionHandler(&sf::UnitConversionServer::Handler, this);
}

UnitConversionServer::UnitConversionServer(std::istream& is)
:_profile(is)
{
	// Install handler.
	sf::setUnitConversionHandler(&sf::UnitConversionServer::Handler, this);
}


UnitConversionServer::~UnitConversionServer()
{
	// Install handler.
	sf::setUnitConversionHandler(nullptr, nullptr);
	_profile.flush();
}

bool UnitConversionServer::Handler(
	void* data,
	const std::string& option,
	const std::string& from_unit,
	int from_precision,
	double& multiplier,
	double& offset,
	std::string& to_unit,
	int& to_precision)
{
	// Get reference to server class from passed data pointer.
	auto& self = *static_cast<UnitConversionServer*>(data);
	to_unit.clear();
	multiplier = 0.0;
	offset = 0.0;
	to_precision = 0;

	// When pass though is selected.
	if (self._unitSystem == usPassThrough)
	{
		return false;
	}
	auto section = std::string("System ").append(getUnitSystemName(self._unitSystem));
	if (self._profile.getSection() != section)
	{
		if (!self._profile.setSection(section))
		{
			return false;
		}
	}
	std::string value;
	auto key = std::string(from_unit).append(1, '|').append(itostr(from_precision, 10));
	if (self._profile.getString(key, value))
	{
		//std::clog << key << "=" << value << std::endl;
		// Format of the key and value: unit|precision=unit,multiplier,offset,precision
		auto sl = sf::explode(value, ",");
		// Need at least 4 parts.
		if (sl.size() >= 4)
		{
			to_unit = sl[0];
			multiplier = std::stod(sl[1]);
			offset = std::stod(sl[2]);
			to_precision = std::stoi(sl[3]);
			return true;
		}
	}
	return false;
}

void UnitConversionServer::setUnitSystem(UnitConversionServer::EUnitSystem us)
{
	_unitSystem = us;
}

std::string UnitConversionServer::getUnitSystemName(UnitConversionServer::EUnitSystem us)
{
	std::string rv;
	switch (us)
	{
		case usImperial:
			return "Imperial";
		case usMetric:
			return "Metric";
		default:
			return "";
	}
	return rv;
}

}
