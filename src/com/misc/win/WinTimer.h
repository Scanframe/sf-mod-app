#pragma once

#include "global.h"
#include <windows.h>

// Just to satisfy the IDE.
#ifndef CALLBACK
	#define CALLBACK
#endif

namespace sf
{

class _MISC_CLASS WinTimer
{
	public:
		/**
		 * Default constructor.
		 */
		WinTimer();

		/**
		 * Initializing constructor.
		 * @param interval
		 */
		explicit WinTimer(unsigned interval);

		/**
		 * Destructor.
		 */
		~WinTimer();

		/**
		 * Set the timer interval in msec.
		 * A value of zero disables the timer.
		 * @param interval
		 * @return
		 */
		bool set(unsigned interval);

		/**
		 * When the interval of zero is returned the timer is disabled.
		 * @return
		 */
		[[nodiscard]] unsigned getInterval() const;

		/**
		 * Timer callback function.
		 */
		static void CALLBACK callback(
			HWND hwnd,// Handle of window for timer messages
			UINT msg,// WM_TIMER message.
			UINT timer_id,// timer identifier.
			DWORD ticks// Current system time in clock ticks.
		);

		// Overrideable timer routine.
		virtual void TimerRoutine(DWORD ticks) = 0;

	private:
		/**
		 * Actual resolution.
		 */
		unsigned _interval{0};
		/**
		 * ID of the timer assigned when created. Zero if not created.
		 */
		UINT _timerId{0};
		/**
		 * One and only timer for now.
		 */
		static WinTimer* _winTimer;
};

inline unsigned WinTimer::getInterval() const
{
	return _interval;
}

}// namespace sf
