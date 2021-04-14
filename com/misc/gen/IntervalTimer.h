#pragma once

#include <ctime>
#include "../global.h"

namespace sf
{
/**
 * @brief Timer class that has fixed time intervals at which it becomes true.
 *
 * If one ore several intervals are missed, this timer will not catch up.
 */
class _MISC_CLASS IntervalTimer
{
	public:
		/**
		 * @brief Default constructor is by default disabled until Set() is called.
		 */
		IntervalTimer();

		/**
		 * @brief Initializing constructor setting the timers interval time in msec.
		 *
		 * Is by default enabled.
		 */
		explicit IntervalTimer(clock_t t);

		/**
		 * @brief Sets the timers interval time in msec.
		 * When zero is passed it is always active.
		 */
		void set(clock_t t);

		/**
		 * @brief Tests if the timer is active if it is active it returns non-zero and resets the timer to a new event.
		 *
		 * @return True when active.
		 */
		[[nodiscard]] bool active() const;

		/**
		 * @brief Allows passing clock() value ourself to reduce overhead in case of multiple timers.
		 *
		 * @param time Clock() value.
		 * @return True when active.
		 */
		[[nodiscard]] bool active(clock_t time) const;

		/**
		 * @brief Enables the timer with the current interval time.
		 */
		void enable();

		/**
		 * @brief Disables the timer.
		 */
		void disable();

		/**
		 * @brief Gets timer enabled.
		 *
		 * @return True on enabled.
		 */
		[[nodiscard]] bool isEnabled() const;

		/**
		 * @brief Gets the set interval time in msec.
		 * @return Interval time.
		 */
		[[nodiscard]] clock_t getIntervalTime() const;

		/**
		 * @brief Gets the time left in msec before function active becomes true.
		 * @return Time left.
		 */
		[[nodiscard]] clock_t getTimeLeft() const;

		/**
		 * @brief Gets the time left in msec before function active becomes true.
		 * @param time Clock() value.
		 * @return Time left.
		 */
		[[nodiscard]] clock_t getTimeLeft(clock_t time) const;

		/**
		 * @brief Same as time left but it also gives the time passed the target time and before that as a negative value.
		 *
		 * @return Time over value.
		 */
		[[nodiscard]] clock_t getTimeOver() const;

		/**
		 * @brief Same as time left but it also gives the time passed the target time and before that as a negative value.
		 *
		 * @param t Clock() value.
		 * @return Time over value.
		 */
		[[nodiscard]] clock_t getTimeOver(clock_t t) const;

		/**
		 * @brief Test operator.
		 *
		 * Used in if and while statements.
		 */
		explicit operator bool() const;

		 /**
		  * This operator is used to minimize clock() calls by passing the value itself.
		  *
		 * @param t Clock() value.
		  * @return True if timer is active.
		  */
		bool operator()(clock_t t) const;

	private:
		/**
		 * @brief Holds the interval time with which the target time is set.
		 */
		clock_t _interval{0};
		/**
		 * @brief Holds the target time at which the timer should active.
		 */
		clock_t _target{0};
		/**
		 * @brief Holds the enable flag
		 */
		bool _enabled{false};
};

inline
IntervalTimer::IntervalTimer(clock_t t)
{
	set(t);
}

inline
void IntervalTimer::enable()
{
	_enabled = true;
}

inline
void IntervalTimer::disable()
{
	_enabled = false;
}

inline
bool IntervalTimer::isEnabled() const
{
	return _enabled;
}

inline
clock_t IntervalTimer::getIntervalTime() const
{
	return _interval;
}

inline
IntervalTimer::operator bool() const
{
	return active();
}

inline
bool IntervalTimer::operator()(clock_t t) const
{
	return active(t);
}

}
