#include <catch2/catch.hpp>

#include <iostream>
#include <sstream>
#include <misc/gen/TVector.h>

SCENARIO("sf::TVector", "[generic][vector]")
{
	GIVEN("A Counted Vector")
	{
		typedef sf::TVector<int> v_type;
		v_type v{5, 6, 7, 1, 2, 3, 4};
		CHECK(v.count() == 7);

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
				v.add(8);
				auto str = dynamic_cast<std::ostringstream&>(std::ostringstream() << v).str();
				REQUIRE_THAT(str, Catch::Matchers::Equals("{5, 6, 7, 1, 2, 3, 4, 8}"));
			}

			THEN("AddAt()")
			{
				v.addAt(8, 4);
				auto str = dynamic_cast<std::ostringstream&>(std::ostringstream() << v).str();
				REQUIRE_THAT(str, Catch::Matchers::Equals("{5, 6, 7, 1, 8, 2, 3, 4}"));
			}

			THEN("Find()")
			{
				REQUIRE(v.find(2) == 4);
			}

			THEN("operator[] access")
			{
				REQUIRE(v[2] == 7);
			}

			THEN("Exception out of range")
			{
				REQUIRE_THROWS_AS(v[10], std::logic_error);
			}

			SECTION("Test Vector")
			{
				// Iterate though a growing vector with segmentation fault.
				for (v_type::size_type i = 0; i < v.size(); ++i)
				{
					auto value = v.at(i);
					if (--value > 0)
					{
						v.add(value);
					}
					//std::clog << "Value: " << value << "  Vector size: " << v.size() << std::endl;
				}
				REQUIRE(v.size() == 28);
			}

		}
	}
}
