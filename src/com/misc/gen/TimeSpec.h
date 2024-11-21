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
struct _MISC_CLASS TimeSpec :public timespec
{
	public:
		/**
		 * @brief Integer type to indicate nanoseconds.
		 */
		typedef long nsec_type;

		/**
		 * @brief Default constructor.
		 */
		TimeSpec();

		/**
		 * @brief Copy constructor.
		 */
		TimeSpec(const TimeSpec& ts);

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
		explicit TimeSpec(const timespec& ts);

		/**
		 * @brief Initialize using seconds with a floating point.
		 * @param sec Seconds.
		 */
		explicit TimeSpec(double sec);

		/**
		 * @brief Initialize using seconds and nanoseconds.
		 * @param sec  Seconds.
		 * @param nsec Nanoseconds.
		 */
		explicit TimeSpec(time_t sec, nsec_type nsec);

		/**
		 * @brief Assignment operator using base structure.
		 *
		 * @param ts Base structure.
		 * @return Itself.
		 */
		TimeSpec& operator=(const timespec& ts);

		/**
		 * @brief Test if it has been set and thus not-zero.
		 */
		bool isZero() const;

		/**
		 * @brief Test if it has been set and thus not-zero.
		 */
		explicit operator bool() const;

		/**
		 * @brief clear this instance.
		 */
		void clear();

		/**
		 * @brief Compare operator.
		 */
		bool operator>=(const timespec& ts) const;

		/**
		 * @brief Compare operator.
		 */
		bool operator>(const timespec& ts) const;

		/**
		 * @brief Compare operator.
		 */
		bool operator<(const timespec& ts) const;

		/**
		 * @brief Compare operator.
		 */
		bool operator<=(const timespec& ts) const;

		/**
		 * @brief Compare operator.
		 */
		bool operator==(const timespec& ts) const;

		/**
		 * @brief Compare operator.
		 */
		bool operator!=(const timespec& ts) const;

		/**
		 * @brief Assignment operator.
		 *
		 * @param ts Other instance.
		 * @return Itself.
		 */
		TimeSpec& operator=(const TimeSpec& ts);

		/**
		 * @brief Operator to add nanoseconds.
		 *
		 * @param nsec Nano seconds increment value.
		 * @return Itself
		 */
		TimeSpec& operator+=(nsec_type nsec);

		/**
		 * @brief Operator to add nanoseconds.
		 *
		 * @param nsec Nano seconds decrement value.
		 * @return Itself
		 */
		TimeSpec& operator-=(nsec_type nsec);

		/**
		 * @brief Operator to add another timespec.
		 *
		 * @param t Time increment value.
		 * @return Itself
		 */
		TimeSpec& operator+=(const timespec& t);

		/**
		 * @brief Operator to subtract another timespec.
		 *
		 * @param t Time decrement value.
		 * @return Itself
		 */
		TimeSpec& operator-=(const timespec& t);

		/**
		 * @brief Operator to add another timespec.
		 *
		 * @param t Time increment value.
		 * @return Itself
		 */
		TimeSpec operator+(const timespec& t) const;

		/**
		 * @brief Operator to add another timespec.
		 *
		 * @param t Time decrement value.
		 * @return Itself
		 */
		TimeSpec operator-(const timespec& t) const;

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

inline TimeSpec::TimeSpec()
	: timespec{0, 0}
{
}

inline TimeSpec::TimeSpec(const TimeSpec& ts)
	: timespec(ts)
{
}

inline TimeSpec::TimeSpec(const timespec& ts)
	: timespec()
{
	assign(ts);
}

inline TimeSpec::TimeSpec(double sec)
	: timespec()
{
	assign(sec);
}

inline TimeSpec::TimeSpec(time_t sec, nsec_type nsec)
	: timespec{sec, nsec}
{
}

inline TimeSpec& TimeSpec::operator=(const timespec& ts)
{
	return assign(ts);
}

inline bool TimeSpec::isZero() const
{
	return tv_sec || tv_nsec;
}

inline TimeSpec::operator bool() const
{
	return tv_sec || tv_nsec;
}

inline void TimeSpec::clear()
{
	tv_sec = 0;
	tv_nsec = 0;
}

inline bool TimeSpec::operator>=(const timespec& ts) const
{
	return timespecCompare(*this, ts) >= 0;
}

inline bool TimeSpec::operator>(const timespec& ts) const
{
	return timespecCompare(*this, ts) > 0;
}

inline bool TimeSpec::operator<(const timespec& ts) const
{
	return timespecCompare(*this, ts) < 0;
}

inline bool TimeSpec::operator<=(const timespec& ts) const
{
	return timespecCompare(*this, ts) <= 0;
}

inline bool TimeSpec::operator==(const timespec& ts) const
{
	return timespecCompare(*this, ts) == 0;
}

inline bool TimeSpec::operator!=(const timespec& ts) const
{
	return timespecCompare(*this, ts) != 0;
}

inline TimeSpec& TimeSpec::operator=(const TimeSpec& ts)
{
	return assign(ts);
}

inline TimeSpec& TimeSpec::operator+=(nsec_type nsec)
{
	return add(0, nsec);
}

inline TimeSpec& TimeSpec::operator-=(nsec_type nsec)
{
	return add(0, -nsec);
}

inline TimeSpec& TimeSpec::operator+=(const timespec& t)
{
	return add(t);
}

inline TimeSpec& TimeSpec::operator-=(const timespec& t)
{
	return sub(t);
}

inline TimeSpec TimeSpec::operator+(const timespec& t) const
{
	return TimeSpec(*this).add(t);
}

inline TimeSpec TimeSpec::operator-(const timespec& t) const
{
	return TimeSpec(*this).sub(t);
}

} // namespace sf

/**
 * @brief Addition operator for std::timespec types which also work for #sf::TimeSpec.
 * @param lhs Left value of the addition.
 * @param rhs Right value of the addition.
 * @return Subtraction result as a #sf::TimeSpec.
 */
inline sf::TimeSpec operator+(const timespec& lhs, const timespec& rhs)
{
	return sf::TimeSpec(lhs).add(rhs);
}

/**
 * @brief Subtraction operator for std::timespec types which also work for #sf::TimeSpec.
 * @param lhs Left value of the subtraction.
 * @param rhs Right value of the subtraction.
 * @return Subtraction result as a #sf::TimeSpec.
 */
inline sf::TimeSpec operator-(const timespec& lhs, const timespec& rhs)
{
	return sf::TimeSpec(lhs).sub(rhs);
}

/**
 * @brief Operator for writing the #sf::TimeSpec to an output-stream.
 * Calls #TQuaternion<T>::toString() for the operator.
 * @tparam T Base floating point type.
 * @param os Output stream.
 * @param ts TimeSpec to stream out.
 * @return The passed output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const sf::TimeSpec& ts)
{
	return os << ts.toString();
}
