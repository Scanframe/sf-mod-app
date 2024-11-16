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
		CHECK(q0[sf::Quaternion::realW] == 1.0);
		CHECK(q0[sf::Quaternion::imagX] == 0.0);
		CHECK(q0[sf::Quaternion::imagY] == 0.0);
		CHECK(q0[sf::Quaternion::imagZ] == 0.0);
		// Check the initializing constructor.
		sf::Quaternion q1(1.0, 2.0, 3.0, 4.0);
		CHECK(q1[sf::Quaternion::realW] == 1.0);
		CHECK(q1[sf::Quaternion::imagX] == 2.0);
		CHECK(q1[sf::Quaternion::imagY] == 3.0);
		CHECK(q1[sf::Quaternion::imagZ] == 4.0);
		// Check the initializing constructor using a vector and an angle.
		sf::Quaternion
			q2({1.0, 1.0, 1.0}, sf::numbers::pi_v<sf::Quaternion::value_type> / 3.0);
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
		// Negate operator.
		CHECK(-sf::Quaternion(1, 2, 3, 4) == sf::Quaternion(-1, -2, -3, -4));
		// Magnitude
		CHECK(sf::Quaternion(1, 2, 3, 4).magnitudeSqr() == Approx(30).margin(sf::Quaternion::tolerance));
		CHECK(sf::Quaternion(1, 2, 3, 4).magnitude() == Approx(5.47722557505166119).margin(sf::Quaternion::tolerance));
		// Squared
		CHECK(sf::Quaternion(1, 2, 3, 4).squared() == sf::Quaternion(-28, 4, 6, 8));
		// Conjugate operation.
		CHECK(sf::Quaternion(1, 2, 3, 4).conjugate() == sf::Quaternion(1.0, -2.0, -3.0, -4.0));
		// Inverse method.
		CHECK(sf::Quaternion(1, 2, 3, 4).inverse() == sf::Quaternion(1.0 / 30.0, -2.0 / 30.0, -3.0 / 30.0, -4.0 / 30.0));
		CHECK(sf::Quaternion(2, 3, 4, 5).inverse() == sf::Quaternion(1.0 / 27.0, -3.0 / 54.0, -4.0 / 54.0, -5.0 / 54.0));
		// Multiplication
		CHECK(sf::Quaternion(1, 2, 3, 4) * sf::Quaternion(4, 3, 2, 1) == sf::Quaternion(-12, 6, 24, 12));
		// Conversion to a Matrix.
		sf::Matrix44 mtx;
		sf::Quaternion(1, 2, 3, 4).toMatrix(mtx);
		CHECK(mtx.getAxis(sf::Matrix44::axisX) == sf::Vector3D(-49, 4, 22));
		CHECK(mtx.getAxis(sf::Matrix44::axisY) == sf::Vector3D(20, -39, 20));
		CHECK(mtx.getAxis(sf::Matrix44::axisZ) == sf::Vector3D(10, 28, -25));
		CHECK(mtx.getAxis(sf::Matrix44::axisT) == sf::Vector3D(0, 0, 0));

		CHECK(sf::Quaternion(2, 3, 4, 5).log() == sf::Quaternion(1.99449202328214, 0.549487105217117, 0.732649473622823, 0.915811842028528));
		CHECK(sf::Quaternion(2, 3, 4, 5).exp() == sf::Quaternion(5.21185524897636, 2.22221706442803, 2.96295608590404, 3.70369510738005));
		// Check if the log() and consequent exp() result in the original quaternion.
		CHECK(sf::Quaternion(1.0, 2.0, 3.0, 4.0).log().exp() == sf::Quaternion(1.0, 2.0, 3.0, 4.0));
		CHECK((sf::Quaternion(1, 2, 3, 4).log() * sf::Quaternion(2, 0, 0, 0)).exp().isEqual(sf::Quaternion(-28, 4, 6, 8), sf::Quaternion::tolerance * 10.0));
		CHECK((sf::Quaternion(1, 2, 3, 4).log() * 2.0).exp().isEqual(sf::Quaternion(-28, 4, 6, 8), sf::Quaternion::tolerance * 10.0));
	}

	SECTION("Transformations")
	{
		// Check the conversion of a quaternion to a matrix.
		CHECK((sf::Quaternion(1, 2, 3, 4).toMatrix()) == sf::Matrix44(-0.666666666666666, 0.133333333333333, 0.733333333333333, 0.0, 0.666666666666667, -0.333333333333333, 0.666666666666667, 0.0, 0.333333333333333, 0.933333333333333, 0.133333333333334, 0.0, 0.0, 0.0, 0.0, 1.0));
		CHECK(sf::Quaternion(std::sqrt(2.0) / 2, std::sqrt(2.0) / 2, 0, 0).toMatrix() == sf::Matrix44(1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1));
		// Transform a 3D-vector using the quaternion transform function.
		CHECK(sf::Quaternion(1, 2, 3, 4).normalize().transform(sf::Vector3D(1, 2, 3)) == sf::Vector3D(1.8, 2, 2.6));
		// When using an intermediate matrix the result must be the same.
		CHECK((sf::Quaternion(1, 2, 3, 4).toMatrix() * sf::Vector3D(1, 2, 3)) == sf::Vector3D(1.8, 2, 2.6));
		// Logarithm and exponential function must result in the original sf::Quaternion.
		CHECK(sf::Quaternion(1.0, 2.0, 3.0, 4.0).log().exp() == sf::Quaternion(1.0, 2.0, 3.0, 4.0));
		// Log interpolation.
		auto q1 = sf::Quaternion(1.0, 2.0, 3.0, 4.0).normalize();
		auto q2 = sf::Quaternion(4.0, 3.0, 2.0, 1.0).normalize();
		CHECK(q1 == sf::Quaternion(0.182574185835055, 0.365148371670111, 0.547722557505166, 0.730296743340221));
		CHECK(q2 == sf::Quaternion(0.730296743340221, 0.547722557505166, 0.365148371670111, 0.182574185835055));
		auto q_diff = q2 * q1.conjugate();
		CHECK(q_diff == sf::Quaternion(0.666666666666667, -0.333333333333333, 0, -0.666666666666667));
		auto log_q_diff = q_diff.log();
		CHECK(log_q_diff == sf::Quaternion(0, -0.376137344227054, 0, -0.752274688454107));
		double t = 0.5;
		auto log_q_scaled = t * log_q_diff;
		CHECK(log_q_scaled == sf::Quaternion(0, -0.376137344227054 * 0.5, 0, -0.752274688454107 * 0.5));
		CHECK(log_q_scaled.exp() == sf::Quaternion(0.912870929175277, -0.182574185835055, 0, -0.365148371670111));
		CHECK(log_q_scaled.exp() * q1 == sf::Quaternion(0.5, 0.5, 0.5, 0.5));
		CHECK(q1.interpolateLogarithmic(q2, 0.5) == sf::Quaternion(0.5, 0.5, 0.5, 0.5));
		// Check when factor is 0.0 the result is equal to q1 only for normalized quaternions.
		CHECK(q1.interpolateLogarithmic(q2, 0.0) == q1);
		// Check when factor is 0.0 the result is equal to q2 only for normalized quaternions.
		CHECK(q1.interpolateLogarithmic(q2, 1.0) == q2);
		// Check the whole interpolation of non-normalized quaternions.
		CHECK(sf::Quaternion(1.0, 2.0, 3.0, 4.0).interpolateLogarithmic(sf::Quaternion(4.0, 3.0, 2.0, 1.0), 0.5) == sf::Quaternion(15, 15, 15, 15));
		CHECK(sf::Quaternion(4.0, 3.0, 2.0, 1.0).interpolateLogarithmic(sf::Quaternion(1.0, 2.0, 3.0, 4.0), 0.5) == sf::Quaternion(15, 15, 15, 15));
		sf::Matrix44::value_type mtx[4][4];
		sf::Quaternion(1, 2, 3, 4).toMatrix().copyTo(mtx);
		CHECK(sf::Quaternion().fromMatrix(mtx) == sf::Quaternion(1, 2, 3, 4).normalize());
		CHECK(sf::Quaternion(1, 2, 3, 4).toMatrix().quaternion() == sf::Quaternion(1, 2, 3, 4).normalize());
	}
}
