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
		std::string operator()(const sf::Vector3D& v)
		{
			std::ostringstream os;
			return dynamic_cast<std::ostringstream&>(os << v).str();
		}
} Helper;

}// namespace

TEST_CASE("sf::Vector3D", "[con][generic][vector]")
{
	using Catch::Matchers::Equals, Catch::Approx;

	SECTION("Constructor")
	{
		// Check the default constructor.
		sf::Vector3D v0;
		CHECK(v0[0] == 0.0);
		CHECK(v0[1] == 0.0);
		CHECK(v0[2] == 0.0);
		// Check the initializing constructor.
		sf::Vector3D v1(1.23, 4.56, 7.89);
		CHECK(v1[0] == 1.23);
		CHECK(v1[1] == 4.56);
		CHECK(v1[2] == 7.89);
		// Check the initializing list.
		sf::Vector3D v2{1.23, 4.56, 6.78};
		CHECK(v2[0] == 1.23);
		CHECK(v2[1] == 4.56);
		// Check coordinate value functions.
		v0.x() = 1.234;
		CHECK(v0[0] == 1.234);
		v0.y() = 4.321;
		CHECK(v0[1] == 4.321);
		// Check coordinate const value functions.
		const sf::Vector3D vc(v0);
		CHECK(vc.x() == 1.234);
		CHECK(vc.y() == 4.321);
		CHECK(vc[0] == 1.234);
		CHECK(vc[1] == 4.321);
	}

	SECTION("String + Stream Operators")
	{
		sf::Vector3D v1(1.23, 4.56, 7.89);
		// Check ostream operator using the helper class.
		CHECK(v1.toString() == "(1.23,4.56,7.89)");
		sf::Vector3D v2;
		// Input string stream operator.
		std::istringstream("(1.23,4.56,7.89)") >> v2;
		CHECK(v2[0] == Catch::Approx(1.23).margin(sf::Vector3D::tolerance));
		CHECK(v2[1] == Catch::Approx(4.56).margin(sf::Vector3D::tolerance));
		CHECK(v2[2] == Catch::Approx(7.89).margin(sf::Vector3D::tolerance));
		// Output string stream.
		CHECK(Helper(v2) == "(1.23,4.56,7.89)");
		// Check if it throws correct exception on faulty string argument.
		CHECK_THROWS_AS(v1.fromString(std::string("[1.23,345,678]")), std::invalid_argument);
	}

	SECTION("Array Operators")
	{
		// Check array functions.
		sf::Vector3D v1{2, 3, 4};
		CHECK(v1[0] == 2);
		CHECK(v1[1] == 3);
		CHECK(v1[2] == 4);
		// Check if out of bounds throws correct exception.
		CHECK_THROWS_AS(v1[3], std::out_of_range);
		// Check the const array operator.
		const sf::Vector3D& v2(v1);
		// Check the const array operator.
		CHECK_THROWS_AS(v2[5], std::out_of_range);
		// Check Array assignment using the array operator.
		v1[0] = 5.1;
		v1[1] = 7.2;
		v1[2] = 9.3;
		CHECK(v1.toString() == "(5.1,7.2,9.3)");
	}

	SECTION("Compare Operators")
	{
		auto epsilon = std::numeric_limits<sf::Vector2D::value_type>::epsilon();
		CHECK(sf::Vector3D(3.0, 6.0, 9.0) == sf::Vector3D(3.0, 6.0, 9.0));
		CHECK(sf::Vector3D(3.0, 6.0, 9.0) == sf::Vector3D(3.0 - epsilon, 6.0 + epsilon, 9.0 + epsilon));
		CHECK(sf::Vector3D(6.0, 3.0, 9.0) != sf::Vector3D(3.0, 6.0, 9.0));
		CHECK(sf::Vector3D(6.0, 3.0, 9.0) != sf::Vector3D(6.0 + epsilon * 11, 3.0, 9.0));
	}

	SECTION("Arithmetic Operators")
	{
		sf::Vector3D v1{3.0, 6.0, 9.0};
		// Multiply
		v1 *= 2.0;
		CHECK(v1.toString() == "(6,12,18)");
		CHECK((6.0 * sf::Vector3D(1.0, 2.0, 3.0)).toString() == "(6,12,18)");
		CHECK((sf::Vector3D(1.0, 2.0, 3.0) * 6.0).toString() == "(6,12,18)");
		// Divide a vector.
		v1 /= 6.0;
		CHECK(v1.toString() == "(1,2,3)");
		sf::Vector3D v2(12.0, 24.0, 36.0);
		CHECK((v2 / 2.0).toString() == "(6,12,18)");
		// Add vector.
		CHECK((v1 += sf::Vector3D(2.0, 1.0, -6.0)).toString() == "(3,3,-3)");
		// Subtract a vector.
		CHECK((v1 -= {1.0, 2.0, -2.0}).toString() == "(2,1,-1)");
		// Addition operator.
		CHECK((sf::Vector3D(2.0, 3.0, 4.0) + sf::Vector3D(1.0, 2.0, 3.0)).toString() == "(3,5,7)");
		// Subtraction operator.
		CHECK((sf::Vector3D(2.0, 3.0, 4.0) - sf::Vector3D(1.0, 2.0, 3.0)).toString() == "(1,1,1)");
	}

	SECTION("Arithmetic Functions")
	{
		sf::Vector3D v1{3.0, 6.0, 9.0};
		// Normalized vector length equals 1.0.
		CHECK(v1.normalized().length() == Approx(1.0).margin(sf::Vector3D::tolerance));
		// Normalize vector length equals 1.0.
		auto v2(v1);
		v2.normalize();
		CHECK(v2.length() == Approx(1.0).margin(sf::Vector3D::tolerance));
		// Slope calculation.
		sf::Vector3D v3{3.0, 6.0, 9.0};
		// Dot product calculation.
		CHECK(v3.dotProduct({-1.0, -3.0, -5.0}) == Approx(-66.0).margin(sf::Vector3D::tolerance));
		// Cross product calculation.
		CHECK(v3.crossProduct({-1.0, -3.0, -5.0}) == sf::TVector3D(-3.0, -6.0, -3.0));
		// Angle calculation.
		// Calculating angles between vectors using dot and cross products never exceeds 180 degree angles.
		auto a = v1.assign(1, 1, 1).angle({1, 1, 1});
		CHECK(sf::toDegrees(v1.assign(1, 2, 3).angle({4, -5, 6})) == Approx(68.56244696639714675).margin(sf::Vector3D::tolerance));
		CHECK(sf::toDegrees(v1.assign(3, 4, 0).angle({-3, 4, 5})) == Approx(78.58056166224206152).margin(sf::Vector3D::tolerance));
		CHECK(sf::toDegrees(v1.assign(7, 8, -1).angle({4, -2, 5})) == Approx(84.39138284455458461).margin(sf::Vector3D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, 1, 1).angle({-1, -1, 1})) == Approx(109.47122063449069174).margin(sf::Vector3D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, 1, 1).angle({1, 1, 1})) == Approx(0.0).margin(sf::Vector3D::tolerance));
		// Too small values to calculate an angle with.
		CHECK(sf::toDegrees(v1.assign(1, 1, 0).angle({0, 1e-10, 0})) == Approx(45.0).margin(sf::Vector3D::tolerance));

		CHECK(sf::toDegrees(v1.assign(1e-10, 1e-10, 0).angle({0, -1e-10, 0})) == Approx(135.0).margin(sf::Vector3D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1e-10, 0, 1e-10).angle({0, 0, -1e-10})) == Approx(135.0).margin(sf::Vector3D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1e-30, 1e-30, 0).angle({0, -1e-30, 0})) == Approx(135.0).margin(sf::Vector3D::tolerance));
		CHECK(sf::toDegrees(v1.assign(0, 1e-30, 1e-30).angle({0, -1e-30, 0})) == Approx(135.0).margin(sf::Vector3D::tolerance));
		//CHECK_THROWS_AS(sf::toDegrees(v1.assign(1e-30, 1e-30, 0).angle({0, -1e-30, 0})), std::invalid_argument);
		CHECK(sf::toDegrees(v1.assign(1e-4, 1e-4, 0).angle({0, 1e-4, 0})) == Approx(45.0).margin(sf::Vector3D::tolerance));
		//
		sf::Vector3D v4(1.0, std::sqrt(3.0), 0);
		sf::Vector3D v5(1.0, -std::sqrt(3.0), 0);

		CHECK(v4.dotProduct(v5) == Approx(-2.0).margin(1E-10));
		CHECK(sf::toDegrees(v4.angle(v5)) == Approx(120.0).margin(sf::Vector3D::tolerance));
		CHECK(sf::toDegrees(v5.angle(v4)) == Approx(120.0).margin(sf::Vector3D::tolerance));
	}
}
