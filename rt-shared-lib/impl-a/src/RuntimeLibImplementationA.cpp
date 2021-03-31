#include "RuntimeLibImplementationA.h"
#include "misc/gen/dbgutils.h"
#include "misc/gen/gen_utils.h"


// Declaration of the dynamic library information.
_DL_INFORMATION("Implementation Library A",
	R"(This is the description of the library and what it contains.
	A second line of a multi lined description.)"
)

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
	_RTTI_NOTIFY(DO_CLOG, "Calling(" << Interface().getRegisterName(this) << "): " << __FUNCTION__)
	FMessage = sf::string_format("Greetings from Class '%s' created by name '%s' ", _RTTI_TYPENAME.c_str(),
		Interface().getRegisterName(this).c_str());
	return FMessage.c_str();
}
