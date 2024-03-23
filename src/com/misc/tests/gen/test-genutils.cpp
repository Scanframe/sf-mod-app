#include <test/catch.h>

#include <misc/gen/gen_utils.h>
#include <unistd.h>

namespace sf
{

}

TEST_CASE("sf::General-Utils", "[con][generic][utils]")
{
	using Catch::Matchers::Equals;

#if false
	SECTION("Type Sizes")
	{
		std::clog << "Integer sizes: "
			<< "int(" << sizeof(int) * CHAR_BIT << "), "
			<< "long(" << sizeof(long) * CHAR_BIT << "), "
			<< "long long(" << sizeof(long long) * CHAR_BIT << "), "
			<< std::endl;
	}
#endif

	SECTION("sf::LocaleNumStr", "Number string conversions.")
	{
		int precision = 8;
		precision = sf::clip(precision, 0, std::numeric_limits<double>::digits10);
		auto* strValue = "12345.67890123";
		auto value = sf::stod(strValue);
		CHECK(sf::stringf("%.*lf", precision, value) == strValue);
	}

	SECTION("sf::calculateOffset", "Offset calculation.")
	{
		CHECK(sf::calculateOffset(10000000ll, -1000000000ll, 1000000000ll, 1000ll, true) == 505ll);
		CHECK(sf::calculateOffset(10, -1000, 1000, 1000ll, true) == 505ll);

		CHECK(sf::calculateOffset(10000000ll, -1000000000ll, 1000000000ll, 1000.0f, true) == 505.0f);
		CHECK(sf::calculateOffset(10, -1000, 1000, 1000.0f, true) == 505.0f);

		CHECK(sf::calculateOffset(10e6, -1e9, 1e9, 1000000000000ll, true) == 505000000000ll);
	}

	SECTION("sf::random", "Random range number.")
	{
		sf::TVector<int> results(2);
		for (auto& i: results)
		{
			i = sf::random(-100, 100);
		}
		//std::clog << "Results: " << results << std::endl;
		CHECK(results[0] != results[1]);
	}

	SECTION("sf::numberString", "Numeric value to std string.")
	{
		CHECK(sf::requiredDigits(0.0025, 0.01, 0.1) == 3);
		CHECK(sf::requiredDigits(1, 0, 100) == 3);

		CHECK(sf::ipow(29, 0) == 1);
		CHECK(sf::ipow(27, 1) == 27);
		CHECK(sf::ipow(24l, 5) == 7962624);

		//CHECK(sf::numberString(101, 3) == "101");
		CHECK(sf::numberString(123456.0l, 6) == "+123456");
		CHECK(sf::numberString(123.1, 3) == "+123");
		CHECK(sf::numberString(100.0l, 3) == "+100");
		CHECK(sf::numberString(299.996e1l, 2) == "+3000");
		CHECK(sf::numberString(299.996e4l, 2) == "+3000e+3");
		CHECK(sf::numberString(-0.99999l, 2) == "-1.0");
		CHECK(sf::numberString(1e-2L, 4) == "+10.00e-3");
		CHECK(sf::numberString(123e-1L, 2) == "+12");
		CHECK(sf::numberString(-1234567890e-7L, 6) == "-123.457");
		CHECK(sf::numberString(1234567890e-10L, 6) == "+0.123457");
		CHECK(sf::numberString(-1234567890e-10L, 6) == "-0.123457");
		CHECK(sf::numberString(1234567890e10L, 6) == "+12345.7e+15");
		CHECK(sf::numberString(1234567890e12l, 4) == "+1235e+18");
		CHECK(sf::numberString(0.299996e1l, 4) == "+3.000");
		CHECK(sf::numberString(299.996e1l, 4) == "+3000");
		CHECK(sf::numberString(45.67890e-6L, 2) == "+46e-6");
		CHECK(sf::numberString(782.3e-7L, 4) == "+78.23e-6");
		CHECK(sf::numberString(782.3e-7L, 5) == "+78.230e-6");
		CHECK(sf::numberString(782.3e-7L, 6) == "+78.2300e-6");
		CHECK(sf::numberString(299.996e-7L, 4) == "+30.00e-6");
	}

	SECTION("sf::itoa", "Template function itoa<T>()")
	{
		char buf[sizeof(long long) * CHAR_BIT + 1];
		CHECK_THAT(sf::itoa(-56789, buf, 10), Equals("-56789"));
		CHECK_THAT(sf::itoa(0x7fedba31, buf, 16), Equals("7fedba31"));
		CHECK_THAT(sf::itoa(1234567890, buf, 10), Equals("1234567890"));
		CHECK_THAT(sf::itoa(-1234567890, buf, 10), Equals("-1234567890"));
		CHECK_THAT(sf::itoa(987654321, buf, 10), Equals("987654321"));
		CHECK_THAT(sf::itoa(0x123456789abcdefL, buf, 16), Equals("123456789abcdef"));

		CHECK_THAT(sf::itostr(0x123456789abcdefL, 16), Equals("123456789abcdef"));
	}

	SECTION("sf::round", "Template function round<T>()")
	{
		CHECK(sf::round(13l, 3l) == 12);
		CHECK(sf::round(14l, 3l) == 15);
	}

	SECTION("sf::escape,unescape", "C escaping strings")
	{
		CHECK_THAT(sf::escape("\t\b\r\a\"\'\\"), Equals("\\t\\b\\r\\a\\\"\\'\\\\"));
		// Escape using delimiter character.
		CHECK_THAT(sf::escape("delim{\"inside\"}", '"'), Equals("delim{\\x22inside\\x22}"));
		CHECK_THAT(sf::unescape("\\t\\b\\r\\a\\\"\\'\\\\"), Equals("\t\b\r\a\"\'\\"));
		// Escape using delimiter character.
		CHECK_THAT(sf::unescape("delim{\\x22inside\\x22}"), Equals("delim{\"inside\"}"));
	}

	SECTION("sf::floating-point", "Floating value inquiry template functions")
	{
		// Magnitude of a floating .
		CHECK(sf::magnitude(double(0.001234)) == -2);
		CHECK(sf::magnitude((long double) (-0.001234)) == -2);
		CHECK(sf::magnitude(double(0.123400)) == 0);
		CHECK(sf::magnitude(double(1.23400)) == 1);
		CHECK(sf::magnitude((long double) (123400.0)) == 6);
		CHECK(sf::magnitude(double(-123400.0)) == 6);
		// Digits.
		CHECK(sf::digits(double(12300.0)) == -2);
		CHECK(sf::digits(double(0.0123)) == 4);
		CHECK(sf::digits(double(123.0)) == 0);
		// Precision.
		CHECK(sf::precision(double(12300.0)) == 3);
		CHECK(sf::precision(double(0.0123)) == 3);
		CHECK(sf::precision(double(123.0)) == 3);
	}

	SECTION("sf::null_ref", "null_ref, not_null_ref")
	{
		// Check if not_ref_null reports right value.
		{
			auto& sz(sf::null_ref<size_t>());
			CHECK(sf::not_ref_null(sz));
		}
		// Check if not_ref_null.
		{
			size_t size;
			auto& sz(size);
			CHECK_FALSE(sf::not_ref_null(sz));
		}
	}

	SECTION("sf::getExecutableName", "Executable functions")
	{
#if IS_WIN
		std::string name = "t_sf-misc.exe";
#else
		std::string name = "t_sf-misc.bin";
#endif
		REQUIRE(sf::getExecutableName() == name);
	}
}
