#include <misc/gen/IniProfile.h>
#include <gii/gen/GiiUtils.h>
#include "IniProfileSettings.h"

namespace sf
{

bool IniProfileSettings::load(const std::string& filepath)
{
	IniProfile ini(filepath);
	return LoadVariableValues(ini);
}

bool IniProfileSettings::read(const std::string& filepath)
{
	IniProfile ini(filepath);
	return SaveVariableValues(ini);
}

const std::string& IniProfileSettings::getSettingsFileSuffix() const
{
	return _settingsFileSuffix;
}
}
