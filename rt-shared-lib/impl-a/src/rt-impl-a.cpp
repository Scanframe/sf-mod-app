#include "rt-impl-a.h"
#include "com/misc/dbgutils.h"

/*
Implementation defines:
	IMPLEMENT_REGISTER_OBJECT(T, name, data)
	IMPLEMENT_REREGISTER_OBJECT(T, name, data)
	IMPLEMENT_REGISTERBASE(T)
	IMPLEMENT_REGISTERBASE_AND_OBJECT(T, name, data)
*/

// Anonymous namespace to prevent export or name collisions.
namespace
{
struct RegisterData
{
	explicit RegisterData(int flag) : Flag(flag) {}
	int Flag;
};
}

IMPLEMENT_REGISTER_OBJECT
(
	RuntimeLibImplementationA,
	"RuntimeLibImplementationA",
	new RegisterData(123)
)

IMPLEMENT_REREGISTER_OBJECT
(
	RuntimeLibImplementationA, 2,
	"RuntimeLibImplementationA2",
	new RegisterData(456)
)

RuntimeLibImplementationA::RuntimeLibImplementationA(RuntimeIface::Parameters& params)
	: RuntimeIface(params)
{
	auto data = reinterpret_cast<const RegisterData*>(GetRegisterData());
	FMessage = stringf("Name(%s) Handle(%d) Type(%d)", GetRegisterName(), params.Handle, data->Flag);
	_RTTI_NOTIFY(DO_DEFAULT, "Constructed: " << FMessage);
}

const char* RuntimeLibImplementationA::getString()
{
	return FMessage.c_str();
}
