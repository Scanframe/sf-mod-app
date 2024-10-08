#include <cstring>
#include <math/Types.h>
#include <sstream>
#include <test/catch.h>

TEST_CASE("sf::Vector2D", "[con][generic][vector]")
{
	using Catch::Matchers::Equals, Catch::Approx;

	SECTION("Constructor")
	{
		// Check the default constructor.
		sf::Vector2D v0;
		CHECK(v0[0] == 0.0);
		CHECK(v0[1] == 0.0);
		// Check the initializing constructor.
		sf::Vector2D v1(1.23, 4.56);
		CHECK(v1[0] == 1.23);
		CHECK(v1[1] == 4.56);
		// Check the initializing list.
		sf::Vector2D v2{1.23, 4.56};
		CHECK(v2[0] == 1.23);
		CHECK(v2[1] == 4.56);
		// Check coordinate value functions.
		v0.x() = 1.234;
		CHECK(v0[0] == 1.234);
		v0.y() = 4.321;
		CHECK(v0[1] == 4.321);
		// Check coordinate const value functions.
		const sf::Vector2D vc(v0);
		CHECK(vc.x() == 1.234);
		CHECK(vc.y() == 4.321);
		CHECK(vc[0] == 1.234);
		CHECK(vc[1] == 4.321);
	}

	SECTION("String + Stream Operators")
	{
		sf::Vector2D v1(1.23, 4.56);
		// Check ostream operator using the helper class.
		CHECK(v1.toString() == "(1.23,4.56)");
		sf::Vector2D v2;
		// Input string stream operator.
		std::istringstream("(1.23,4.56)") >> v2;
		CHECK(v2[0] == Catch::Approx(1.23).margin(sf::Vector2D::tolerance));
		CHECK(v2[1] == Catch::Approx(4.56).margin(sf::Vector2D::tolerance));
		// Output string stream.
		CHECK((std::ostringstream() << v2).str() == "(1.23,4.56)");
		// Check if it throws correct exception on faulty string argument.
		CHECK_THROWS_AS(v1.fromString(std::string("[1.23,345]")), std::invalid_argument);
	}

	SECTION("Array Operators")
	{
		// Check array functions.
		sf::Vector2D v1{2, 3};
		CHECK(v1[0] == 2);
		CHECK(v1[1] == 3);
		// Check if out of bounds throws correct exception.
		CHECK_THROWS_AS(v1[2], std::out_of_range);
		// Check the const array operator.
		const sf::Vector2D& v2(v1);
		CHECK_THROWS_AS(v2[-1], std::out_of_range);
		// Check Array assignment using the array operator.
		v1[0] = 5.1;
		v1[1] = 7.2;
		CHECK(v1.toString() == "(5.1,7.2)");
	}

	SECTION("Compare Operators")
	{
		sf::Vector2D v1;
		v1.assign(3.0, 6.0);
	}

	SECTION("Arithmetic Operators")
	{
		sf::Vector2D v1{3.0, 6.0};
		// Multiply
		v1 *= 2.0;
		CHECK(v1.toString() == "(6,12)");
		CHECK((6.0 * sf::Vector2D(1.0, 2.0)).toString() == "(6,12)");
		CHECK((sf::Vector2D(1.0, 2.0) * 6.0).toString() == "(6,12)");
		// Divide a vector.
		v1 /= 6.0;
		CHECK(v1.toString() == "(1,2)");
		sf::Vector2D v2(12.0, 24.0);
		CHECK((v2 / 2.0).toString() == "(6,12)");
		// Add vector.
		CHECK((v1 += sf::Vector2D(2.0, 1.0)).toString() == "(3,3)");
		// Subtract a vector.
		CHECK((v1 -= {1.0, 2.0}).toString() == "(2,1)");
		// Addition operator.
		CHECK((sf::Vector2D(2.0, 3.0) + sf::Vector2D(1.0, 2.0)).toString() == "(3,5)");
		// Subtraction operator.
		CHECK((sf::Vector2D(2.0, 3.0) - sf::Vector2D(1.0, 2.0)).toString() == "(1,1)");
	}

	SECTION("Arithmetic Functions")
	{
		sf::Vector2D v1{3.0, 6.0};
		// Normalized vector length equals 1.0.
		CHECK(v1.normalized().length() == Approx(1.0).margin(sf::Vector2D::tolerance));
		// Normalize vector length equals 1.0.
		auto v2(v1);
		v2.normalize();
		CHECK(v2.length() == Approx(1.0).margin(sf::Vector2D::tolerance));
		// Slope calculation.
		sf::Vector2D v3{3.0, 6.0};
		CHECK(v3.slope() == Approx(2.0).margin(sf::Vector2D::tolerance));
		// Dot product calculation.
		CHECK(v3.dotProduct({-1, -3}) == Approx(-21.0).margin(sf::Vector2D::tolerance));
		// Cross product calculation.
		CHECK(v3.crossProduct({-1, -3}) == Approx(15.0).margin(sf::Vector2D::tolerance));
		// Angle calculation.
		CHECK(v3.angle() == Approx(1.10714871779409041).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, 1).angle()) == Approx(45.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, -1).angle()) == Approx(-45.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, -1).angleNormalized()) == Approx(315.0).margin(sf::Vector2D::tolerance));
		// Calculating angles between vectors using dot and cross products never exceeds 180 degree angles.
		CHECK(sf::toDegrees(v1.assign(1, 1).angle({1, 1})) == Approx(0.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, -1).angle({1, 1})) == Approx(90.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, 1).angle({-1, 1})) == Approx(90.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, 1).angle({-1, -1})) == Approx(180.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1, 1).angle({1, 1})) == Approx(0.0).margin(sf::Vector2D::tolerance));
		// Too small values to calculate an angle with.
		CHECK(sf::toDegrees(v1.assign(1, 1).angle({0, 1e-10})) == Approx(45.0).margin(sf::Vector2D::tolerance));

		CHECK(sf::toDegrees(v1.assign(1e-10, 1e-10).angle({0, -1e-10})) == Approx(135.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1e-30, 1e-30).angle({0, -1e-30})) == Approx(135.0).margin(sf::Vector2D::tolerance));
		//CHECK_THROWS_AS(sf::toDegrees(v1.assign(1e-30, 1e-30).angle({0, -1e-30})), std::invalid_argument);
		CHECK(sf::toDegrees(v1.assign(1e-20, 1e-20).angle()) == Approx(45.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v1.assign(1e-4, 1e-4).angle({0, 1e-4})) == Approx(45.0).margin(sf::Vector2D::tolerance));
		//
		sf::Vector2D v4(1.0, std::sqrt(3.0));
		sf::Vector2D v5(1.0, -std::sqrt(3.0));

		CHECK(sf::toDegrees(v4.angle()) == Approx(60.0).margin(1E-10));
		CHECK(sf::toDegrees(v5.angle()) == Approx(-60.0).margin(1E-10));

		CHECK(v4.dotProduct(v5) == Approx(-2.0).margin(1E-10));
		CHECK(v4.crossProduct(v5) == Approx(4.0).margin(1E-10));
		CHECK(sf::toDegrees(v4.angle(v5)) == Approx(120.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(v5.angle(v4)) == Approx(120.0).margin(sf::Vector2D::tolerance));
	}

	SECTION("Special Functions")
	{
		sf::Vector2D v1{3.0, 6.0};
		// >0: Same side,
		CHECK(sf::Vector2D::areOnSameSide({-4, -2}, {10, 5}, {6, 4}, {8, 3}) == -1);
		// <0: Opposite sides
		CHECK(sf::Vector2D::areOnSameSide({-4, -2}, {10, 5}, {6, 4}, {4, 3}) == 1);
		// 0 : A point on the line
		CHECK(sf::Vector2D::areOnSameSide({-4, -2}, {10, 5}, {2, 1}, {4, 2}) == 0);
		// Pointer cast function.
		const sf::Vector2D::value_type* ptr = v1;
		CHECK(ptr[0] == 3.0);
		CHECK(ptr[1] == 6.0);
	}
}
