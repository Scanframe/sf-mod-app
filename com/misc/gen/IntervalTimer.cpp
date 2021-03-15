#include "IntervalTimer.h"

namespace sf
{

TIntervalTimer::TIntervalTimer()
{
	Target = clock();
}

void TIntervalTimer::Set(clock_t t)
{
	FlagEnable = true;
	Interval = (t * (clock_t) CLOCKS_PER_SEC) / 1000L;
	Target = clock() + Interval;
}

bool TIntervalTimer::Active() const
{
	return FlagEnable && Active(clock());
}

bool TIntervalTimer::Active(clock_t t) const
{
	if (FlagEnable && t > Target)
	{
		if (!Interval)
		{
			return true;
		}
		// Set new interval target
		*(clock_t*) &Target = (clock() / Interval + 1) * Interval;
		return true;
	}
	return false;
}

clock_t TIntervalTimer::GetTimeLeft() const
{
	return GetTimeLeft(clock());
}

clock_t TIntervalTimer::GetTimeLeft(clock_t clk) const
{
	if (FlagEnable)
	{
		clock_t retval = ((Target - clk) * 1000L) / (clock_t) CLOCKS_PER_SEC;
		if (retval > 0L)
		{
			return retval;
		}
	}
	return 0L;
}

clock_t TIntervalTimer::GetTimeOver() const
{
	return GetTimeOver(clock());
}

clock_t TIntervalTimer::GetTimeOver(clock_t clk) const
{
	if (FlagEnable)
	{
		return ((clk - Target) * 1000L) / (clock_t) CLOCKS_PER_SEC;
	}
	return 0L;
}

}
