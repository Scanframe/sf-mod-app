#pragma once

#include <ctime>
#include <string>
#include "../global.h"

namespace sf
{

/**
 * Class wrapper for timespec struct.
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
	 * Copy constructor.
	 * @param ts
	 */
	TimeSpec(const TimeSpec& ts)
		:timespec(ts)
	{
		assign(ts);
	}

	/**
	 * Parent copy constructor.
	 * @param ts
	 */
	explicit TimeSpec(const timespec& ts)
		:timespec()
	{
		assign(ts);
	}

	/**
	 * Initialize using whole nano seconds.
	 * @param nsec
	 */
	explicit TimeSpec(long int nsec)
		:timespec()
	{
		assign(0, nsec);
	}

	/**
	 * Initialize using seconds with a floating point.
	 * @param sec
	 */
	explicit TimeSpec(double sec) :timespec()
	{
		assign(sec);
	}

	/**
	 * Initialize using seconds and nano seconds.
	 * @param sec
	 * @param nsec
	 */
	explicit TimeSpec(long int sec, long int nsec) :timespec()
	{
		assign(sec, nsec);
	}

	/**
	 * Assignment operator using base structure.
	 * @param ts
	 * @return
	 */
	TimeSpec& operator=(const timespec& ts)
	{
		assign(ts);
		return *this;
	}

	/**
	 * Assignment operator.
	 * @param ts
	 * @return
	 */
	TimeSpec& operator=(const TimeSpec& ts)
	{
		assign(ts);
		return *this;
	}

	/**
	 * Operator to add nano seconds.
	 * @param nsec
	 * @return
	 */
	TimeSpec& operator+=(long int nsec)
	{
		return add(0, nsec);
	}

	/**
	 * Assigns a double as seconds.
	 * @param sec
	 * @return
	 */
	TimeSpec& assign(double sec);

	/**
	 * Assigns a timespec base type.
	 * @param ts
	 * @return
	 */
	TimeSpec& assign(const timespec& ts)
	{
		this->tv_sec = ts.tv_sec;
		this->tv_nsec = ts.tv_nsec;
		return *this;
	}

	/**
	 * Modifies the current value using the passed factor where 0.1 is + or - 10%.
	 * @param factor
	 * @return
	 */
	TimeSpec& randomize(double factor = 0.1);

	/**
	 * Assign the the current time.
	 * @return
	 */
	TimeSpec& setTimeOfDay();

	/**
	 * Adds seconds and nanoseconds to the current value.
	 * @param sec
	 * @param nsec
	 * @return
	 */
	TimeSpec& add(long int sec, long int nsec);

	/**
	 * Adds a timespec base type to the current value.
	 * @param ts
	 * @return
	 */
	TimeSpec& add(const timespec& ts) {return add(ts.tv_sec, ts.tv_nsec);}

	/**
	 * Subtracts timespec base type from the current value.
	 * @param ts
	 * @return
	 */
	TimeSpec& sub(const timespec& ts) {return add(-ts.tv_sec, -ts.tv_nsec);}

	/**
	 * Assigns seconds and nanoseconds to the current value.
	 * @param sec Seconds
	 * @param nsec Nanosecond
	 * @return
	 */
	TimeSpec& assign(long int sec, long int nsec);

	/**
	 * Returns the value as a double.
	 * @return
	 */
	[[nodiscard]] double toDouble() const;

	/**
	 * Returns the time as a readable string.
	 * @return
	 */
	[[nodiscard]] std::string toString() const;
};

/**
 * Operator for streaming a TTimeSpec class.
 */
inline
std::ostream& operator<<(std::ostream& os, const TimeSpec& ts)
{
	return (os << ts.toString());
}

}