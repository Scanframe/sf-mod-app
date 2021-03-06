#include <catch2/catch.hpp>

#include <iostream>
#include <sstream>
#include <misc/mcvector.h>

TEST_CASE("sf::Variable", "[variable]")
{
	using Catch::Equals;

	SECTION("Type Sizes")
	{
		std::clog << "Integer sizes: "
			<< "int(" << sizeof(int) * CHAR_BIT << "), "
			<< "long(" << sizeof(long) * CHAR_BIT << "), "
			<< "long long(" << sizeof(long long) * CHAR_BIT << "), "
			<< std::endl;
	}
}
