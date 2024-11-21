#include <misc/gen/IniProfile.h>
#include <gii/gen/GiiUtils.h>
#include "IniProfileSettings.h"

namespace sf
{

bool IniProfileSettings::load(const std::string& filepath)
{
	IniProfile ini(filepath);
	return loadVariableValues(ini);
}

bool IniProfileSettings::read(const std::string& filepath)
{
	IniProfile ini(filepath);
	return saveVariableValues(ini);
}

const std::string& IniProfileSettings::getSettingsFileSuffix() const
{
	return _settingsFileSuffix;
}
}
