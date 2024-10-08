#include "Thread.h"
#include "dbgutils.h"
#include "gnu_compat.h"
#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <string>
#include <typeinfo>
#if IS_WIN
	#include "../win/win_utils.h"
	#include <processthreadsapi.h>
	#include <windows.h>
#else
	#include <sys/syscall.h>
#endif

namespace sf
{

thread_local Thread* thisThread{nullptr};

#if !IS_WIN
__attribute__((constructor)) void installSignalHandlers()
{
	struct sigaction sa{};
	sa.sa_handler = nullptr;
	sa.sa_flags = SA_SIGINFO;
	::sigemptyset(&sa.sa_mask);
	// Install a signal handler which throws an exception.
	sa.sa_sigaction = [](int sig, siginfo_t* info, void* ucontext) -> void {
		SF_COND_NORM_NOTIFY(thisThread->_debug, DO_DEFAULT, SF_RTTI_NAME(*thisThread) << " Signal handler initiating termination.")
		thisThread->TerminationSignal();
	};
	if (::sigaction(Thread::getTerminationSignal(), &sa, nullptr) < 0)
	{
		if (thisThread)
		{
			SF_COND_NORM_NOTIFY(thisThread->_debug, DO_DEFAULT, SF_RTTI_NAME(*thisThread) << SF_CLS_SEP << " sigaction(): " << error_string(errno).c_str())
		}
	}
	// Install a signal handler that only reports.
	sa.sa_sigaction = [](int sig, siginfo_t* info, void* ucontext) -> void {
		if (thisThread)
		{
			SF_COND_NORM_NOTIFY(thisThread->_debug, DO_DEFAULT, SF_RTTI_NAME(*thisThread) << SF_CLS_SEP << "Signal received on '" << thisThread->_name << "'");
		}
	};
	if (::sigaction(SIGUSR2, &sa, nullptr) < 0)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "sigaction(): " << error_string(errno).c_str())
	}
}
#endif

void Thread::TerminationSignal()
{
	SF_COND_RTTI_NOTIFY(thisThread->_debug, DO_DEFAULT, "Calling " << __FUNCTION__ << "()");
}

int Thread::getTerminationSignal()
{
#if IS_WIN
	return SIGTERM;
#else
	return SIGTERM;
#endif
}

Thread::Thread(bool)
	// Default termination time is 10 seconds.
	: _terminationTime(10, 0)
	, _name("Main")
	, _status(tsRunning)
	, _isMain(true)
	, _handle(pthread_self())
{
	// Do not allow 2 instances for the main thread.
	if (thisThread != nullptr)
	{
		throw ThreadException("Cannot create main instance only once!");
	}
	// Assign the current thread ID.
	_threadId = getCurrentId();
	// Assign thread local pointer.
	thisThread = this;
}

Thread::Thread(const std::string& name)
	// Default termination time is 10 seconds.
	: _terminationTime(10, 0)
	, _name(name)
{
	_condition = new Condition(stringf("Thread(%s)", _name.c_str()));
	// Initialize the exit code union.
	_exitCode.Code = -1;
}

Thread::~Thread()
{
	// Only a non-main running thread can be terminated.
	if (!_isMain && getStatus() == tsRunning)
	{
		// An exception will result in a core dump.
		// So only a notification and thread termination.
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Thread ID ( " << _threadId << ") destroyed before termination.")
		// Terminate the thread graceful using the TerminationTime period.
		terminateAndWait();
	}
	// Clear thread local pointer.
	thisThread = nullptr;
}

Thread& Thread::getCurrent()
{
	// Check if the current thread was created by this class.
	if (!thisThread)
	{
		throw ThreadException("Current thread is not created by this class!");
	}
	return *thisThread;
}

Thread::handle_type Thread::start()
{
	return start(Thread::Attributes());
}

Thread::handle_type Thread::start(const Thread::Attributes& attr)
{
	// When not the wrapped main thread allow starting it.
	if (_isMain)
	{
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Attempted to start the main thread which is impossible.")
	}
	else
	{
		Mutex::Lock lock(_mutex);
		// When starting an already running thread do nothing.
		if (_status == tsRunning)
		{
			return _handle;
		}
		// Setting termination-flag to 'false' to enable restart of thread.
		_terminationRequested = false;
		// Reset the exit code.
		_exitCode.Code = -1;
		// Allow thread creation function modify members.
		//lock.release();
		// Create the actual thread.
		auto error = ::pthread_create(&_handle, attr, +[](void* self) -> void* {
		intptr_t ec(static_cast<Thread*>(self)->create());
		// Windows does not return the exit code through pthread_join().
		// Besides, when the thread terminates by itself pthread_join() is never called.
		static_cast<Thread*>(self)->_exitCode.Code = ec;
		// Return the integer exit code as pointer.
		return (void*) ec; }, this);
		// Reacquire the lock to access data members.
		//lock.acquire();
		// On error thread creating throw an exception.
		if (error != 0)
		{
			// Failed to start thread.
			_status = tsInvalid;
			throw ExceptionSystemCall("pthread_create", error, typeid(*this).name(), __FUNCTION__);
		}
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Waiting on signal from condition '" << _name << "'")
		// Wait for a trigger from the created thread for a short time.
		if (_condition->wait(_mutex, _startupTime))
		{
			SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Signal on condition '" << _name << "'")
		}
		else
		{
			throw ThreadException("Thread did not run within expected time of %s !", _startupTime.toString().c_str());
		}
		// Set the status to running.
		_status = tsRunning;
	}
	// Return the valid thread handle.
	return _handle;
}

void Thread::terminate()
{
	// Make this function MT safe.
	Mutex::Lock lock(_mutex);
	// Set the flag for termination.
	_terminationRequested = true;
	// When the thread is waiting on a condition signal.
	if (_condition_waiting)
	{
		// Signal the waiting thread.
		_condition_waiting->notifyAll();
	}
}

void Thread::waitForExit()
{
	// Cannot wait for your own thread to exit.
	if (isSelf())
	{
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Tried to execute on itself!");
		return;
	}
	Mutex::Lock lock(_mutex);
#if IS_WIN
	if (_status == tsRunning)
	{
		int error{0};
		auto w_handle = ::pthread_gethandle(_handle);
		//WaitForSingleObject(::pthread_gethandle(_handle), INFINITE);
		//CloseHandle(::pthread_gethandle(_handle));
		SetEvent(::pthread_gethandle(_handle));
		lock.release();
		auto result = ::WaitForSingleObjectEx(w_handle, _terminationTime.toMilliSecs(), true);
		if (result == WAIT_FAILED)
		{
			throw ExceptionSystemCall("WaitForSingleObjectEx", result, typeid(*this).name(), __FUNCTION__);
		}
		else if (result == WAIT_TIMEOUT)
		{
			SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Graceful termination period has passed on " << getCurrentId())
			lock.acquire();
			// Send user signal to thread to be handled.
			if (_handle)
			{
				error = ::pthread_join(_handle, &_exitCode.Ptr);
				if (error)
				{
					SF_NORM_NOTIFY(DO_DEFAULT, "pthread_join()" << error_string(errno).c_str())
				}
			}
		}
		//
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "ExitCode (" << _exitCode.Code << ")")
	}
#else
	if (_status == tsRunning)
	{
		// Alert system calls by this signal. To unblock the blocking functions.
		::pthread_kill(_handle, SIGUSR2);
		// Release the lock.
		lock.release();
		// Give the other thread time to handle.
		yieldToOther();
		// Try joining the thread for a limited time.
		int error = ::pthread_timedjoin_np(_handle, &_exitCode.Ptr, &TimeSpec().setTimeOfDay().add(_terminationTime));
		// Acquire the lock again to safely access the data members.
		lock.acquire();
		// If the handle has been cleared the thread has stopped.
		if (error == ESRCH || !_handle)
		{
			SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Instance '" << _name << "' has exited with (" << _exitCode.Code << ")");
			return;
		}
		// Check if it timed out.
		if (error != ETIMEDOUT && error != 0)
		{
			throw ExceptionSystemCall("pthread_timedjoin_np", error, typeid(*this).name(), __FUNCTION__);
		}
		else
		{
			SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Graceful termination period of " << _terminationTime.toString() << " has passed on instance " << _name << " !")
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
			// Joining the thread again.
			if (!error)
*/
			{
				if (_handle)
				{
					error = ::pthread_join(_handle, &_exitCode.Ptr);
					if (error)
					{
						SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "pthread_join()" << error_string(errno).c_str())
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

void Thread::setName(const char* name)
{
	// Only the first 15 characters are used.
	int error = ::pthread_setname_np(_threadId, name);
	if (error > 0)
	{
		throw ExceptionSystemCall("pthread_setname_np", error, typeid(*this).name(), __FUNCTION__);
	}
}

std::string Thread::getName() const
{
	char name[32];
	int error = ::pthread_getname_np(pthread_self(), name, sizeof(name));
	if (error > 0)
	{
		throw ExceptionSystemCall("pthread_getname_np", error, typeid(*this).name(), __FUNCTION__);
	}
	return std::string(name);
}

bool Thread::setPriority(int pri, int sp)
{
	auto pri_max = ::sched_get_priority_max(sp);
	auto pri_min = ::sched_get_priority_min(sp);
	// IMPL: What is with this second param in ::sched_setscheduler()
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

void Thread::setTerminationTime(const TimeSpec& ts)
{
	Mutex::Lock lock(_mutex);
	_terminationTime = ts;
}

bool Thread::isSelf() const
{
	Mutex::Lock lock(_mutex);
	return _threadId == getCurrentId();
	/*
	// Alternative to check if this is the current thread.
	return ::pthread_equal(_handle, pthread_self()) == 0;
*/
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

void Thread::setConditionWaiting(sf::Condition* condition)
{
	Mutex::Lock lock(_mutex);
	_condition_waiting = condition;
}

void Thread::cleanup()
{
	SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Cleanup of '" << _name << "'")
}

int Thread::create()
{
	int exit_code = EXIT_SUCCESS;
	Mutex::Lock lock(_mutex);
	// Assign thread local.
	thisThread = this;
	// Assignments of accessible members need a mutex.
	//Mutex::Lock lock(thread->_mutex);
	// Assign the thread first.
	_threadId = getCurrentId();
	// When on Windows install a signal handler which when received throws an exception.
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
		auto func = [](void* ptr) -> void {
			auto& thread(*(static_cast<Thread*>(ptr)));
			// Call the non-static and maybe derived function.
			thread.cleanup();
			//			// Assignments of accessible members need a mutex.
			//			Mutex::Lock lock(thread._mutex);
			SF_COND_NORM_NOTIFY(thread._debug, DO_DEFAULT, SF_RTTI_NAME(thread) << SF_CLS_SEP << " Resetting '" << thread._name << "'")
			// Allow reclaim of storage when thread ends and is not joined. (not used now)
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
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Notify(" << _condition->getName() << ") from '" << _name << "'")
		// Notify start function calling thread its is running.
		_condition->notifyAll();
		lock.release();
		// Call the non-static and overloaded function.
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
	// Intercept termination exception.
	catch (TerminateException& ex)
	{
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Thread is terminated: " << ex.what())
		// Set the status to 'terminated' in order to not terminate again.
		_status = tsTerminated;
		// Clear the thread handle and thread ID.
		_handle = 0;
		_threadId = 0;
	}
	catch (std::exception& ex)
	{
		exit_code = EXIT_FAILURE;
		SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Intercepted (" << SF_RTTI_NAME(ex) << "): " << ex.what())
	}
	//
	SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Instance '" << _name << "' exited with code " << exit_code);
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
			"Invalid",
			"Created",
			"Running",
			"Finished",
			"Terminated"
		};
	if (status < 0)
	{
		status = getStatus();
	}
	if ((size_t) status < (sizeof(names) / sizeof(names[0])))
	{
		return names[status];
	}
	return "Unknown?";
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
		// When the thread should terminate throw a terminate exception.
		if (Thread::getCurrent().shouldTerminate())
		{
			throw Thread::TerminateException(sf::demangle(typeid(*this).name()).append("(").append(_name).append(")::").append(__FUNCTION__));
		}
		// Make the sleep call.
		auto error = ::nanosleep(&ts, &rem);
		// When no error signal sleep completion.
		if (error == 0)
		{
			// Signal sleep completion.
			return true;
		}
		// Copy the errno for debugging purposes since errno is declared as a define calling a function.
		error = errno;
		// Check if the pause has been interrupted by a non-blocked signal that was delivered to the process.
		if (error != EINTR)
		{
			throw ExceptionSystemCall("nanosleep", error, typeid(*this).name(), __FUNCTION__);
		}
		// Check if allowed to interrupt.
		if (alertable)
		{
			// Signal sleep was not completed due to interruption.
			return false;
		}
		else
		{
			// Signal that sleep did not finish so sleep the remaining time.
			ts = rem;
		}
	}
#endif
}

bool Thread::yieldToOther()
{
#if IS_WIN
	return ::SwitchToThread();
#else
	auto err = ::sched_yield();
	if (err != 0)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "pthread_yield()" << ::strerror(errno))
		return false;
	}
	return true;
#endif
}

int Thread::getExitCode() const
{
	return (int) _exitCode.Code;
}

Thread::handle_type Thread::getCurrentHandle()
{
	return ::pthread_self();
}

Thread::id_type Thread::getCurrentId()
{
#if IS_WIN
	return ::GetCurrentThreadId();
#else
	return (pid_t)::syscall(SYS_gettid);
#endif
}

Thread::id_type Thread::getMainId()
{
#if IS_WIN
	return getMainThreadId();
#else
	// The process ID is the main thread id.
	return (pid_t)::syscall(SYS_getpid);
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
	: _attributes(new pthread_attr_t)
{
	//memset(_attributes, 0, sizeof(pthread_attr_t));
	int error = ::pthread_attr_init(_attributes);
	if (error)
	{
		throw ExceptionSystemCall("pthread_attr_init", error, typeid(*this).name(), __FUNCTION__);
	}
}

Thread::Attributes::Attributes(Thread::handle_type th)
	: _attributes(new pthread_attr_t())
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

}// namespace sf
