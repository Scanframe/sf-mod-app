#include <catch2/catch.hpp>
#include <misc/gen/Thread.h>
#include <misc/gen/ThreadRelay.h>
#include <misc/gen/Semaphore.h>
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/PerformanceTimer.h>
#include <misc/gen/dbgutils.h>
#include <iostream>
#include <cstring>
#include <memory>

extern int debug_level;

namespace sf
{

struct Worker
{
	int method1(int v1);
	int method2(int v1, int v2);
	int method3(int v1, int v2, int v3);
};

std::unique_ptr<ThreadRelay> threadRelay;

int Worker::method1(int v1)
{
	int rv;
	// Create the relay instance and use the bool operator to check if the code needs execution.
	if (ThreadRelay::Relay1<Worker, decltype(&Worker::method1), int, int>
		(*threadRelay, this, &Worker::method1, rv, v1))
	{
		return Thread::getCurrentId() + v1;
	}
	return rv;
}

int Worker::method2(int v1, int v2)
{
	int rv;
	// Create the relay instance and use the bool operator to check if the code needs execution.
	if (ThreadRelay::Relay2<Worker, decltype(&Worker::method2), int, int, int>
		(*threadRelay, this, &Worker::method2, rv, v1, v2))
	{
		return Thread::getCurrentId() + v1 + v2;
	}
	return rv;
}

int Worker::method3(int v1, int v2, int v3)
{
	int rv;
	// Create the relay instance and use the bool operator to check if the code needs execution.
	if (ThreadRelay::Relay3<Worker, decltype(&Worker::method3), int, int, int, int>
		(*threadRelay, this, &Worker::method3, rv, v1, v2, v3))
	{
		return Thread::getCurrentId() + v1 + v2 + v3;
	}
	return rv;
}

}

TEST_CASE("sf::Thread", "[generic][thread]")
{
	SECTION("Count", "Getting the amount of threads in a process.")
	{
		SF_COND_NORM_NOTIFY(debug_level, DO_CLOG, "Thread count: " << sf::getThreadCount());
		CHECK(sf::getThreadCount() > 0);
	}

	SECTION("Termination", "Terminating a sleeping thread.")
	{
		class Scope
		{
			public:
				explicit Scope(bool& flag, bool debug)
					:_flag(flag), _debug(debug) {}

				~Scope()
				{
					SF_COND_RTTI_NOTIFY(_debug, DO_DEFAULT, "Out of scope... " << sf::Thread::getCurrentId())
					_flag = true;
				}

			private:
				bool& _flag;
				bool _debug;
		};

		bool out_of_scope = false;
		sf::ThreadClosure tc(sf::ThreadClosure::func_type([&out_of_scope](sf::Thread& thread) -> int
		{
			Scope scope(out_of_scope, thread.isDebug());
			do
			{
				// Sleep for some time to get interrupted by a call to terminateAndWait().
				if (thread.sleep(sf::TimeSpec(10, 0), true))
				{
					SF_COND_NORM_NOTIFY(thread.isDebug(), DO_DEFAULT, "Test: Woken up normal:" << sf::Thread::getCurrentId())
				}
				else
				{
					SF_COND_NORM_NOTIFY(thread.isDebug(), DO_DEFAULT, "Test: Woken up by interruption: " << sf::Thread::getCurrentId())
				}
			}
			while (!thread.shouldTerminate());
			return 0xff;
		}));

		sf::PerformanceTimer pf;
		// Set debugging depending on the command line.
		tc.setDebug(debug_level > 0);
		// Start the thread.
		tc.start();
		// Allow some time for the thread to process.
		::usleep(20000);
		// Terminate the thread and wait.
		tc.terminateAndWait();
		// Check if the thread ended in a short enough time.
		CHECK(pf.elapseMSec() < 1000);
		// Check the exit.
		CHECK(tc.getExitCode() == 0xff);
		// Check if the scope class was cleaned up.
		CHECK(out_of_scope);
	}

	SECTION("Condition", "Condition and mutex.")
	{
		sf::Mutex mutex;
		sf::Condition condition;
		sf::ThreadClosure tc(sf::ThreadClosure::func_type([&](sf::Thread& thread) -> int
		{
			// Start locking the mutex.
			sf::Mutex::Lock lock(mutex);
			// Wait for the condition signal on the locked mutex.
			condition.wait(mutex);
			return 0xff;
		}));
		// Set debugging depending on the command line.
		tc.setDebug(debug_level > 0);
		// Start the thread.
		tc.start();
		//
		if (tc.getStatus() != sf::Thread::tsRunning)
		{
			// Give thread some time.
			::usleep(20000);
		}
		// Signal the locked thread.
		condition.notifyOne(mutex);
		//
		if (tc.getStatus() != sf::Thread::tsFinished)
		{
			// Give thread some time.
			::usleep(20000);
		}
		//
		tc.terminateAndWait();
		// Check the exit.
		CHECK(tc.getExitCode() == 0xff);
	}

	SECTION("Semaphore", "Semaphore between main and another thread.")
	{
		using namespace sf;
		Mutex mutex;
		strings events;
		auto addEVent = [&](const std::string& s)
		{
			Mutex::Lock lock(mutex);
			events.append(s);
		};
		// Create semaphores.
		Semaphore sem1(0), sem2(0);
		// Create the threads.
		ThreadClosure tc1;
		// Set debugging depending on the command line for the thread.
		//tc1.setDebug(debug_level > 0);
		//
		tc1.assign(ThreadClosure::func_type([&](Thread& thread) -> int
		{
			addEVent("T1S1L");
			Semaphore::Lock lock(sem1);
			// Allow main thread to go into semaphore lock 2.
			thread.sleep(TimeSpec(0.3));
			addEVent("T1S2P");
			sem2.post();
			addEVent(stringf("T1S2V%d", sem2.value()));
			// Wait until termination is requested.
			while (!thread.shouldTerminate())
			{
				addEVent("T1W");
				thread.sleep(TimeSpec(0.3));
			}
			addEVent(stringf("T1S2V%d", sem2.value()));
			addEVent("T1X");
			return 0xff;
		}));
		// Start the thread.
		addEVent("T0T1-S");
		tc1.start();
		// Sleep 50ms to keep the event order the same each test run.
		usleep(500000);
		// Post an event so the thread continues.
		addEVent("T0S1P");
		sem1.post();
		// Wait for the thread to post.
		addEVent("T0S2W");
		Semaphore::Lock lock2;
		// Give the thread time to register the semaphore value.
		usleep(30000);
		// Time out.
		while (!lock2.acquire(sem2, TimeSpec(0.05)))
		{
			addEVent("T0S2A");
		}
		// Wait some time before terminating.
		usleep(1000000);
		// Terminate the thread.
		addEVent("T0T1-T");
		tc1.terminate();
		// Terminate the threads.
		tc1.waitForExit();
		// Check the exit.
		CHECK(tc1.getExitCode() == 0xff);
		// Check the order of events.
		CHECK(events.join(",") == "T0T1-S,T1S1L,T0S1P,T0S2W,T0S2A,T0S2A,T0S2A,T0S2A,T0S2A,T1S2P,T1S2V1,T1W,T1W,T1W,T1W,T0T1-T,T1S2V0,T1X");
	}

	SECTION("ThreadRelay", "Relaying a method to be called in another thread.")
	{
		using namespace sf;
		Worker worker;
		threadRelay = std::make_unique<ThreadRelay>();
		Mutex mutex;
		TVector<Thread::id_type> results;
		//
		ThreadClosure tc1([&results, &worker, &mutex](Thread& thread) -> int
		{
			auto r = worker.method1(1);
			Mutex::Lock lock(mutex);
			results.insert(results.end(), r);
			return 0x10;
		});
		//
		ThreadClosure tc2([&results, &worker, &mutex](Thread& thread) -> int
		{
			auto r = worker.method2(1, 2);
			Mutex::Lock lock(mutex);
			results.insert(results.end(), r);
			return 0x10;
		});
		//
		ThreadClosure tc3([&results, &worker, &mutex](Thread& thread) -> int
		{
			Mutex::Lock lock(mutex);
			auto r = worker.method3(1, 2, 3);
			results.insert(results.end(), r);
			return 0x10;
		});
		//
		for (auto tc: {&tc1, &tc2, &tc3})
		{
			tc->start();
		}
		// Make the thread get into semaphore.
		usleep(100000);
		//
		threadRelay->checkForWork();
		//
		for (auto tc: {&tc1, &tc2, &tc3})
		{
			tc->terminate();
			usleep(100000);
			tc->waitForExit();
			CHECK(tc->getExitCode() == 0x10);
		}
		// 3 threads so at least 3 results.
		CHECK(results.count() == 3);
		//
		auto tid = Thread::getCurrentId();
		// Since the threads are released in an unpredictable way we need to sort them before comparing.
		std::sort(results.begin(), results.end());
		// Compare the results with what is expected.
		CHECK(results == TVector<Thread::id_type>{tid + 1, tid + 3, tid + 6});
	}
}

