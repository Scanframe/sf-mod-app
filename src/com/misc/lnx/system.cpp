#include "gen/system.h"
#include "gen/TimeSpec.h"
#include "gen/dbgutils.h"
#include "gen/time.h"
#include <dlfcn.h>

namespace sf
{

namespace
{

timespec startTime{0, 0};

__attribute__((constructor)) void initializeStartTime()
{
	clock_gettime(CLOCK_MONOTONIC_COARSE, &startTime);
}

}// namespace

timespec getTimeRunning()
{
	return TimeSpec(getTime()).sub(startTime);
}

bool loadDynamicLibrary(const std::string& path)
{
	auto handle = ::dlopen(path.c_str(), RTLD_NODELETE | RTLD_LAZY);
	if (!handle)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "Could not load dynamic library: " << path);
		return false;
	}
	return true;
}

}// namespace sf
