#pragma once

#include "../gen/Mutex.h"
#include "../gen/TimeSpec.h"
#include "../global.h"
#include <atomic>

namespace sf
{

/**
* @brief Wrapper for the pthread_cond mechanism.
 *
* The condition is a single or all thread notification mechanism.
*/
class _MISC_CLASS Condition
{
	public:
		/**
		 * @brief Handle type.
		 */
		typedef ::pthread_cond_t* handle_type;

		/**
		 * @brief Constructor.
		 */
		explicit Condition(const std::string& name = "");

		/**
		 * @brief Destructor.
		 */
		~Condition();

		/**
		 * @brief Waits for a signal with a timeout when a non-zero timeout is passed.
		 *
		 * @param mutex
		 * @param timeout Timeout applicable when not-zero.
		 * @return True when signaled and False when not.
		 */
		bool wait(Mutex& mutex, const TimeSpec& timeout = TimeSpec());

		/**
		 * @brief Signals a single thread.
		 * @return Returns true when a thread was signaled.
		 */
		bool notifyOne();

		/**
		 * @brief Signals all threads.
		 * @return Amount of waiting threads.
		 */
		int notifyAll();

		/**
		* @brief Casting operator.
		*/
		operator handle_type();

		/**
		 * @brief Prevent copying.
		 */
		const Condition& operator=(const Condition&) = delete;

		/**
		 * @brief Returns the amount of threads waiting.
		 */
		int getWaiting() const;

		const std::string& getName();

	private:
		/**
		 * @brief Amount of threads waiting for work.
		 */
		std::atomic<int> _waiting{0};
		/**
		 * @brief Holds the actual handle of the OS.
		 */
		::pthread_cond_t _cond{};
		/**
		 * @brief Holds the name for this instance for debugging purposes.
		 */
		std::string _name;
};

inline Condition::operator handle_type()
{
	return &_cond;
}

}// namespace sf
