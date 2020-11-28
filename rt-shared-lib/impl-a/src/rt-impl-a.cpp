#include "rt-impl-a.h"
#include "com/misc/dbgutils.h"

/*
Implementation defines:
	IMPLEMENT_REGISTER_OBJECT(T, name, data)
	IMPLEMENT_REREGISTER_OBJECT(T, name, data)
	IMPLEMENT_REGISTERBASE(T)
	IMPLEMENT_REGISTERBASE_AND_OBJECT(T, name, data)
*/

IMPLEMENT_REGISTER_OBJECT(RuntimeLibImplementationA, "Implementation_A", nullptr)
// Register this class also under a different name.
IMPLEMENT_REREGISTER_OBJECT(RuntimeLibImplementationA, 2, "Implementation_Alias", nullptr)


__attribute__((constructor)) void register_on_load()
{
	_NORM_NOTIFY(DO_DEFAULT, "Constructing the new way")
	RuntimeIface::ClassRegistration.Register
		(
			"Implementation_A",
			"A description of the class.",
			misc::TClassRegistration<RuntimeIface, RuntimeIface::Parameters>::callback_t
			(
				[](const RuntimeIface::Parameters& params)->RuntimeIface*
				{
					return new RuntimeLibImplementationA(params);
				}
			)
		);
}

RuntimeLibImplementationA::RuntimeLibImplementationA(const RuntimeIface::Parameters& params)
	: RuntimeIface(params)
{
	FMessage = stringf("Handle(%d)", params.Handle);
	_RTTI_NOTIFY(DO_DEFAULT, "Constructed: " << FMessage)
}

const char* RuntimeLibImplementationA::getString()
{
	FMessage = stringf("Register Name (%s)", GetRegisterName());
	return FMessage.c_str();
}
