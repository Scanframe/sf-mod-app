#include "IntervalTimer.h"

namespace sf
{

IntervalTimer::IntervalTimer()
{
	_target = clock();
}

void IntervalTimer::set(clock_t t)
{
	_enabled = true;
	_interval = (t * (clock_t) CLOCKS_PER_SEC) / 1000L;
	_target = clock() + _interval;
}

bool IntervalTimer::active() const
{
	return _enabled && active(clock());
}

bool IntervalTimer::active(clock_t t) const
{
	if (_enabled && t > _target)
	{
		if (!_interval)
		{
			return true;
		}
		// Set new interval target
		*(clock_t*) &_target = (clock() / _interval + 1) * _interval;
		return true;
	}
	return false;
}

clock_t IntervalTimer::getTimeLeft() const
{
	return getTimeLeft(clock());
}

clock_t IntervalTimer::getTimeLeft(clock_t clk) const
{
	if (_enabled)
	{
		clock_t retval = ((_target - clk) * 1000L) / (clock_t) CLOCKS_PER_SEC;
		if (retval > 0L)
		{
			return retval;
		}
	}
	return 0L;
}

clock_t IntervalTimer::getTimeOver() const
{
	return getTimeOver(clock());
}

clock_t IntervalTimer::getTimeOver(clock_t clk) const
{
	if (_enabled)
	{
		return ((clk - _target) * 1000L) / (clock_t) CLOCKS_PER_SEC;
	}
	return 0L;
}

}
