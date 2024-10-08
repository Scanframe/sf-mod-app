#include "system.h"
#include "TimeSpec.h"
#include "gnu_compat.h"
#include "time.h"
#include <ctime>

namespace sf
{

namespace
{

timespec startTime{0, 0};

//__attribute__((constructor)) void initializeStartTime()
SF_CONSTRUCTOR_COMPAT_FUNC(initializeStartTime)
{
#if IS_WIN
	clock_gettime(CLOCK_MONOTONIC, &startTime);
#else
	clock_gettime(CLOCK_MONOTONIC_COARSE, &startTime);
#endif
}

}// namespace

timespec getTimeRunning()
{
	return TimeSpec(getTime()).sub(startTime);
}

}// namespace sf
