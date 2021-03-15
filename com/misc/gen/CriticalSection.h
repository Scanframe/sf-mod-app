#pragma once

#include <string>
#include <pthread.h>

#include "global.h"

namespace sf
{

/**
 * Lightweight intra process thread synchronization.
 * Can only be used with other critical sections, and only within the same process.
 */
class _MISC_CLASS CriticalSection
{
	public:
		/**
		 * Handle type.
		 */
		typedef ::pthread_mutex_t* handle_type;

		/**
		 * Constructor.
		 */
		CriticalSection();

		/**
		 * Destructor.
		 */
		~CriticalSection();

		/**
		 * Casting operator.
		 */
		operator handle_type() // NOLINT(google-explicit-constructor)
		{
			return &_mutex;
		}

		/**
		 * Locking class for easy locking and unlocking by destructor.
		 */
		class Lock
		{
			public:
				/**
				 * Constructor.
				 */
				explicit Lock(const CriticalSection& crit_sec, bool try_lock = false);

				/**
				 * Destructor.
				 */
				~Lock();

				/**
				 * Returns true when if it was acquired.
				 */
				bool Acquire(bool try_lock = false);

				/**
				 * Releases the critical section.
				 * @return True if actually released.
				 */
				bool Release();

				/**
				 * Returns true if the class was locked. Always true when try_lock is false.
				 */
				explicit operator bool() const
				{
					return _locked;
				}

			private:
				/**
				 * Copy constructor to prevent copying.
				 */
				Lock(const Lock& lock);

				/**
				 * Holds the critical section reference.
				 */
				CriticalSection& _critSecObj;
				/**
				 * Holds the state of the lock for the destructor.
				 */
				bool _locked{false};
		};

		friend class Lock;

		friend class Condition;

	private:
		/**
		 * Holds the actual handle of the OS.
		 */
		::pthread_mutex_t _mutex{0};

		/**
		 * Copy Destructor.
		 */
		CriticalSection(const CriticalSection&);

		/**
		 * Copy operator.
		 */
		const CriticalSection& operator=(const CriticalSection&);

		/**
		 * Arbitrary test whether the mutex is destroyed or not.
		 */
		bool IsMutexDestroyed();

		/**
		 * Using memset to clear the mutex used.
		 */
		void ClearMutex();

		/**
		 * For locking without TLock instance.
		 */
		bool Acquire(bool try_lock);

		/**
		* Releases the critical section.
		*/
		void Release();
};

}
