#pragma once

#include "../global.h"
#include "TimeSpec.h"
#include <pthread.h>
#include <string>

namespace sf
{

/**
 * @brief Lightweight intra process thread synchronization.
 *
 * Can only be used with other mutex, and only within the same process.
 */
class _MISC_CLASS Mutex
{
	public:
		/**
		 * @brief Handle type.
		 */
		typedef ::pthread_mutex_t* handle_type;

		/**
		 * @brief Default constructor.
		 */
		Mutex(const char* name = nullptr);

		/**
		 * @brief Destructor.
		 */
		~Mutex();

		/**
		 * Casting operator.
		 */
		inline operator handle_type();

		/**
		 * @brief Locking class for easy locking and unlocking by destructor.
		 */
		class _MISC_CLASS Lock
		{
			public:
				/**
				 * @brief Constructor.
				 */
				explicit Lock(const Mutex& mutex, bool try_lock = false);

				/**
				 * @brief Destructor.
				 */
				~Lock();

				/**
				 * @brief Returns true when if it was acquired.
				 */
				bool acquire(bool try_lock = false, const TimeSpec& timeout = {});

				/**
				 * Releases the mutex.
				 * @return True if actually released.
				 */
				bool release();

				/**
				 * @brief Returns true if the class was locked.
				 * Always true when try_lock is false.
				 */
				explicit operator bool() const
				{
					return _locked;
				}

			private:
				/**
				 * @brief Copy constructor to prevent copying.
				 */
				Lock(const Lock& lock);

				/**
				 * @brief Holds the mutex reference.
				 */
				Mutex& _mutexRef;
				/**
				 * @brief Holds the state of the lock for the destructor.
				 */
				bool _locked{false};
		};

		friend class Condition;

	private:
		/**
		 * @brief Holds the actual handle of the OS.
		 */
		::pthread_mutex_t _mutex{0};

		/**
		 * @brief Copy Destructor.
		 */
		Mutex(const Mutex&);

		/**
		 * @brief Copy operator.
		 */
		const Mutex& operator=(const Mutex&);

		/**
		 * @brief Arbitrary test whether the mutex is destroyed or not.
		 */
		bool isMutexDestroyed();

		/**
		 * @brief Using memset to clear the mutex used.
		 */
		void clearMutex();

		/**
		 * @brief For locking without TLock instance.
		 */
		bool acquire(bool try_lock, const TimeSpec& timeout);

		/**
		 * @brief Releases the mutex.
		 */
		void release();

		/**
* @brief Holds the name for this instance for debugging purposes.
		 */
		std::string _name;

		/**
		 * @brief Holds the last thread which acquired it.
		 */
		pid_t _threadId{-1};
};

/**
 * Casting operator.
 */
inline Mutex::operator handle_type()
{
	return &_mutex;
}

}// namespace sf
