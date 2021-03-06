#include <catch2/catch.hpp>
#include <misc/closure.h>
#include <iostream>
#include "misc/genutils.h"

TEST_CASE("sf::Closure", "[closure]")
{
	using Catch::Equals;

	//	std::cout << "C++ std: " << __cplusplus << std::endl;
	typedef sf::Closure<std::string, const char*, int> my_closure_type;
	//
	SECTION("sf::Closure Valid")
	{
		// Value passed to lambda function.
		int abc = 456;
		// Construct closure with lambda function.
		my_closure_type closure([abc](const char* fmt, int value) -> std::string
		{
			return sf::string_format(fmt, value + abc);
		});
		// Closure assignment check.
		REQUIRE(closure.isAssigned());
		//
		REQUIRE_THAT(closure("format (%d)", 123), Equals("format (579)"));
	}

	SECTION("sf::Closure Invalid")
	{
		// Construct c with lambda function.
		my_closure_type c;
		// Closure is not assigned using bool operator.
		REQUIRE(!c);
		// Closure throws an exception.
		REQUIRE_THROWS_AS(c("format (%d)", 123), std::bad_function_call);
	}

	SECTION("sf::Closure Copy")
	{
		// Construct closure with lambda function.
		my_closure_type closure([](const char* fmt, int value) -> std::string
		{
			return sf::string_format(fmt, value);
		});
		//
		REQUIRE_THAT(my_closure_type(closure)("format (%d)", 123), Equals("format (123)"));

		my_closure_type c;
		c = closure;
		REQUIRE_THAT(c("format (%d)", 123), Equals("format (123)"));
	}

	SECTION("sf::Closure Assignment Lambda")
	{
		// Construct closure with lambda function.
		auto fn = [](const char* fmt, int value) -> std::string
		{
			return sf::string_format(fmt, value);
		};
		//
		my_closure_type c1, c2;
		//
		c2.Assign(c1.Assign(fn));
		//
		REQUIRE_THAT(c1("format (%d)", 123), Equals("format (123)"));
		REQUIRE_THAT(c2("format (%d)", 123), Equals("format (123)"));
		//
		c2.Unassign();
		// Closure is not assigned using bool operator.
		REQUIRE(!c2);
	}

	SECTION("sf::Closure Assignment Class Member")
	{
		struct MyTest
		{
			// Construct closure with lambda function.
			static std::string my_func(const char* fmt, int value)
			{
				return sf::string_format(fmt, value);
			}
		};
		//
		my_closure_type c1(MyTest::my_func);
		//
		REQUIRE_THAT(c1("format (%d)", 123), Equals("format (123)"));
	}

}

