#pragma once

#include <unistd.h>
#include <string>
#include <utility>
#include <pthread.h>
#include "../gen/target.h"
#include "../gen/Condition.h"
#if IS_WIN
#include <windows.h>
#else
#include <sys/eventfd.h>
#endif
#include "TimeSpec.h"
#include "Exception.h"
#include "Mutex.h"
#include "TClosure.h"
#include "../global.h"

namespace sf
{

/**
* @brief Thread wrapper class to be used for attachment to an existing thread or a new to be created thread.
 *
 * This class can only be used to derive from since the constructor #Thread() is protected by design.
 * at least the #run() function must be overridden.
 */
class _MISC_CLASS Thread
{
	public:
		/**
		 * @brief Local declaration of the handle type.
		 */
		typedef pthread_t handle_type;

		/**
		 * @brief Local declaration of the thread id type.
		 */
#if IS_WIN
		typedef DWORD id_type;
#else
		typedef pid_t id_type;
#endif

		/**
		* @brief Thread states enumeration.
		* @see #getStatus(), #getStatusText()
		*/
		enum EStatus
		{
			tsCreated,
			tsRunning,
			tsFinished,
			tsInvalid
		};

		/**
		* @brief Thread attributes used internally for starting a thread.
		*/
		class Attributes
		{
			public:

				/**
				 * Handle type of a thread.
				 */
				typedef ::pthread_attr_t* handle_type;

				/**
				* @brief Default Constructor.
				*/
				Attributes();

				/**
				 * @brief Initialize using the passed thread handle.
				 *
				 * When the handle is '0' the current thread is used.
				 * @see #setup()
				 * @throw Exception
				 */
				explicit Attributes(Thread::handle_type th);

				/**
				* @brief Destructor.
				*/
				~Attributes();

				/**
				 * @brief When the handle is '0' the current thread is used.
				 *
				 * @param th Thread handle declared in
				 * @throw TException
				 */
				void setup(Thread::handle_type th = 0);

				/**
				 * @brief Gets the stack size of the thread which initialized it.
				 */
				[[nodiscard]] size_t getStackSize() const;

				/**
				 * @brief Sets the stack size for a new thread.
				 *
				 * The stack size retrieved using #getCurrentStackSize() is an aligned value and can
				 * reports less then is passed in this function.
				 */
				void setStackSize(size_t sz);

				/**
				 * @brief Thread schedule policies.
				 */
				enum ESchedulePolicy :int
				{
					/** Scheduling behavior is determined by the operating system.*/
					spScheduleOther = SCHED_OTHER,
					/** Threads are scheduled in a first-in-first-out order within each priority.*/
					spScheduleFifo = SCHED_FIFO,
					/** Threads are scheduled in a round-robin fashion within each priority.*/
					spScheduleRoundRobin = SCHED_RR,
#if IS_WIN
					spScheduleBatch = spScheduleOther,
					spScheduleIdle = spScheduleOther,
					spScheduleResetOnFork = spScheduleOther
#else
					spScheduleBatch = SCHED_BATCH,
					spScheduleIdle = SCHED_IDLE,
					spScheduleResetOnFork = SCHED_RESET_ON_FORK
#endif
				};

				/**
				 * @brief Sets scheduling priority for the to be started thread.
				 */
				void setSchedulePolicy(ESchedulePolicy policy);

				/**
				 * @brief Gets the current set scheduling priority.
				 */
				[[nodiscard]] ESchedulePolicy getSchedulePolicy() const;

				/**
				 * @brief Handle casting operator.
				 */
				operator handle_type() const // NOLINT(google-explicit-constructor)
				{
					return _attributes;
				}

			private:
				/**
				 * @brief Holds the actual attributes.
				 */
				handle_type _attributes;
		};

		/**
		 * @brief Starts a thread with default attributes which calls on its turn the overridden run().
		 * @see #run()
		 */
		handle_type start();

		/**
		 * @brief Starts a thread which calls on its turn the overridden function Run().
		 *
		 * @param attr Attributes for stack size and scheduling and prioritisation.
		 * @return Handle of the create thread.
		 */
		handle_type start(const Attributes& attr);

		/**
		 * @brief Can be overloaded to signal the thread to terminate.
		 *
		 * Makes #shouldTerminate() return false.
		 * @see #shouldTerminate()
		 */
		virtual void terminate();

		/**
		 * @brief Same as calling Terminate() and thereafter calling WaitForExit()
		 *
		 * @see #terminate(), #waitForExit()
		 */
		void terminateAndWait();

		/**
		* @brief Waits for the thread to exit.
		*/
		void waitForExit();

		/**
		 * @brief Returns the status.
		 *
		 * @see #EStatus
		 */
		[[nodiscard]] EStatus getStatus() const;

		/**
		 * @brief Returns the status a string.
		 *
		 * By default the current state is returned.
		 * @see #getStatus()
		 */
		[[nodiscard]] const char* getStatusText(EStatus status = (EStatus) -1) const;

		/**
		 * @brief Returns the exist value of the thread function.
		 */
		[[nodiscard]] int getExitCode() const;

		/**
		 * @brief Returns true if the calling thread is this thread.
		 */
		[[nodiscard]] bool isSelf() const;

		/**
		 * @brief Returns the thread ID. Is currently in Linux the same as the handle.
		 */
		[[nodiscard]] id_type getId() const;

		/**
		 * @brief Returns the handle of this instance.
		 */
		[[nodiscard]] handle_type getHandle() const;

		/**
		 * @brief Returns the thread termination signal.
		 */
		static int getTerminationSignal();

		/**
		 * @brief Casting operator for THandle type.
		 */
		operator handle_type() const // NOLINT(google-explicit-constructor)
		{
			return _handle;
		}

		/**
		* @brief Enumeration of thread priorities.
		*/
		enum EPriority :int
		{
			tpIdle = -15,
			tpLowest = -2,
			tpBelowNormal = -1,
			tpNormal = 0,
			tpAboveNormal = 1,
			tpHighiest = 2,
			tpTimeCritical = 15
		};

		/**
		* @brief Returns the current priority of this instance.
		*/
		[[nodiscard]] int getPriority() const;

		/**
		* @brief Can pass a enumerate EPriority for simplicity.
		 *
		* @see #EPriority
		*/
		bool setPriority(int pri, int sp = Attributes::spScheduleOther);

		/**
		 * @brief Sets the time needed for the thread to terminate.
		 *
		 * Default is one 100 ms.
		 */
		void setTerminationTime(const TimeSpec& ts);

		/**
		 * @brief Call by the thread itself to determine if it should terminate.
		 *
		 * Another thread then this one it too but has no effect.
		 */
		[[nodiscard]] bool shouldTerminate() const;

		/**
		 * @brief Alternative to returning from then run() method.
		 *
		 * Called from within the thread that wants to exit early.
		 * It save guarded from other threads calling it.
		 * @see #run()
		 */
		void exit(int code);

		/**
		 * Enables debug logging.
		 * @param yn True for debug output.
		 */
		inline void setDebug(bool yn);

		/**
		 * @brief Yield control of the current thread.
		 *
		 * The name 'Yield()' is defined as a macro in MingW.
		 * @return True when successful.
		 */
		static bool yieldToOther();

		/**
		 * @brief Special thread exception thrown by signal handler.
		 *
		 * @see #Exception
		 */
		struct TerminateException :public Exception
		{
			TerminateException();
		};

		/**
		 * @brief Makes the current thread sleep for the given amount time until a signal interrupts when alertable is true.
		 *
		 * @return True when completed and false when interrupted.
		 */
		bool sleep(const TimeSpec& ts, bool alertable = true) const; // NOLINT(modernize-use-nodiscard)

		/**
		 * @brief Return the current thread handle.
		 *
		 * @return Thread handle.
		 */
		static handle_type getCurrentHandle();

		/**
		 * @brief Gets the main thread ID.
		 * @return Main thread Id.
		 */
		static id_type getMainId();

		/**
		 * @brief Gets the current thread ID.
		 * @return Current thread Id.
		 */
		static id_type getCurrentId();

		/**
		 * @brief Returns the current thread initial stack size.
		 * @return The stack size.
		 * @throw ExceptionSystemCall
		 */
		static size_t getCurrentStackSize();

		/**
		 * Special thread exception.
		 * @see #::sf::Exception
		 */
		class ThreadError :public Exception
		{
			public:
				/**
				* @brief Enumeration of error types.
				*/
				enum EErrorType
				{
					teSuspendBeforeRun,
					teResumeBeforeRun,
					teResumeDuringRun,
					teSuspendAfterExit,
					teResumeAfterExit,
					teCreationFailure,
					teDestroyBeforeExit,
				};

				/**
				* @brief Constructor
				*/
				ThreadError(EErrorType type, const Thread* thread);

				/**
				* @brief Returns the error enumeration value.
				*/
				[[nodiscard]] inline EErrorType getErrorType() const;

			private:
				/**
				 * @brief Holds the error type.
				 */
				EErrorType _type;
		};

		/**
		 * @brief Copying constructor disabled and not implemented.
		 */
		Thread(const Thread&) = delete;

		/**
		 * @brief Copy constructor not implemented.
		 */
		const Thread& operator=(const Thread&) = delete;

		/**
		 * @brief Gets the debug flag.
		 */
		[[nodiscard]] inline bool isDebug() const;

	protected:
		/**
		 * @brief Protected constructor which demands to derive a class.
		 */
		Thread();

		/**
		 * @brief Virtual Destructor.
		 */
		virtual ~Thread();

		/**
		 * @brief Function which can be overloaded in a derived class.
		 *
		 * Is called after a thread cancel.
		 */
		virtual void cleanup();

	protected:
		/**
		 * @brief Function which needs to be overloaded in a derived class.
		 * This function is the actual thread function.
		 */
		virtual int run() = 0;

	private:
		/**
		 * @brief Called to create the thread
		 */
		int create();

		/**
		* @brief Critical section for the data members.
		*/
		Mutex _mutex{};
		/**
		* @brief Holds the Thread ID of the thread but its value is in Linux the same as the Handle.
		*/
		pid_t _threadId{0};
		/**
		* @brief Holds the thread handle.
		*/
		handle_type _handle{0};
		/**
		* @brief Holds the thread status.
		*/
		EStatus _status{EStatus::tsInvalid};
		/**
		* @brief Holds the flag to signal termination.
		*/
		bool _terminationRequested{false};
		/**
		 * @brief Time needed to terminate.
		 */
		TimeSpec _terminationTime{};
		/**
		 * Condition used in startup.
		 * When start() has left the thread can be terminated without errors.
		 */
		Condition _condition;
		/**
		 * @brief Holds the exit code of the terminated thread.
		 */
		union {void* Ptr; intptr_t Code;} _exitCode{nullptr};
		/**
		 * @brief Debug flag.
		 */
		bool _debug{false};

		friend void installSignalHandlers();
};

inline
void Thread::setTerminationTime(const TimeSpec& ts)
{
	_terminationTime = ts;
}

inline
void Thread::setDebug(bool yn)
{
	_debug = yn;
}

inline
bool Thread::isDebug() const
{
	return _debug;
}

inline
Thread::ThreadError::EErrorType Thread::ThreadError::getErrorType() const
{
	return _type;
}

/**
 * @brief Class which combines a thread class and closure template.
 *
 * Allows linking of a lambda function to be called as a thread run function.
 * Usage is like:
 * ```c++
 * ThreadClosure tc(ThreadClosure::func_type([](Thread& thread)->int
 * {
 * 	while (!thread.shouldTerminate());
 * 	{
 * 		if (Thread::sleep(TimeSpec(0.3), true))
 * 		{
 * 			std::clog << "Woken up normal" << std::endl;
 * 		}
 * 		else
 * 		{
 * 			std::clog << "Woken up by interruption" << std::endl;
 * 		}
 * 	}
 * 	return 0;
 * }));
 * tc.start();
 * sf::Thread::sleep(sf::TimeSpec(0.5));
 * tc.terminateAndWait();
 * ```
 */
class _MISC_CLASS ThreadClosure :public Thread, public TClosure<int, Thread&>
{
	public:
		/**
		 * @brief Default constructor.
		 */
		ThreadClosure()
			:Thread(), TClosure<int, Thread&>()
		{
		}

		/**
		 * @brief Thread run function assigment constructor.
		 * @param func Function ruin by the thread.
		 */
		explicit ThreadClosure(const func_type& func)
			:Thread(), TClosure<int, Thread&>(func)
		{
		}

	protected:
		/**
		 * Overrides run function and calls the closure assigned one.
		 */
		int run() override;
};

}
