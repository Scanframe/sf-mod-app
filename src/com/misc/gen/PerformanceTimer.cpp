#include "PerformanceTimer.h"

namespace sf
{

PerformanceTimer::PerformanceTimer()
{
	reset();
}

void PerformanceTimer::reset()
{
	_start = getTime();
}

TimeSpec PerformanceTimer::elapse()
{
	return TimeSpec(getTime()).sub(_start);
}

TimeSpec PerformanceTimer::elapse(const TimeSpec& ts)
{
	return TimeSpec(ts).sub(_start);
}

unsigned long PerformanceTimer::elapseUSec()
{
	TimeSpec et(elapse());
	return et.tv_sec * 1000000l + et.tv_nsec / 1000l;
}

unsigned long PerformanceTimer::elapseMSec()
{
	TimeSpec et(elapse());
	return et.tv_sec * 1000l + et.tv_nsec / 1000000l;
}

}// namespace sf