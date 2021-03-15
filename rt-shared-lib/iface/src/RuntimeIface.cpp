#include "RuntimeIface.h"
#include "misc/gen/dbgutils.h"

SF_IMPL_IFACE(RuntimeIface, RuntimeIface::Parameters, Interface)

RuntimeIface::RuntimeIface(const RuntimeIface::Parameters& params)
{
	_RTTI_NOTIFY(DO_DEFAULT, "Constructed: " << params.Handle);
}


RuntimeIface::~RuntimeIface()
{
	_RTTI_NOTIFY(DO_DEFAULT, "Destructed");
}
