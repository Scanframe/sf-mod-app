#pragma once

#include "Exception.h"
#include "misc/lnx/Sync.h"
#include "misc/global.h"

namespace sf
{

/**
 * @brief
 * TStaticSync provides a system-independent interface to build sets of classes
 * that act somewhat like monitors, i.e., classes in which only one member
 * function can execute at any one time regardless of which instance it is
 * being called on. TStaticSync uses CriticalSection, so it is portable to all
 * platforms that CriticalSection has been ported to.
 *
 * TStaticSync Public Interface
 *
 * None. TStaticSync can only be a base class.
 *
 * TStaticSync Protected Interface
 *
 *     TStaticSync<T>(const TStaticSync<T>&);
 *                             Copy constructor. Does not copy the
 *                             CriticalSection object.
 *
 *     const TStaticSync<T>& operator =(const TStaticSync<T>&);
 *                             Assignment operator. Does not copy the
 *                             CriticalSection object.
 *
 *     class Lock;            Handles locking and unlocking of member
 *                             functions.
 *
 * Example
 *
 *     class ThreadSafe : private TStaticSync<ThreadSafe>
 *     {
 *       public:
 *         void f();
 *         void g();
 *       private:
 *         static int i;
 *     };
 *
 *     void ThreadSafe::f()
 *     {
 *       TLock lock;
 *       if (i == 2)
 *         i = 3;
 *     }
 *
 *     void ThreadSafe::g()
 *     {
 *       TLock lock;
 *       if (i == 3)
 *         i = 2;
 *     }
 */
template<class T>
class TStaticSync
{
	public:
		TStaticSync<T>& operator=(const TStaticSync<T>&) = delete;

	protected:
		/**
		 * Default constructor.
		 */
		TStaticSync();

		/**
		 * Copy constructor.
		 */
		TStaticSync(const TStaticSync<T>&);

		/**
		 * Destructor.
		 */
		~TStaticSync();

		/**
		 *
		 */
		class Lock :public CriticalSection::Lock
		{
			public:
				/**
				 * Default constructor.
				 */

				Lock()
					:CriticalSection::Lock(*TStaticSync<T>::_critSec, false) {}

				/**
				 * Try constructor.
				 * @param try_lock
				 */
				explicit Lock(bool try_lock)
					:CriticalSection::Lock(*TStaticSync<T>::_critSec, try_lock) {}
		};

	private:
		/**
		 * Holds the singleton critical section for all instances.
		 */
		static CriticalSection* _critSec;
		/**
		 * Usage counter on which the critical section is created or deleted.
		 */
		static unsigned _count;

		friend class TLock;
};

/**
 * If this is the first TStaticSync<T> object to be constructed, create the
 * semaphore.
 * The copy constructor only has to increment the count, since there will
 * already be at least one TStaticSync<T> object, namely, the one being copied.
 */
template<class T>
inline
TStaticSync<T>::TStaticSync()
{
	if (_count++ == 0)
	{
		_critSec = new CriticalSection;
	}
}

template<class T>
inline
TStaticSync<T>::TStaticSync(const TStaticSync<T>&)
{
	_count++;
}

/**
 * If this is the only remaining TStaticSync<T> object, destroy the semaphore.
 */
template<class T>
inline
TStaticSync<T>::~TStaticSync()
{
	if (--_count == 0)
	{
		delete _critSec;
	}
}

}
