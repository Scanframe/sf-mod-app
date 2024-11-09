#include <cstring>
#include <math/Types.h>
#include <sstream>
#include <test/catch.h>

namespace
{
/**
 * Helper class getting streamed output in to a string for comparison of type and value.
 */
struct
{
		std::string operator()(const sf::Quaternion& quat)
		{
			std::ostringstream os;
			return dynamic_cast<std::ostringstream&>(os << quat).str();
		}
} Helper;

}// namespace

TEST_CASE("sf::Quaterion", "[con][generic][quaterion]")
{
	using Catch::Matchers::Equals, Catch::Approx;

	SECTION("Constructor")
	{
		// Check the default constructor.
		sf::Quaternion q0;
		CHECK(q0[0] == 0.0);
		CHECK(q0[1] == 0.0);
		CHECK(q0[2] == 0.0);
		CHECK(q0[3] == 1.0);
		// Check the initializing constructor.
		sf::Quaternion q1(1.0, 2.0, 3.0, 4.0);
		CHECK(q1[0] == 1.0);
		CHECK(q1[1] == 2.0);
		CHECK(q1[2] == 3.0);
		CHECK(q1[3] == 4.0);
		// Check the initializing constructor using a vector and an angle.
		sf::Quaternion q2({1.0, 1.0, 1.0}, sf::numbers::pi_v<sf::Quaternion::value_type> / 3.0);
		CHECK(q2.x() == Approx(0.28867513459481287).margin(sf::Quaternion::tolerance));
		CHECK(q2.y() == Approx(0.28867513459481287).margin(sf::Quaternion::tolerance));
		CHECK(q2.z() == Approx(0.28867513459481287).margin(sf::Quaternion::tolerance));
		CHECK(q2.real() == Approx(0.86602540378443871).margin(sf::Quaternion::tolerance));
	}

	SECTION("String + Stream Operators")
	{
		CHECK(Helper(sf::Quaternion(1.1, 2.2, 3.3, 4.4)) == "(1.1,2.2,3.3,4.4)");
		sf::Quaternion q0;
		std::istringstream("(1.1,2.2,3.3,4.4)") >> q0;
		// Check if it throws correct exception on faulty string argument.
		CHECK_THROWS_AS(sf::Quaternion().fromString(std::string("(1.1,2.2,3.3, 4.4)")), std::invalid_argument);
	}

	SECTION("Operations")
	{
		// Check magnitude calculation of quaternion 1+2i+3j+4k.
		sf::Quaternion q0(2.0, 3.0, 4.0, 1.0);
		CHECK(q0.magnitude() == Approx(5.477).margin(0.001));
		// Inverse
		sf::Quaternion q1(-q0);
		CHECK(q1.x() == Approx(-2.0).margin(sf::Quaternion::tolerance));
		CHECK(q1.y() == Approx(-3.0).margin(sf::Quaternion::tolerance));
		CHECK(q1.z() == Approx(-4.0).margin(sf::Quaternion::tolerance));
		CHECK(q1.real() == Approx(-1.0).margin(sf::Quaternion::tolerance));

		CHECK((-q0).magnitude() == Approx(5.477).margin(0.001));
	}
}
