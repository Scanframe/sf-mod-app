#include "Mutex.h"
#include "Exception.h"
#include "Thread.h"
#include <cstring>

namespace sf
{

Mutex::Mutex(const char* name)
	: _name(name ? name : "unnamed")
{
	::pthread_mutexattr_t attr{};
	int error = ::pthread_mutexattr_init(&attr);
	if (error)
		throw ExceptionSystemCall("pthread_mutexattr_init", error, typeid(*this).name(), __FUNCTION__);
	error = ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	if (error)
		throw ExceptionSystemCall("pthread_mutexattr_settype", error, typeid(*this).name(), __FUNCTION__);
	error = ::pthread_mutex_init(const_cast<pthread_mutex_t*>(&_mutex), &attr);
	if (error)
		throw ExceptionSystemCall("pthread_mutex_init", error, typeid(*this).name(), __FUNCTION__);
	error = ::pthread_mutexattr_destroy(&attr);
	if (error)
		throw ExceptionSystemCall("pthread_mutexattr_destroy", error, typeid(*this).name(), __FUNCTION__);
}

Mutex::~Mutex()
{
	int error = ::pthread_mutex_destroy(const_cast<pthread_mutex_t*>(&_mutex));
	if (error)
	{
		//throw ExceptionSystemCall("pthread_mutex_destroy", error, typeid(*this).name(), __FUNCTION__);
	}
	// Clear the mutex structure to function as a sentry incase it is still referenced.
	clearMutex();
}

Mutex::Lock::Lock(const Mutex& sec, bool try_lock)
	: _mutexRef(const_cast<Mutex&>(sec))
{
#if !IS_WIN
	// Check if the mutex has not been destroyed already.
	// This could happen when a class is created statically.
	if (_mutexRef._mutex.__data.__kind == -1)
	{
		_locked = false;
		return;
	}
#endif
	acquire(try_lock);
}

Mutex::Lock::~Lock()
{
	release();
}

bool Mutex::Lock::acquire(bool try_lock, const TimeSpec& timeout)
{
	// Only when not locked.
	if (!_locked && !_mutexRef.isMutexDestroyed())
	{
		_locked = _mutexRef.acquire(try_lock, timeout);
	}
	return _locked;
}

bool Mutex::Lock::release()
{
	if (_locked && !_mutexRef.isMutexDestroyed())
	{
		_locked = false;
		_mutexRef.release();
		return true;
	}
	return false;
}

bool Mutex::isMutexDestroyed()
{
	for (size_t i = 0; i < sizeof(_mutex); i++)
	{
		if (((char*) &_mutex)[i] != 0)
		{
			return false;
		}
	}
	return true;
}

void Mutex::clearMutex()
{
	memset(&_mutex, 0, sizeof(_mutex));
}

bool Mutex::acquire(bool try_lock, const TimeSpec& timeout)
{
	int error;
	// When try locking is requested.
	if (try_lock)
	{
		if (timeout)
		{
			// Get the realtime clock and add the timeout to get the absolute time needed for the call.
			auto tm = TimeSpec(getTime(true)).add(timeout);
			error = ::pthread_mutex_timedlock(const_cast<pthread_mutex_t*>(&_mutex), &tm);
			// Check for an ETIMEDOUT error which means locking failed.
			if (error == ETIMEDOUT)
			{
				// Signal not locked.
				return false;
			}
		}
		else
		{
			// Make the OS call to try locking.
			error = ::pthread_mutex_trylock(const_cast<pthread_mutex_t*>(&_mutex));
			// Check for an EBUSY error which means locking failed.
			if (error == EBUSY)
			{
				// Signal not locked.
				return false;
			}
		}
		// In case of an error throw an exception.
		if (error)
		{
			throw ExceptionSystemCall("pthread_mutex_trylock", error, typeid(*this).name(), __FUNCTION__);
			// Lock was successful on no error.
		}
	}
	else
	{
		error = ::pthread_mutex_lock(const_cast<pthread_mutex_t*>(&_mutex));
		if (error)
		{
			throw ExceptionSystemCall("pthread_mutex_lock", error, typeid(*this).name(), __FUNCTION__);
		}
	}
	// Return when acquire succeeded.
	return true;
}

void Mutex::release()
{
#if !IS_WIN
	// In case the mutex was used in a condition wait situation the mutex is not owned.
	// When terminated using an exception to kill a thread using constructor cleanup after termination
	// exception a release of the locked mutex is not needed since it was not locked when waiting for a condition.
	if (_mutex.__data.__owner == 0)
	{
		return;
	}
#endif
	int error = ::pthread_mutex_unlock(&_mutex);
	if (error)
	{
		throw ExceptionSystemCall("pthread_mutex_unlock", error, typeid(*this).name(), __FUNCTION__);
	}
}

}// namespace sf