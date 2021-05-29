#include <sys/time.h> // NOLINT(modernize-deprecated-headers)
#include <gen/gen_utils.h>
#include "ElapseTimer.h"

namespace sf
{

ElapseTimer::ElapseTimer()
{
	_target = gettime();
}

ElapseTimer::ElapseTimer(unsigned long usec)
{
	set(usec);
}

bool ElapseTimer::operator()(const timespec& t) const
{
	return _enabled && timespeccmp(t, _target) >= 0;
}

bool ElapseTimer::isActive() const
{
	return _enabled && timespeccmp(gettime(), _target) >= 0;
}

void ElapseTimer::set(unsigned long usec)
{
	timespec ct{};
	ct.tv_sec = usec / 1000000l;
	ct.tv_nsec = usec % 1000000l;
	set(ct);
}

void ElapseTimer::set(const timespec& t)
{
	// Assign the elapse time for usage by Reset().
	_elapsedTime = t;
	timespec ct = gettime();
	_target.tv_sec = ct.tv_sec + _elapsedTime.tv_sec;
	_target.tv_nsec = ct.tv_nsec + _elapsedTime.tv_nsec;
	// Correction on the nano seconds.
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
	return getTimeLeft(gettime());
}

TimeSpec ElapseTimer::getTimeLeft(const timespec& t) const
{
	TimeSpec ct;
	if (_enabled)
	{
		ct = t;
		ct.tv_sec -= _target.tv_sec;
		// Check if the current nano seconds part is larger then the start.
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

}