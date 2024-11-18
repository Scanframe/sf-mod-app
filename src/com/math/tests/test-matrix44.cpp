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
		std::string operator()(const sf::Matrix44& m)
		{
			std::ostringstream os;
			return dynamic_cast<std::ostringstream&>(os << m).str();
		}
} Helper;

}// namespace

TEST_CASE("sf::Matrix44", "[con][generic][vector]")
{
	using Catch::Matchers::Equals, Catch::Approx;

	SECTION("Constructor")
	{
		sf::Matrix44::value_type mtx[4][4];
		// Check the default constructor.
		sf::Matrix44().copyTo(mtx);
		CHECK(mtx[0][0] == 1);
		CHECK(mtx[0][1] == 0);
		CHECK(mtx[0][2] == 0);
		CHECK(mtx[0][3] == 0);
		CHECK(mtx[1][0] == 0);
		CHECK(mtx[1][1] == 1);
		CHECK(mtx[1][2] == 0);
		CHECK(mtx[1][3] == 0);
		CHECK(mtx[2][0] == 0);
		CHECK(mtx[2][1] == 0);
		CHECK(mtx[2][2] == 1);
		CHECK(mtx[2][3] == 0);
		CHECK(mtx[3][0] == 0);
		CHECK(mtx[3][1] == 0);
		CHECK(mtx[3][2] == 0);
		CHECK(mtx[3][3] == 1);
		// Check the initialization constructor.
		sf::Matrix44 m0(
			1, 2, 3, 4,
			11, 12, 13, 14,
			21, 22, 23, 24,
			31, 32, 33, 34
		);
		m0.copyTo(mtx);
		CHECK(mtx[0][0] == 1);
		CHECK(mtx[0][1] == 2);
		CHECK(mtx[0][2] == 3);
		CHECK(mtx[0][3] == 4);
		CHECK(mtx[1][0] == 11);
		CHECK(mtx[1][1] == 12);
		CHECK(mtx[1][2] == 13);
		CHECK(mtx[1][3] == 14);
		CHECK(mtx[2][0] == 21);
		CHECK(mtx[2][1] == 22);
		CHECK(mtx[2][2] == 23);
		CHECK(mtx[2][3] == 24);
		CHECK(mtx[3][0] == 31);
		CHECK(mtx[3][1] == 32);
		CHECK(mtx[3][2] == 33);
		CHECK(mtx[3][3] == 34);
		// Check the pointer operator.
		sf::Matrix44::value_type* ptr = m0;
		CHECK(ptr[0] == 1);
		CHECK(ptr[1] == 2);
		CHECK(ptr[2] == 3);
		CHECK(ptr[3] == 4);
		CHECK(ptr[4] == 11);
		CHECK(ptr[5] == 12);
		CHECK(ptr[6] == 13);
		CHECK(ptr[7] == 14);
		CHECK(ptr[8] == 21);
		CHECK(ptr[9] == 22);
		CHECK(ptr[10] == 23);
		CHECK(ptr[11] == 24);
		CHECK(ptr[12] == 31);
		CHECK(ptr[13] == 32);
		CHECK(ptr[14] == 33);
		CHECK(ptr[15] == 34);
		// Check the const pointer operator.
		const sf::Matrix44::value_type* cptr = m0;
		CHECK(cptr[0] == 1);
		CHECK(cptr[1] == 2);
		CHECK(cptr[2] == 3);
		CHECK(cptr[3] == 4);
		CHECK(cptr[4] == 11);
		CHECK(cptr[5] == 12);
		CHECK(cptr[6] == 13);
		CHECK(cptr[7] == 14);
		CHECK(cptr[8] == 21);
		CHECK(cptr[9] == 22);
		CHECK(cptr[10] == 23);
		CHECK(cptr[11] == 24);
		CHECK(cptr[12] == 31);
		CHECK(cptr[13] == 32);
		CHECK(cptr[14] == 33);
		CHECK(cptr[15] == 34);
		// Check the Initializer list constructor.
		sf::Matrix44 m1 = {
			1, 2, 3, 4,
			11, 12, 13, 14,
			21, 22, 23, 24,
			31, 32, 33, 34
		};
		m1.copyTo(mtx);
		CHECK(mtx[0][0] == 1);
		CHECK(mtx[0][1] == 2);
		CHECK(mtx[0][2] == 3);
		CHECK(mtx[0][3] == 4);
		CHECK(mtx[1][0] == 11);
		CHECK(mtx[1][1] == 12);
		CHECK(mtx[1][2] == 13);
		CHECK(mtx[1][3] == 14);
		CHECK(mtx[2][0] == 21);
		CHECK(mtx[2][1] == 22);
		CHECK(mtx[2][2] == 23);
		CHECK(mtx[2][3] == 24);
		CHECK(mtx[3][0] == 31);
		CHECK(mtx[3][1] == 32);
		CHECK(mtx[3][2] == 33);
		CHECK(mtx[3][3] == 34);
		// Copy constructor.
		sf::Matrix44 m2(m1);
		m2.copyTo(mtx);
		CHECK(mtx[0][0] == 1);
		CHECK(mtx[0][1] == 2);
		CHECK(mtx[0][2] == 3);
		CHECK(mtx[0][3] == 4);
		// Move operator.
		sf::Matrix44 m3;
		m3 = sf::Matrix44(
			1, 2, 3, 4,
			11, 12, 13, 14,
			21, 22, 23, 24,
			31, 32, 33, 34
		);
		m3.copyTo(mtx);
		CHECK(mtx[0][0] == 1);
		CHECK(mtx[0][1] == 2);
		CHECK(mtx[0][2] == 3);
		CHECK(mtx[0][3] == 4);
		CHECK(mtx[1][0] == 11);
		CHECK(mtx[1][1] == 12);
		CHECK(mtx[1][2] == 13);
		CHECK(mtx[1][3] == 14);
		CHECK(mtx[2][0] == 21);
		CHECK(mtx[2][1] == 22);
		CHECK(mtx[2][2] == 23);
		CHECK(mtx[2][3] == 24);
		CHECK(mtx[3][0] == 31);
		CHECK(mtx[3][1] == 32);
		CHECK(mtx[3][2] == 33);
		CHECK(mtx[3][3] == 34);
		// Move constructor.
		sf::Matrix44 m4(sf::Matrix44(1, 2, 3, 4, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34));
		m4.copyTo(mtx);
		CHECK(mtx[0][0] == 1);
		CHECK(mtx[0][1] == 2);
		CHECK(mtx[0][2] == 3);
		CHECK(mtx[0][3] == 4);
		CHECK(mtx[1][0] == 11);
		CHECK(mtx[1][1] == 12);
		CHECK(mtx[1][2] == 13);
		CHECK(mtx[1][3] == 14);
		CHECK(mtx[2][0] == 21);
		CHECK(mtx[2][1] == 22);
		CHECK(mtx[2][2] == 23);
		CHECK(mtx[2][3] == 24);
		CHECK(mtx[3][0] == 31);
		CHECK(mtx[3][1] == 32);
		CHECK(mtx[3][2] == 33);
		CHECK(mtx[3][3] == 34);
	}

	SECTION("String + Stream Operators")
	{
		// Conversion to string from the default unit matrix.
		CHECK(sf::Matrix44().toString() == "({1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1})");
		//
		sf::Matrix44 mtx;
		mtx.fromString("({1,2,3,4},{11,12,13,14},{21,22,23,24},{31,32,33,34})");
		// Convert string representing into.
		CHECK(mtx == sf::Matrix44(1, 2, 3, 4, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34));
		// Input string stream operator.
		sf::Matrix44 m2;
		std::istringstream("({1,2,3,4},{11,12,13,14},{21,22,23,24},{31,32,33,34})") >> m2;
		CHECK(m2 == sf::Matrix44(1, 2, 3, 4, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34));
		// Output string stream.
		CHECK(Helper(sf::Matrix44(1, 2, 3, 4, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34)) == "({1,2,3,4},{11,12,13,14},{21,22,23,24},{31,32,33,34})");
		// Check if it throws correct exception on faulty string argument.
		CHECK_THROWS_AS(sf::Matrix44().fromString(std::string("({1,0}, {0,1})")), std::invalid_argument);
	}

	SECTION("Matrix Operations")
	{
		// A unit-matrix multiplied by a unit-matrix doers not change the original matrix.
		CHECK((sf::Matrix44() * sf::Matrix44()).toString() == "({1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1})");
		sf::Matrix44 mtx(1, 2, 3, 4, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34);
		CHECK((mtx * sf::Matrix44()) == mtx);
		// Turn/reset the matrix into a unit matrix.
		mtx.unit();
		CHECK(mtx == sf::Matrix44());
		//
		sf::Matrix44 m(sf::toRadians(180.0), 0.0, 0.0);
		CHECK((m * sf::Vector3D(1.0, 2.0, 3.0)) == sf::Vector3D(1, -2, -3));
		// Rotating around each axis for 180 degree results in the same matrix.
		CHECK((sf::Matrix44(sf::toRadians(180.0), sf::toRadians(180.0), sf::toRadians(180.0)) * sf::Vector3D(1, 2, 3)) == sf::Vector3D(1, 2, 3));
		CHECK((sf::Matrix44(sf::toRadians(90.0), 0, 0) * sf::Vector3D(1, 2, 3)) == sf::Vector3D(1, 3, -2));
		CHECK((sf::Matrix44(0, sf::toRadians(90.0), 0) * sf::Vector3D(1, 2, 3)) == sf::Vector3D(-3, 2, 1));
		CHECK((sf::Matrix44(0, 0, sf::toRadians(90.0)) * sf::Vector3D(1, 2, 3)) == sf::Vector3D(2, -1, 3));
		//
		CHECK(sf::Matrix44(1, 2, 3, 0, 4, 5, 6, 0, 7, 8, 9, 0, 0, 0, 0, 1).determinant() == Approx(0).margin(sf::Matrix44::tolerance));
		CHECK(sf::Matrix44(-1, 2, 3, 4, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34).determinant() == Approx(20).margin(sf::Matrix44::tolerance));
	}

	SECTION("Vector Operations")
	{
		// Check detection of a 3D rotational
		CHECK(sf::Matrix44(sf::toRadians(90.0), sf::toRadians(90.0), sf::toRadians(90.0)).setTranslation(1, 5, -1).isRotational());
		CHECK_FALSE(sf::Matrix44(1, 2, 3, 4, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34).isRotational());
		// Rotation matrix.
		CHECK((sf::Matrix44(sf::toRadians(90.0), sf::toRadians(90.0), sf::toRadians(90.0)) * sf::Vector3D(1, 2, 3)) == sf::Vector3D(1, 3, -2));
		CHECK(sf::Matrix44(sf::toRadians(90.0), sf::toRadians(90.0), sf::toRadians(90.0)).determinant() == Approx(1.0).margin(sf::Matrix44::tolerance));
		// Check Transpose a matrix
		CHECK(sf::Matrix44(1, 2, 3, 4, 11, 12, 13, 14, 21, 22, 23, 24, 31, 32, 33, 34).transposed() == sf::Matrix44(1,11,21,31,2,12,22,32,3,13,23,33,4,14,24,34));
		// Check if the rotation and translation are applied to the vector.
		sf::Matrix44 mtx(0, 0, sf::toRadians(-90.0));
		mtx.setTranslation(10, 10, 10);
		CHECK(mtx * sf::Vector3D(1, 0, 0) == sf::Vector3D(10, 11, 10));
		// Check if the matrix multiplied by its inverse is a unit/identity matrix.
		auto rot = sf::toRadians(45.0);
		CHECK(sf::Matrix44(rot, rot, rot) * sf::Matrix44(rot, rot, rot).inverse() == sf::Matrix44());
	}
}
