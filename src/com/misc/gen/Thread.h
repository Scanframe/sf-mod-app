#pragma once

#include "../global.h"
#include "Condition.h"
#include "Exception.h"
#include "Mutex.h"
#include "TClosure.h"
#include "TimeSpec.h"
#include "target.h"
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <utility>
#if IS_WIN
	#include <windows.h>
#else
	#include <sys/eventfd.h>
#endif

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
		enum EStatus : int
		{
			/**
			 * @brief The thread is not setup yet.
			 */
			tsInvalid = 0,
			/**
			 * @brief The thread is created but not started.
			 */
			tsCreated,
			/**
			 * @brief Thread is running.
			 */
			tsRunning,
			/**
			 * @brief The thread has run and finished.
			 */
			tsFinished,
			/**
			 * @brief The thread was externally terminated.
			 */
			tsTerminated
		};

		/**
		* @brief Thread attributes used internally for starting a thread.
		*/
		class Attributes
		{
			public:
				/**
				 * Handle type of thread.
				 */
				typedef ::pthread_attr_t* handle_type;

				/**
				* @brief Default Constructor.
				*/
				Attributes();

				/**
				 * @brief Prevent copying.
				 */
				Attributes(const Attributes&) = delete;

				/**
				 * @brief Prevent assignment.
				 */
				Attributes& operator=(const Attributes&) = delete;

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
				 * report less than is passed in this function.
				 */
				void setStackSize(size_t sz);

				/**
				 * @brief Thread schedule policies.
				 */
				enum ESchedulePolicy : int
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
				inline operator handle_type() const;

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
		 * @param attr Attributes for stack size and scheduling and prioritisation.
		 * @return Handle of the created thread.
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
		 * By default, the current state is returned.
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
		inline operator handle_type() const;

		/**
		* @brief Enumeration of thread priorities.
		*/
		enum EPriority : int
		{
			tpIdle = -15,
			tpLowest = -2,
			tpBelowNormal = -1,
			tpNormal = 0,
			tpAboveNormal = 1,
			tpHighest = 2,
			tpTimeCritical = 15
		};

		/**
		 * @brief Sets the name of the thread available in the debugger.
		 * The name is clipped to the first 15 characters only.
		 * @note The thread must be created for this.
		 */
		void setName(const char* name);

		/**
		 * @brief Gets the name of the thread available in the debugger.
		 * @note The thread must be created for this.
		 */
		std::string getName() const;

		/**
		 * @brief Gets the current priority of this instance.
		 */
		[[nodiscard]] int getPriority() const;

		/**
		 * @brief Can pass an enumerate EPriority for simplicity.
		 *
		 * @see #EPriority
		 */
		bool setPriority(int pri, int sp = Attributes::spScheduleOther);

		/**
		 * @brief Sets the time needed for the thread to terminate.
		 * Default is one 100 ms.
		 */
		void setTerminationTime(const TimeSpec& ts);

		/**
		 * @brief Call by the thread itself to determine if it should terminate.
		 * Another thread then this one it too but has no effect.
		 */
		[[nodiscard]] bool shouldTerminate() const;

		/**
		 * @brief Alternative to returning from then run() method.
		 *
		 * Called from within the thread that wants to exit early.
		 * It save-guarded from other threads calling it.
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
		 * The name 'Yield()' is defined as a macro in MingW.
		 * @return True when successful.
		 */
		static bool yieldToOther();

		/**
		 * Thread exception.
		 * @see #::sf::ExceptionBase
		 */
		class ThreadException : public ExceptionBase<ThreadException>
		{
			public:
				/**
			 * @brief Constructor initializing message.
			 */
				explicit ThreadException(const char* message) noexcept
					: ExceptionBase()
				{
					formatMessage(message);
				}

				/**
			 * @brief Formatting constructor.
			 */
				template<typename... Args>
				explicit ThreadException(const char* format, Args&&... args) noexcept
					: ExceptionBase()
				{
					formatMessage(format, args...);
				}
		};

		/**
		 * @brief Special thread exception thrown by system blocking functions to terminate the thread.
		 * @see #ThreadException
		 */
		class TerminateException : public ThreadException
		{
			public:
				explicit TerminateException(const std::string& from)
					: ThreadException("Terminating from %s", from.c_str())
				{}
		};

		/**
		 * @brief Makes the current thread sleep for the given amount time until a signal interrupts when alertable is true.
		 * @return True when completed and false when interrupted.
		 */
		bool sleep(const TimeSpec& ts, bool alertable = true) const;

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
		 * @return Current thread id.
		 */
		static id_type getCurrentId();

		/**
		 * @brief Gets the sf::Thread instance reference of the current thread.
		 * @return
		 */
		static Thread& getCurrent();

		/**
		 * @brief Returns the current thread initial stack size.
		 * @return The stack size.
		 * @throw ExceptionSystemCall
		 */
		static size_t getCurrentStackSize();

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
		Thread(const std::string& name);

		/**
		 * @brief Wraps this class around the main thread of the application.
		 * The boolean bogus argument is to be different from the  default constructor.
		 */
		Thread(bool);

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

		/**
		 * @brief Called to unblock system functions.
		 */
		void TerminationSignal();

	private:
		/**
		 * @brief Called to create the thread.
		 */
		int create();

		/**
		 * @brief Sets the active blocking the condition of this thread.
		 */
		void setConditionWaiting(Condition* condition);

		/**
		* @brief Name of this thread for debugging purposes.
		*/
		std::string _name;
		/**
		* @brief Critical section for the data members.
		*/
		Mutex _mutex{"thread"};
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
		 * @brief Time needed to start thread and wait for it.
		 */
		TimeSpec _startupTime{0.3};
		/**
		 * @brief Condition used in starting up.
		 *
		 * When start() has left the thread can be terminated without errors.
		 */
		Condition* _condition{nullptr};
		/**
		 * @brief Holds the current condition waiting otherwise it is null.
		 *
		 * When terminating a thread this list allows unblocking not interruptable system calls
		 * `::pthread_cond_timedwait()` and `::pthread_cond_timedwait()`.
		 */
		Condition* _condition_waiting{nullptr};
		/**
		 * @brief Holds the flag on if this instance is of the main thread.
		 */
		bool _isMain{false};
		/**
		 * @brief Holds the exit code of the terminated thread.
		 */
		union
		{
				void* Ptr;
				intptr_t Code;
		} _exitCode{nullptr};

		/**
		 * @brief Debug flag.
		 */
		bool _debug{false};

		/**
		 * Allow access to privates to this class from functions or classes.
		 */
		friend void installSignalHandlers();
		friend class Condition;
};

inline Thread::operator handle_type() const
{
	return _handle;
}

inline Thread::Attributes::operator handle_type() const
{
	return _attributes;
}

inline void Thread::setDebug(bool yn)
{
	_debug = yn;
}

inline bool Thread::isDebug() const
{
	return _debug;
}

/**
 * @brief Wrapper class for the main thread.
 */
class _MISC_CLASS ThreadMain : public Thread
{
	public:
		/**
		 * @brief Calls on protected boolean constructor.
		 */
		ThreadMain();
		/**
		 * Overrides run function but does nothing since it is the main thread.
		 */
		int run() override;
};

inline ThreadMain::ThreadMain()
	: Thread(true)
{}

inline int ThreadMain::run()
{
	return 0;
}

}// namespace sf
