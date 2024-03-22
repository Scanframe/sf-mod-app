#include "ThreadRelay.h"

namespace sf
{

ThreadRelay::ThreadRelay()
	: _threadId(Thread::getCurrentId())
{
}

void ThreadRelay::makeOwner(Thread::id_type threadId)
{
	if (!threadId)
	{
		threadId = Thread::getCurrentId();
	}
	_threadId = threadId;
}

int ThreadRelay::checkForWork()
{
	// Initialize the return value.
	int rv = 0;
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
				// Prevent calling twice.
				e->_sentry = false;
				// Make the implemented derived class call the method.
				e->call();
				// Release/pulse the waiting thread.
				e->_semaphore.post();
				// Increment the return value.
				rv++;
			}
		}
	}
	// Amount of calls made.
	return rv;
}

ThreadRelay::RelayBase::operator bool()
{
	// When the thread is a different one.
	if (_tr._threadId != Thread::getCurrentId())
	{
		auto self = this;
		// Insert this class in the list Thread-safe.
		Sync::Lock lock(&_tr);
		_tr._list.insert(_tr._list.end(), self);
		lock.release();
		// Wait until the other thread handled this instance.
		Semaphore::Lock sem(_semaphore);
		lock.acquire();
		// Remove entry from the list.
		_tr._list.erase(std::remove(_tr._list.begin(), _tr._list.end(), self), _tr._list.end());
		lock.release();
		// Signal the code in the function does not need execution.
		return false;
	}
	// Signal the code needs to be executed.
	return true;
}

}// namespace sf
