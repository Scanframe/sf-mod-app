#include <catch2/catch.hpp>

#include <iostream>
#include <sstream>
#include <misc/mcvector.h>

SCENARIO("Counter Vector", "[mcvector]")
{
	GIVEN("A Counted Vector")
	{
		sf::mcvector<int> v{5, 6, 7, 1, 2, 3, 4};
		REQUIRE(v.Count() == 7);
//		std::clog << v << std::endl;

		WHEN("After vector initialisation")
		{
			THEN("Content Check")
			{
				auto str = dynamic_cast<std::ostringstream&>(std::ostringstream() << v).str();
				REQUIRE_THAT(str, Catch::Matchers::Equals("{5, 6, 7, 1, 2, 3, 4}"));
			}

			THEN("Sorting")
			{
				std::sort(v.begin(), v.end());
				auto str = dynamic_cast<std::ostringstream&>(std::ostringstream() << v).str();
				REQUIRE_THAT(str, Catch::Matchers::Equals("{1, 2, 3, 4, 5, 6, 7}"));
			}

			THEN("Add()")
			{
				v.Add(8);
				auto str = dynamic_cast<std::ostringstream&>(std::ostringstream() << v).str();
				REQUIRE_THAT(str, Catch::Matchers::Equals("{5, 6, 7, 1, 2, 3, 4, 8}"));
			}

			THEN("AddAt()")
			{
				v.AddAt(8, 4);
				auto str = dynamic_cast<std::ostringstream&>(std::ostringstream() << v).str();
				REQUIRE_THAT(str, Catch::Matchers::Equals("{5, 6, 7, 1, 8, 2, 3, 4}"));
			}

			THEN("Find()")
			{
				REQUIRE(v.Find(2) == 4);
			}

			THEN("operator[] access")
			{
				REQUIRE(v[2] == 7);
			}

			THEN("Exception out of range")
			{
				REQUIRE_THROWS_AS(v[10], std::logic_error);
			}

		}
	}
}