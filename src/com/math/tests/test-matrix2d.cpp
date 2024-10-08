#include <cstring>
#include <math/Types.h>
#include <sstream>
#include <test/catch.h>

TEST_CASE("sf::Matrix2D", "[con][generic][vector]")
{
	using Catch::Matchers::Equals, Catch::Approx;

	SECTION("Constructor")
	{
		// Check the default constructor.
		sf::Matrix2D m0(1.0, 2.0, 3.0, 4.0);
		sf::Matrix2D::value_type mtx[2][2];
		m0.copyTo(mtx);
		CHECK(mtx[0][0] == 1.0);
		CHECK(mtx[0][1] == 2.0);
		CHECK(mtx[1][0] == 3.0);
		CHECK(mtx[1][1] == 4.0);
		// Check the pointer operator.
		double* ptr = m0;
		CHECK(ptr[0] == 1.0);
		CHECK(ptr[1] == 2.0);
		CHECK(ptr[2] == 3.0);
		CHECK(ptr[3] == 4.0);
		// Check the const pointer operator.
		const double* cptr = m0;
		CHECK(cptr[0] == 1.0);
		CHECK(cptr[1] == 2.0);
		CHECK(cptr[2] == 3.0);
		CHECK(cptr[3] == 4.0);
		// Check the Initializer list constructor.
		sf::Matrix2D m1 = {1.0, 2.0, 3.0, 4.0};
		m1.copyTo(mtx);
		CHECK(mtx[0][0] == 1.0);
		CHECK(mtx[0][1] == 2.0);
		CHECK(mtx[1][0] == 3.0);
		CHECK(mtx[1][1] == 4.0);
		// Copy constructor.
		sf::Matrix2D m2(m1);
		m2.copyTo(mtx);
		CHECK(mtx[0][0] == 1.0);
		CHECK(mtx[0][1] == 2.0);
		CHECK(mtx[1][0] == 3.0);
		CHECK(mtx[1][1] == 4.0);
		// Move operator.
		sf::Matrix2D m3;
		m3 = sf::Matrix2D(1.0, 2.0, 3.0, 4.0);
		m3.copyTo(mtx);
		CHECK(mtx[0][0] == 1.0);
		CHECK(mtx[0][1] == 2.0);
		CHECK(mtx[1][0] == 3.0);
		CHECK(mtx[1][1] == 4.0);
		// Move constructor.
		sf::Matrix2D m4(sf::Matrix2D(1.0, 2.0, 3.0, 4.0));
		m4.copyTo(mtx);
		CHECK(mtx[0][0] == 1.0);
		CHECK(mtx[0][1] == 2.0);
		CHECK(mtx[1][0] == 3.0);
		CHECK(mtx[1][1] == 4.0);
	}

	SECTION("String + Stream Operators")
	{
		// Conversion to string
		CHECK(sf::Matrix2D().toString() == "({1,0},{0,1})");
		// Convert string representing into
		CHECK(sf::Matrix2D().fromString("({2,3},{4,6})") == sf::Matrix2D(2.0, 3.0, 4.0, 6.0));
		// Input string stream operator.
		sf::Matrix2D m2;
		std::istringstream("({2,3},{4,6})") >> m2;
		CHECK(m2 == sf::Matrix2D(2.0, 3.0, 4.0, 6.0));
		// Output string stream.
		CHECK((std::ostringstream() << sf::Matrix2D(2.0, 3.0, 4.0, 6.0)).str() == "({2,3},{4,6})");
		CHECK((std::ostringstream() << sf::Matrix2D(0.123e4, 3.432, +43e+9, -0.6e-4)).str() == "({1230,3.432},{43e9,-6e-05})");
		// Check if it throws correct exception on faulty string argument.
		CHECK_THROWS_AS(sf::Matrix2D().fromString(std::string("({1,0}, {0,1})")), std::invalid_argument);
	}

	SECTION("Matrix Operations")
	{
		// Check the commutative property of the matrix.
		CHECK((sf::Matrix2D(2.0, 2.0) * sf::Matrix2D(sf::toRadials(-60.0))) == (sf::Matrix2D(sf::toRadials(-60.0)) * sf::Matrix2D(2.0, 2.0)));
	}

	SECTION("Vector Operations")
	{
		// Create rotation matrix
		CHECK((sf::Matrix2D(sf::toRadials(90.0)) * sf::Vector2D(3.0, 5.0)).toString() == "(-5,3)");
		// Get the rotation of a matrix.
		CHECK(sf::toDegrees(sf::Matrix2D(sf::toRadials(-45.0)).getRotation()) == Approx(-45.0).margin(sf::Vector2D::tolerance));
		// Check for a correct domain error when not having a square.
		CHECK_THROWS_AS(sf::Matrix2D(1.0, 2.0, 1.0, 2.0).getRotation(), std::domain_error);
		//
		CHECK(sf::toDegrees(sf::Matrix2D(sf::toRadials(-45.0)).transposed().getRotation()) == Approx(45.0).margin(sf::Vector2D::tolerance));
		CHECK(sf::toDegrees(sf::Matrix2D(sf::toRadials(90.0)).getRotation()) == Approx(90.0).margin(sf::Vector2D::tolerance));
		//
		CHECK((sf::Matrix2D(sf::toRadials(-60.0)) * sf::Vector2D(1.0, std::sqrt(3.0))) == sf::Vector2D(2.0, 0.0));
		CHECK((sf::Matrix2D(sf::toRadials(60.0)).transposed() * sf::Vector2D(1.0, std::sqrt(3.0))) == sf::Vector2D(2.0, 0.0));
		// Check scaling a vector.
		CHECK((sf::Matrix2D(-2.0, -2.0) * sf::Vector2D(1.5, 3.0)) == sf::Vector2D(-3.0, -6.0));
		// Combine scale and rotation matrices to a single one.
		CHECK(sf::Matrix2D(2.0, 2.0) * sf::Matrix2D(sf::toRadials(-60.0)) * sf::Vector2D(1.0, std::sqrt(3.0)) == sf::Vector2D(4.0, 0.0));

		// Check operator '*=' for matrix transformation (Rvalue) on vector (Lvalue).
		sf::Vector2D v(1.0, std::sqrt(3.0));
		v *= sf::Matrix2D(sf::toRadials(-60.0));
		CHECK(v == sf::Vector2D(2.0, 0.0));
	}
}
