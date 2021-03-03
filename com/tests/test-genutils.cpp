#include <catch2/catch.hpp>
#include <misc/genutils.h>

TEST_CASE("Generic Utils", "[main]")
{

	SECTION("Sizes")
	{
		std::clog << "Integer sizes: "
			<< "int(" << sizeof(int) * CHAR_BIT << "), "
			<< "long(" << sizeof(long) * CHAR_BIT << "), "
			<< "long long(: " << sizeof(long long) * CHAR_BIT << "), "
			<< std::endl;
	}

	SECTION("sf::itoa")
	{
		char buf[sizeof(int) * CHAR_BIT + 1];
		REQUIRE_THAT(sf::itoa(-56789, buf, 10), Catch::Matchers::Equals("-56789"));
		REQUIRE_THAT(sf::itoa(0x7fedba31, buf, 16), Catch::Matchers::Equals("7fedba31"));
		REQUIRE_THAT(sf::itoa(1234567890, buf, 10), Catch::Matchers::Equals("1234567890"));
	};

	SECTION("sf::ltoa")
	{
		char buf[sizeof(long) * CHAR_BIT + 1];
		REQUIRE_THAT(sf::itoa(-1234567890, buf, 10), Catch::Matchers::Equals("-1234567890"));
		REQUIRE_THAT(sf::itoa(987654321, buf, 10), Catch::Matchers::Equals("987654321"));
		REQUIRE_THAT(sf::lltoa(0x123456789abcdefL, buf, 16), Catch::Matchers::Equals("123456789abcdef"));
	};

	SECTION("sf::escape")
	{
		REQUIRE_THAT(sf::escape("\t\b\r\a\"\'\\"), Catch::Matchers::Equals("\\t\\b\\r\\a\\\"\\'\\\\"));
		// Escape using delimiter character.
		REQUIRE_THAT(sf::escape("delim{\"inside\"}", '"'), Catch::Matchers::Equals("delim{\\x22inside\\x22}"));
	};

	SECTION("sf::unescape")
	{
		REQUIRE_THAT(sf::unescape("\\t\\b\\r\\a\\\"\\'\\\\"), Catch::Matchers::Equals("\t\b\r\a\"\'\\"));
		// Escape using delimiter character.
		REQUIRE_THAT(sf::unescape("delim{\\x22inside\\x22}"), Catch::Matchers::Equals("delim{\"inside\"}"));
	};

}
