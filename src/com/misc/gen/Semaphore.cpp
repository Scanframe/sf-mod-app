#include "Semaphore.h"
#include "gnu_compat.h"
#if IS_WIN
#include "../win/win_utils.h"
#endif

namespace sf
{

void Semaphore::reportError(const char* funcName) const
{
	char buffer[BUFSIZ];
	SF_NORM_NOTIFY(DO_CLOG, SF_RTTI_NAME(*this) << SF_CLS_SEP << ::strerror_r(errno, buffer, sizeof(buffer)))
}

Semaphore::Semaphore(int initialCount)
	:_handle()
{
	auto result = sem_init(&_handle, false, initialCount);
	if (result)
	{
		reportError("sem_init");
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
		auto result = sem_timedwait(const_cast<handle_type*>(&semaphore._handle), &ts.add(timeout));
		if (result)
		{
			if (errno != ETIMEDOUT)
			{
				semaphore.reportError("sem_timedwait");
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
		auto result = sem_trywait(const_cast<handle_type*>(&semaphore._handle));
		if (result)
		{
			// When not able to lock (EAGAIN)
			if (errno != EINTR || errno != EAGAIN)
			{
				semaphore.reportError("sem_trywait");
			}
			return false;
		}
	}
	else
	{
		auto result = sem_wait(const_cast<handle_type*>(&semaphore._handle));
		if (result && errno != EINTR)
		{
			semaphore.reportError("sem_wait");
			return false;
		}
	}
	// Set pointer also used as flag.
	_semaphore = &semaphore;
	// Object was signaled.
	return true;
}

bool Semaphore::post()
{
	auto result = sem_post(&_handle);
	if (result)
	{
		reportError("sem_post");
		return false;
	}
	return true;
}

int Semaphore::value() const
{
	int value{-1};
	auto result = sem_getvalue(const_cast<handle_type*>(&_handle), &value);
	if (result)
	{
		reportError("sem_getvalue");
		return -1;
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

}
