#pragma once

#include "../gen/Mutex.h"
#include "../gen/TimeSpec.h"
#include "../global.h"

namespace sf
{

/**
* Class 'Condition' is a wrapper for the pthread_cond mechanism.
* The condition is a single or all thread notification mechanism.
*/
class _MISC_CLASS Condition
{
	public:
		/**
		 * Handle type.
		 */
		typedef ::pthread_cond_t* handle_type;

		/**
		 * Constructor.
		 */
		Condition();

		/**
		 * Destructor.
		 */
		~Condition();

		/**
		 * Waits for a signal.
		 */
		bool wait(Mutex& mutex);

		/**
		 * Waits for a signal with a timeout.
		 */
		bool wait(Mutex& mutex, const TimeSpec& timeout);

		/**
		 * Signals a single thread.
		 * Returns the amount of waiting threads.
		 */
		int notifyOne(Mutex& mutex);

		/**
		 * Signals all threads.
		 * Returns the amount of waiting threads.
		 */
		int notifyAll(Mutex& mutex);

		/**
		* Casting operator.
		*/
		operator handle_type() // NOLINT(google-explicit-constructor)
		{
			return &_cond;
		}

	private:
		/**
		 * Prevent copying.
		 */
		const Condition& operator=(const Condition&);

		/**
		 * Work to be done and is counted down when the wait function is released.
		 */
		int _work{0};
		/**
		 * Amount of threads waiting.
		 */
		int _waiting{0};
		/**
		 * Holds the actual handle of the OS.
		 */
		::pthread_cond_t _cond{};
};

}
