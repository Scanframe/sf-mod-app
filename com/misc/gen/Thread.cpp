#include <cerrno>
#include <string>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <csignal>
#include <typeinfo>
#include "gen/gen_utils.h"
#include "gen/dbgutils.h"
#include "gen/PerformanceTimer.h"
#include "Thread.h"
#if IS_WIN
#  include <windows.h>
#  include <processthreadsapi.h>
#  include <csignal>
#  include "../win/win_utils.h"
#else
#include <sys/syscall.h>
#endif

namespace sf
{

thread_local volatile Thread* thisThread = nullptr;

#if !IS_WIN

__attribute__((constructor)) void installSignalHandlers()
{
	char buffer[BUFSIZ];
	struct sigaction sa{};
	sa.sa_handler = nullptr;
	sa.sa_sigaction = [](int sig, siginfo_t* info, void* ucontext) -> void
	{
		SF_COND_NORM_NOTIFY(thisThread->_debug, DO_DEFAULT, SF_RTTI_NAME(*thisThread) << " Throwing termination exception.")
		throw Thread::TerminateException();
	};
	sa.sa_flags = SA_SIGINFO;
	::sigemptyset(&sa.sa_mask);
// Install a signal handler which throws an exception.
	if (::sigaction(Thread::getTerminationSignal(), &sa, nullptr) < 0)
	{
		if (thisThread)
		{
			SF_COND_NORM_NOTIFY(thisThread->_debug, DO_DEFAULT, SF_RTTI_NAME(*thisThread) << SF_CLS_SEP
				<< " sigaction()" << ::strerror_r(errno, buffer, sizeof(buffer)))
		}
	}
	// Install a signal handler to do nothing.
	sa.sa_sigaction = [](int sig, siginfo_t* info, void* ucontext) -> void
	{
		if (thisThread)
		{
			SF_COND_NORM_NOTIFY(thisThread->_debug, DO_DEFAULT, SF_RTTI_NAME(*thisThread) << SF_CLS_SEP
				<< "Signal received... " << Thread::getCurrentId());
		}
	};
	// Install a signal handler that only reports.
	if (::sigaction(SIGUSR2, &sa, nullptr) < 0)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "sigaction()" << ::strerror_r(errno, buffer, sizeof(buffer)))
	}
}

#endif

int Thread::getTerminationSignal()
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
	 , _status(tsCreated)
	 , _terminationRequested(false)
// Default termination time is 10 seconds.
	 , _terminationTime(10, 0)
{
	// Initialize the exit code union.
	_exitCode.Code = -1;
}

Thread::~Thread()
{
	if (getStatus() == tsRunning)
	{
		//throw ThreadError(ThreadError::teDestroyBeforeExit, this);
		//
		// An exception will result in a core dump.
		// So only a notification and thread termination.
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Thread ID ( " << _threadId << ") destroyed before termination.")
		// Terminate the thread graceful using the TerminationTime period.
		terminateAndWait();
	}
}

Thread::handle_type Thread::start()
{
	return start(Thread::Attributes());
}

Thread::handle_type Thread::start(const Thread::Attributes& attr)
{
	Mutex::Lock lock(_mutex);
	// When starting running thread do nothing.
	if (_status == tsRunning)
	{
		return _handle;
	}
	// Setting termination flag to false to enable restart of thread.
	_terminationRequested = false;
	// Reset the exit code.
	_exitCode.Code = -1;
	// Allow thread creation function modify members.
	//lock.release();
	// Create the actual thread.
	auto error = ::pthread_create(&_handle, attr, [](void* self) -> void*
	{
		int64_t ec(static_cast<Thread*>(self)->create());
#if IS_WIN
		// Windows does not return the exit code through pthread_join.
		static_cast<Thread*>(self)->_exitCode.Code = ec;
#endif
		// Return the integer exit code as pointer.
		return (void*) ec;
	}, this);
	// Reacquire the lock to access data members.
//	lock.acquire();
	// On error thread creating throw an exception.
	if (error != 0)
	{
		// Failed to start thread.
		_status = tsInvalid;
		throw ThreadError(ThreadError::teCreationFailure, this);
	}
	SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Waiting for condition... " << getCurrentId())
	// Wait for a trigger from the created thread.
	if (_condition.wait(_mutex, TimeSpec(_terminationTime)))
	{
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Notified... " << getCurrentId())
	}
	else
	{
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "NOT notified, timed out! ..." << getCurrentId())
	}
	// Set the status to running.
	_status = tsRunning;
	// Return the valid thread handle.
	return _handle;
}

void Thread::terminate()
{
	_terminationRequested = true;
}

void Thread::waitForExit()
{
#if IS_WIN
	Mutex::Lock lock(_mutex);
	//
	if (_status == tsRunning)
	{
		int error{0};
		// Alert system calls by this signal. To unblock blocking functions.
		//::pthread_kill(_handle, SIGINT);
		//WaitForSingleObject(::pthread_gethandle(_handle), INFINITE);
		//CloseHandle(::pthread_gethandle(_handle));
		SetEvent(::pthread_gethandle(_handle));
		lock.release();
		auto result = ::WaitForSingleObjectEx(::pthread_gethandle(_handle), _terminationTime.toMilliSecs(), true);
		if (result == WAIT_FAILED)
		{
			throw ExceptionSystemCall("WaitForSingleObjectEx", error, typeid(*this).name(), __FUNCTION__);
		}
		else if (result == WAIT_TIMEOUT)
		{
			_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Graceful termination period has passed on " << getCurrentId())
			lock.acquire();
			// Send user signal signal to thread to be handled.
			if (_handle)
			{
				error = ::pthread_join(_handle, &_exitCode.Ptr);
				if (error)
				{
					char buffer[BUFSIZ];
					_NORM_NOTIFY(DO_DEFAULT, "pthread_join()" << ::strerror_r(errno, buffer, sizeof(buffer)))
				}
			}
		}
		//
		_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "ExitCode (" << _exitCode.Code << ")")
	}
#else
	Mutex::Lock lock(_mutex);
	//
	if (_status == tsRunning)
	{
		// Alert system calls by this signal. To unblock blocking functions.
		::pthread_kill(_handle, SIGUSR2);
		// Release the lock.
		lock.release();
		// Give the other thread time to handle.
		yieldToOther();
		// Try joining the the thread for a limited time.
		int error = ::pthread_timedjoin_np(_handle, &_exitCode.Ptr, &TimeSpec().setTimeOfDay().add(_terminationTime));
		// Acquire the lock again to safely access the data members.
		lock.acquire();
		// If the handle has been cleared the thread has stopped.
		if (error == ESRCH || !_handle)
		{
			SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Thread has exited (" << _exitCode.Code << ")");
			return;
		}
		// Check if it timed out.
		if (error != ETIMEDOUT)
		{
			throw ExceptionSystemCall("pthread_timedjoin_np", error, typeid(*this).name(), __FUNCTION__);
		}
		else
		{
			SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Graceful termination period (" << _terminationTime.tv_sec
				<< "s, " << _terminationTime.tv_nsec << "ns) has passed on " << _threadId << " !")
			if (_handle)
			{
				// 2 types of termination.

				// Best result when using a signal.
				error = ::pthread_kill(_handle, getTerminationSignal());
				if (error)
				{
					throw ExceptionSystemCall("pthread_kill", error, typeid(*this).name(), __FUNCTION__);
				}

/*
				// Cancel calls the cleanup handler.
				error = ::pthread_cancel(_handle);
				if (error)
				{
					throw ExceptionSystemCall("pthread_cancel", error, typeid(*this).name(), __FUNCTION__);
				}
*/
			}
/*
			// Joining the the thread again.
			if (!error)
*/
			{
				if (_handle)
				{
					error = ::pthread_join(_handle, &_exitCode.Ptr);
					if (error)
					{
						char buffer[BUFSIZ];
						SF_NORM_NOTIFY(DO_DEFAULT, "pthread_join()" << ::strerror_r(errno, buffer, sizeof(buffer)))
					}
				}
			}
		}
		//
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "ExitCode (" << _exitCode.Code << ")")
	}
#endif
}

void Thread::terminateAndWait()
{
	terminate();
	waitForExit();
}

bool Thread::setPriority(int pri, int sp)
{
	auto pri_max = ::sched_get_priority_max(sp);
	auto pri_min = ::sched_get_priority_min(sp);
	// IMPL: Whats with this second param in ::sched_setscheduler()
	struct sched_param param{};
	// Clip the priority to what is allowed by the OS.
	param.sched_priority = clip(pri, pri_max, pri_min);
	int error = ::sched_setscheduler(_threadId, sp, &param);
	if (error > 0)
	{
		throw ExceptionSystemCall("sched_setscheduler", error, typeid(*this).name(), __FUNCTION__);
	}
	return !!error;
}

int Thread::getPriority() const
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

Thread::EStatus Thread::getStatus() const
{
	Mutex::Lock lock(_mutex);
	return _status;
}

bool Thread::isSelf() const
{
	Mutex::Lock lock(_mutex);
	return _threadId == getCurrentId();
	//return pthread_equal(_handle, pthread_self()) == 0;
}

Thread::id_type Thread::getId() const
{
	Mutex::Lock lock(_mutex);
	return _threadId;
}

Thread::handle_type Thread::getHandle() const
{
	Mutex::Lock lock(_mutex);
	return _handle;
}

bool Thread::shouldTerminate() const
{
	// Only when this thread.
	if (_threadId == getCurrentId())
	{
		// Cancellation point.
		::pthread_testcancel();
	}
	//
	return _terminationRequested;
}

void Thread::cleanup()
{
	SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Cleanup... " << getCurrentId())
}

Thread::TerminateException::TerminateException()
	:Exception("ID(%d)", getCurrentId()) {}

int Thread::create()
{
	int exit_code = 0;
	Mutex::Lock lock(_mutex);
	// Assign thread local.
	thisThread = this;
	// Assignments of accessible members need a mutex.
	//Mutex::Lock lock(thread->_mutex);
	// Assign the thread first.
	_threadId = getCurrentId();
	// When on windows install a signal handler which when received throws an exception.
#if !IS_WIN
	// Enable the cancel state for this thread.
	::pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
	// Asynchronous Cancel. When the thread receives the Cancel signal, it exits immediately.
	::pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
#else
	/*
	signal(getTerminationSignal(), [](int sig) -> void
	{
		_NORM_NOTIFY(DO_DEFAULT, "Termination signal received... " << getCurrentId());
		throw Thread::TerminateException();
	});
*/
#endif
	try
	{
		auto func = [](void* ptr) -> void
		{
			auto& thread(*(static_cast<Thread*>(ptr)));
			// Call the non static and maybe derived function.
			thread.cleanup();
			SF_COND_NORM_NOTIFY(thread._debug, DO_DEFAULT,
				SF_RTTI_NAME(thread) << SF_CLS_SEP << "Planning to reset data members... " << getCurrentId())
			// Assignments of accessible members need a mutex.
			//Mutex::Lock lock(thread._mutex);
			SF_COND_NORM_NOTIFY(thread._debug, DO_DEFAULT, SF_RTTI_NAME(thread) << SF_CLS_SEP << "Resetting data members... " << getCurrentId())
			// Allow reclaim of storage.
			//::pthread_detach(thread._handle);
			// Set the status to Finished.
			thread._status = tsFinished;
			// Clear the thread handle and thread ID.
			thread._handle = 0;
			thread._threadId = 0;
		};
#if !IS_WIN && false
		// Install a cleanup handler.
		pthread_cleanup_push(func, this);
#endif
			SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Thread Notify condition... " << getCurrentId())
			// Notify start function calling thread its is running.
			_condition.notifyAll(_mutex);
			lock.release();
			// Call the non static and overloaded function.
			exit_code = run();
			// Locking for cleanup call.
			lock.acquire();
#if !IS_WIN && false
			// Execute the cleanup function.
		pthread_cleanup_pop(1);
#else
		func(this);
#endif
	}
	catch (std::exception& e)
	{
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Intercepted (" << SF_RTTI_NAME(e) << "): " << e.what())
	}
	SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Thread exited (" << exit_code << ")... " << getCurrentId())
	return exit_code;
}

void Thread::exit(int code)
{
	// Only when this is the thread.
	if (_threadId == getCurrentId() && _status != tsFinished)
	{
		_exitCode.Code = code;
		_status = tsFinished;
		::pthread_exit(&_handle);
	}
}

const char* Thread::getStatusText(Thread::EStatus status) const
{
	static const char* names[] =
		{
			"Created",
			"Running",
			"Suspended",
			"Finished",
			"Invalid"
		};
	if (status < 0)
	{
		status = getStatus();
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
	(void) thread;
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

bool Thread::sleep(const TimeSpec& time, bool alertable) const
{
#if IS_WIN
	auto times = (time.tv_sec * 1000 + time.tv_nsec / 1000000) / 20;
	if (times)
	{
		while (times--)
		{
			::SleepEx(20, alertable);
			if (shouldTerminate())
			{
				return false;
			}
			else
			{
				yieldToOther();
			}
		}
	}
	return true;
#else
	// For the remainder.
	TimeSpec ts(time);
	TimeSpec rem;
	//
	for (;;)
	{
		// Make the sleep call.
		int result = ::nanosleep(&ts, &rem);
		// Check for an error.
		if (!result)
		{
			// Signal sleep completion.
			return true;
		}
		// Check if the pause has been interrupted by a non-blocked signal that was delivered to the process.
		if (errno == EINTR)
		{
			// Was interrupted so check if can be alerted.
			if (alertable)
			{
				// Signal sleep was not completed due to interruption.
				return false;
			}
		}
		else
		{
			Thread* t{};
			(void) t;
			throw ExceptionSystemCall("nanosleep", errno, typeid(*t).name(), __FUNCTION__);
		}
		// Signal that sleep did not finish so sleep the remaining time.
		ts = rem;
	}
#endif
}

bool Thread::yieldToOther()
{
#if IS_WIN
	return ::SwitchToThread();
#else
	auto err = ::pthread_yield();
	if (err)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "pthread_yield()" << ::strerror(errno))
		return false;
	}
	return true;
#endif
}

int Thread::getExitCode() const
{
	return (int)_exitCode.Code;
}

Thread::handle_type Thread::getCurrentHandle()
{
	return pthread_self();
}

Thread::id_type Thread::getCurrentId()
{
#if IS_WIN
	return ::GetCurrentThreadId();
#else
	return (pid_t) ::syscall(SYS_gettid);
#endif
}

#if IS_WIN

void GetCurrentThreadUsedStackLimits(PULONG_PTR LowLimit, PULONG_PTR HighLimit)
{
	auto* tib = (NT_TIB*) NtCurrentTeb();
	*HighLimit = (ULONG_PTR) tib->StackBase;
	*LowLimit = (ULONG_PTR) tib->StackLimit;
}

#endif

size_t Thread::getCurrentStackSize()
{
	size_t sz = 0;
#if IS_WIN
	ULONG_PTR low, high;
	GetCurrentThreadUsedStackLimits(&low, &high);
	sz = (char*) high - (char*) low;
#else
	Thread* th{};
	(void) th;
	pthread_attr_t attr;
	auto error = ::pthread_getattr_np(pthread_self(), &attr);
	if (error)
	{
		throw ExceptionSystemCall("pthread_getattr_np", error, typeid(*th).name(), __FUNCTION__);
	}
	error = pthread_attr_getstacksize(&attr, &sz);
	if (error)
	{
		throw ExceptionSystemCall("pthread_getattr_np", error, typeid(*th).name(), __FUNCTION__);
	}
#endif
	return sz;
}

Thread::Attributes::Attributes()
	:_attributes(new pthread_attr_t)
{
	//memset(_attributes, 0, sizeof(pthread_attr_t));
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
	setup(th);
}

void Thread::Attributes::setup(Thread::handle_type th)
{
	if (!th)
	{
		auto sz = getCurrentStackSize();
		setStackSize(sz);
	}
/*
	// When 0 is passed the current thread is selected for initialization.
	if (!th)
	{
		th = ::pthread_self();
	}
	pthread_attr_t attr;
	size_t sz;
	int error = ::pthread_getattr_np(th, &attr);
	if (!error)
	{
		error = pthread_attr_getstacksize(&attr, &sz);
	}
	//int error = ::pthread_getattr_default_np(_attributes);
	if (error)
	{
		throw ExceptionSystemCall("pthread_getattr_np", error, typeid(*this).name(), __FUNCTION__);
	}
*/
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
		return;
	}
	delete _attributes;
}

size_t Thread::Attributes::getStackSize() const
{
	size_t sz = 0;
	int error = ::pthread_attr_getstacksize(_attributes, &sz);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_getstacksize", error, typeid(*this).name(), __FUNCTION__);
	}
	return sz;
}

void Thread::Attributes::setStackSize(size_t sz)
{
	// Assign the passed stack size.
	int error = ::pthread_attr_setstacksize(_attributes, sz);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_setstacksize", error, typeid(*this).name(), __FUNCTION__);
	}
}

void Thread::Attributes::setSchedulePolicy(Thread::Attributes::ESchedulePolicy policy)
{
	int error = ::pthread_attr_setschedpolicy(_attributes, policy);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_setschedpolicy", error, typeid(*this).name(), __FUNCTION__);
	}
}

Thread::Attributes::ESchedulePolicy Thread::Attributes::getSchedulePolicy() const
{
	int policy = 0;
	int error = ::pthread_attr_getschedpolicy(_attributes, &policy);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_getschedpolicy", error, typeid(*this).name(), __FUNCTION__);
	}
	return (ESchedulePolicy) policy;
}

int ThreadClosure::run()
{
	return isAssigned() ? call(*this) : 0;
}

}
