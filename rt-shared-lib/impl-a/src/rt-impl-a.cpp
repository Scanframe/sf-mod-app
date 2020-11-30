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
	_RTTI_NOTIFY(DO_DEFAULT, "Constructed: " << string_format("Handle(%d)", params.Handle))
}

const char* RuntimeLibImplementationA::getString()
{
	FMessage = string_format("Register Name (%s)", _RTTI_TYPENAME.c_str());
	return FMessage.c_str();
}
