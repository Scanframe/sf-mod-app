#include <cstring>
#include <iostream>
#include <misc/gen/Value.h>
#include <test/catch.h>

namespace
{
/**
 * Helper class getting streamed output in to a string for comparison of type and value.
 */
struct
{
		std::string operator()(const sf::Value& v)
		{
			std::ostringstream os;
			return dynamic_cast<std::ostringstream&>(os << v).str();
		}
} Helper;

}// namespace

TEST_CASE("sf::Value", "[con][generic][value]")
{
	using Catch::Matchers::Equals;

	SECTION("Constructors")
	{
		// Smallest number.
		CHECK_THAT(Helper(sf::Value(std::numeric_limits<double>::max())), Equals("(FLOAT,\"1.79769313486232e+308\")"));
		// Floating point
		CHECK_THAT(Helper(sf::Value(12345.6789)), Equals("(FLOAT,\"12345.6789\")"));
		// Floating point without decimals.
		CHECK_THAT(Helper(sf::Value(12345.0)), Equals("(FLOAT,\"12345\")"));
		// Default
		CHECK_THAT(Helper(sf::Value()), Equals("(UNDEF,\"\")"));
		// Unsigned
		CHECK_THAT(Helper(sf::Value(unsigned(1234567890))), Equals("(INTEGER,\"1234567890\")"));
		// Integer
		CHECK_THAT(Helper(sf::Value(int(1234567890))), Equals("(INTEGER,\"1234567890\")"));
		// Long Integer
		CHECK_THAT(Helper(sf::Value(sf::Value::int_type(1234567890))), Equals("(INTEGER,\"1234567890\")"));
		// Boolean
		CHECK_THAT(Helper(sf::Value(true)), Equals("(INTEGER,\"1\")"));
		// Const's character string type.
		CHECK_THAT(Helper(sf::Value("Just a text used as binary data.")), Equals("(STRING,\"Just a text used as binary data.\")"));
		// rom std::string to string type.
		CHECK_THAT(Helper(sf::Value(std::string("Just a text used as binary data."))), Equals("(STRING,\"Just a text used as binary data.\")"));
		// Raw data to binary type.
		const char* raw = "Just a text used as binary data.";
		CHECK_THAT(Helper(sf::Value(raw, strlen(raw))), Equals("(BINARY,\"4a7573742061207465787420757365642061732062696e61727920646174612e\")"));
	}

	SECTION("General")
	{
		// Is zero test function.
		CHECK(sf::Value(0).isZero());
		CHECK_FALSE(sf::Value("0").isZero());
		// Validity function testing type on vitInvalid type by getting non-existing type string.
		CHECK_FALSE(sf::Value(sf::Value::getType("INTeger"), "abc", 4).isValid());
		CHECK(sf::Value::getType("FLOAT") == sf::Value::vitFloat);
		CHECK(sf::Value::getType("CUSTOM") == sf::Value::vitCustom);
		CHECK(sf::Value::getType("bogus") == sf::Value::vitInvalid);
		CHECK(sf::Value("Brown-Bear").getString() == "Brown-Bear");
		// Number types test function.
		CHECK(sf::Value(4).isNumber());
		CHECK(sf::Value(4.4).isNumber());
		CHECK(!sf::Value("123").isNumber());
		// Round function rounding in multiples of given value.
		// Float rounding.
		CHECK_THAT(Helper(sf::Value(123.999).round(sf::Value(0.01))), Equals("(FLOAT,\"124\")"));
		// Integers rounding.
		CHECK_THAT(Helper(sf::Value(13).round(sf::Value(5))), Equals("(INTEGER,\"15\")"));
		CHECK_THAT(Helper(sf::Value(12).round(sf::Value(5))), Equals("(INTEGER,\"10\")"));
		CHECK_THAT(Helper(sf::Value(12).round(sf::Value(5.4))), Equals("(INTEGER,\"10\")"));
		CHECK_THAT(Helper(sf::Value(13).round(sf::Value(5.6))), Equals("(INTEGER,\"12\")"));
	}

	SECTION("Operators")
	{
		// Bool operator on integer type.
		CHECK(!!sf::Value(0));
		CHECK(!sf::Value(1));
		CHECK(!sf::Value(true));
		// Bool operator on an empty string.
		CHECK(!!sf::Value(""));
		// Bool operator on smallest value float value.
		CHECK(!!sf::Value(std::numeric_limits<sf::Value::flt_type>::denorm_min()));
		// Check if the bool operator on an undefined type.
		CHECK(!!sf::Value());
	}

	SECTION("Reference")
	{
		// Original value.
		sf::Value v(123);
		// Reference value.
		sf::Value r(&v);
		// Reference the original by passing a pointer of an instance to the constructor.
		CHECK_THAT(Helper(r), Equals("(INTEGER,\"123\")"));
		// Change original.
		v.setType(sf::Value::vitFloat);
		// Check reference content.
		CHECK_THAT(Helper(r), Equals("(FLOAT,\"123\")"));
		// Undo the reference by setting.
		CHECK_THAT(Helper(r.set(345)), Equals("(INTEGER,\"345\")"));
		// The original has not been changed.
		CHECK_THAT(Helper(v), Equals("(FLOAT,\"123\")"));
	}

	SECTION("Math Multiply")
	{
		// Multiply Float * Float
		CHECK_THAT(Helper(sf::Value(123.678).mul(sf::Value(2.2))), Equals("(FLOAT,\"272.0916\")"));
		// Multiply Float * Integer
		CHECK_THAT(Helper(sf::Value(123.4).mul(sf::Value(2))), Equals("(FLOAT,\"246.8\")"));
		// Multiply Float * String, string is converted to float first.
		CHECK_THAT(Helper(sf::Value(123.456).mul(sf::Value("2.8"))), Equals("(FLOAT,\"345.6768\")"));

		// Multiply Integer * Integer
		CHECK_THAT(Helper(sf::Value(123).mul(sf::Value(3))), Equals("(INTEGER,\"369\")"));
		// Multiply Integer * String, string converted to integer first.
		CHECK_THAT(Helper(sf::Value(123).mul(sf::Value("2.8"))), Equals("(INTEGER,\"246\")"));
		// Multiply Integer * Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::Value(123).mul(sf::Value(2.8))), Equals("(INTEGER,\"369\")"));
		// Multiply Integer * Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::Value(123).mul(sf::Value(2.45))), Equals("(INTEGER,\"246\")"));

		// Multiply String * Integer (has no effect)
		CHECK_THAT(Helper(sf::Value("123").mul(sf::Value(2))), Equals("(STRING,\"123\")"));
		// Multiply String * Float (has no effect)
		CHECK_THAT(Helper(sf::Value("123").mul(sf::Value(2.8))), Equals("(STRING,\"123\")"));
		// Multiply String * String (has no effect)
		CHECK_THAT(Helper(sf::Value("123").mul(sf::Value("2"))), Equals("(STRING,\"123\")"));
	}

	SECTION("Math Divide")
	{
		// Float / Float(zero) does not change the value.
		CHECK_THAT(Helper(sf::Value(246.8).div(sf::Value(0.0))), Equals("(FLOAT,\"246.8\")"));
		// Float / Integer(zero) does not change the value.
		CHECK_THAT(Helper(sf::Value(246.8).div(sf::Value(0))), Equals("(FLOAT,\"246.8\")"));
		// Float / Float
		CHECK_THAT(Helper(sf::Value(246.8).div(sf::Value(2.0))), Equals("(FLOAT,\"123.4\")"));
		// Float / Integer
		CHECK_THAT(Helper(sf::Value(246.8).div(sf::Value(2))), Equals("(FLOAT,\"123.4\")"));
		// Float / String, string is converted to float first.
		CHECK_THAT(Helper(sf::Value(246.8).div(sf::Value("2.0"))), Equals("(FLOAT,\"123.4\")"));

		// Integer / Integer
		CHECK_THAT(Helper(sf::Value(246).div(sf::Value(2))), Equals("(INTEGER,\"123\")"));
		// Integer / Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::Value(246).div(sf::Value(2.0))), Equals("(INTEGER,\"123\")"));
		// Integer / String, string converted to integer first.
		CHECK_THAT(Helper(sf::Value(246).div(sf::Value("2.8"))), Equals("(INTEGER,\"123\")"));

		// String / Integer (has no effect)
		CHECK_THAT(Helper(sf::Value("123").div(sf::Value(2))), Equals("(STRING,\"123\")"));
		// String / Float (has no effect)
		CHECK_THAT(Helper(sf::Value("123").div(sf::Value(2.8))), Equals("(STRING,\"123\")"));
		// String / String (has no effect)
		CHECK_THAT(Helper(sf::Value("123").div(sf::Value("2"))), Equals("(STRING,\"123\")"));
	}

	SECTION("Math Add")
	{
		// Float + Float(zero) does not change the value.
		CHECK_THAT(Helper(sf::Value(246.8).add(sf::Value(0.0))), Equals("(FLOAT,\"246.8\")"));
		// Float + Integer(zero) does not change the value.
		CHECK_THAT(Helper(sf::Value(246.8).add(sf::Value(0))), Equals("(FLOAT,\"246.8\")"));
		// Float + Float
		CHECK_THAT(Helper(sf::Value(246.8).add(sf::Value(12.1))), Equals("(FLOAT,\"258.9\")"));
		// Float + Integer
		CHECK_THAT(Helper(sf::Value(246.8).add(sf::Value(2))), Equals("(FLOAT,\"248.8\")"));
		// Float + String, string is converted to float first.
		CHECK_THAT(Helper(sf::Value(246.8).add(sf::Value("3.2"))), Equals("(FLOAT,\"250\")"));

		// Integer + Integer
		CHECK_THAT(Helper(sf::Value(246).add(sf::Value(2))), Equals("(INTEGER,\"248\")"));
		// Integer + Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::Value(246).add(sf::Value(2.8))), Equals("(INTEGER,\"249\")"));
		// Integer + String, string converted to integer first.
		CHECK_THAT(Helper(sf::Value(246).add(sf::Value("-2.8"))), Equals("(INTEGER,\"244\")"));

		// String + Integer (has no effect)
		CHECK_THAT(Helper(sf::Value("123").add(sf::Value(2))), Equals("(STRING,\"1232\")"));
		// String + Float (has no effect)
		CHECK_THAT(Helper(sf::Value("123").add(sf::Value(2.8))), Equals("(STRING,\"1232.8\")"));
		// String + String (has no effect)
		CHECK_THAT(Helper(sf::Value("123").add(sf::Value("2"))), Equals("(STRING,\"1232\")"));
	}

	SECTION("Math Subtract")
	{
		// Float - Float
		CHECK_THAT(Helper(sf::Value(246.8).sub(sf::Value(12.1))), Equals("(FLOAT,\"234.7\")"));
		// Float - Integer
		CHECK_THAT(Helper(sf::Value(246.8).sub(sf::Value(-2))), Equals("(FLOAT,\"248.8\")"));
		// Float - String, string is converted to float first.
		CHECK_THAT(Helper(sf::Value(246.8).sub(sf::Value("-3.2"))), Equals("(FLOAT,\"250\")"));

		// Integer - Integer
		CHECK_THAT(Helper(sf::Value(246).sub(sf::Value(-2))), Equals("(INTEGER,\"248\")"));
		// Integer - Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::Value(246).sub(sf::Value(2.8))), Equals("(INTEGER,\"243\")"));
		// Integer - String, string converted to integer first.
		CHECK_THAT(Helper(sf::Value(246).sub(sf::Value("-2.8"))), Equals("(INTEGER,\"248\")"));

		// String - Integer (has no effect)
		CHECK_THAT(Helper(sf::Value("123").sub(sf::Value(2))), Equals("(STRING,\"123\")"));
		// String - Float (has no effect)
		CHECK_THAT(Helper(sf::Value("123").sub(sf::Value(2.8))), Equals("(STRING,\"123\")"));
		// String - String (has no effect)
		CHECK_THAT(Helper(sf::Value("123").sub(sf::Value("2"))), Equals("(STRING,\"123\")"));
	}

	SECTION("Math Modulo")
	{
		// Float % Float
		CHECK_THAT(Helper(sf::Value(246.8).mod(sf::Value(12.1)).round(sf::Value(0.001))), Equals("(FLOAT,\"4.8\")"));
		CHECK_THAT(Helper(sf::Value(18.0).mod(sf::Value(-5.0)).round(sf::Value(0.001))), Equals("(FLOAT,\"-2\")"));
		// Float % Integer
		CHECK_THAT(Helper(sf::Value(246.8).mod(sf::Value(-2)).round(sf::Value(0.001))), Equals("(FLOAT,\"-1.2\")"));
		// Float % String, string is converted to float first.
		CHECK_THAT(Helper(sf::Value(246.8).mod(sf::Value("-3.2")).round(sf::Value(0.001))), Equals("(FLOAT,\"-2.8\")"));

		// Integer % Integer
		CHECK_THAT(Helper(sf::Value(13).mod(sf::Value(2))), Equals("(INTEGER,\"1\")"));
		// Integer % Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::Value(13).mod(sf::Value(5.4))), Equals("(INTEGER,\"3\")"));
		// Integer % String, string converted to integer first.
		CHECK_THAT(Helper(sf::Value(18).mod(sf::Value(-5))), Equals("(INTEGER,\"-2\")"));

		// String % Integer (has no effect)
		CHECK_THAT(Helper(sf::Value("123").mod(sf::Value(2))), Equals("(STRING,\"123\")"));
		// String % Float (has no effect)
		CHECK_THAT(Helper(sf::Value("123").mod(sf::Value(2.8))), Equals("(STRING,\"123\")"));
		// String % String (has no effect)
		CHECK_THAT(Helper(sf::Value("123").mod(sf::Value("2"))), Equals("(STRING,\"123\")"));
	}

	SECTION("Math Compare")
	{
		// Float == Float
		CHECK(sf::Value(12.0).compare(sf::Value(12.0)) == 0);
		CHECK(sf::Value(12.0).compare(sf::Value(11.0)) > 0);
		CHECK(sf::Value(12.0).compare(sf::Value(13.0)) < 0);
		// Float == Integer
		CHECK(sf::Value(12.0).compare(sf::Value(12)) == 0);
		CHECK(sf::Value(12.0).compare(sf::Value(11)) > 0);
		CHECK(sf::Value(12.0).compare(sf::Value(13)) < 0);
		// Float == String, string is converted to float first.
		CHECK(sf::Value(12.0).compare(sf::Value("12")) == 0);
		CHECK(sf::Value(12.0).compare(sf::Value("11")) > 0);
		CHECK(sf::Value(12.0).compare(sf::Value("13")) < 0);
		// Integer == Integer
		CHECK(sf::Value(12).compare(sf::Value(12)) == 0);
		CHECK(sf::Value(12).compare(sf::Value(11)) > 0);
		CHECK(sf::Value(12).compare(sf::Value(13)) < 0);
		// String == Integer
		CHECK(sf::Value("12").compare(sf::Value(12)) == 0);
		CHECK(sf::Value("12").compare(sf::Value(11)) > 0);
		CHECK(sf::Value("12").compare(sf::Value(13)) < 0);
		// String == Float
		CHECK(sf::Value("12").compare(sf::Value(12.0)) == 0);
		CHECK(sf::Value("12").compare(sf::Value(11.0)) > 0);
		CHECK(sf::Value("12").compare(sf::Value(13.0)) < 0);
		// String == String
		CHECK(sf::Value("12").compare(sf::Value("12")) == 0);
		CHECK(sf::Value("12").compare(sf::Value("12.0")) < 0);
		CHECK(sf::Value("12").compare(sf::Value("11.0")) > 0);
		CHECK(sf::Value("12").compare(sf::Value("13.0")) < 0);
	}

	SECTION("Binary Data")
	{
		//CHECK(!sf::Value(sf::Value::vitBinary, nullptr, 0).IsZero());
	}
}
