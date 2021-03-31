#pragma once

#include <ctime>
#include <string>
#include "../global.h"

namespace sf
{

/**
 * @brief Class wrapper for timespec structure to modify.
 */
struct _MISC_CLASS TimeSpec :public timespec
{
	/**
	 * Default constructor.
	 */
	TimeSpec() :timespec()
	{
		assign(0, 0);
	}

	/**
	 * @brief Copy constructor.
	 * @param ts
	 */
	TimeSpec(const TimeSpec& ts)
		:timespec(ts)
	{
		assign(ts);
	}

	/**
	 * @brief Parent copy constructor.
	 * @param ts
	 */
	explicit TimeSpec(const timespec& ts)
		:timespec()
	{
		assign(ts);
	}

	/**
	 * @brief Initialize using whole nano seconds.
	 * @param nsec Nanoseconds.
	 */
	explicit TimeSpec(long int nsec)
		:timespec()
	{
		assign(0, nsec);
	}

	/**
	 * @brief Initialize using seconds with a floating point.
	 * @param sec Seconds.
	 */
	explicit TimeSpec(double sec) :timespec()
	{
		assign(sec);
	}

	/**
	 * @brief Initialize using seconds and nano seconds.
	 * @param sec  Seconds.
	 * @param nsec Nanoseconds.
	 */
	explicit TimeSpec(long int sec, long int nsec) :timespec()
	{
		assign(sec, nsec);
	}

	/**
	 * @brief Assignment operator using base structure.
	 * @param ts Base structure.
	 * @return Itself.
	 */
	TimeSpec& operator=(const timespec& ts)
	{
		assign(ts);
		return *this;
	}

	/**
	 * @brief Assignment operator.
	 * @param ts Other instance.
	 * @return Itself.
	 */
	TimeSpec& operator=(const TimeSpec& ts)
	{
		assign(ts);
		return *this;
	}

	/**
	 * @brief Operator to add nano seconds.
	 * @param nsec Nanoseconds.
	 * @return Itself
	 */
	TimeSpec& operator+=(long int nsec)
	{
		return add(0, nsec);
	}

	/**
	 * @brief Assigns a double as seconds.
	 * @param sec Seconds.
	 * @return Itself
	 */
	TimeSpec& assign(double sec);

	/**
	 * @brief Assigns a timespec base type.
	 * @param ts Instance.
	 * @return Itself.
	 */
	TimeSpec& assign(const timespec& ts)
	{
		this->tv_sec = ts.tv_sec;
		this->tv_nsec = ts.tv_nsec;
		return *this;
	}

	/**
	 * @brief Modifies the current value using the passed factor where 0.1 is + or - 10%.
	 *
	 * @param factor For randomization where 1 = 100%
	 * @return Itself.
	 */
	TimeSpec& randomize(double factor = 0.1);

	/**
	 * @brief Assign the the current time.
	 * @return Itself.
	 */
	TimeSpec& setTimeOfDay();

	/**
	 * @brief Adds seconds and nanoseconds to the current value.
	 * @param sec Seconds.
	 * @param nsec Nanoseconds.
	 * @return Itself.
	 */
	TimeSpec& add(long int sec, long int nsec);

	/**
	 * @brief Adds a timespec base type to the current value.
	 * @param ts Instance.
	 * @return
	 */
	TimeSpec& add(const timespec& ts);

	/**
	 * @brief Subtracts timespec base type from the current value.
	 * @param ts Base type.
	 * @return Itself.
	 */
	TimeSpec& sub(const timespec& ts);

	/**
	 * @brief Assigns seconds and nanoseconds to the current value.
	 * @param sec Seconds
	 * @param nsec Nanosecond
	 * @return Itself.
	 */
	TimeSpec& assign(long int sec, long int nsec);

	/**
	 * @brief Gets the time value as a double.
	 * @return Seconds.
	 */
	[[nodiscard]] double toDouble() const;

	/**
	 * @brief Returns the time as #toDouble() result converted to a string.
	 * @return String
	 */
	[[nodiscard]] std::string toString() const;
};

/**
 * @brief Operator for streaming a TTimeSpec class.
 */
inline
std::ostream& operator<<(std::ostream& os, const TimeSpec& ts)
{
	return (os << ts.toString());
}

}