#include "ThreadRelay.h"

namespace sf
{

ThreadRelay::ThreadRelay()
	:_threadId(Thread::getCurrentId())
{
}

void ThreadRelay::makeOwner()
{
	_threadId = Thread::getCurrentId();
}

void ThreadRelay::checkForWork()
{
	// Only the creator thread can call this function.
	if (_threadId == Thread::getCurrentId())
	{
		// Prevent the other thread from accessing.
		Sync::Lock lock(this);
		// Iterate through the list of waiting entries.
		for (auto e: _list)
		{
			// Sentry prevents the method from being called more than once.
			if (!e->_sentry)
			{
				e->call();
			}
			// Release/pulse the waiting thread.
			e->_semaphore.post();
		}
	}
}

ThreadRelay::RelayBase::operator bool()
{
	// When the thread is a different one.
	if (_tr._threadId != Thread::getCurrentId())
	{
		// Insert this class in the list Thread-safe.
		Sync::Lock lock(&_tr);
		_tr._list.insert(_tr._list.end(), this);
		lock.release();
		// Wait until the other thread handled this instance.
		Semaphore::Lock sem(_semaphore);
		// Prevent calling twice.
		_sentry = true;
		// Signal the code in the function does not need execution.
		return false;
	}
	// Signal the code needs to be executed.
	return true;
}

}
