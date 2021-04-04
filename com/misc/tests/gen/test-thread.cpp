#include <catch2/catch.hpp>

#include <iostream>
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/Thread.h>
#include <misc/gen/dbgutils.h>
#include <cstring>
#include <misc/gen/PerformanceTimer.h>

extern int debug_level;

TEST_CASE("sf::Thread", "[generic][thread]")
{
	SECTION("Termination", "Testing termination.")
	{
		class Scope
		{
			public:
				explicit Scope(bool& flag, bool debug)
					:_flag(flag), _debug(debug)
				{}
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

		sf::PerformanceTimer  pf;
		//
		tc.setDebug(debug_level > 0);
		tc.start();
		::usleep(20000);
		tc.terminateAndWait();
		CHECK(pf.elapseMSec() < 1000);
		// Check the exit.
		CHECK(tc.getExitCode() == 0xff);
		// Check if the the scope class was cleaned up.
		CHECK(out_of_scope);
	}
}

