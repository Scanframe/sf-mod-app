#pragma once

#include "../gen/TimeSpec.h"

namespace sf
{

/**
 * High resolution timer for testing performance.
 */
class _MISC_CLASS PerformanceTimer
{
	public:
		/**
		 * Default constructor. Initializes the timer and resets it.
		 */
		PerformanceTimer();

		/**
		 * Copy constructor.
		 */
		PerformanceTimer(const PerformanceTimer& pt) = default;

		/**
		 * Assignment operator.
		 */
		PerformanceTimer& operator=(const PerformanceTimer& pt)
		{
			if (this != &pt)
			{
				_start = pt._start;
			}
			return *this;
		}

		/**
		 * Resets the time to start measuring.
		 */
		void reset();

		/**
		 * Returns the current elapsed time in seconds since the last reset.
		 */
		TimeSpec elapse();

		/**
		 * Returns the current elapsed time in seconds since the last reset.
		 */
		TimeSpec elapse(const TimeSpec& ts);

		/**
		 * Returns the current elapsed time in micro seconds since the last reset.
		 */
		unsigned long elapseUSec();

		/**
		 * Returns the current elapsed time in milli seconds since the last reset.
		 */
		unsigned long elapseMSec();

	protected:
		/**
		 * Holds the start time.
		 */
		TimeSpec _start;
};

}
