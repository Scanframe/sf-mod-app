#include <cstring>
#include <iostream>
#include <math/Types.h>
#include <test/catch.h>

namespace
{
}

TEST_CASE("sf::Vector3D", "[con][generic][vector]")
{
	using Catch::Matchers::Equals;

	SECTION("Constructor & Streams", "")
	{
		CHECK((std::cerr << "Hello from the Vector3D test." << std::endl).good());
	}
}
