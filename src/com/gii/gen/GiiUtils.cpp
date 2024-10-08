#include "GiiUtils.h"
#include "Variable.h"
#include <misc/gen/dbgutils.h>
#include <misc/gen/string.h>

namespace sf
{

// Maximum of amount of loops to use for loading settings files.
#define MAX_LOAD_LOOPS 10
#define KEY_NAME "Name"
#define KEY_VALUE "Value"
#define KEY_UNIT "unit"
#define SEC_SYSPARAMS "System Parameters"

bool shouldBeSaved(const Variable& var)
{
	// Save when variable is global and as the parameter flag set.
	bool yesno =
		var.isGlobal() &&
		var.isFlag(Variable::flgParameter) &&
		!var.isFlag(Variable::flgExport);
	// If it has currently the readonly flag set test the creation flags.
	if (yesno && var.isFlag(Variable::flgReadonly))
	{// When the creation flag was read only doe not save it.
		if (var.getFlags() & Variable::flgReadonly)
			yesno = false;
	}
	//
	return yesno;
}

bool shouldBeLoaded(const Variable& var)
{
	// Load when variable is global and as the parameter flag set and is
	// not read only.
	bool yesno = var.isGlobal() &&
		!var.isFlag(Variable::flgExport) &&
		var.isFlag(Variable::flgParameter) &&
		!var.isFlag(Variable::flgReadonly);
	return yesno;
}

bool SaveVariableValues(IniProfile& profile)
{
	// Preset the section.
	profile.setSection(SEC_SYSPARAMS);
	// Iterate through all variables in the system.
	for (auto var: Variable::getList())
	{
		// Check if the variable is a candidate.
		if (shouldBeSaved(*var))
		{
			// Create key name from ID.
			std::string key = "0x" + itostr(var->getId(), 16);
			// Get local copy.
			std::string value = var->getCur().getString();
			// If of a string type convert it to a regular string.
			if (var->getType() == Value::vitString)
			{
				value = escape(value);
			}
			profile.setString(key, value);
			profile.insertComment(key, var->getSetupString());
		}
	}
	return true;
}

bool LoadVariableValues(IniProfile& profile)
{
	// Preset the section.
	profile.setSection(SEC_SYSPARAMS);
	// Load settings twice so certain two-step variable are set to.
	for (int j = 0; j < MAX_LOAD_LOOPS; j++)
	{
		bool change = false;
		// Iterate through all variables in the system.
		for (auto var: Variable::getList())
		{
			// Check if the variable is a candidate.
			if (shouldBeSaved(*var))
			{
				// Create key name from ID.
				std::string key = "0x" + itostr(var->getId(), 16);
				if (profile.keyExists(key))
				{
					std::string value = profile.getString(key);
					// If of a string type convert it to a regular string.
					if (var->getType() == Value::vitString)
					{
						value = unescape(value);
					}
					// Check if the value was read and set it.
					change |= var->loadCur(Value(value));
				}
			}
		}
		// Abort the loop when no changes have been established.
		if (!change)
		{
			SF_NORM_NOTIFY(DO_DEFAULT, "LoadVariableSettingsFromProfile(): No changes after loading loop [" << j << "].");
			break;
		}
	}
	return true;
}

}// namespace sf
