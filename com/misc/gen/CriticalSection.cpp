#include <cstring>
#include "CriticalSection.h"
#include "gen/Exception.h"

namespace sf
{

CriticalSection::CriticalSection()
{
	::pthread_mutexattr_t attr{};
	int error = ::pthread_mutexattr_init(&attr);
	if (error)
		throw ExceptionSystemCall("pthread_mutexattr_init", error, typeid(*this).name(), __FUNCTION__);
	error = ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	if(error)
		throw ExceptionSystemCall("pthread_mutexattr_settype", error, typeid(*this).name(), __FUNCTION__);
	error = ::pthread_mutex_init(const_cast<pthread_mutex_t*>(&_mutex), &attr);
	if (error)
		throw ExceptionSystemCall("pthread_mutex_init", error, typeid(*this).name(), __FUNCTION__);
	error = ::pthread_mutexattr_destroy(&attr);
	if (error)
			throw ExceptionSystemCall("pthread_mutexattr_destroy", error, typeid(*this).name(), __FUNCTION__);
}

CriticalSection::~CriticalSection()
{
	int error = ::pthread_mutex_destroy(const_cast<pthread_mutex_t*>(&_mutex));
	if (error)
	{
		// TODO: weird error.
		//throw ExceptionSystemCall("pthread_mutex_destroy", error, typeid(*this).name(), __FUNCTION__);
	}
}

CriticalSection::lock::lock(const CriticalSection& sec, bool try_lock)
	:_critSecObj(const_cast<CriticalSection&>(sec))
{
	#if !IS_WIN
	// Check if the critical section has not been destroyed already.
	// This could happen when a class is created statically.
	if (_critSecObj._mutex.__data.__kind == -1)
	{
		_locked = false;
		return;
	}
	#endif
	acquire(try_lock);
}

CriticalSection::lock::~lock()
{
	release();
}

bool CriticalSection::lock::acquire(bool try_lock)
{
	// Only when not locked.
	if (!_locked && _critSecObj.isMutexDestroyed())
	{
		_locked = _critSecObj.acquire(try_lock);
	}
	return _locked;
}

bool CriticalSection::lock::release()
{
	if (_locked && _critSecObj.isMutexDestroyed())
	{
		_locked = false;
		_critSecObj.release();
		return true;
	}
	return false;
}

bool CriticalSection::isMutexDestroyed()
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

void CriticalSection::clearMutex()
{
	memset(&_mutex, 0, sizeof(_mutex));
}

bool CriticalSection::acquire(bool try_lock)
{
	bool locked = true;
	// When try locking is requested.
	if (try_lock)
	{
		// Initialize variable.
		locked = true;
		// Make the OS call to try locking.
		int error = ::pthread_mutex_trylock(const_cast<pthread_mutex_t*>(&_mutex));
		// Check for an BUSY error which means the lock failed.
		if (error == EBUSY)
		{
			locked = false;
			// In case of an error throw an exception.
		}
		else if (error)
		{
			throw ExceptionSystemCall("pthread_mutex_trylock", error, typeid(*this).name(), __FUNCTION__);
			// Lock was success full on no error.
		}
		else
		{
			locked = true;
		}
	}
	else
	{
		int error = ::pthread_mutex_lock(const_cast<pthread_mutex_t*>(&_mutex));
		if (error)
		{
			throw ExceptionSystemCall("pthread_mutex_lock", error, typeid(*this).name(), __FUNCTION__);
		}
	}
	// Return when acquire succeeded.
	return locked;
}

void CriticalSection::release()
{
	int error = ::pthread_mutex_unlock(&_mutex);
	if (error)
	{
		throw ExceptionSystemCall("pthread_mutex_unlock", error, typeid(*this).name(), __FUNCTION__);
	}
}

}