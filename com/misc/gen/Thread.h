#pragma once

#include <unistd.h>
#include <string>
#include <pthread.h>
#if !IS_WIN
#include <sys/eventfd.h>
#endif
#include "TimeSpec.h"
#include "Exception.h"
#include "CriticalSection.h"
#include "global.h"

namespace sf
{

/**
* @brief Thread wrapper class to be used for attachment to an existing thread or a
* new to be created thread. For the latter this class must be derived and the
* at least the Run() function must be overloaded.
*/
class _MISC_CLASS Thread
{
	public:
		/**
		* @brief Local declaration of the handle type.
		*/
		typedef pthread_t handle_type;

		/**
		* @brief Thread states enumeration.
		* @see #getStatus(), #getStatusText()
		*/
		enum EStatus
		{
			tsCREATED,
			tsRUNNING,
			tsFINISHED,
			tsINVALID
		};

		/**
		* @brief Thread attributes used internally for starting a thread.
		*/
		class Attributes
		{
			public:
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
				 * @param th Thread handle declared in
				 * @throw TException
				 */
				void setup(Thread::handle_type th);

				/**
				 * Returns the stack size of the thread which initialized it.
				 */
				[[nodiscard]] size_t getStackSize() const;

				/**
				 * Sets the stack size for a new thread.
				 */
				void setStackSize(size_t sz);

				/**
				 * @brief Thread schedule policies.
				 *
				 * SCHED_FIFO Threads are scheduled in a first-in-first-out order within each priority.
				 * SCHED_OTHER Scheduling behavior is determined by the operating system.
				 * SCHED_RR Threads are scheduled in a round-robin fashion within each priority.
				 */
				enum ESchedulePolicy
				{
					spSCHED_OTHER = SCHED_OTHER,
					spSCHED_FIFO = SCHED_FIFO,
					spSCHED_ROUND_ROBIN = SCHED_RR,
#if IS_WIN
					spSCHED_BATCH = spSCHED_OTHER,
					spSCHED_IDLE = spSCHED_OTHER,
					spSCHED_RESET_ON_FORK = spSCHED_OTHER
#else
					spSCHED_BATCH = SCHED_BATCH,
					spSCHED_IDLE = SCHED_IDLE,
					spSCHED_RESET_ON_FORK = SCHED_RESET_ON_FORK
#endif
				};

				/**
				 * @brief Sets scheduling priority for the to be started thread.
				 */
				void setSchedulePolicy(ESchedulePolicy policy);

				/**
				 * @brief Returns the current set scheduling priority.
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
		 * @brief Starts a thread which calls on its turn the overloaded function Run().
		 */
		handle_type start(const Attributes& attr);

		/**
		 * @brief Cancels the running thread and Cleanup() is called when the thread is running.
		 */
		void cancel();

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
		[[nodiscard]] pid_t getThreadId() const;

		/**
		 * @brief Returns the handle of this instance.
		 */
		[[nodiscard]] handle_type getHandle() const;

		/**
		 * @brief Returns the thread termination signal.
		 */
		static int getTerminationSignal();

		/**
		 * @brief Returns the thread ID as long.
		 */
		static int getCurrentThreadId();

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
		enum EPriority
		{
			tpIDLE = -15,
			tpLOWEST = -2,
			tpBELOWNORMAL = -1,
			tpNORMAL = 0,
			tpABOVENORMAL = 1,
			tpHIGHIEST = 2,
			tpTIMECRITICAL = 15
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
		bool setPriority(int, int = 0);

		/**
		 * @brief Sets the time needed for the thread to terminate.
		 *
		 * Default is one 100 ms.
		 */
		void setTerminationTime(const TimeSpec& ts);

		/**
		 * @brief Special thread exception thrown by signal handler.
		 *
		 * @see #Exception
		 */
		struct terminateException :public Exception
		{
			terminateException();
		};

		/**
		 * @brief Return the current thread handle.
		 *
		 * @return Thread handle.
		 */
		static handle_type getCurrentThreadHandle();

		/**
		 * Special thread exception.
		 * @see TException
		 */
		class threadError :public Exception
		{
			public:
				/**
				* @brief Enumeration of error types.
				*/
				enum EErrorType
				{
					teSUSPENDBEFORERUN,
					teRESUMEBEFORERUN,
					teRESUMEDURINGRUN,
					teSUSPENDAFTEREXIT,
					teRESUMEAFTEREXIT,
					teCREATIONFAILURE,
					teDESTROYBEFOREEXIT,
				};

				/**
				* @brief Constructor
				*/
				threadError(EErrorType type, const Thread* thread);

				/**
				* @brief Returns the error enumeration value.
				*/
				[[nodiscard]] EErrorType getErrorType() const;

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

	protected:
		/**
		 * @brief Create a thread. Derived class overrides Run().
		 */
		Thread();

		/**
		 * @brief Virtual Destructor.
		 */
		virtual ~Thread();

		/**
		 * @brief Call by the thread to determine if it should terminate
		 */
		[[nodiscard]] bool shouldTerminate() const;

		/**
		 * @brief Alternative to returning from Run().
		 *
		 * Called from within the thread that wants to exit early.
		 */
		void exit(int code);

		/**
		 * @brief Sleeps for the given amount time until a signal interrupts when alertable is true.
		 */
		bool sleep(const TimeSpec& ts, bool alertable = true);

		/**
		 * @brief Yield control of the current thread.
		 *
		 * The name 'Yield()' is defined as a macro in MingW.
		 * @return True when successful.
		 */
		bool yieldToOther();

		/**
		 * @brief Function which can be overloaded in a derived class.
		 *
		 * Is called after a thread cancel.
		 */
		virtual void cleanup();

	private:
		/**
		 * @brief Function which needs to be overloaded in a derived class.
		 * This function is the actual main thread function.
		 */
		virtual int run();

		/**
		 * @brief Passed to the thread creation function.
		 */
		static void* execute(void* thread);

		/**
		* @brief Passed to the pthread_cleanup_push function.
		*/
		static void doCleanup(void* thread);

		/**
		* @brief Critical section for the data members.
		*/
		CriticalSection _critSec{};
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
		EStatus _status{EStatus::tsINVALID};
		/**
		* @brief Holds the flag to signal termination.
		*/
		bool _terminationRequested{false};
		/**
		 * @brief Time needed to terminate.
		 */
		TimeSpec _terminationTime{};
		/**
		 * @brief Holds the exit code of the the terminated thread.
		 */
		union {void* Ptr; int Code;} _exitCode{nullptr};
};

//
inline
void Thread::setTerminationTime(const TimeSpec& ts)
{
	_terminationTime = ts;
}

inline
Thread::threadError::EErrorType Thread::threadError::getErrorType() const
{
	return _type;
}

}
