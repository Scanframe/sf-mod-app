#include "Condition.h"
#include "Exception.h"
#include "dbgutils.h"
#include "gen/Thread.h"

namespace sf
{

Condition::Condition(const std::string& name)
	: _name(name)
{
	::pthread_condattr_t attr;
	//
	int error = ::pthread_condattr_init(&attr);
	if (!error)
	{
		error = ::pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
	}
	if (!error)
	{
		error = ::pthread_cond_init(&_cond, &attr);
	}
	if (!error)
	{
		error = ::pthread_condattr_destroy(&attr);
	}
	if (error)
	{
		throw ExceptionSystemCall("pthread_condattr_*", error, typeid(*this).name(), __FUNCTION__);
	}
}

Condition::~Condition()
{
	int error = ::pthread_cond_destroy(&_cond);
	if (error)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "::pthread_cond_destroy() failed!");
	}
}

bool Condition::wait(Mutex& mutex, const TimeSpec& timeout)
{
	// Add this condition for the current thread to allow unblocking by signaling when terminating.
	Thread::getCurrent().setConditionWaiting(this);
	// Increment the thread waiting counter.
	_waiting++;
	int error = 0;
	// Initialize the return value telling the caller the thread did not time out.
	bool rv = true;
	// Set the condition this thread is waiting on.
	Thread::getCurrent().setConditionWaiting(this);
	if (timeout)
	{
		// Check if the condition wait result.
		error = ::pthread_cond_timedwait(&_cond, mutex, &TimeSpec().setTimeOfDay().add(timeout));
	}
	else
	{
		// Wait for a signal and do not return sooner.
		error = ::pthread_cond_wait(&_cond, mutex);
	}
	// Reset the condition this thread is waiting on.
	Thread::getCurrent().setConditionWaiting(nullptr);
	// In case of a real error throw a system call exception.
	if (error && (error != ETIMEDOUT))
	{
		// Remove this condition from the current thread.
		Thread::getCurrent().setConditionWaiting(nullptr);
		throw ExceptionSystemCall("pthread_cond_timedwait", error, typeid(*this).name(), __FUNCTION__);
	}
	// Decrement the thread waiting counter after.
	_waiting--;
	// When the thread should terminate throw a terminate exception.
	if (Thread::getCurrent().shouldTerminate())
	{
		throw Thread::TerminateException(sf::demangle(typeid(*this).name()).append("(").append(_name).append(")::").append(__FUNCTION__));
	}
	// When just timed out
	if (error == ETIMEDOUT)
	{
		rv = false;
	}
	// Remove this condition from the current thread.
	Thread::getCurrent().setConditionWaiting(nullptr);
	// Signal the caller if the thread did time out (false).
	return rv;
}

bool Condition::notifyOne()
{
	// Check if a thread can be signaled.
	bool retval = (_waiting > 0);
	// Signal a single thread.
	int error = ::pthread_cond_signal(&_cond);
	// In case of an error throw an exception.
	if (error)
	{
		throw ExceptionSystemCall("pthread_cond_signal", error, typeid(*this).name(), __FUNCTION__);
	}
	// Signal if a thread was signalled.
	return retval;
}

int Condition::notifyAll()
{
	// Check if a thread can be signaled.
	if (_waiting > 0)
	{
		// Signal all threads.
		int error = ::pthread_cond_broadcast(&_cond);
		// In case of an error throw an exception.
		if (error)
		{
			throw ExceptionSystemCall("pthread_cond_broadcast", error, typeid(*this).name(), __FUNCTION__);
		}
		// Signal success to the caller that at least a thread was signaled.
		return true;
	}
	// Signal no thread was able to get signaled.
	return false;
}

int Condition::getWaiting() const
{
	return _waiting;
}

const std::string& Condition::getName()
{
	return _name;
}

}// namespace sf
