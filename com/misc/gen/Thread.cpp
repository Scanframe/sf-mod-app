#include <ctime>
#include <cerrno>
#include <string>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <csignal>
#include <typeinfo>
#include "gen/gen_utils.h"
#include "gen/dbgutils.h"
#include "Thread.h"
#if IS_WIN
#  include <windows.h>
#  include <processthreadsapi.h>
	// Last header above does not declare the function due to _WIN32_WINNT geing to low.
	#if _WIN32_WINNT < 0x0602
		WINBASEAPI VOID WINAPI GetCurrentThreadStackLimits (PULONG_PTR LowLimit, PULONG_PTR HighLimit);
	#endif
#endif


namespace sf
{

int Thread::GetCurrentThreadId()
{
#if IS_WIN
	return (long)::GetCurrentThreadId();
#else
	return (pid_t) ::gettid();
#endif
}

int Thread::GetTerminationSignal()
{
#if IS_WIN
	return SIGTERM;
#else
	return SIGUSR1;
#endif
}

Thread::Thread()
	:_threadId(0)
	 , _handle(0)
	 , _status(tsCREATED)
	 , _terminationRequested(false)
// Default termination time is 60 seconds.
	 , _terminationTime(60, 0)
{
	// Initialize the exit code union.
	_exitCode.Code = -1;
}

Thread::~Thread()
{
	if (GetStatus() == tsRUNNING)
	{
		//throw ThreadError(ThreadError::teDESTROYBEFOREEXIT, this);
		//
		// An exception will result in a core dump.
		// So only a notification and thread termination.
		_RTTI_NOTIFY(DO_DEFAULT, "Thread ID ( " << _threadId << ") destroyed before termination.");
		// Terminate the thread graceful using the TerminationTime period.
		TerminateAndWait();
	}
}

Thread::handle_type Thread::Start(const Thread::Attributes& attr)
{
	// If Start() has already been called for this thread, release the
	// previously created system thread object before launching a new one.
	if ((GetStatus() != tsCREATED) && _handle)
	{
		// Throw an error.
		throw ThreadError(ThreadError::teCREATIONFAILURE, this);
	}
	// Setting termination flag to false to enable restart of thread.
	_terminationRequested = false;
	// Reset the exit code.
	_exitCode.Code = -1;
	//
	if (::pthread_create(&_handle, attr, &Thread::Execute, this) == 0)
	{
		_status = tsINVALID;
		throw ThreadError(ThreadError::teCREATIONFAILURE, this);
	}
	//
	_status = tsRUNNING;
	//
	return _handle;
}

void Thread::Terminate()
{
	_terminationRequested = true;
}

void Thread::WaitForExit()
{
	if (_status == tsRUNNING)
	{
#if IS_WIN
		// Joining the the thread for termination.
		int error= ::pthread_join(_handle, &_exitCode.Ptr);
#else
		// Default constructor sets the time of day.
		TimeSpec ts;
		ts.setTimeOfDay().add(_terminationTime);
		// Try joining the the thread.
		int error = ::pthread_timedjoin_np(_handle, &_exitCode.Ptr, &ts);
#endif
		// Check if it timed out.
		if (error == ETIMEDOUT)
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Graceful termination period (" << _terminationTime.tv_sec
				<< "s, " << _terminationTime.tv_nsec << "ns) has passed on ID(" << _threadId << ")!");
			// Reset the error value.
			error = 0;
			// 2 types of termination.
			// Best result when using a signal.
			if (1)
			{
				// Get the handle through a critical section lock.
				handle_type handle = GetHandle();
				// Send user signal signal to thread to be handled.
				if (handle)
				{
					error = ::pthread_kill(_handle, GetTerminationSignal());
				}
			}
				// Cancel calls the cleanup handler.
			else
			{
				// Get the handle through a critical section lock.
				handle_type handle = GetHandle();
				if (handle)
				{
					error = ::pthread_cancel(_handle);
				}
			}
			// Joining the the thread again.
			if (!error)
			{
				// Check if the handle is valid.
				handle_type handle = GetHandle();
				if (handle)
				{
					error = ::pthread_join(_handle, &_exitCode.Ptr);
				}
			}
		}
		//
		if (error)
		{
			throw ExceptionSystemCall("pthread_kill", error, typeid(*this).name(), __FUNCTION__);
		}
		//
		_COND_RTTI_NOTIFY(_exitCode.Code, DO_DEFAULT, "ExitCode (" << _exitCode.Code << ")");
	}
}

void Thread::TerminateAndWait()
{
	Terminate();
	WaitForExit();
}

bool Thread::SetPriority(int pri, int sp)
{
	// IMPL: Whats with this second param in ::sched_setscheduler()
	struct sched_param param{};
	param.sched_priority = pri;
	int error = ::sched_setscheduler(_threadId, sp, &param);
	if (error > 0)
	{
		throw ExceptionSystemCall("sched_setscheduler", error, typeid(*this).name(), __FUNCTION__);
	}
	return !!error;
}

int Thread::GetPriority() const
{
	int policy = 0;
	struct sched_param param{};
	int error = pthread_getschedparam(_threadId, &policy, &param);
	//int error = ::sched_getparam(_threadId, &param);
	if (error)
	{
		throw ExceptionSystemCall("sched_getparam", error, typeid(*this).name(), __FUNCTION__);
	}
	return param.sched_priority;
}

Thread::EStatus Thread::GetStatus() const
{
	CriticalSection::Lock lock(_critSec);
	return _status;
}

bool Thread::IsSelf() const
{
	CriticalSection::Lock lock(_critSec);
	//return _threadId == GetCurrentThreadId();
	return pthread_equal(_handle, pthread_self()) == 0;
}

pid_t Thread::GetThreadId() const
{
	CriticalSection::Lock lock(_critSec);
	return _threadId;
}

Thread::handle_type Thread::GetHandle() const
{
	CriticalSection::Lock lock(_critSec);
	return _handle;
}

bool Thread::ShouldTerminate() const
{
	// Cancellation point.
	::pthread_testcancel();
	//
	return _terminationRequested;
}

int Thread::Run()
{
	return -1;
}

void Thread::Cleanup()
{
	_RTTI_NOTIFY(DO_DEFAULT, "Called");
}

void Thread::DoCleanup(void* thread)
{
	auto* t(static_cast<Thread*>(thread));
	try
	{
		// Call the non static and maybe derived function.
		t->Cleanup();
	}
	catch (std::exception& e)
	{
		_NORM_NOTIFY(DO_DEFAULT, typeid(*t).name() << ": C++  '" << e.what()
			<< "' on Thread cleanup Intercepted!");
	}
	// Assignments of accessible members need a critical section.
	CriticalSection::Lock lock(t->_critSec);
	// Allow reclaim of storage.
	//::pthread_detach(t->Handle);
	// Clear the thread handle and thread ID.
	t->_handle = 0;
	t->_threadId = 0;
	// Set the status to Finished.
	t->_status = tsFINISHED;
}

Thread::TerminateException::TerminateException()
	:Exception("ID(%d)", GetCurrentThreadId()) {}

void* Thread::Execute(void* thread)
{
#if !IS_WIN
	struct sigaction sa{};
	sa.sa_handler = nullptr;
	sa.sa_sigaction = [](int sig, siginfo_t* info, void* ucontext) -> void
	{
		throw Thread::TerminateException();
	};
	sa.sa_flags = SA_SIGINFO;
	::sigemptyset(&sa.sa_mask);
	// Install a signal handler which throws an exception.
	if (::sigaction(GetTerminationSignal(), &sa, nullptr) < 0)
	{
		_NORM_NOTIFY(DO_DEFAULT, "sigaction()" << strerror(errno));
		return (void*) -1;
	}
#endif
	//
	auto* thrd(static_cast<Thread*>(thread));
	// PTHREAD_CANCEL_ENABLE and PTHREAD_CANCEL_DISABLE
	//::pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	// PTHREAD_CANCEL_DEFERRED or PTHREAD_CANCEL_ASYNCHRONOUS.
	//::pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	{
		// Assignments of accessible members need a critical section.
		CriticalSection::Lock lock(thrd->_critSec);
		// Assign the thread first.
		thrd->_threadId = GetCurrentThreadId();
	}
	// Install a cleanup handler.
	//pthread_cleanup_push(&DoCleanup, thrd);
	try
	{
		// Call the non static and overloaded function.
		thrd->_exitCode.Code = thrd->Run();
	}
	catch (std::exception& e)
	{
		_NORM_NOTIFY(DO_DEFAULT, demangle(typeid(*thrd).name()) << "::" << __FUNCTION__
			<< "() Intercepted (" << demangle(typeid(e).name()) << "): " << e.what());
	}
	// Execute the cleanup function.
	//pthread_cleanup_pop(1);
	//
	// Code below is not executed when a cleanup handler is installed.
	//
	if (1)
	{
		// Assignments of accessible members need a critical section.
		CriticalSection::Lock lock(thrd->_critSec);
		// Clear the thread handle and thread ID.
		thrd->_handle = 0;
		thrd->_threadId = 0;
		// Set the status to Finished.
		thrd->_status = tsFINISHED;
		// Allow reclaim of storage.
		// This seems not the way to go since pthread_join is not possible.
		//::pthread_detach(thrd->Handle);
	}
	// Return the value.
	return thrd->_exitCode.Ptr;
}

void Thread::Exit(int code)
{
	_exitCode.Code = code;
	_status = tsFINISHED;
	::pthread_exit(&_handle);
}

void Thread::Cancel()
{
	// Accessing data members need a critical section lock.
	CriticalSection::Lock lock(_critSec);
	int error = ::pthread_cancel(_handle);
	lock.Release();
	if (error)
	{
		throw ExceptionSystemCall("pthread_cancel", error, typeid(*this).name(), __FUNCTION__);
	}
}

const char* Thread::GetStatusText(Thread::EStatus status) const
{
	static const char* names[] =
		{
			"CREATED",
			"RUNNING",
			"SUSPENDED",
			"FINISHED",
			"INVALID"
		};
	if (status < 0)
	{
		status = GetStatus();
	}
	if (status >= 0 && (size_t) status < (sizeof(names) / sizeof(names[0])))
	{
		return names[status];
	}
	return "Unknown?";

}

Thread::ThreadError::ThreadError(ThreadError::EErrorType type, const Thread* thread)
	:Exception()
	 , _type(type)
{
	static const char* names[] =
		{
			"Suspend() before Run()",
			"Resume() before Run()",
			"Resume() during Run()",
			"Suspend() after Exit()",
			"Resume() after Exit()",
			"Creation failure",
			"Destroyed before Exit()"
		};
	FormatMsg("%s: %s", demangle(typeid(*thread).name()).c_str(), names[_type]);
}

bool Thread::Sleep(const TimeSpec& time, bool alertable)
{
	// For the remainder.
	TimeSpec ts(time);
	TimeSpec rem;
	//
	do
	{
		// Make the sleep call.
		int result = ::nanosleep(&ts, &rem);
		// When alertable return false to indicate this.name
		if (!result)
		{
			return true;
		}
		// Check if the pause has been interrupted by a non-blocked
		// signal that was delivered to the process.
		if (errno != EINTR)
		{
			return false;
		}
		// Was interrupted so check if can be alerted.
		if (alertable)
		{
			return false;
		}
		// Signal that sleep did not finish so sleep the remaining time.
		ts = rem;
	}
		// Sleep the full length when not alertable.
	while (!alertable);
	// Normally this part will not be reached.
	return true;
}

bool Thread::YieldToOther()
{
#if IS_WIN
	return ::SwitchToThread();
#else
	return !::pthread_yield();
#endif
}

int Thread::GetExitCode() const
{
	return _exitCode.Code;
}

Thread::handle_type Thread::GetCurrentThreadHandle()
{
	return pthread_self();
}

Thread::Attributes::Attributes()
	:_attributes(new pthread_attr_t())
{
	int error = ::pthread_attr_init(_attributes);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_init", error, typeid(*this).name(), __FUNCTION__);
	}
}

Thread::Attributes::Attributes(Thread::handle_type th)
	:_attributes(new pthread_attr_t())
{
	int error = ::pthread_attr_init(_attributes);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_init", error, typeid(*this).name(), __FUNCTION__);
	}
	// Initialize the with the passed handle.
	Setup(th);
}

void Thread::Attributes::Setup(Thread::handle_type th)
{
#if IS_WIN
	ULONG_PTR low, high;
	//::GetCurrentThreadStackLimits(&low, &high);
	size_t sz((char*)high - (char*)low);
	SetStackSize(sz);
#else
	// When 0 is passed the current thread is selected for initialization.
	if (!th)
	{
		th = ::pthread_self();
	}
	int error = ::pthread_getattr_np(th, _attributes);
	if (error)
	{
		throw ExceptionSystemCall("pthread_getattr_np", error, typeid(*this).name(), __FUNCTION__);
	}
#endif
}

Thread::Attributes::~Attributes()
{
	int error = ::pthread_attr_destroy(_attributes);
	if (error)
	{
		// Prevent a memory leak.
		delete _attributes;
		// TODO: weird error.
		//throw ExceptionSystemCall("pthread_attr_destroy", error, typeid(*this).name(), __FUNCTION__);
	}
	delete _attributes;
}

size_t Thread::Attributes::GetStackSize() const
{
	size_t sz = 0;
	int error = ::pthread_attr_getstacksize(_attributes, &sz);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_getstacksize", error, typeid(*this).name(), __FUNCTION__);
	}
	return sz;
}

void Thread::Attributes::SetStackSize(size_t sz)
{
	// Assign the passed stack size.
	int error = ::pthread_attr_setstacksize(_attributes, sz);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_setstacksize", error, typeid(*this).name(), __FUNCTION__);
	}
}

void Thread::Attributes::SetSchedulePolicy(Thread::Attributes::ESchedulePolicy policy)
{
	// Assign the passed stack size.
	int error = ::pthread_attr_setschedpolicy(_attributes, policy);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_setschedpolicy", error, typeid(*this).name(), __FUNCTION__);
	}
}

Thread::Attributes::ESchedulePolicy Thread::Attributes::GetSchedulePolicy() const
{
	int policy = 0;
	int error = ::pthread_attr_getschedpolicy(_attributes, &policy);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_getschedpolicy", error, typeid(*this).name(), __FUNCTION__);
	}
	return (ESchedulePolicy) policy;
}

}
