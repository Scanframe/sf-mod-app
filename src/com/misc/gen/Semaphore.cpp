#include "Semaphore.h"
#include "Exception.h"
#if IS_WIN
	#include "../win/win_utils.h"
#endif

namespace sf
{

Semaphore::Semaphore(int initialCount)
	: _handle()
{
	// Windows/Wine does not allow 'pshared' equals 'true'.
	auto result = ::sem_init(&_handle, false, initialCount);
	if (result)
	{
		throw ExceptionSystemCall("sem_init", errno, typeid(*this).name(), __FUNCTION__);
	}
}

Semaphore::~Semaphore()
{
	::sem_destroy(&_handle);
}

bool Semaphore::Lock::acquire(const Semaphore& semaphore, const TimeSpec& timeout)
{
	// Check if a timeout was set.
	if (timeout)
	{
		// Get the real time.
		TimeSpec ts(getTime(true));
		// Do a timed wait.
		auto result = ::sem_timedwait(const_cast<handle_type*>(&semaphore._handle), &ts.add(timeout));
		if (result)
		{
			if (errno != ETIMEDOUT)
			{
				throw ExceptionSystemCall("sem_timedwait", errno, typeid(*this).name(), __FUNCTION__);
			}
			return false;
		}
	}
	else
	{
		return acquire(semaphore, false);
	}
	// Set pointer also used as flag.
	_semaphore = &semaphore;
	// Object was signaled.
	return true;
}

bool Semaphore::Lock::acquire(const Semaphore& semaphore, bool tryWait)
{
	if (tryWait)
	{
		auto result = ::sem_trywait(const_cast<handle_type*>(&semaphore._handle));
		if (result)
		{
			// When not able to lock (EAGAIN)
			if (errno != EINTR || errno != EAGAIN)
			{
				throw ExceptionSystemCall("sem_trywait", errno, typeid(*this).name(), __FUNCTION__);
			}
			return false;
		}
	}
	else
	{
		auto result = ::sem_wait(const_cast<handle_type*>(&semaphore._handle));
		if (result && errno != EINTR)
		{
			throw ExceptionSystemCall("sem_wait", errno, typeid(*this).name(), __FUNCTION__);
		}
	}
	// Set pointer also used as flag.
	_semaphore = &semaphore;
	// Object was signaled.
	return true;
}

bool Semaphore::post()
{
	auto result = ::sem_post(&_handle);
	// Check for an error.
	if (result)
	{
		// When invalid throw
		if (errno == EINVAL)
		{
			throw ExceptionSystemCall("sem_wait", errno, typeid(*this).name(), __FUNCTION__);
		}
		// EOVERFLOW
		return false;
	}
	return true;
}

int Semaphore::value() const
{
	int value{0};
	auto result = ::sem_getvalue(const_cast<handle_type*>(&_handle), &value);
	if (result)
	{
		throw ExceptionSystemCall("sem_wait", errno, typeid(*this).name(), __FUNCTION__);
	}
	return value;
}

void Semaphore::Lock::release(bool relinquish)
{
	// Release if acquired.
	if (_semaphore)
	{
		const_cast<Semaphore*>(_semaphore)->release();
		if (relinquish)
		{
			_semaphore = nullptr;
		}
	}
}

void Semaphore::release()
{
	// Deliberately left empty, so another implementation other than sem_xxx based.
}

}// namespace sf
