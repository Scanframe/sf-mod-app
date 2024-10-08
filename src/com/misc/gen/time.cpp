#include "time.h"
#include "Exception.h"
#include "gnu_compat.h"

#include "Exception.h"
#include "TimeSpec.h"
#include "dbgutils.h"
#include "gnu_compat.h"
#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include <cxxabi.h>
#include <utility>

namespace sf
{

int timespecCompare(const timespec& ts1, const timespec& ts2)
{
	if (ts1.tv_sec > ts2.tv_sec)
	{
		return 1;
	}
	else if (ts1.tv_sec < ts2.tv_sec)
	{
		return -1;
	}
	else if (ts1.tv_nsec > ts2.tv_nsec)
	{
		return 1;
	}
	else if (ts1.tv_nsec < ts2.tv_nsec)
	{
		return -1;
	}
	// Finally it is equal.
	return 0;
}

timespec getTime(bool realTime)
{
	timespec ct{};
#if IS_WIN
	if (clock_gettime(realTime ? CLOCK_REALTIME : CLOCK_MONOTONIC, &ct))
#else
	if (clock_gettime(realTime ? CLOCK_REALTIME : CLOCK_MONOTONIC_COARSE, &ct))
#endif
	{
		throw ExceptionSystemCall("clock_gettime", errno, nullptr, __FUNCTION__);
	}
	return ct;
}

}// namespace sf