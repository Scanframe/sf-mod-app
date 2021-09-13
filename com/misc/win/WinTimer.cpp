#include "WinTimer.h"
#include "../gen/dbgutils.h"

namespace sf
{

WinTimer* WinTimer::_winTimer = nullptr;

WinTimer::WinTimer()
{
	if (_winTimer)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT | DO_MSGBOX, "Cannot create more the one instance yet!")
	}
	else
	{
		_winTimer = this;
	}
}

WinTimer::WinTimer(unsigned interval)
{
	if (_winTimer)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT | DO_MSGBOX, "Cannot create more the one instance yet!")
	}
	else
	{
		_winTimer = this;
		set(interval);
	}
}

WinTimer::~WinTimer()
{
	if (_winTimer == this)
	{
		_winTimer = nullptr;
	}
}

bool WinTimer::set(unsigned interval)
{
	if (_winTimer != this)
	{
		return false;
	}
	if (!interval || _timerId)
	{
		// Check if the timer is event pending.
		if (_timerId)
		{
			bool result = ::KillTimer(nullptr, _timerId);
			SF_COND_NORM_NOTIFY(!result, DO_CERR, "SetSustainTimer(): Failed to Kill Timer!" << _timerId)
			// Clear the member which is also used as a flag.
			_timerId = 0;
			_interval = 0;
			// Bail out on a failure.
			if (!result)
			{
				return false;
			}
		}
	}
	//
	if (interval)
	{
		_interval = interval;
		_timerId = ::SetTimer(nullptr, 0, _interval, (TIMERPROC) callback);
		SF_COND_NORM_NOTIFY(!_timerId, DO_CERR, "SetSustainTimer(): Failed to Set Timer!" << _timerId)
		// Signal success or failure
		return !_timerId;
	}
	return true;
}

void CALLBACK WinTimer::callback(HWND, UINT, UINT, DWORD ticks)
{
	if (_winTimer)
	{
		_winTimer->TimerRoutine(ticks);
	}
}

}
