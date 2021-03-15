#include "../gen/dbgutils.h"
#include "../gen/Exception.h"
#include "Condition.h"
#include "gen/Thread.h"

namespace sf
{

Condition::Condition()
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
		// TODO: weird error.
		//throw ExceptionSystemCall("pthread_cond_destroy", error, typeid(*this).name(), __FUNCTION__);
	}
}

bool Condition::Wait()
{
	CriticalSection::Lock lock(_mutex);
	bool rv = true;
	//
	_waiting++;
	//
	int error = ::pthread_cond_wait(&_cond, _mutex);
	// In case of an error throw an exception.
	if (error)
	{
		throw ExceptionSystemCall("pthread_cond_wait", error, typeid(*this).name(), __FUNCTION__);
	}
	/*
	// Try intercepting thread termination exception.
	try
	{
		//
		int error = ::pthread_cond_wait(&Cond, Mutex);
		// In case of an error throw an exception.
		if (error)
			throw ExceptionSystemCall("pthread_cond_wait", error, typeid(*this).name(), __FUNCTION__);
	}
	catch (TThread::TerminateException& tt)
	{
		RTTI_NOTIFY(DO_DEFAULT, tt.what());
		retval = false;
	}
	*/
	//
	_waiting--;
	// Decrement when really working.
	_work--;
	//
	return rv;
}

bool Condition::Wait(const TimeSpec& timeout)
{
	CriticalSection::Lock lock(_mutex);
	//
	bool rv = true;
	//
	_waiting++;
	// Try intercepting thread termination exception.
	try
	{
		TimeSpec ts;
		ts.setTimeOfDay().add(timeout);
		//
		int error = ::pthread_cond_timedwait(&_cond, _mutex, &ts);
		if (error == ETIMEDOUT)
		{
			rv = false;
			// In case of a real error throw an exception.
		}
		else if (error)
		{
			throw ExceptionSystemCall("pthread_cond_timedwait", error, typeid(*this).name(), __FUNCTION__);
		}
	}
	catch (Thread::TerminateException& tt)
	{
		_RTTI_NOTIFY(DO_DEFAULT, tt.what());
		rv = false;
	}
	//
	_waiting--;
	// Decrement when really working.
	_work--;
	//
	return rv;
}

int Condition::NotifyOne()
{
	CriticalSection::Lock lock(_mutex);
	// Set the work member.
	_work++;
	//
	int rv = _waiting;
	//
	int error = ::pthread_cond_signal(&_cond);
	// In case of an error throw an exception.
	if (error)
	{
		throw ExceptionSystemCall("pthread_cond_signal", error, typeid(*this).name(), __FUNCTION__);
	}
	//
	return rv;
}

int Condition::NotifyAll()
{
	CriticalSection::Lock lock(_mutex);
	// Set the work the same as the amount of threads waiting.
	int retval = _work = _waiting;
	//
	int error = ::pthread_cond_broadcast(&_cond);
	// In case of an error throw an exception.
	if (error)
	{
		throw ExceptionSystemCall("pthread_cond_broadcast", error, typeid(*this).name(), __FUNCTION__);
	}
	//
	return retval;
}

}
