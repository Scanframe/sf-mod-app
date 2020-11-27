#include "rt-iface.h"
#include "com/misc/dbgutils.h"

IMPLEMENT_REGISTERBASE(RuntimeIface)

RuntimeIface::RuntimeIface(RuntimeIface::Parameters& params)
{
	_RTTI_NOTIFY(DO_DEFAULT, "Constructed: " << params.Handle);
}
