#pragma once

#include "../global.h"
#include <QMutexLocker>

namespace sf
{

/**
 * @brief QSync provides a interface to build classes that act like monitors.
 *
 * This class does not provide a try lock mechanism
 */
class _MISC_CLASS QtSync
{
	protected:
		/**
		 * @brief Only by inheritance is allowed to create this instance.
		 */
		QtSync() = default;

		/**
		 * @brief Copy constructor does not copy the Mutex object,
		 * since the new object is not being used in any of its own
		 * member functions. This means that the new object must start
		 * in an unlocked state.
		 */
		QtSync(const QtSync&) {}

		/**
		 * @brief Does not copy the Mutex object, since the new object is not
		 * being used in any of its own member functions.
		 * This means that the new object must start in an unlocked state.
		 */
		QtSync& operator=(const QtSync&)
		{
			return *this;
		}

	public:
		/**
		 * @brief Class locking the mutex.
		 */
		class _MISC_CLASS Lock
		{
			public:
				/**
				 * @brief Locks the QMutex object in the QSync object.
				 */
				explicit Lock(QtSync* sync, bool tryLock = false, int timeout = -1)
					:_mutex(sync->_mutex)
					 , _locked(false)
				{
					_locked = lock(tryLock, timeout);
				}

				/**
				 * @brief Destructor unlocking when locked.
				 */
				~Lock()
				{
					unlock();
				}

				/**
				 * @brief Gets the lock status.
				 * @return True when locked and False when unlocked.
				 */
				inline explicit operator bool() const
				{
					return _locked;
				}

				/**
				 * @brief Used when try locking the sync/mutex.
				 * @param tryLock True an attempt is made to lock.
				 * @param timeout Milliseconds to try locking.
				 * @return TRue when successful to lock, otherwise False.
				 */
				[[nodiscard]] bool lock(bool tryLock = false, int timeout = -1);

				/**
				 * @brief Unlocks the mutex.
				 */
				inline void unlock()
				{
					// Only when locked un lock is needed.
					if (_locked)
					{
						_mutex.unlock();
						_locked = false;
					}
				}

			private:
				/**
				 * @brief Holds the mutex reference of the passed QSync object.
				 */
				QMutex& _mutex;
				/**
				 * @brief Holds the status of the
				 */
				bool _locked;
		};

	private:
		QMutex _mutex;
};

}
