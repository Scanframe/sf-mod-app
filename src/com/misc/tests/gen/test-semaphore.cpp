#include <iostream>
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/Semaphore.h>
#include <misc/gen/TStrings.h>
#include <misc/gen/ThreadClosure.h>
#include <test/catch.h>

extern int debug_level;

TEST_CASE("sf::Semaphore", "[con][generic][thread]")
{
	using namespace sf;
	ThreadMain main_thread;
	main_thread.setDebug(true);
	ElapseTimer timer(TimeSpec(10));

	/*
	 * Timing in Wine/Windows is different.
	 * In order to get the same results as in Linux sleep commands are entered.
	 */
	SECTION("Post", "Semaphore between main and another thread.")
	{
		Mutex mutex;
		strings events;
		// Lamda function to add string events.
		auto addEVent = [&](const std::string& s) {
			Mutex::Lock lock(mutex);
			// Prevent duplicates only add a change.
			if (events.size() && events[events.size() - 1] == s)
			{
				//SF_NORM_NOTIFY(DO_DEFAULT, "Skipping value '" << s << "'");
			}
			else
			{
				events.append(s);
			}
		};
		// Create semaphores.
		Semaphore sem1(0);
		Semaphore sem2(0);
		// Create the threads.
		ThreadClosure tc1("tc1");
		// Set debugging depending on the command line for the thread.
		tc1.setDebug(debug_level > 0);
		//
		tc1.assign(ThreadClosure::func_type([&](Thread& thread) -> int {
			addEVent("T1S1L");
			Semaphore::Lock lock(sem1);
			// Allow main thread to go into semaphore lock 2.
			thread.sleep(TimeSpec(0.1));
			addEVent("T1S2P");
			// Add event with semaphore value pre post.
			addEVent(string_format("T1S2VP%d", sem2.value()));
			sem2.post();
			// Add event with semaphore value after post.
			addEVent(string_format("T1S2VB%d", sem2.value()));
			// Wait until termination is requested.
			while (!thread.shouldTerminate())
			{
				thread.sleep(TimeSpec(0.1));
			}
			addEVent(string_format("T1S2VA%d", sem2.value()));
			addEVent("T1X");
			return 0xff;
		}));
		// Start the thread.
		addEVent("T0T1-S");
		// Start the thread.
		tc1.start();
		// Allow thread to go in the lock.
		main_thread.sleep(TimeSpec(0.1));
		// Post an event so the thread continues.
		addEVent("T0S1P");
		sem1.post();
		// Wait for the thread to post.
		addEVent("T0S2W");
		Semaphore::Lock lock2;
		// Allow thread to process.
		main_thread.sleep(TimeSpec(0.5));
		// Time out.
		while (!lock2.acquire(sem2, TimeSpec(0.05)))
		{
			if (!timer)
				break;
		}
		// Allow thread to process.
		main_thread.sleep(TimeSpec(0.1));
		// Terminate the thread.
		addEVent("T0T1-T");
		// Set the termination flag.
		tc1.terminate();
		// Wait some time before terminating.
		main_thread.sleep(TimeSpec(3));
		// Terminate the threads.
		tc1.waitForExit();
		// Check the exit.
		CHECK(tc1.getExitCode() == 0xff);
		// Check the order of events.
		CHECK(events.join(",") == "T0T1-S,T1S1L,T0S1P,T0S2W,T1S2P,T1S2VP0,T1S2VB1,T0T1-T,T1S2VA0,T1X");
	}
}
