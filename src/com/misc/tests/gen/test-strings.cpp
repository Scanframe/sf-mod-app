#include <misc/gen/TStrings.h>
#include <misc/gen/string.h>
#include <test/catch.h>

TEST_CASE("sf::String", "[con][generic][strings]")
{
	using Catch::Matchers::Equals;

	SECTION("sf::toNumber", "Convert string to a given type.")
	{
		// Check if conversion of floating point is correct.
		std::string::size_type idx;
		CHECK(sf::toNumber<float, std::string>("12.34", &idx) == Catch::Approx(12.34).margin(1E-8));
		CHECK(idx == 5);
		CHECK(sf::toNumber<double, std::string>("12.34", &idx) == Catch::Approx(12.34).margin(1E-8));
		CHECK(idx == 5);
		CHECK(sf::toNumber<long double, std::string>("12.34", &idx) == Catch::Approx(12.34).margin(1E-8));
		CHECK(idx == 5);

		CHECK(sf::toNumber<float, std::wstring>(L"12.34", &idx) == Catch::Approx(12.34).margin(1E-8));
		CHECK(idx == 5);
		CHECK(sf::toNumber<double, std::wstring>(L"12.34", &idx) == Catch::Approx(12.34).margin(1E-8));
		CHECK(idx == 5);
		CHECK(sf::toNumber<long double, std::wstring>(L"12.34", &idx) == Catch::Approx(12.34).margin(1E-8));
		CHECK(idx == 5);
	}

	SECTION("sf::toNumberDefault", "Convert string to a given type and use default on failure.")
	{
		// Check if conversion of integer values is correct.
		CHECK(sf::toNumberDefault<int, std::string>("1234", 4321) == 1234);
		CHECK(sf::toNumberDefault<long int, std::string>("1234", 4321) == 1234);
		CHECK(sf::toNumberDefault<long long int, std::string>("1234", 4321) == 1234);

		CHECK(sf::toNumberDefault<int, std::wstring>(L"1234", 4321) == 1234);
		CHECK(sf::toNumberDefault<long int, std::wstring>(L"1234", 4321) == 1234);
		CHECK(sf::toNumberDefault<long long int, std::wstring>(L"1234", 4321) == 1234);

		CHECK(sf::toNumberDefault<unsigned int, std::string>("1234", 4321) == 1234);
		CHECK(sf::toNumberDefault<unsigned long int, std::string>("1234", 4321) == 1234);
		CHECK(sf::toNumberDefault<unsigned long long int, std::string>("1234", 4321) == 1234);

		CHECK(sf::toNumberDefault<unsigned int, std::string>("-1234", 4321) == -1234u);
		CHECK(sf::toNumberDefault<unsigned long int, std::string>("-1234", 4321) == -1234ul);
		CHECK(sf::toNumberDefault<unsigned long long int, std::string>("-1234", 4321) == -1234ul);

		CHECK(sf::toNumberDefault<float, std::string>("12.34", 43.21f) == Catch::Approx(12.34).margin(1E-8));
		CHECK(sf::toNumberDefault<double, std::string>("12.34", 43.21) == Catch::Approx(12.34).margin(1E-8));
		CHECK(sf::toNumberDefault<long double, std::string>("12.34", 43.21L) == Catch::Approx(12.34).margin(1E-8));

		// Check to see if conversion going wrong the default is picked up.
		CHECK(sf::toNumberDefault<float, std::string>("12,34", 43.21f) != Catch::Approx(12.34).margin(1E-4));
		CHECK(sf::toNumberDefault<double, std::string>("12,34", 43.21) != 43.21f);
		CHECK(sf::toNumberDefault<long double, std::string>("12,34", 43.21L) != 43.21f);
		// Check the same on wide character strings.
		CHECK(sf::toNumberDefault<float, std::wstring>(L"12,34", 43.21f) != Catch::Approx(12.34).margin(1E-4));
		CHECK(sf::toNumberDefault<double, std::wstring>(L"12,34", 43.21) != 43.21f);
		CHECK(sf::toNumberDefault<long double, std::wstring>(L"12,34", 43.21L) != 43.21f);
	}

	SECTION("sf::LocaleNumStr", "Number string conversions.")
	{
		int precision = 8;
		precision = sf::clip(precision, 0, std::numeric_limits<double>::digits10);
		auto* strValue = "12345.67890123";
		auto value = sf::stod<double>(strValue);
		CHECK(sf::stringf("%.*lf", precision, value) == strValue);
	}

	SECTION("sf::gcvtString", "Numeric value to an std string.")
	{
		CHECK(sf::gcvtString(3e3) == "3000");
		CHECK(sf::gcvtString(3e4) == "30000");
		CHECK(sf::gcvtString(12.25e9) == "12250000000");
		CHECK(sf::gcvtString(12.257) == "12.257");
		CHECK(sf::gcvtString(1234.2567) == "1234.2567");
		CHECK(sf::gcvtString(12.3456e-6L, 4) == "1.235e-05");
	}

	SECTION("sf::toString", "Numeric value to an std string.")
	{
		CHECK(sf::toString(1234e+17) == "1.234e+20");
		CHECK(sf::toString(1234e+7) == "1234e7");
		// Double values.
		CHECK(sf::toString(1234e+7, 6) == "1.234e+10");
		CHECK(sf::toString(1234e-7, 6) == "0.0001234");
		// Long double values.
		CHECK(sf::toString(1234e+7l, 6) == "1.234e+10");
		CHECK(sf::toString(1234e-7l, 6) == "0.0001234");
	}

	SECTION("sf::toStringPrecision", "Numeric value to an std string with a precision.")
	{
		CHECK(sf::toStringPrecision(1234.2567) == "1234.256700");
		//CHECK(sf::numberString(1234e-18, 6, true) == "1.23400e-15");
		// 2 decimals after and rounds the value.
		CHECK(sf::toStringPrecision(1234.2567, 2) == "1234.26");
		// 3 decimals after and rounds the value.
		CHECK(sf::toStringPrecision(1234.2567, 3) == "1234.257");
		// Clipping the amount of decimals to zero.
	}

	SECTION("sf::numberString", "Numeric value to std string with set resolution/digits.")
	{
		CHECK(sf::requiredDigits(0.0025, 0.01, 0.1) == 3);
		CHECK(sf::requiredDigits(1, 0, 100) == 3);

		CHECK(sf::ipow(29, 0) == 1);
		CHECK(sf::ipow(27, 1) == 27);
		CHECK(sf::ipow(24l, 5) == 7962624);

		CHECK(sf::numberString(1234e-18, 6) == "+1.23400e-15");
		CHECK(sf::numberString(1234e+18, 6) == "+1.23400e+21");
		CHECK(sf::numberString(123456e-18, 6) == "+123456e-18");
		CHECK(sf::numberString(123456e+18, 6) == "+123456e+18");
		CHECK(sf::numberString(123400e-18, 6) == "+123400e-18");
		CHECK(sf::numberString(123400e+18, 6) == "+123400e+18");
		CHECK(sf::numberString(1234567890e-7, 6) == "+123.457");
		CHECK(sf::numberString(-1234567890e-7, 6) == "-123.457");
		CHECK(sf::numberString(101, 3, false) == "101");
		CHECK(sf::numberString(123456.0l, 6) == "+123456");
		CHECK(sf::numberString(123.1, 3) == "+123");
		CHECK(sf::numberString(100.0l, 3) == "+100");
		CHECK(sf::numberString(299.996e1l, 2) == "+3000");
		CHECK(sf::numberString(299.996e4l, 2) == "+3.0e+6");
		CHECK(sf::numberString(299.996e1l, 4) == "+3000");
		CHECK(sf::numberString(-0.99999l, 2) == "-1.0");
		CHECK(sf::numberString(1e-2L, 4) == "+10.00e-3");
		CHECK(sf::numberString(123e-1L, 2) == "+12");
		CHECK(sf::numberString(1234567890e-10L, 6) == "+0.123457");
		CHECK(sf::numberString(-1234567890e-10L, 6) == "-0.123457");
		CHECK(sf::numberString(1234567890e10L, 6) == "+12.3457e+18");
		CHECK(sf::numberString(1234567890e12l, 4) == "+1.235e+21");
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

	SECTION("std::string Proof Unique")
	{
		std::string_view s("My String");
		std::string s1(s);
		std::string s2;
		s2.assign(s1);
		auto* p1 = static_cast<const void*>(s1.c_str());
		auto* p2 = static_cast<const void*>(s2.c_str());
		// Proof that a string makes a real copy of the string.
		CHECK(p1 != p2);
	}

	SECTION("sf::escape + sf::unescape", "C escaping functions")
	{
		CHECK_THAT(sf::escape("\t\b\r\a\"\'\\"), Equals("\\t\\b\\r\\a\\\"\\'\\\\"));
		// Escape using delimiter character.
		CHECK_THAT(sf::escape("delim{\"inside\"}", '"'), Equals("delim{\\x22inside\\x22}"));
		CHECK_THAT(sf::unescape("\\t\\b\\r\\a\\\"\\'\\\\"), Equals("\t\b\r\a\"\'\\"));
		// Escape using delimiter character.
		CHECK_THAT(sf::unescape("delim{\\x22inside\\x22}"), Equals("delim{\"inside\"}"));
	}

	SECTION("sf::strings Split + Join")
	{
		sf::strings sl;

		sl.clear();
		CHECK(sl.explode(R"("first",second,3,4.0)", ",") == std::vector<std::string>{"\"first\"", "second", "3", "4.0"});

		CHECK(sl.join(",") == "\"first\",second,3,4.0");

		sl.clear();
		CHECK(sl.split(R"("first",second,3,4.0)", ',', '"') == std::vector<std::string>{"first", "second", "3", "4.0"});

		sl.clear();
		CHECK(sl.split(R"(,"second",3,4.0)", ',', '"') == std::vector<std::string>{"", "second", "3", "4.0"});

		sl.clear();
		CHECK(sl.split(R"("first",second,3,4.0)", ',') == std::vector<std::string>{"\"first\"", "second", "3", "4.0"});
	}
}