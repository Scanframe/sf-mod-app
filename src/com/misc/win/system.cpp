#include "gen/system.h"
#include "gen/TimeSpec.h"
#include "gen/dbgutils.h"
#include "gen/gnu_compat.h"
#include "gen/time.h"
#include <windows.h>

namespace sf
{

namespace
{

timespec startTime{0, 0};

SF_CONSTRUCTOR_COMPAT_FUNC(initializeStartTime)
{
	clock_gettime(CLOCK_MONOTONIC, &startTime);
}

}// namespace

timespec getTimeRunning()
{
	return TimeSpec(getTime()).sub(startTime);
}

bool loadDynamicLibrary(const std::string& path)
{
	HINSTANCE handle = ::LoadLibraryA(path.c_str());
	if (!handle)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "Could not load dynamic library: " << path);
		return false;
	}
	return true;
}

}// namespace sf
