#pragma once

#include "Exception.h"
#include "Mutex.h"
#include "../global.h"

namespace sf
{

/**
 * Sync provides a system-independent interface to build classes that act like
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
 *                             Copy constructor. Does not copy the
 *                             Mutex object.
 *
 *     const Sync& operator =(const Sync&);
 *                             Assignment operator. Does not copy the
 *                             Mutex object.
 *
 *     class Lock;            Handles locking and unlocking of member
 *                             functions.
 *
 * Example
 *
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
 */
class _MISC_CLASS Sync
{
	protected:
		/**
		 * Only multi inheritance is allowed to create this instance.
		 */
		Sync() = default;

		/**
		 * Copy constructor does not copy the Mutex object,
		 * since the new object is not being used in any of its own
		 * member functions. This means that the new object must start
		 * in an unlocked state.
		 */
		Sync(const Sync&);

		/**
		 * Does not copy the Mutex object, since the new object is not
		 * being used in any of its own member functions.
		 * This means that the new object must start in an unlocked state.
		 */
		Sync& operator=(const Sync&)
		{
			return *this;
		}

	public:
		class Lock :public Mutex::Lock
		{
			public:
				explicit Lock(const Sync*, bool try_sync = false);

			private:
				static const Mutex& GetRef(const Sync*);
		};

		friend class Lock;

	private:
		Mutex _mutex;
};

inline
Sync::Sync(const Sync&)
{
}

/**
 * Locks the Mutex object in the Sync object.
 */
inline
Sync::Lock::Lock(const Sync* sync, bool try_sync)
	:Mutex::Lock(GetRef(sync), try_sync)
{
}

/**
 * Returns a reference to the Mutex object contained in the Sync
 * object.
 */
inline
const Mutex& Sync::Lock::GetRef(const Sync* sync)
{
	return sync->_mutex;
}

}
