#include "rt-iface.h"
#include "com/misc/dbgutils.h"

RuntimeIface::RuntimeIface()
{
	_RTTI_NOTIFY(DO_DEFAULT, "Constructed");
}

const char* RuntimeIface::GetGlobalString()
{
	_NORM_NOTIFY(DO_DEFAULT, "Debug string from within a dynamic library")
	return "My global string";
}
