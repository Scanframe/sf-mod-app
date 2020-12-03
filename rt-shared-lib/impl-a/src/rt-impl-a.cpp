#include "rt-impl-a.h"
#include "misc/dbgutils.h"
#include "misc/genutils.h"

// Register this derived class.
SF_REG_CLASS
(
	RuntimeIface, RuntimeIface::Parameters, Interface,
	RuntimeLibImplementationA, "Class-A", "Actual name of the class."
)

RuntimeLibImplementationA::RuntimeLibImplementationA(const RuntimeIface::Parameters& params)
	: RuntimeIface(params)
{
	_RTTI_NOTIFY(DO_CLOG, "Constructed: " << sf::string_format("Handle(%d)", params.Handle))
}

const char* RuntimeLibImplementationA::getString()
{
	_RTTI_NOTIFY(DO_CLOG, "Calling: " << __FUNCTION__)
	FMessage = sf::string_format("Class Name is: %s", _RTTI_TYPENAME.c_str());
	return FMessage.c_str();
}
