#pragma once

#include <ctime>
#include "../global.h"

namespace sf
{
/**
 * Timer class that has fixed time intervals at which it becomes true.
 * If one ore several intervals are missed, this timer will not catch up.
 */
class _MISC_CLASS TIntervalTimer
{
	public:
		// Default constructor is by default disabled until Set() is called.
		TIntervalTimer();

		// Initializing constructor setting the timers interval time in msec.
		// Is by default enabled.
		explicit TIntervalTimer(clock_t t);

		// Sets the timers interval time in msec.
		// When zero is passed it is always active.
		//
		void Set(clock_t t);

		// Tests if the timer is active if it is active it returns non-zero
		// and resets the timer to a new event.
		[[nodiscard]] bool Active() const;

		// Passes clock() value ourself to reduce overhead in case of
		// multiple timers.
		[[nodiscard]] bool Active(clock_t) const;

		// Enables the timer with the current interval time.
		void Enable();

		// Disables the timer.
		void Disable();

		// returns non-zero is the timer is enabled.
		[[nodiscard]] bool IsEnabled() const;

		// Returns the set interval time in msec.
		[[nodiscard]] clock_t GetIntervalTime() const;

		// Returns the time left in msec before function active becomes true.
		[[nodiscard]] clock_t GetTimeLeft() const;

		[[nodiscard]] clock_t GetTimeLeft(clock_t) const;

		// Same as time left but it also gives the time passed the target time
		// and before that as a negative value.
		[[nodiscard]] clock_t GetTimeOver() const;

		[[nodiscard]] clock_t GetTimeOver(clock_t) const;

		// Test operator. Used in if and while statements.
		explicit operator bool() const;

		// This operator is used to minimize clock() calls.
		// Return non zero if timer is active.
		int operator()(clock_t t) const;

	private:
		// Data member holding the interval time with which the target time is set.
		// time is set
		clock_t Interval{0};
		// Holds the target time at which the timer should active.
		clock_t Target{0};
		// Enable flag
		bool FlagEnable{false};
};

inline
TIntervalTimer::TIntervalTimer(clock_t t)
{
	Set(t);
}

inline
void TIntervalTimer::Enable()
{
	FlagEnable = true;
}

inline
void TIntervalTimer::Disable()
{
	FlagEnable = false;
}

inline
bool TIntervalTimer::IsEnabled() const
{
	return FlagEnable;
}

inline
clock_t TIntervalTimer::GetIntervalTime() const
{
	return Interval;
}

inline
TIntervalTimer::operator bool() const
{
	return Active();
}

inline
int TIntervalTimer::operator()(clock_t t) const
{
	return Active(t);
}

}