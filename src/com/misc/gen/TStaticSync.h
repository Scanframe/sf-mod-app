#pragma once

#include "../global.h"
#include "Exception.h"
#include "Sync.h"

namespace sf
{

/**
 * @brief TStaticSync provides a system-independent interface to build sets of classes
 * that act somewhat like monitors, i.e., classes in which only one member
 * function can execute at any one time regardless of which instance it is
 * being called on. TStaticSync uses Mutex, so it is portable to all
 * platforms that Mutex has been ported to.
 *
 * TStaticSync Public Interface
 *
 * None. TStaticSync can only be a base class.
 *
 * TStaticSync Protected Interface
 *
 *     TStaticSync<T>(const TStaticSync<T>&);
 *                             Copy constructor. Does not copy the Mutex object.
 *
 *     const TStaticSync<T>& operator =(const TStaticSync<T>&);
 *                             Assignment operator. Does not copy the Mutex object.
 *
 *     class Lock;            Handles locking and unlocking of member functions.
 *
 * Example:
 * ```c++
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
 * ```
 */
template<class T>
class TStaticSync
{
	public:
		/**
		 * @brief Do not allow copying.
		 */
		TStaticSync<T>& operator=(const TStaticSync<T>&) = delete;

	protected:
		/**
		 * @brief Default constructor.
		 */
		TStaticSync();

		/**
		 * @brief Copy constructor.
		 */
		TStaticSync(const TStaticSync<T>&);

		/**
		 * @brief Destructor.
		 */
		~TStaticSync();

		/**
		 *
		 */
		class Lock : public Mutex::Lock
		{
			public:
				/**
				 * @brief Default constructor.
				 */
				Lock()
					: Mutex::Lock(*TStaticSync<T>::_mutex, false)
				{}

				/**
				 * @brief Try constructor.
				 *
				 * @param try_lock When true the constructor returns even when not locked.
				 */
				explicit Lock(bool try_lock)
					: Mutex::Lock(*TStaticSync<T>::_mutex, try_lock)
				{}
		};

	private:
		/**
		 * @brief Holds the singleton mutex for all instances.
		 */
		static Mutex* _mutex;
		/**
		 * @brief Usage counter on which the mutex is created or deleted.
		 */
		static unsigned _count;
};

template<class T>
inline TStaticSync<T>::TStaticSync()
{
	/**
	 * If this is the first TStaticSync<T> object to be constructed, create the semaphore.
	 *
	 * The copy constructor only has to increment the count, since there will
	 * already be at least one TStaticSync<T> object, namely, the one being copied.
	 */
	if (_count++ == 0)
	{
		_mutex = new Mutex;
	}
}

template<class T>
inline TStaticSync<T>::TStaticSync(const TStaticSync<T>&)
{
	_count++;
}

template<class T>
inline TStaticSync<T>::~TStaticSync()
{
	// If this is the only remaining TStaticSync<T> object, destroy the semaphore.
	if (--_count == 0)
	{
		delete _mutex;
	}
}

}// namespace sf
