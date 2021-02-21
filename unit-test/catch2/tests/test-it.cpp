// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include  "../src/functions.h"

TEST_CASE("My Tests", "[main]")
{
	SECTION("Addition")
	{
		REQUIRE(my_addition(10, 12) == 22);
	};

	SECTION("Subtraction")
	{
		REQUIRE(my_subtraction(12, 4) == 8);
	};

}
