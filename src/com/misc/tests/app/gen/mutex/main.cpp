#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <misc/gen/Condition.h>
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/ThreadClosure.h>
#include <misc/gen/dbgutils.h>
#include <misc/gen/pointer.h>

pthread_t* thread_data{nullptr};
pthread_cond_t cv;
pthread_mutex_t raw_mutex;

void* thread_raw(void* v)
{
	int error = 0;
	printf("Locking and waiting. Type 'u' to unlock me.\n");
	error = pthread_mutex_lock(&raw_mutex);
	if (error)
	{
		perror(__FUNCTION__);
	}
	error = pthread_cond_wait(&cv, &raw_mutex);
	if (error)
	{
		perror(__FUNCTION__);
	}
	printf("I've been unlocked.\n");
	error = pthread_mutex_unlock(&raw_mutex);
	if (error)
	{
		perror(__FUNCTION__);
	}
	return nullptr;
}

void create_or_signal(bool create)
{
	if (create)
	{
		pthread_create(thread_data, nullptr, thread_raw, nullptr);
	}
	else
	{
		pthread_cond_signal(&cv);
	}
}

sf::Condition sf_condition;// NOLINT(cert-err58-cpp)
sf::Mutex sf_mutex;// NOLINT(cert-err58-cpp)

void* sf_thread(void* v)
{
	printf("SF Locking and waiting. Type 'u' to unlock me.\n");
	sf::Mutex::Lock lock(sf_mutex);
	sf_condition.wait(sf_mutex);
	printf("SF I've been unlocked.\n");
	//lock.release();
	return nullptr;
}

sf::ThreadClosure sf_thread_closure(
	[](sf::Thread& thread) -> int// NOLINT(cert-err58-cpp)
	{
		sf_thread(nullptr);
		return 0xff;
	}
);

void sf_create_or_signal(bool create)
{
	if (create)
	{
		//pthread_create(thread_data, nullptr, sf_thread, nullptr);
		sf_thread_closure.start();
	}
	else
	{
		sf_condition.notifyOne();
	}
}

int main_()
{
	char cmd[1024];
	thread_data = (pthread_t*) malloc(sizeof(pthread_t));
	printf("Type 'l' to run a thread that locks and waits.\n");
	printf("Type 'u' to unlock the same thread.\n");
	while (fscanf(stdin, "%s", cmd) != EOF)
	{
		if (strcmp(cmd, "l") == 0)
		{
			sf_create_or_signal(true);
		}
		else if (strcmp(cmd, "u") == 0)
		{
			sf_create_or_signal(false);
		}
		else if (strcmp(cmd, "x") == 0)
		{
			break;
		}
	}
	sf::free_null(thread_data);
	return 0;
}

void has_timed_out(bool flag)
{
	if (flag)
	{
		SF_NORM_NOTIFY(DO_COUT, "Test timed out.");
		exit(EXIT_FAILURE);
	}
}

// For testing destructor cleanup during exception handling.
class TestAutoCleanup
{
	public:
		explicit TestAutoCleanup(int& flag)
			: _flag(flag)
		{
			// Signal not destructor called.
			_flag = -1;
		}
		~TestAutoCleanup()
		{
			if (std::uncaught_exceptions())
			{
				SF_RTTI_NOTIFY(DO_COUT, "Exception cleanup in progress.");
				// Signal destructor called in exception cleanup.
				_flag = 1;
			}
			else
			{
				SF_RTTI_NOTIFY(DO_COUT, "Normal cleanup.");
				// Signal destructor called in normal cleanup.
				_flag = 0;
			}
		}

	private:
		int& _flag;
};

int main()
{
	sf::ThreadMain main_thread;
	main_thread.setDebug(true);
	std::atomic<bool> flag{false};
	// Set an elapse timer to limit the maximum time it is allowed to take (0.3s).
	sf::ElapseTimer timer(sf::TimeSpec(1));
	sf::Condition condition("closure");
	int cleanup;

	//sf::Mutex mutex("outer");
	sf::ThreadClosure tc(
		sf::ThreadClosure::func_type([&](sf::Thread& thread) -> int {
			TestAutoCleanup cleanup_test(cleanup);
			sf::Mutex mutex("inner");
			// Need a locked mutex for the condition.
			sf::Mutex::Lock lock(mutex);
			SF_NORM_NOTIFY(DO_COUT, "Thread is waiting on condition signal");
			// Wait for a signal.
			condition.wait(mutex);
			// Set the flag to signal waiting is over.
			flag = true;
			SF_NORM_NOTIFY(DO_COUT, "Thread was signaled");
			// Simulate the work...
			thread.sleep(sf::TimeSpec(0.1));
			SF_NORM_NOTIFY(DO_COUT, "Thread done and exiting");
			return 0xff;
		}),
		"closure"
	);
	// Enable debug printing.
	tc.setDebug(true);
	// Start the thread.
	tc.start();
	// Signal the locked thread.
	while (!flag)
	{
		SF_NORM_NOTIFY(DO_CLOG, "Signal not received yet.");
		condition.notifyOne();
		main_thread.sleep(sf::TimeSpec(0.001));
		has_timed_out(timer.isActive());
	}
	//
	do
	{
		SF_NORM_NOTIFY(DO_COUT, "Waiting for thread Finished state: " << condition.getWaiting());
		// Give thread some time.
		main_thread.sleep(sf::TimeSpec(0.02));
		has_timed_out(timer.isActive());
	} while (tc.getStatus() != sf::Thread::tsFinished);
	//
	tc.terminateAndWait();
	// Check the exit.
	if (tc.getExitCode() != 0xff)
	{
		SF_NORM_NOTIFY(DO_CLOG, "Exit code not expected!");
	}
	return 0;
}
