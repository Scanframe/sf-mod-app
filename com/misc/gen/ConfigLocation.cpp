#include "gen_utils.h"
#include "dbgutils.h"
#include "ConfigLocation.h"

namespace sf
{

// Anonymous namespace.
namespace
{

ConfigLocationClosure configLocationHandler;

}

void setConfigLocationHandler(const ConfigLocationClosure& handler)
{
	configLocationHandler = handler;
}

std::string getConfigLocation(const std::string& option, bool separator)
{
	std::string rv;
	// Is a handler installed.
	if (configLocationHandler)
	{
		rv = configLocationHandler(option);
	}
	else
	{
		// Return the working directory by default.
		rv = getWorkingDirectory();
	}
	// Check directory existence.
	if (!fileExists(rv))
	{
		SF_COND_NORM_NOTIFY(IsDebug(), DO_CLOG, __FUNCTION__  << " Directory '" << rv << "' does not exist!");
	}
	// Add separator when requested.
	if (separator)
	{
		rv.append(0, getDirectorySeparator());
	}
	return rv;
}


}
