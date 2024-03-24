#pragma once

#include "../global.h"
#include "TimeSpec.h"

namespace sf
{
/**
 * @brief Timer class that has fixed time intervals at which it becomes true.
 *
 * If one or several intervals are missed, this timer will not catch up.
 */
class _MISC_CLASS IntervalTimer
{
	public:
		/**
		 * @brief Default constructor.
		 *
		 * The timer is by default disabled until set() is called.
		 */
		IntervalTimer() = default;

		/**
		 * @brief Initializing constructor setting the elapse time using a timespec struct.
		 */
		explicit inline IntervalTimer(const timespec& t);

		/**
		 * @brief Initializing constructor setting the elapse time in usec.
		 */
		explicit IntervalTimer(int usec);

		/**
		 * @brief Initializing constructor setting the elapse time in seconds and fraction using a double.
		 */
		explicit IntervalTimer(double sec);

		/**
		 * @brief Sets the timers interval time.
		 *
		 * When zero is passed it is always active.
		 */
		void set(const timespec& t);

		/**
		 * @brief Tests if the timer is active if it is active it returns non-zero and resets the timer to a new event.
		 *
		 * @return True when active.
		 */
		[[nodiscard]] bool active() const;

		/**
		 * @brief Allows passing timespec value our self to reduce overhead in case of multiple timers.
		 *
		 * @param t #sf::getTime() value.
		 * @return True when active.
		 */
		[[nodiscard]] bool active(const timespec& t) const;

		/**
		 * @brief Enables the timer with the current interval time.
		 */
		inline void enable();

		/**
		 * @brief Disables the timer.
		 */
		inline void disable();

		/**
		 * @brief Gets timer enabled.
		 *
		 * @return True on enabled.
		 */
		[[nodiscard]] inline bool isEnabled() const;

		/**
		 * @brief Gets the set interval time in TimeSpec format.
		 *
		 * @return Interval time.
		 */
		[[nodiscard]] inline const TimeSpec& getInterval() const;
		/**
		 * @brief Gets the set interval time in TimeSpec format.
		 *
		 * @return Interval time.
		 */
		[[nodiscard]] inline const TimeSpec& getTarget() const;

		/**
		 * @brief Gets the time left in msec before function active becomes true.
		 *
		 * @return Time left.
		 */
		[[nodiscard]] TimeSpec getTimeLeft() const;

		/**
		 * @brief Gets the time left in TimeSpec format before function active becomes true.
		 *
		 * @param t #sf::getTime() value.
		 * @return Time left.
		 */
		[[nodiscard]] TimeSpec getTimeLeft(const timespec& t) const;

		/**
		 * @brief Same as time left, but it also gives the time passed the target time and before that as a negative value.
		 *
		 * @return Time over value.
		 */
		[[nodiscard]] TimeSpec getTimeOver() const;

		/**
		 * @brief Same as time left, but it also gives the time passed the target time and before that as a negative value.
		 *
		 * @param t Clock() value.
		 * @return Time over value.
		 */
		[[nodiscard]] TimeSpec getTimeOver(const timespec& t) const;

		/**
		 * @brief Test operator.
		 *
		 * Used in if and while statements.
		 */
		explicit inline operator bool() const;

		/**
		  * This operator is used to minimize clock() calls by passing the value itself.
		  *
		 * @param t Clock() value.
		  * @return True if timer is active.
		  */
		inline bool operator()(const timespec& t) const;

	private:
		/**
		 * @brief Holds the interval time with which the target time is set.
		 */
		TimeSpec _interval{0, 0};
		/**
		 * @brief Holds the target time at which the timer should active.
		 */
		TimeSpec _target{0, 0};
		/**
		 * @brief Holds the enable flag
		 */
		bool _enabled{false};
};

inline IntervalTimer::IntervalTimer(const timespec& t)
{
	set(t);
}

inline void IntervalTimer::enable()
{
	_enabled = true;
}

inline void IntervalTimer::disable()
{
	_enabled = false;
}

inline bool IntervalTimer::isEnabled() const
{
	return _enabled;
}

inline const TimeSpec& IntervalTimer::getInterval() const
{
	return _interval;
}

inline const TimeSpec& IntervalTimer::getTarget() const
{
	return _target;
}

inline IntervalTimer::operator bool() const
{
	return active();
}

inline bool IntervalTimer::operator()(const timespec& t) const
{
	return active(t);
}

}// namespace sf
