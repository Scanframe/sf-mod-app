#include <catch2/catch.hpp>
#include <misc/gen/genutils.h>

TEST_CASE("sf::General-Utils", "[utils]")
{
	using Catch::Equals;

	SECTION("_type Sizes")
	{
		std::clog << "Integer sizes: "
			<< "int(" << sizeof(int) * CHAR_BIT << "), "
			<< "long(" << sizeof(long) * CHAR_BIT << "), "
			<< "long long(" << sizeof(long long) * CHAR_BIT << "), "
			<< std::endl;
	}

	SECTION("sf::itoa")
	{
		char buf[sizeof(long long) * CHAR_BIT + 1];
		CHECK_THAT(sf::itoa(-56789, buf, 10), Equals("-56789"));
		CHECK_THAT(sf::itoa(0x7fedba31, buf, 16), Equals("7fedba31"));
		CHECK_THAT(sf::itoa(1234567890, buf, 10), Equals("1234567890"));
		CHECK_THAT(sf::itoa(-1234567890, buf, 10), Equals("-1234567890"));
		CHECK_THAT(sf::itoa(987654321, buf, 10), Equals("987654321"));
		CHECK_THAT(sf::itoa(0x123456789abcdefL, buf, 16), Equals("123456789abcdef"));
	}

	SECTION("sf::escape,unescape")
	{
		CHECK_THAT(sf::escape("\t\b\r\a\"\'\\"), Equals("\\t\\b\\r\\a\\\"\\'\\\\"));
		// Escape using delimiter character.
		CHECK_THAT(sf::escape("delim{\"inside\"}", '"'), Equals("delim{\\x22inside\\x22}"));
		CHECK_THAT(sf::unescape("\\t\\b\\r\\a\\\"\\'\\\\"), Equals("\t\b\r\a\"\'\\"));
		// Escape using delimiter character.
		CHECK_THAT(sf::unescape("delim{\\x22inside\\x22}"), Equals("delim{\"inside\"}"));
	}

	SECTION("sf::floating-point")
	{
		// Magnitude of a floating .
		CHECK(sf::magnitude(double(0.001234)) == -2);
		CHECK(sf::magnitude((long double)(-0.001234)) == -2);
		CHECK(sf::magnitude(double(0.123400)) == 0);
		CHECK(sf::magnitude(double(1.23400)) == 1);
		CHECK(sf::magnitude((long double)(123400.0)) == 6);
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

}
