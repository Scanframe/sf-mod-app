#include <catch2/catch.hpp>
#include <iostream>
#include <cstring>
#include <sstream>
#include <misc/value.h>

namespace
{
/**
 * Helper class getting streamed output in to a string for comparison of type and value.
 */
struct
{
	std::string operator()(const sf::TValue& v)
	{
		return dynamic_cast<std::ostringstream&>(std::ostringstream() << v).str();
	}
} Helper;

}

TEST_CASE("sf::Value", "[main]")
{

	SECTION("Constructors")
	{
		// Floating point without decimals.
		REQUIRE_THAT(Helper(sf::TValue(12345.0)), Catch::Matchers::Equals("(FLOAT,\"12345\")"));
		// Default
		REQUIRE_THAT(Helper(sf::TValue()), Catch::Matchers::Equals("(UNDEF,\"\")"));
		// Unsigned
		REQUIRE_THAT(Helper(sf::TValue(unsigned(1234567890))), Catch::Matchers::Equals("(INTEGER,\"1234567890\")"));
		// Integer
		REQUIRE_THAT(Helper(sf::TValue(int(1234567890))), Catch::Matchers::Equals("(INTEGER,\"1234567890\")"));
		// Long Integer
		REQUIRE_THAT(Helper(sf::TValue(long(1234567890))), Catch::Matchers::Equals("(INTEGER,\"1234567890\")"));
		// Floating point
		REQUIRE_THAT(Helper(sf::TValue(12345.67890)), Catch::Matchers::Equals("(FLOAT,\"12345.6789\")"));
		// Boolean
		REQUIRE_THAT(Helper(sf::TValue(true)), Catch::Matchers::Equals("(INTEGER,\"1\")"));
		// Const character string type.
		REQUIRE_THAT(Helper(sf::TValue("Just a text used as binary data.")),
			Catch::Matchers::Equals("(STRING,\"Just a text used as binary data.\")"));
		// rom std::string to string type.
		REQUIRE_THAT(Helper(sf::TValue(std::string("Just a text used as binary data."))),
			Catch::Matchers::Equals("(STRING,\"Just a text used as binary data.\")"));
		// Raw data to binary type.
		const char* raw = "Just a text used as binary data.";
		REQUIRE_THAT(Helper(sf::TValue(raw, strlen(raw))),
			Catch::Matchers::Equals("(BINARY,\"4a7573742061207465787420757365642061732062696e61727920646174612e\")"));
	}

	SECTION("Multiply Float")
	{
		REQUIRE_THAT(Helper(sf::TValue(123.678) * sf::TValue(2.2)), Catch::Matchers::Equals("(FLOAT,\"272.0916\")"));
	}

	SECTION("Multiply Float * Integer")
	{
		REQUIRE_THAT(Helper(sf::TValue(1.23) * sf::TValue(2)), Catch::Matchers::Equals("(FLOAT,\"2.46\")"));
		REQUIRE_THAT(Helper(sf::TValue(-1.23) * sf::TValue(2)), Catch::Matchers::Equals("(FLOAT,\"-2.46\")"));
	}

	SECTION("Multiply Integer * Float")
	{
		// Automatic rounding multiplier to nearest integer and result is an integer.
		REQUIRE_THAT(Helper(sf::TValue(2) * sf::TValue(1.5)), Catch::Matchers::Equals("(INTEGER,\"4\")"));
		REQUIRE_THAT(Helper(sf::TValue(2) * sf::TValue(-1.5)), Catch::Matchers::Equals("(INTEGER,\"-4\")"));
	}

	SECTION("Division Integer / Float")
	{
		// Automatic rounding multiplier to nearest integer and result is an integer.
		REQUIRE_THAT(Helper(sf::TValue(2) / sf::TValue(1.5)), Catch::Matchers::Equals("(INTEGER,\"1\")"));
		REQUIRE_THAT(Helper(sf::TValue(2) / sf::TValue(-1.5)), Catch::Matchers::Equals("(INTEGER,\"-1\")"));
	}

	SECTION("Division Float / Integer")
	{
		REQUIRE_THAT(Helper(sf::TValue(2.468) / sf::TValue(2)), Catch::Matchers::Equals("(FLOAT,\"1.234\")"));
		REQUIRE_THAT(Helper(sf::TValue(-2.468) / sf::TValue(2)), Catch::Matchers::Equals("(FLOAT,\"-1.234\")"));
	}

	SECTION("Reference Type")
	{
		// Original value.
		sf::TValue v(123);
		// Reference value.
		sf::TValue r(&v);
		// Reference the original by passing a pointer of an instance to the constructor.
		REQUIRE_THAT(Helper(r), Catch::Matchers::Equals("(INTEGER,\"123\")"));
		// Change original.
		v.SetType(sf::TValue::vitFLOAT);
		// Check reference content.
		REQUIRE_THAT(Helper(r), Catch::Matchers::Equals("(FLOAT,\"123\")"));
		// Undo the reference by setting.
		r.Set(345);
		REQUIRE_THAT(Helper(r), Catch::Matchers::Equals("(INTEGER,\"345\")"));
		// The original has not been changed.
		REQUIRE_THAT(Helper(v), Catch::Matchers::Equals("(FLOAT,\"123\")"));
	}

}
