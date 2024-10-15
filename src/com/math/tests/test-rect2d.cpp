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
		std::string operator()(const sf::Rectangle2D& r)
		{
			std::ostringstream os;
			return dynamic_cast<std::ostringstream&>(os << r).str();
		}
} Helper;

}// namespace

TEST_CASE("sf::Rectangle2D", "[con][generic][rectangle]")
{
	using Catch::Matchers::Equals, Catch::Approx;

	SECTION("Constructor / Equal / Assign")
	{
		CHECK(sf::Rectangle2D().isEmpty());
		// Initializing constructor.
		sf::Rectangle2D rc1(1.0, 2.0, 3.0, 4.0);
		// Initializer list constructor.
		sf::Rectangle2D rc2{1.0, 2.0, 3.0, 4.0};
		// Check equal method.
		CHECK(rc1.isEqual(rc2));
		// Dual vector constructor.
		sf::Rectangle2D rc3(sf::Vector2D(1.0, 2.0), sf::Vector2D(3.0, 4.0));
		CHECK(rc1.isEqual(rc3));
		// Check equal operator.
		CHECK(rc1 == rc2);
		// Check unequal operator.
		CHECK_FALSE(rc1 != rc2);
		// Exception on invalid too few of initializer list values.
		CHECK_THROWS_AS((sf::Rectangle2D{1.0, 2.0, 3.0}), std::invalid_argument);
		// Exception on invalid too many of initializer list values.
		CHECK_THROWS_AS((sf::Rectangle2D{1.0, 2.0, 3.0, 4.0, 5.0}), std::invalid_argument);
		// Width and height assignments.
		CHECK(sf::Rectangle2D().assignWidthHeight(1.0, 2.0, 3.0, 4.0) == sf::Rectangle2D(1.0, 2.0, 4.0, 6.0));
		CHECK(sf::Rectangle2D().assignWidthHeight(sf::Vector2D(1.0, 2.0), sf::Vector2D(3.0, 4.0)) == sf::Rectangle2D(1.0, 2.0, 4.0, 6.0));
	}

	SECTION("String/Stream")
	{
		sf::Rectangle2D rc1(1.0, 2.0, 3.0, 4.0);
		CHECK(rc1.toString() == "(1,2,3,4)");
		CHECK(sf::Rectangle2D().fromString("(1,2,3,4)") == sf::Rectangle2D(1.0, 2.0, 3.0, 4.0));
		// Input string stream.
		sf::Rectangle2D rc2;
		std::istringstream("(2,3,4,6)") >> rc2;
		CHECK(rc2 == sf::Rectangle2D(2.0, 3.0, 4.0, 6.0));
		CHECK_THROWS_AS(std::istringstream("(2,3,6)") >> rc2, std::invalid_argument);
		// Output string stream.
		CHECK(Helper(sf::Rectangle2D(2.0, 3.0, 4.0, 6.0)) == "(2,3,4,6)");
	}

	SECTION("Operations")
	{
		// Contains vector.
		CHECK(sf::Rectangle2D(-1.0, -1.0, 4.0, 4.0).contains(sf::Vector2D(0.0, 0.0)));
		CHECK_FALSE(sf::Rectangle2D(-1.0, -1.0, 4.0, 4.0).contains(sf::Vector2D(1.0, 5.0)));
		CHECK_FALSE(sf::Rectangle2D(-1.0, -1.0, 4.0, 4.0).contains(sf::Vector2D(-1.0, -2.0)));
		// Contains rectangle.
		CHECK(sf::Rectangle2D(-1.0, -1.0, 4.0, 4.0).contains(sf::Rectangle2D(0, 0, 3.0, 3.0)));
		CHECK_FALSE(sf::Rectangle2D(-1.0, -1.0, 4.0, 4.0).contains(sf::Rectangle2D(-2.0, -3.0, 3.0, 3.0)));
		CHECK(sf::Rectangle2D(-1.0, -1.0, 4.0, 4.0).touches(sf::Rectangle2D(-5.0, -6.0, -1.0, -1.0)));
		// Combine rectangles.
		CHECK((sf::Rectangle2D(-1.0, -1.0, 4.0, 7.0) | sf::Rectangle2D(-5.0, -6.0, -1.0, -1.0)) == sf::Rectangle2D(-5.0, -6.0, 4.0, 7.0));
		CHECK((sf::Rectangle2D(-2.0, -3.0, 4.0, 7.0) | sf::Rectangle2D(-5.0, -6.0, -1.0, -1.0)) == sf::Rectangle2D(-5.0, -6.0, 4.0, 7.0));
		// Get the overlapping rectangle of the 2 rectangles.
		CHECK((sf::Rectangle2D(0.0, 0.0, 4.0, 7.0) & sf::Rectangle2D(-5.0, -6.0, -1.0, -1.0)).normalized() == sf::Rectangle2D(-1.0, -1.0, 0.0, 0.0));
		// Inflate empty rectangle.
		CHECK((sf::Rectangle2D().inflate(2.0, 1.0)) == sf::Rectangle2D(-2.0, -1.0, 2.0, 1.0));
		// Inflate non-empty rectangle.
		CHECK((sf::Rectangle2D(1.0, 2.0, 3.0, 4.0).inflate(2.0, 1.0)) == sf::Rectangle2D(-1.0, 1.0, 5.0, 5.0));
		CHECK((sf::Rectangle2D(1.0, 2.0, 3.0, 4.0).moveTo(0, 0)) == sf::Rectangle2D(0.0, 0.0, 2.0, 2.0));
		CHECK((sf::Rectangle2D(1.0, 2.0, 3.0, 4.0).offset(-1.0, -2.0)) == sf::Rectangle2D(0.0, 0.0, 2.0, 2.0));
		CHECK((sf::Rectangle2D(1.0, 2.0, 3.0, 4.0) += sf::Vector2D(-1.0, -2.0)) == sf::Rectangle2D(0.0, 0.0, 2.0, 2.0));
		CHECK((sf::Rectangle2D(1.0, 2.0, 3.0, 4.0) -= sf::Vector2D(1.0, 2.0)) == sf::Rectangle2D(0.0, 0.0, 2.0, 2.0));
		CHECK(sf::Rectangle2D(1.0, 2.0, 3.0, 4.0).area() == Approx(4.0).margin(sf::Rectangle2D::tolerance));
	}
}
