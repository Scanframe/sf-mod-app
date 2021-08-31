#include <catch2/catch.hpp>

#include <unistd.h>
#include <misc/gen/TimeSpec.h>
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/PerformanceTimer.h>
#include <misc/gen/IntervalTimer.h>
#include <misc/gen/gen_utils.h>

TEST_CASE("sf::TimeSpec", "[generic][timers]")
{

	SECTION("Construct(double)", "Constructing from 'double' type")
	{
		sf::TimeSpec ts(123.456);
		REQUIRE(ts.tv_sec == 123);
		REQUIRE(ts.tv_nsec == 456000000);
	}

	SECTION("Construct(timespec)", "Constructing from 'timespec' type")
	{
		sf::TimeSpec ts(123, 456000000);
		REQUIRE(ts.toDouble() == 123.456);
		REQUIRE(ts.toString() == "123.456s");
	}

	SECTION("Add", "Add time to the TimeSpec class")
	{
		sf::TimeSpec ts(123, 550000000);
		ts.add(0, 550000000);
		REQUIRE(ts.tv_sec == 124);
		REQUIRE(ts.tv_nsec == 100000000);
	}

	SECTION("Subtract", "Subtract time from the TimeSpec class")
	{
		sf::TimeSpec ts(124, 100000000);
		ts.sub({0, 550000000});
		REQUIRE(ts.tv_sec == 123);
		REQUIRE(ts.tv_nsec == 550000000);
	}

	SECTION("Compare", "Compare instances")
	{
		CHECK(!!sf::TimeSpec() == false);
		CHECK(sf::TimeSpec(24, 100000000) == sf::TimeSpec(24, 100000000));
		CHECK(sf::TimeSpec(4, 100000000) != sf::TimeSpec(24, 100000000));
		CHECK(sf::TimeSpec(3, 400000000) > sf::TimeSpec(1, 100000000));
		CHECK(sf::TimeSpec(1, 100000000) < sf::TimeSpec(3, 400000000));
		CHECK(sf::TimeSpec(3, 400000000) >= sf::TimeSpec(1, 100000000));
		CHECK(sf::TimeSpec(1, 100000000) <= sf::TimeSpec(3, 400000000));
	}

}

TEST_CASE("sf::Timers", "[generic][timers]")
{
	using Catch::Equals;

	SECTION("PerformanceTimer", "Individual class only")
	{
		// Windows has minimum of 20us sleep time.
		sf::PerformanceTimer pt;
		usleep(200000);
		REQUIRE(pt.elapseUSec() == Approx(200000).epsilon(0.15));
		REQUIRE(pt.elapseMSec() == Approx(200).epsilon(0.15));
		REQUIRE(pt.elapse().toDouble() == Approx(200e-3).epsilon(0.15));
#if !IS_WIN
		pt.reset();
		usleep(1);
		REQUIRE(pt.elapse().toDouble() == Approx(1e-6).margin(1e8));
#endif
	}

	SECTION("ElapseTimer", "Individual class only")
	{
		// Performance timer to check the elapse-timer.
		sf::PerformanceTimer pt;
		// Initialize the elapse-timer.
		sf::ElapseTimer et(sf::TimeSpec(1, 234000000));
		//
		CHECK_THAT(et.getElapseTime().toString(), Equals("1.234s"));
		// Check if the timer is enabled by default.
		REQUIRE(et.isEnabled());
		// Check if the timer has not elapsed yet.
		REQUIRE(!et.isActive());
		// The maximum loops for this test is 2 seconds.
		int max_loops = 2000000 / 1000;
		// Wait for the timer to elapse or for the counter.
		while (!et && max_loops-- > 0)
		{
			::usleep(1000);
		}
		REQUIRE(pt.elapse().toDouble() == Approx(1.2345).margin(0.05));
		// Check if the timer has elapsed.
		REQUIRE(et.isActive());
		// Reset the timer.
		et.reset();
		// Disable the timer.
		et.disable();
		// The maximum loops for this test is 2 seconds.
		max_loops = 1500000 / 150000;
		// Reset the performance timer.
		pt.reset();
		// Wait for the timer to elapse or for the counter.
		while (!et && max_loops-- > 0)
		{
			// Minimum value for windows.
			::usleep(150000);
		}
		// Since the timer is disabled it will never be activated/elapsed.
		CHECK(pt.elapse().toDouble() >= 1.5);
		// Check if the timer is disabled.
		CHECK(!et.isEnabled());
	}

	SECTION("IntervalTimer", "Individual class only")
	{
		// Performance timer to check the elapse-timer.
		sf::PerformanceTimer pt;
		// Initialize the elapse-timer.
		sf::IntervalTimer it(0.2);
		//
		CHECK(it.getInterval() == sf::TimeSpec(0, 200000000l));
		//
		CHECK((it.getInterval() + sf::getTime()).toDouble() == Approx(it.getTarget().toDouble()).margin(0.001));

		// Check if the timer is enabled by default.
		REQUIRE(it.isEnabled());
		// Counter for the mount of timeouts.
		int count = 0;
		// The maximum loops for this test is 1.1 seconds.
		int max_loops = 1100000 / 1000;
		while (max_loops-- > 0)
		{
			::usleep(1000);
			// Check it the timer timed out.
			if (it)
			{
				count++;
			}
		}
		CHECK(count == 5);
		CHECK(pt.elapse().toDouble() == Approx(1.1).margin(0.15));
	}

}
