#include <catch2/catch.hpp>

#include <iostream>
#include <misc/gen/TDeque.h>

namespace
{
template<typename T>
std::string toString(const sf::TDeque<T>& v)
{
	std::ostringstream os;
	return dynamic_cast<std::ostringstream&>(v.write(os, false)).str();
}
}

SCENARIO("sf::TDeque", "[generic][deque]")
{
	GIVEN("A Counted Deque")
	{
		typedef sf::TDeque<int> v_type;
		v_type v{5, 6, 7, 1, 2, 3, 4};
		CHECK(v.count() == 7);

		WHEN("After deque initialisation")
		{
			THEN("Content Check")
			{
				REQUIRE_THAT(toString(v), Catch::Matchers::Equals("{5, 6, 7, 1, 2, 3, 4}"));
			}

			THEN("Sorting")
			{
				std::sort(v.begin(), v.end());
				REQUIRE_THAT(toString(v), Catch::Matchers::Equals("{1, 2, 3, 4, 5, 6, 7}"));
			}

			THEN("Pop Front")
			{
				v.push_back(0);
				v.push_front(9);
				REQUIRE_THAT(toString(v), Catch::Matchers::Equals("{9, 5, 6, 7, 1, 2, 3, 4, 0}"));
			}

			THEN("Add()")
			{
				auto sz = v.size();
				auto pos = v.add(8);
				// Check the inserted position
				CHECK(sz == pos);
				REQUIRE_THAT(toString(v), Catch::Matchers::Equals("{5, 6, 7, 1, 2, 3, 4, 8}"));
			}

			THEN("AddAt()")
			{
				v_type::size_type pos = 4;
				REQUIRE(v.addAt(8, pos));
				REQUIRE(v.at(pos) == 8);
				REQUIRE_THAT(toString(v), Catch::Matchers::Equals("{5, 6, 7, 1, 8, 2, 3, 4}"));
			}

			THEN("Detach()")
			{
				REQUIRE(v.detachAt(3));
				REQUIRE(v.detachAt(4));
				REQUIRE_THAT(toString(v), Catch::Matchers::Equals("{5, 6, 7, 2, 4}"));
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

			SECTION("Test Deque")
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
