#include "rt-iface.h"
#include "com/misc/dbgutils.h"

IMPLEMENT_REGISTER_BASE(RuntimeIface)

typename misc::TClassRegistration<RuntimeIface, RuntimeIface::Parameters> RuntimeIface::ClassRegistration; // NOLINT(cert-err58-cpp)

RuntimeIface::RuntimeIface(const RuntimeIface::Parameters& params)
{
	_RTTI_NOTIFY(DO_DEFAULT, "Constructed: " << params.Handle);
}
