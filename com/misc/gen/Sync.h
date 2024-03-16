#pragma once

#include "../global.h"
#include "Exception.h"
#include "Mutex.h"

namespace sf
{

/**
 * @brief Sync provides a system-independent interface to build classes that act like
 * monitors, i.e., classes in which only one member can execute on a particular
 * instance at any one time. Sync uses Mutex, so it is portable to
 * all platforms that Mutex has been ported to.
 *
 * Sync Public Interface
 *
 * None. Sync can only be a base class.
 *
 * Sync Protected Interface
 *
 *     Sync(const Sync&);
 *                             Copy constructor. Does not copy the Mutex object.
 *
 *     const Sync& operator =(const Sync&);
 *                             Assignment operator. Does not copy the Mutex object.
 *
 *     class Lock;            Handles locking and unlocking of member functions.
 *
 * Example
 * ```c++
 *     class ThreadSafe : private Sync
 *     {
 *       public:
 *         void f();
 *         void g();
 *       private:
 *         int i;
 *     };
 *
 *     void ThreadSafe::f()
 *     {
 *       Lock lock(this);
 *       if (i == 2)
 *         i = 3;
 *     }
 *
 *     void ThreadSafe::g()
 *     {
 *       Lock lock(this);
 *       if (i == 3)
 *         i = 2;
 *     }
 * ```
 */
class _MISC_CLASS Sync
{
	protected:
		/**
		 * @brief Only multi inheritance is allowed to create this instance.
		 */
		Sync() = default;

		/**
		 * @brief Copy constructor does not copy the Mutex object,
		 * since the new object is not being used in any of its own
		 * member functions. This means that the new object must start
		 * in an unlocked state.
		 */
		Sync(const Sync&) {}

		/**
		 * @brief Does not copy the Mutex object, since the new object is not
		 * being used in any of its own member functions.
		 * This means that the new object must start in an unlocked state.
		 */
		Sync& operator=(const Sync&)
		{
			return *this;
		}

	public:
		/**
		 * @brief Locks the Mutex object in the Sync object.
		 */
		class Lock : public Mutex::Lock
		{
			public:
				/**
				 * @brief Locks the Mutex object in the Sync object.
				 */
				inline explicit Lock(const Sync* sync, bool try_sync = false)
					: Mutex::Lock(sync->_mutex, try_sync)
				{}
		};

	private:
		Mutex _mutex;
};

}// namespace sf
