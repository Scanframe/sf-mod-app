#include <iostream>
#include <misc/gen/ScriptEngine.h>
#include <test/catch.h>

TEST_CASE("sf::ScriptEngine", "[con][calc]")
{
	using Catch::Approx;

	SECTION("sf::calculator", "Calculator functions")
	{
		// Floating point value.
		CHECK(sf::calculator("1.23456789", 0.0) == Approx(1.23456789).margin(std::numeric_limits<double>::denorm_min()));
		// Floating point value in scientific notation.
		CHECK(sf::calculator("1.23e-3", 0.0) == Approx(0.00123).margin(std::numeric_limits<double>::denorm_min()));
		// Hex integer value.
		CHECK(sf::calculator("0xFFFF", 0.0) == Approx(65535.0).margin(std::numeric_limits<double>::denorm_min()));
		// Floating point multiplication by integer.
		CHECK(sf::calculator("1.2 * 3", 0.0) == Approx(3.6).margin(std::numeric_limits<double>::denorm_min()));
		// Floating point multiplication by floating point.
		CHECK(sf::calculator("1.2 * 3.0", 0.0) == Approx(3.6).margin(std::numeric_limits<double>::denorm_min()));
		// Floating point division by integer.
		CHECK(sf::calculator("11. / 2", 0.0) == Approx(5.5).margin(std::numeric_limits<double>::denorm_min()));
		// Integer division by integer.
		CHECK(sf::calculator("11 / 2", 0.0) == Approx(5.0).margin(std::numeric_limits<double>::denorm_min()));
		// Square root of the sinus of  1/4 of PI (45 deg) is 0.5 exact.
		CHECK(sf::calculator("pow(sin(PI/4), 2.0)", 0.0) == Approx(0.5).margin(std::numeric_limits<double>::denorm_min()));
		// Modulus function.
		CHECK(sf::calculator("mod(23,5)", 0.0) == Approx(3).margin(std::numeric_limits<double>::denorm_min()));
		// Modulus operator using integer.
		CHECK(sf::calculator("23 % 5", 0.0) == Approx(3).margin(std::numeric_limits<double>::denorm_min()));
		// Modulus operator using floating point.
		CHECK(sf::calculator("23 % 5.5", 0.0) == Approx(1).margin(std::numeric_limits<double>::denorm_min()));
		// Order of operand processing.
		CHECK(sf::calculator("1.0 + 2.4 / 2", 0.0) == Approx(2.2).margin(std::numeric_limits<double>::denorm_min()));
		CHECK(sf::calculator("1.0 + 2.4 * 2", 0.0) == Approx(5.8).margin(std::numeric_limits<double>::denorm_min()));
		// String conversion
		CHECK(sf::calculator("\"Brown-\" + \"Bear\"", sf::Value(sf::Value::vitString)) == sf::Value("Brown-Bear"));
		CHECK(sf::calculator("\"Brown-\" + 123", sf::Value(sf::Value::vitString)) == sf::Value("Brown-123"));
	}
}
