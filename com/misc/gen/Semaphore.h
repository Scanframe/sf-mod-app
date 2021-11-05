#pragma once

#include "../global.h"
#include "TimeSpec.h"
#include <semaphore.h>

namespace sf
{

/**
 * @brief Semaphore class built around posix sem_xxx functionality.
 */
class _MISC_CLASS Semaphore
{
	public:
		/**
		 * @brief Constructor initializing the counter.
		 */
		explicit Semaphore(int initialCount = 0);

		/**
		 * @brief Destructor cleaning up the allocated semaphore.
		 */
		~Semaphore();

		/**
		 * @brief Signals any thread which is locked.
		 * @return True on success.
		 */
		bool post();

		/**
		 * @brief Gets the semaphore integer value.
		 * @return Last value but it can be changed in the meantime.
		 */
		[[nodiscard]] int value() const;

		/**
		 * @brief Locking class.
		 */
		class _MISC_CLASS Lock
		{
			public:
				// Default constructor which sees to it that it is released.
				inline Lock();

				/**
				 * @brief Constructor @see acquire()
				 * @param semaphore Semaphore to lock.
				 * @param timeout Timeout to wait for a lock.
				 */
				explicit inline Lock(const Semaphore& semaphore, const TimeSpec& timeout);

				/**
				 * @brief Constructor @see acquire()
				 * @param semaphore Semaphore to lock.
				 * @param tryWait When True the constructor is blocking.
				 */
				explicit inline Lock(const Semaphore& semaphore, bool tryWait = false);

				/**
				 * @brief Destructor cleaning up.
				 */
				inline ~Lock();

				/**
				 * @brief Waits to acquire the semaphore.
				 * Is called from the constructor.
				 * The timeout is the time the function will block.
				 * Use method #isAcquired to check what the current status is.
				 * @param semaphore Semaphore to lock.
				 * @param timeout Timeout to wait for a lock.
				 * @return True when acquired.
				 */
				bool acquire(const Semaphore& semaphore, const TimeSpec& timeout);

				/**
				 * @brief Waits to acquire the semaphore.
				 * Is also called from the constructor.
				 * When the 'tryWait' argument is true the method will not block but return immediately.
				 * @param semaphore Semaphore to lock.
				 * @param tryWait When True the function is blocking.
				 * @return True when acquired.
				 */
				bool acquire(const Semaphore& semaphore, bool tryWait = false);

				/**
				 * @brief See if semaphore was acquired or timed-out.
				 * @return True when acquired.
				 */
				[[nodiscard]] inline bool isAcquired() const;

				/**
				 * @brief Releases
				 * @param relinquish True when this lock can acquire again.
				 */
				void release(bool relinquish = true);

			private:
				// Holds the semaphore pointer when signaled.
				const Semaphore* _semaphore;
		};

	protected:
		/**
		 * Type of handle/structure referencing the semaphore.
		 */
		typedef sem_t handle_type;

		/**
		 * @brief Called by Lock destructor.
		 */
		void release();

		/**
		 * @brief Reports an error from call to a sem_xxx function.
		 */
		void reportError(const char* funcName) const;

		/**
		 * @brief Holds the semaphore handle/structure.
		 */
		handle_type _handle;

};

inline
Semaphore::Lock::Lock()
	:_semaphore(nullptr)
{
}

inline
Semaphore::Lock::Lock(const Semaphore& semaphore, const TimeSpec& timeout)
	:_semaphore(nullptr)
{
	acquire(semaphore, timeout);
}

inline
Semaphore::Lock::Lock(const Semaphore& semaphore, bool tryWait)
	:_semaphore(nullptr)
{
	acquire(semaphore, tryWait);
}

inline
Semaphore::Lock::~Lock()
{
	release();
}

inline
bool Semaphore::Lock::isAcquired() const
{
	// Object was signaled.
	return _semaphore != nullptr;
}

}
