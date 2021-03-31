#include <catch2/catch.hpp>

#include <misc/gen/Range.h>
#include <misc/gen/RangeManager.h>
#include <misc/gen/dbgutils.h>

namespace
{

// Helper function.
void Report(sf::RangeManager& rm, const sf::Range::Vector& results)
{
	/*
	std::clog << "Resulting: " << results << std::endl;
	std::clog << "Requests: " << rm.getRequests() << std::endl;
	std::clog << "Accessibles: " << rm.getAccessibles() << std::endl;
	std::clog << "ActualRequests: " << rm.getActualRequests().sort() << std::endl;
	//*/
}

}

TEST_CASE("sf::RangeManager", "[generic][range]")
{
	sf::RangeCompareExact = true;
	//
	// For each section this piece of code is used separately.
	//
	sf::RangeManager rm;
	// Range access list
	sf::Range::Vector accessibles, results, requests, actual;
	// Set the range in which request are allowed to be made.
	rm.setManaged({0, 99});
	// Initialize the accessibles vector.
	accessibles = {{{0, 20}, {30, 40}, {50, 60}, {70, 80}}};

	SECTION("Request Out Of Range", "Making an out of range request")
	{
		// Set the accessible ranges.
		rm.unitTest(&accessibles);
		REQUIRE(rm.request({10, 100, -1}, results) == sf::RangeManager::rmOutOfRange);
		REQUIRE(results.empty());
	}

	SECTION("Request In Range", "Making an in range request")
	{
		// Set the accessible ranges.
		rm.unitTest(&accessibles);
		REQUIRE(rm.request({10, 65, -1}, results) == sf::RangeManager::rmInaccessible);
		REQUIRE(results == sf::Range::Vector{{{20, 30, -1}, {40, 50, -1}, {60, 65, -1}}});
	}

	SECTION("Request With Requested", "Requesting a range where parts of have requested already")
	{
		// Fill the actual request already made.
		actual = {{{40, 50}}};
		// Set the accessible ranges.
		rm.unitTest(&accessibles, &actual);
		//
		REQUIRE(rm.request({15, 55, -1}, results) == sf::RangeManager::rmInaccessible);
		REQUIRE(results == sf::Range::Vector{{{20, 30, -1}}});
		Report(rm, results);
	}

	SECTION("Request Same Request", "Requesting a range which have been requested already")
	{
		// Fill the actual request already made.
		requests = {{{5, 35, -1}, {55, 75, -2}}};
		actual = {{{20, 30}, {60, 70}}};
		rm.unitTest(&accessibles, &actual, &requests);
		//
		REQUIRE(rm.request({5, 35, -3}, results) == sf::RangeManager::rmInaccessible);
		REQUIRE(results.empty());
		REQUIRE(rm.getRequests() == sf::Range::Vector{{{5, 35, -1}, {55, 75, -2}, {5, 35, -3}}});
		Report(rm, results);
	}

	SECTION("Request Fulfilment", "Requesting a range which have been requested already")
	{
		// Fill the actual request already made.
		requests = {{
			{5, 35, -1},
			{5, 35, -4},
			{55, 75, -2},
			{77, 85, -2}}
		};
		actual = {{{20, 30}, {60, 70}}};
		rm.unitTest(&accessibles, &actual, &requests);
		//
		REQUIRE(rm.setAccessible({{{0, 40}}}, results));
		REQUIRE(results == sf::Range::Vector{{{5, 35, -1}, {5, 35, -4}}});
		REQUIRE(rm.getRequests() == sf::Range::Vector{{{55, 75, -2}, {77, 85, -2}}});
		REQUIRE(rm.getActualRequests() == sf::Range::Vector{{{60, 70, 0}}});
	}

	sf::RangeCompareExact = false;
}

