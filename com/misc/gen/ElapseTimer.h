#pragma once

#include "TimeSpec.h"
#include "../global.h"

namespace sf
{
/**
 * @brief Timer for turning true when a certain amount of time has passed.
 */
class _MISC_CLASS ElapseTimer
{
	public:
		/**
		 * @brief Default constructor
		 */
		ElapseTimer();

		/**
		 * @brief Initializing constructor setting the elapse time using a timespec struct.
		 */
		explicit ElapseTimer(const timespec& t);

		/**
		 * @brief Initializing constructor setting the elapse time in usec.
		 */
		explicit ElapseTimer(unsigned long usec);

		/**
		 * @brief Setting the elapse time and enabling the timer.
		 */
		void set(const timespec& t);

		/**
		 * @brief Setting the elapse time and enabling the timer where param 't' is in nano seconds.
		 */
		void set(unsigned long nsec);

		/**
		 * @brief Sets the timer up to elapse again.
		 */
		void reset();

		/**
		 * @brief Disables timer so Active does not return non-zero any more.
		 */
		void disable();

		/**
		 * @brief Enables timer so without resetting the elapse time.
		 */
		void enable();

		/**
		 * @brief Returns non-zero is the timer is enabled.
		 */
		[[nodiscard]] bool isEnabled() const;

		/**
		 * @brief Returns true if the time has elapsed and enabled.
		 */
		[[nodiscard]] bool isActive() const;

		/**
		 * @brief Returns the elapse time set or initialized by the constructor.
		 */
		[[nodiscard]] TimeSpec getElapseTime() const;

		/**
		 * @brief Returns the time left before the timer elapses.
		 */
		[[nodiscard]] TimeSpec getTimeLeft() const;

		/**
		 * @brief Returns the time left before the timer elapses.
		 */
		[[nodiscard]] TimeSpec getTimeLeft(const timespec&) const;

		/**
		 * @brief Test if timer is active or not.
		 */
		explicit operator bool() const;

		/**
		 * @brief This operator is used to minimize clock() calls.
		 * Return non zero if timer is active.
		 */
		bool operator()(const timespec& t) const;

	private:
		/**
		 * @brief Holds the enable flag
		 */
		bool _enabled{false};
		/**
		 * Holds the elapse target time.
		 */
		TimeSpec _target{};
		/**
		 * @brief Holds the interval the timer elapses when started.
		 */
		TimeSpec _elapsedTime{};
};

inline
ElapseTimer::ElapseTimer(const timespec& t)
{
	set(t);
}

inline
void ElapseTimer::reset()
{
	set(_elapsedTime);
}

inline
void ElapseTimer::disable()
{
	_enabled = false;
}

inline
void ElapseTimer::enable()
{
	_enabled = true;
}

inline
bool ElapseTimer::isEnabled() const
{
	return _enabled;
}

inline
TimeSpec ElapseTimer::getElapseTime() const
{
	return _elapsedTime;
}

inline
ElapseTimer::operator bool() const
{
	return isActive();
}

}
