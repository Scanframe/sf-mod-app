#pragma once

#include "../global.h"
#include "time.h"
#include <ctime>
#include <string>

namespace sf
{

/**
 * @brief Class wrapper for timespec structure to modify.
 */
struct _MISC_CLASS TimeSpec : public timespec
{
		/**
	 * @brief Integer type to indicate nanoseconds.
	 */
		typedef long nsec_type;

		/**
	 * @brief Default constructor.
	 */
		TimeSpec()
			: timespec()
		{
			assign(0, 0);
		}

		/**
	 * @brief Copy constructor.
	 */
		TimeSpec(const TimeSpec& ts)
			: timespec(ts)
		{
			assign(ts);
		}

		/**
	 * @brief Move constructor.
	 */
		TimeSpec(TimeSpec&& ts) = default;

		/**
	 * @brief Move assignment operator is default.
	 */
		TimeSpec& operator=(TimeSpec&&) = default;

		/**
	 * @brief Parent copy constructor.
	 */
		explicit TimeSpec(const timespec& ts)
			: timespec()
		{
			assign(ts);
		}

		/**
	 * @brief Initialize using seconds with a floating point.
	 * @param sec Seconds.
	 */
		explicit TimeSpec(double sec)
			: timespec()
		{
			assign(sec);
		}

		/**
	 * @brief Initialize using seconds and nanoseconds.
	 *
	 * @param sec  Seconds.
	 * @param nsec Nanoseconds.
	 */
		explicit TimeSpec(time_t sec, nsec_type nsec)
			: timespec()
		{
			assign(sec, nsec);
		}

		/**
	 * @brief Assignment operator using base structure.
	 *
	 * @param ts Base structure.
	 * @return Itself.
	 */
		TimeSpec& operator=(const timespec& ts)
		{
			assign(ts);
			return *this;
		}

		/**
	 * @brief Test if it has been set and thus not-zero.
	 */
		inline bool isZero() const
		{
			return tv_sec || tv_nsec;
		}

		/**
	 * @brief Test if it has been set and thus not-zero.
	 */
		inline explicit operator bool() const
		{
			return tv_sec || tv_nsec;
		}

		/**
	 * @brief clear this instance.
	 */
		inline void clear()
		{
			tv_sec = 0;
			tv_nsec = 0;
		}

		/**
	 * @brief Compare operator.
	 */
		inline bool operator>=(const timespec& ts) const
		{
			return timespecCompare(*this, ts) >= 0;
		}

		/**
	 * @brief Compare operator.
	 */
		inline bool operator>(const timespec& ts) const
		{
			return timespecCompare(*this, ts) > 0;
		}

		/**
	 * @brief Compare operator.
	 */
		inline bool operator<(const timespec& ts) const
		{
			return timespecCompare(*this, ts) < 0;
		}

		/**
	 * @brief Compare operator.
	 */
		inline bool operator<=(const timespec& ts) const
		{
			return timespecCompare(*this, ts) <= 0;
		}

		/**
	 * @brief Compare operator.
	 */
		inline bool operator==(const timespec& ts) const
		{
			return timespecCompare(*this, ts) == 0;
		}

		/**
	 * @brief Compare operator.
	 */
		inline bool operator!=(const timespec& ts) const
		{
			return timespecCompare(*this, ts) != 0;
		}

		/**
	 * @brief Assignment operator.
	 *
	 * @param ts Other instance.
	 * @return Itself.
	 */
		TimeSpec& operator=(const TimeSpec& ts)
		{
			assign(ts);
			return *this;
		}

		/**
	 * @brief Operator to add nanoseconds.
	 *
	 * @param nsec Nano seconds increment value.
	 * @return Itself
	 */
		inline TimeSpec& operator+=(nsec_type nsec)
		{
			return add(0, nsec);
		}

		/**
	 * @brief Operator to add nanoseconds.
	 *
	 * @param nsec Nano seconds decrement value.
	 * @return Itself
	 */
		inline TimeSpec& operator-=(nsec_type nsec)
		{
			return add(0, -nsec);
		}

		/**
	 * @brief Operator to add another timespec.
	 *
	 * @param t Time increment value.
	 * @return Itself
	 */
		inline TimeSpec& operator+=(const timespec& t)
		{
			return add(t);
		}

		/**
	 * @brief Operator to subtract another timespec.
	 *
	 * @param t Time decrement value.
	 * @return Itself
	 */
		inline TimeSpec& operator-=(const timespec& t)
		{
			return sub(t);
		}

		/**
	 * @brief Operator to add another timespec.
	 *
	 * @param t Time increment value.
	 * @return Itself
	 */
		TimeSpec operator+(const timespec& t) const
		{
			return TimeSpec(*this).add(t);
		}

		/**
	 * @brief Operator to add another timespec.
	 *
	 * @param t Time decrement value.
	 * @return Itself
	 */
		TimeSpec operator-(const timespec& t) const
		{
			return TimeSpec(*this).sub(t);
		}

		/**
	 * @brief Assigns a double as seconds.
	 *
	 * @param sec Seconds.
	 * @return Itself
	 */
		TimeSpec& assign(double sec);

		/**
	 * @brief Assigns a timespec base type.
	 *
	 * @param ts Instance.
	 * @return Itself.
	 */
		TimeSpec& assign(const timespec& ts);

		/**
	 * @brief Modifies the current value using the passed factor where 0.1 is + or - 10%.
	 *
	 * @param factor For randomization where 1 = 100%
	 * @return Itself.
	 */
		TimeSpec& randomize(double factor = 0.1);

		/**
	 * @brief assign the current time.
	 *
	 * @return Itself.
	 */
		TimeSpec& setTimeOfDay();

		/**
	 * @brief Adds seconds and nanoseconds to the current value.
	 *
	 * @param sec Seconds.
	 * @param nsec Nanoseconds.
	 * @return Itself.
	 */
		TimeSpec& add(time_t sec, nsec_type nsec);

		/**
	 * @brief Adds a timespec base type to the current value.
	 *
	 * @param ts Base class.
 * @return Itself
	 */
		TimeSpec& add(const timespec& ts);

		/**
 * @brief Subtracts a timespec base type from the current value.
 *
 * @param ts Base class.
 * @return Itself
 */
		TimeSpec& sub(const timespec& ts);

		/**
	 * @brief Assigns seconds and nanoseconds to the current value.
	 *
	 * @param sec Seconds
	 * @param nsec Nanosecond
	 * @return Itself.
	 */
		TimeSpec& assign(time_t sec, nsec_type nsec);

		/**
	 * @brief Gets the time value as a double.
	 * @return Seconds.
	 */
		[[nodiscard]] double toDouble() const;

		/**
	 * @brief Gets the time value as a double.
	 *
	 * @return Seconds.
	 */
		[[nodiscard]] time_t toMilliSecs() const;

		/**
	 * @brief Returns the time as #toDouble() result converted to a string.
	 *
	 * @return String
	 */
		[[nodiscard]] std::string toString() const;
};

/**
 * @brief Operator for streaming a TTimeSpec class.
 */
inline std::ostream& operator<<(std::ostream& os, const TimeSpec& ts)
{
	return (os << ts.toString());
}

}// namespace sf