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
* Thread wrapper class to be used for attachment to an existing thread or a
* new to be created thread. For the latter this class must be derived and the
* at least the Run() function must be overloaded.
*/
class _MISC_CLASS Thread
{
	public:
		/**
		* Local declaration of the handle type.
		*/
		typedef pthread_t handle_type;

		/**
		* Thread states enumeration.
		* @see #GetStatus()
		* @see #GetStatusText()
		*/
		enum EStatus
		{
			tsCREATED,
			tsRUNNING,
			tsFINISHED,
			tsINVALID
		};

		/**
		* Thread attributes used internally for starting a thread.
		*/
		class Attributes
		{
			public:
				typedef ::pthread_attr_t* handle_type;

				/**
				* Default Constructor. Initialize can be used
				*/
				Attributes();

				/**
				* Initialize using the passed thread handle.
				* When the handle is '0' the current thread is used.
				* @see #Setup()
				* @throw Exception
				*/
				explicit Attributes(Thread::handle_type th);

				/**
				* Destructor.
				*/
				~Attributes();

				/**
				* When the handle is '0' the current thread is used.
				* @param th Thread handle declared in
				* @throw TException
				*/
				void Setup(Thread::handle_type th);

				/**
				* Returns the stack size of the thread which initialized it.
				*/
				[[nodiscard]] size_t GetStackSize() const;

				/**
				* Sets the stack size for a new thread.
				*/
				void SetStackSize(size_t sz);

				/**
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
				* Sets scheduling priority for the to be started thread.
				*/
				void SetSchedulePolicy(ESchedulePolicy policy);

				/**
				* Returns the current set scheduling priority.
				*/
				[[nodiscard]] ESchedulePolicy GetSchedulePolicy() const;

				/**
				* Handle casting operator.
				*/
				operator handle_type() const // NOLINT(google-explicit-constructor)
				{
					return _attributes;
				}

			private:
				/**
				 * Holds the actual attributes.
				 */
				handle_type _attributes;
		};

		/**
		* Starts a thread which calls on its turn the overloaded function Run().
		*/
		handle_type Start(const Attributes& attr);

		/**
		* Cancels the running thread and Cleanup() is called when the thread is running.
		* Is not tested yet.
		*/
		void Cancel();

		/**
		* Can be overloaded to signal the thread to terminate.
		* Makes ShouldTerminate() return false.
		* @see #ShouldTerminate()
		*/
		virtual void Terminate();

		/**
		* Same as calling Terminate() and thereafter calling WaitForExit()
		* @see #Terminate()
		* @see #WaitForExit()
		*/
		void TerminateAndWait();

		/**
		* Waits for the thread to exit.
		*/
		void WaitForExit();

		/**
		* Returns the status.
		* @see #EStatus
		*/
		[[nodiscard]] EStatus GetStatus() const;

		/**
		* Returns the status a string.
		* By default the current state is returned.
		* @see #GetStatus()
		*/
		[[nodiscard]] const char* GetStatusText(EStatus status = (EStatus) -1) const;

		/**
		* Returns the exist value of the thread function.
		*/
		[[nodiscard]] int GetExitCode() const;

		/**
		* Returns true if the calling thread is this thread.
		*/
		[[nodiscard]] bool IsSelf() const;

		/**
		* Returns the thread ID. Is currently in Linux the same as the handle.
		*/
		[[nodiscard]] pid_t GetThreadId() const;

		/**
		* Returns the handle of this instance.
		*/
		[[nodiscard]] handle_type GetHandle() const;

		/**
		* Returns the thread termination signal.
		*/
		static int GetTerminationSignal();

		/**
		* Returns the thread ID as long.
		*/
		static int GetCurrentThreadId();


		/**
		* Casting operator for THandle type.
		*/
		operator handle_type() const // NOLINT(google-explicit-constructor)
		{
			return _handle;
		}

		/**
		* Enumeration of thread priorities.
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
		* Returns the current priority of this instance.
		*/
		[[nodiscard]] int GetPriority() const;

		/**
		* Can pass a enumerate EPriority for simplicity.
		* @see #EPriority
		*/
		bool SetPriority(int, int = 0);

		/**
		 * Sets the time needed for the thread to terminate.
		 * Default is one 100 ms.
		 */
		void SetTerminationTime(const TimeSpec& ts);

		/**
		* Special thread exception thrown by signal handler.
		* @see TException
		*/
		struct TerminateException :public Exception
		{
			TerminateException();
		};

		/**
		 * Return the current thread handle.
		 * @return
		 */
		static handle_type GetCurrentThreadHandle();

		/**
		* Special thread exception.
		* @see TException
		*/
		class ThreadError :public Exception
		{
			public:
				/**
				* Enumeration of error types.
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
				* Constructor
				*/
				ThreadError(EErrorType type, const Thread* thread);

				/**
				* Returns the error enumeration value.
				*/
				[[nodiscard]] EErrorType GetErrorType() const;

			private:
				/**
				 * Holds the error type.
				 */
				EErrorType _type;
		};

		/**
		* Copying constructor disabled and not implemented.
		*/
		Thread(const Thread&) = delete;

		/**
		* Copy constructor not implemented.
		*/
		const Thread& operator=(const Thread&) = delete;

	protected:
		/**
		* Create a thread. Derived class overrides Run().
		*/
		Thread();

		/**
		* Virtual Destructor.
		*/
		virtual ~Thread();

		/**
		* Call by the thread to determine if it should terminate
		*/
		[[nodiscard]] bool ShouldTerminate() const;

		/**
		* Alternative to returning from Run().
		* Called from within the thread that wants to exit early.
		*/
		void Exit(int code);

		/**
		 * Sleeps for the given amount time until a signal interrupts
		 * when alertable is true.
		 */
		bool Sleep(const TimeSpec& ts, bool alertable = true);

		/**
		 * Yield control of the current thread.
		 * Returns true when successful.
		 * The name 'Yield()' is defined as a macro in MingW.
		 */
		bool YieldToOther();

		/**
		 * Function which can be overloaded in a derived class.
		 * Is called after a thread cancel.
		 */
		virtual void Cleanup();

	private:
		/**
		 * Function which needs to be overloaded in a derived class.
		 * This function is the actual main thread function.
		 */
		virtual int Run();

		/**
		 * Passed to the thread creation function.
		 */
		static void* Execute(void* thread);

		/**
		* Passed to the pthread_cleanup_push function.
		*/
		static void DoCleanup(void* thread);

		/**
		* Critical section for the data members.
		*/
		CriticalSection _critSec{};
		/**
		* Holds the Thread ID of the thread but its value is in Linux the same as the Handle.
		*/
		pid_t _threadId{0};
		/**
		* Holds the thread handle.
		*/
		handle_type _handle{0};
		/**
		* Holds the thread status.
		*/
		EStatus _status{EStatus::tsINVALID};
		/**
		* Holds the flag to signal termination.
		*/
		bool _terminationRequested{false};
		/**
		 * Time needed to terminate.
		 */
		TimeSpec _terminationTime{};
		/**
		 * Holds the exit code of the the terminated thread.
		 */
		union {void* Ptr; int Code;} _exitCode{nullptr};
};

//
inline
void Thread::SetTerminationTime(const TimeSpec& ts)
{
	_terminationTime = ts;
}

inline
Thread::ThreadError::EErrorType Thread::ThreadError::GetErrorType() const
{
	return _type;
}

}
