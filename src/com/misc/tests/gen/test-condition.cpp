#include <iostream>
#include <memory>
#include <misc/gen/PerformanceTimer.h>
#include <misc/gen/ThreadClosure.h>
#include <misc/gen/ThreadRelay.h>
#include <test/catch.h>

extern int debug_level;

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

TEST_CASE("sf::Condition", "[con][generic][thread]")
{
	sf::ThreadMain main_thread;
	main_thread.setDebug(true);
	sf::Mutex mutex;
	sf::Condition condition;
	int cleanup_flag;
	sf::ThreadClosure tc(sf::ThreadClosure::func_type([&](sf::Thread& thread) -> int {
		// Class for testing destructor cleanup.
		TestAutoCleanup cleanup(cleanup_flag);
		// Start locking the mutex.
		sf::Mutex::Lock lock(mutex);
		// Wait for the condition signal on the locked mutex.
		condition.wait(mutex);
		// Set the exit-code for the thread.
		return 0xff;
	}));
	// Set debugging depending on the command line.
	tc.setDebug(debug_level > 0);

	SECTION("Wait-Signaled", "Wait for signaled condition.")
	{
		// Start the thread.
		tc.start();
		// Status should be running at all times.
		CHECK(tc.getStatus() == sf::Thread::tsRunning);
		// Signal the locked thread.
		condition.notifyOne();
		// When thread is not finished wait for it.
		if (tc.getStatus() != sf::Thread::tsFinished)
		{
			// Allow some time for the thread to process.
			main_thread.sleep(sf::TimeSpec(0.02));
		}
		// Request termination of the thread and wait for it.
		tc.terminateAndWait();
		// Check if the class was destructed normally.
		CHECK(cleanup_flag == 0);
		// Check the exit.
		CHECK(tc.getExitCode() == 0xff);
	}

	SECTION("Wait-Terminate", "Terminate waiting.")
	{
		// Start the thread.
		tc.start();
		// Status should be running at all times.
		CHECK(tc.getStatus() == sf::Thread::tsRunning);
		// Signal the locked thread.
		condition.notifyOne();
		// Terminate immediate
		tc.terminateAndWait();
		// Check if the class was destructed during exception.
		CHECK(cleanup_flag == 1);
		// Check the exit.
		CHECK(tc.getExitCode() == 0);
	}
}
