#include "ElapseTimer.h"
#include <sys/time.h>// NOLINT(modernize-deprecated-headers)

namespace sf
{

ElapseTimer::ElapseTimer()
	: _target(getTime())
{
}

ElapseTimer::ElapseTimer(unsigned long usec)
{
	set(usec);
}

bool ElapseTimer::operator()(const timespec& t) const
{
	return _enabled && timespecCompare(t, _target) >= 0;
}

bool ElapseTimer::isActive() const
{
	return _enabled && timespecCompare(getTime(), _target) >= 0;
}

void ElapseTimer::set(unsigned long usec)
{
#if IS_WIN
	set({time_t(usec / 1000000l), long(usec % 1000000l)});
#else
	set({time_t(usec / 1000000l), time_t(usec % 1000000l)});
#endif
}

void ElapseTimer::set(const timespec& t)
{
	// Assign the elapse-time for usage by reset().
	_elapsedTime = t;
	timespec ct = getTime();
	_target.tv_sec = ct.tv_sec + _elapsedTime.tv_sec;
	_target.tv_nsec = ct.tv_nsec + _elapsedTime.tv_nsec;
	// Correction on the nanoseconds.
	if (_target.tv_nsec > 1000000000l)
	{
		_target.tv_nsec -= 1000000000l;
		_target.tv_sec++;
	}
	// Enable the timer when setting the new timeout value.
	_enabled = true;
}

TimeSpec ElapseTimer::getTimeLeft() const
{
	return getTimeLeft(getTime());
}

TimeSpec ElapseTimer::getTimeLeft(const timespec& t) const
{
	TimeSpec ct;
	if (_enabled)
	{
		ct = t;
		ct.tv_sec -= _target.tv_sec;
		// Check if the current nanoseconds part is larger than the start.
		if (ct.tv_nsec > _target.tv_nsec)
		{
			ct.tv_nsec -= _target.tv_nsec;
		}
		else
		{
			// If smaller subtract 1 second.
			ct.tv_sec--;
			ct.tv_nsec = _target.tv_nsec - ct.tv_nsec;
		}
	}
	return ct;
}

}// namespace sf
