#pragma once

#include "../gen/Mutex.h"
#include "../gen/TimeSpec.h"
#include "../global.h"

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
		Condition();

		/**
		 * @brief Destructor.
		 */
		~Condition();

		/**
		 * @brief Waits for a signal.
		 */
		bool wait(Mutex& mutex);

		/**
		 * @brief Waits for a signal with a timeout.
		 */
		bool wait(Mutex& mutex, const TimeSpec& timeout);

		/**
		 * @brief Signals a single thread.
		 * @return Amount of waiting threads.
		 */
		int notifyOne(Mutex& mutex);

		/**
		 * @brief Signals all threads.
		 * @return Amount of waiting threads.
		 */
		int notifyAll(Mutex& mutex);

		/**
		* @brief Casting operator.
		*/
		operator handle_type() // NOLINT(google-explicit-constructor)
		{
			return &_cond;
		}

		/**
		 * @brief Prevent copying.
		 */
		const Condition& operator=(const Condition&) = delete;

	private:
		/**
		 * @brief Work to be done and is counted down when the wait function is released.
		 */
		int _work{0};
		/**
		 * @brief Amount of threads waiting.
		 */
		int _waiting{0};
		/**
		 * @brief Holds the actual handle of the OS.
		 */
		::pthread_cond_t _cond{};
};

}
