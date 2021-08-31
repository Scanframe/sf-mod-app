

#include "IntervalTimer.h"
#include "gen_utils.h"

namespace sf
{

void IntervalTimer::set(const timespec& t)
{
	_enabled = true;
	_interval = t;
	_target = getTime();
	_target += _interval;
}


IntervalTimer::IntervalTimer(int usec)
{
	_enabled = true;
	_interval = TimeSpec(TimeSpec::nsec_type(usec) * 1000);
	_target = getTime();
	_target += _interval;
}

IntervalTimer::IntervalTimer(double sec)
{
	_enabled = true;
	_interval = TimeSpec(sec);
	_target = getTime();
	_target += _interval;
}

bool IntervalTimer::active(const timespec& t) const
{
	if (!_enabled)
	{
		return false;
	}
	// When zero always return as being active.
	if (!_interval)
	{
		return true;
	}
	// Check if time is over.
	if (timespecCompare(t, _target) >= 0)
	{
		// Set new interval target
		*const_cast<TimeSpec*>(&_target) = _interval + t;
		return true;
	}
	return false;
}

bool IntervalTimer::active() const
{
	return active(getTime());
}

TimeSpec IntervalTimer::getTimeLeft() const
{
	return getTimeLeft(getTime());
}

TimeSpec IntervalTimer::getTimeLeft(const timespec& t) const
{
	if (_enabled)
	{
		auto rv = _target - t;
		if (rv)
		{
			return rv;
		}
	}
	return {};
}

TimeSpec IntervalTimer::getTimeOver() const
{
	return getTimeOver(getTime());
}

TimeSpec IntervalTimer::getTimeOver(const timespec& t) const
{
	if (_enabled && timespecCompare(getTime(), _target) >= 0)
	{
		return TimeSpec(t) - _target;
	}
	return {};
}

}
