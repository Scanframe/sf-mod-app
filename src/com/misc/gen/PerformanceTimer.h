#pragma once

#include "../gen/TimeSpec.h"

namespace sf
{

/**
 * @brief High resolution timer for testing performance.
 */
class _MISC_CLASS PerformanceTimer
{
	public:
		/**
		 * @brief Default constructor. Initializes the timer and resets it.
		 */
		PerformanceTimer();

		/**
		 * @brief Copy constructor.
		 */
		PerformanceTimer(const PerformanceTimer& pt) = default;

		/**
		 * @brief Assignment operator.
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
		 * @brief Resets the time to start measuring.
		 */
		void reset();

		/**
		 * @brief Returns the current elapsed time in seconds since the last reset.
		 */
		TimeSpec elapse();

		/**
		 * @brief Returns the current elapsed time in seconds since the last reset.
		 */
		TimeSpec elapse(const TimeSpec& ts);

		/**
		 * @brief Returns the current elapsed time in micro seconds since the last reset.
		 */
		unsigned long elapseUSec();

		/**
		 * @brief Returns the current elapsed time in milli seconds since the last reset.
		 */
		unsigned long elapseMSec();

	protected:
		/**
		 * @brief Holds the start time.
		 */
		TimeSpec _start;
};

}// namespace sf
