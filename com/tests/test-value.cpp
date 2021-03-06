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

TEST_CASE("sf::Value", "[value]")
{
	using Catch::Equals;

	SECTION("Constructors")
	{
		// Smallest number.
		CHECK_THAT(Helper(sf::TValue(std::numeric_limits<double>::max())), Equals("(FLOAT,\"1.79769313486232e+308\")"));
		// Floating point
		CHECK_THAT(Helper(sf::TValue(12345.6789)), Equals("(FLOAT,\"12345.6789\")"));
		// Floating point without decimals.
		CHECK_THAT(Helper(sf::TValue(12345.0)), Equals("(FLOAT,\"12345\")"));
		// Default
		CHECK_THAT(Helper(sf::TValue()), Equals("(UNDEF,\"\")"));
		// Unsigned
		CHECK_THAT(Helper(sf::TValue(unsigned(1234567890))), Equals("(INTEGER,\"1234567890\")"));
		// Integer
		CHECK_THAT(Helper(sf::TValue(int(1234567890))), Equals("(INTEGER,\"1234567890\")"));
		// Long Integer
		CHECK_THAT(Helper(sf::TValue(long(1234567890))), Equals("(INTEGER,\"1234567890\")"));
		// Boolean
		CHECK_THAT(Helper(sf::TValue(true)), Equals("(INTEGER,\"1\")"));
		// Const character string type.
		CHECK_THAT(Helper(sf::TValue("Just a text used as binary data.")),
			Equals("(STRING,\"Just a text used as binary data.\")"));
		// rom std::string to string type.
		CHECK_THAT(Helper(sf::TValue(std::string("Just a text used as binary data."))),
			Equals("(STRING,\"Just a text used as binary data.\")"));
		// Raw data to binary type.
		const char* raw = "Just a text used as binary data.";
		CHECK_THAT(Helper(sf::TValue(raw, strlen(raw))),
			Equals("(BINARY,\"4a7573742061207465787420757365642061732062696e61727920646174612e\")"));
	}

	SECTION("General")
	{
		// Is zero test function.
		CHECK(sf::TValue(0).IsZero() == true);
		CHECK(sf::TValue("0").IsZero() == false);
		// Validity function testing type on vitINVALID type.
		CHECK(!sf::TValue(sf::TValue::GetType("INTeger"), "abc", 4).IsValid());
		// Number types test function.
		CHECK(sf::TValue(4).IsNumber());
		CHECK(sf::TValue(4.4).IsNumber());
		CHECK(!sf::TValue("123").IsNumber());
		// Round function rounding in multiples of given value.
		// Float rounding.
		CHECK_THAT(Helper(sf::TValue(123.999).Round(sf::TValue(0.01))), Equals("(FLOAT,\"124\")"));
		// Integers rounding.
		CHECK_THAT(Helper(sf::TValue(13).Round(sf::TValue(5))), Equals("(INTEGER,\"15\")"));
		CHECK_THAT(Helper(sf::TValue(12).Round(sf::TValue(5))), Equals("(INTEGER,\"10\")"));
		CHECK_THAT(Helper(sf::TValue(12).Round(sf::TValue(5.4))), Equals("(INTEGER,\"10\")"));
		CHECK_THAT(Helper(sf::TValue(13).Round(sf::TValue(5.6))), Equals("(INTEGER,\"12\")"));
	}

	SECTION("Operators")
	{
		// Bool operator on integer type.
		CHECK(!!sf::TValue(0));
		CHECK(!sf::TValue(1));
		CHECK(!sf::TValue(true));
		// Bool operator on an empty string.
		CHECK(!!sf::TValue(""));
		// Bool operator on smallest value float value.
		CHECK(!!sf::TValue(std::numeric_limits<sf::TValue::flt_type>::denorm_min()));
		// Check if the bool operator on a undefined type.
		CHECK(!!sf::TValue());
	}

	SECTION("Reference")
	{
		// Original value.
		sf::TValue v(123);
		// Reference value.
		sf::TValue r(&v);
		// Reference the original by passing a pointer of an instance to the constructor.
		CHECK_THAT(Helper(r), Equals("(INTEGER,\"123\")"));
		// Change original.
		v.SetType(sf::TValue::vitFLOAT);
		// Check reference content.
		CHECK_THAT(Helper(r), Equals("(FLOAT,\"123\")"));
		// Undo the reference by setting.
		CHECK_THAT(Helper(r.Set(345)), Equals("(INTEGER,\"345\")"));
		// The original has not been changed.
		CHECK_THAT(Helper(v), Equals("(FLOAT,\"123\")"));
	}

	SECTION("Math Multiply")
	{
		// Multiply Float * Float
		CHECK_THAT(Helper(sf::TValue(123.678).Mul(sf::TValue(2.2))), Equals("(FLOAT,\"272.0916\")"));
		// Multiply Float * Integer
		CHECK_THAT(Helper(sf::TValue(123.4).Mul(sf::TValue(2))), Equals("(FLOAT,\"246.8\")"));
		// Multiply Float * String, string is converted to float first.
		CHECK_THAT(Helper(sf::TValue(123.456).Mul(sf::TValue("2.8"))), Equals("(FLOAT,\"345.6768\")"));

		// Multiply Integer * Integer
		CHECK_THAT(Helper(sf::TValue(123).Mul(sf::TValue(3))), Equals("(INTEGER,\"369\")"));
		// Multiply Integer * String, string converted to integer first.
		CHECK_THAT(Helper(sf::TValue(123).Mul(sf::TValue("2.8"))), Equals("(INTEGER,\"246\")"));
		// Multiply Integer * Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::TValue(123).Mul(sf::TValue(2.8))), Equals("(INTEGER,\"369\")"));
		// Multiply Integer * Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::TValue(123).Mul(sf::TValue(2.45))), Equals("(INTEGER,\"246\")"));

		// Multiply String * Integer (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Mul(sf::TValue(2))), Equals("(STRING,\"123\")"));
		// Multiply String * Float (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Mul(sf::TValue(2.8))), Equals("(STRING,\"123\")"));
		// Multiply String * String (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Mul(sf::TValue("2"))), Equals("(STRING,\"123\")"));
	}

	SECTION("Math Divide")
	{
		// Float / Float(zero) does not change the value.
		CHECK_THAT(Helper(sf::TValue(246.8).Div(sf::TValue(0.0))), Equals("(FLOAT,\"246.8\")"));
		// Float / Integer(zero) does not change the value.
		CHECK_THAT(Helper(sf::TValue(246.8).Div(sf::TValue(0))), Equals("(FLOAT,\"246.8\")"));
		// Float / Float
		CHECK_THAT(Helper(sf::TValue(246.8).Div(sf::TValue(2.0))), Equals("(FLOAT,\"123.4\")"));
		// Float / Integer
		CHECK_THAT(Helper(sf::TValue(246.8).Div(sf::TValue(2))), Equals("(FLOAT,\"123.4\")"));
		// Float / String, string is converted to float first.
		CHECK_THAT(Helper(sf::TValue(246.8).Div(sf::TValue("2.0"))), Equals("(FLOAT,\"123.4\")"));

		// Integer / Integer
		CHECK_THAT(Helper(sf::TValue(246).Div(sf::TValue(2))), Equals("(INTEGER,\"123\")"));
		// Integer / Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::TValue(246).Div(sf::TValue(2.0))), Equals("(INTEGER,\"123\")"));
		// Integer / String, string converted to integer first.
		CHECK_THAT(Helper(sf::TValue(246).Div(sf::TValue("2.8"))), Equals("(INTEGER,\"123\")"));

		// String / Integer (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Div(sf::TValue(2))), Equals("(STRING,\"123\")"));
		// String / Float (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Div(sf::TValue(2.8))), Equals("(STRING,\"123\")"));
		// String / String (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Div(sf::TValue("2"))), Equals("(STRING,\"123\")"));
	}

	SECTION("Math Add")
	{
		// Float + Float(zero) does not change the value.
		CHECK_THAT(Helper(sf::TValue(246.8).Add(sf::TValue(0.0))), Equals("(FLOAT,\"246.8\")"));
		// Float + Integer(zero) does not change the value.
		CHECK_THAT(Helper(sf::TValue(246.8).Add(sf::TValue(0))), Equals("(FLOAT,\"246.8\")"));
		// Float + Float
		CHECK_THAT(Helper(sf::TValue(246.8).Add(sf::TValue(12.1))), Equals("(FLOAT,\"258.9\")"));
		// Float + Integer
		CHECK_THAT(Helper(sf::TValue(246.8).Add(sf::TValue(2))), Equals("(FLOAT,\"248.8\")"));
		// Float + String, string is converted to float first.
		CHECK_THAT(Helper(sf::TValue(246.8).Add(sf::TValue("3.2"))), Equals("(FLOAT,\"250\")"));

		// Integer + Integer
		CHECK_THAT(Helper(sf::TValue(246).Add(sf::TValue(2))), Equals("(INTEGER,\"248\")"));
		// Integer + Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::TValue(246).Add(sf::TValue(2.8))), Equals("(INTEGER,\"249\")"));
		// Integer + String, string converted to integer first.
		CHECK_THAT(Helper(sf::TValue(246).Add(sf::TValue("-2.8"))), Equals("(INTEGER,\"244\")"));

		// String + Integer (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Add(sf::TValue(2))), Equals("(STRING,\"1232\")"));
		// String + Float (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Add(sf::TValue(2.8))), Equals("(STRING,\"1232.8\")"));
		// String + String (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Add(sf::TValue("2"))), Equals("(STRING,\"1232\")"));
	}

	SECTION("Math Subtract")
	{
		// Float - Float
		CHECK_THAT(Helper(sf::TValue(246.8).Sub(sf::TValue(12.1))), Equals("(FLOAT,\"234.7\")"));
		// Float - Integer
		CHECK_THAT(Helper(sf::TValue(246.8).Sub(sf::TValue(-2))), Equals("(FLOAT,\"248.8\")"));
		// Float - String, string is converted to float first.
		CHECK_THAT(Helper(sf::TValue(246.8).Sub(sf::TValue("-3.2"))), Equals("(FLOAT,\"250\")"));

		// Integer - Integer
		CHECK_THAT(Helper(sf::TValue(246).Sub(sf::TValue(-2))), Equals("(INTEGER,\"248\")"));
		// Integer - Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::TValue(246).Sub(sf::TValue(2.8))), Equals("(INTEGER,\"243\")"));
		// Integer - String, string converted to integer first.
		CHECK_THAT(Helper(sf::TValue(246).Sub(sf::TValue("-2.8"))), Equals("(INTEGER,\"248\")"));

		// String - Integer (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Sub(sf::TValue(2))), Equals("(STRING,\"123\")"));
		// String - Float (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Sub(sf::TValue(2.8))), Equals("(STRING,\"123\")"));
		// String - String (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Sub(sf::TValue("2"))), Equals("(STRING,\"123\")"));
	}

	SECTION("Math Modulo")
	{
		// Float % Float
		CHECK_THAT(Helper(sf::TValue(246.8).Mod(sf::TValue(12.1)).Round(sf::TValue(0.001))), Equals("(FLOAT,\"4.8\")"));
		CHECK_THAT(Helper(sf::TValue(18.0).Mod(sf::TValue(-5.0)).Round(sf::TValue(0.001))), Equals("(FLOAT,\"-2\")"));
		// Float % Integer
		CHECK_THAT(Helper(sf::TValue(246.8).Mod(sf::TValue(-2)).Round(sf::TValue(0.001))), Equals("(FLOAT,\"-1.2\")"));
		// Float % String, string is converted to float first.
		CHECK_THAT(Helper(sf::TValue(246.8).Mod(sf::TValue("-3.2")).Round(sf::TValue(0.001))), Equals("(FLOAT,\"-2.8\")"));

		// Integer % Integer
		CHECK_THAT(Helper(sf::TValue(13).Mod(sf::TValue(2))), Equals("(INTEGER,\"1\")"));
		// Integer % Float, float is converted to Integer first by rounding.
		CHECK_THAT(Helper(sf::TValue(13).Mod(sf::TValue(5.4))), Equals("(INTEGER,\"3\")"));
		// Integer % String, string converted to integer first.
		CHECK_THAT(Helper(sf::TValue(18).Mod(sf::TValue(-5))), Equals("(INTEGER,\"-2\")"));

		// String % Integer (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Mod(sf::TValue(2))), Equals("(STRING,\"123\")"));
		// String % Float (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Mod(sf::TValue(2.8))), Equals("(STRING,\"123\")"));
		// String % String (has no effect)
		CHECK_THAT(Helper(sf::TValue("123").Mod(sf::TValue("2"))), Equals("(STRING,\"123\")"));
	}

	SECTION("Math Compare")
	{
		// Float == Float
		CHECK(sf::TValue(12.0).Compare(sf::TValue(12.0)) == 0);
		CHECK(sf::TValue(12.0).Compare(sf::TValue(11.0)) > 0);
		CHECK(sf::TValue(12.0).Compare(sf::TValue(13.0)) < 0);
		// Float == Integer
		CHECK(sf::TValue(12.0).Compare(sf::TValue(12)) == 0);
		CHECK(sf::TValue(12.0).Compare(sf::TValue(11)) > 0);
		CHECK(sf::TValue(12.0).Compare(sf::TValue(13)) < 0);
		// Float == String, string is converted to float first.
		CHECK(sf::TValue(12.0).Compare(sf::TValue("12")) == 0);
		CHECK(sf::TValue(12.0).Compare(sf::TValue("11")) > 0);
		CHECK(sf::TValue(12.0).Compare(sf::TValue("13")) < 0);
		// Integer == Integer
		CHECK(sf::TValue(12).Compare(sf::TValue(12)) == 0);
		CHECK(sf::TValue(12).Compare(sf::TValue(11)) > 0);
		CHECK(sf::TValue(12).Compare(sf::TValue(13)) < 0);
		// String == Integer
		CHECK(sf::TValue("12").Compare(sf::TValue(12)) == 0);
		CHECK(sf::TValue("12").Compare(sf::TValue(11)) > 0);
		CHECK(sf::TValue("12").Compare(sf::TValue(13)) < 0);
		// String == Float
		CHECK(sf::TValue("12").Compare(sf::TValue(12.0)) == 0);
		CHECK(sf::TValue("12").Compare(sf::TValue(11.0)) > 0);
		CHECK(sf::TValue("12").Compare(sf::TValue(13.0)) < 0);
		// String == String
		CHECK(sf::TValue("12").Compare(sf::TValue("12")) == 0);
		CHECK(sf::TValue("12").Compare(sf::TValue("12.0")) < 0);
		CHECK(sf::TValue("12").Compare(sf::TValue("11.0")) > 0);
		CHECK(sf::TValue("12").Compare(sf::TValue("13.0")) < 0);
	}

	SECTION("Binary Data")
	{
		//CHECK(!sf::TValue(sf::TValue::vitBINARY, nullptr, 0).IsZero());
	}
}
