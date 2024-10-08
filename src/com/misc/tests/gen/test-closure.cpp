#include <iostream>
#include <misc/gen/TClosure.h>
#include <misc/gen/string.h>
#include <test/catch.h>

TEST_CASE("sf::Closure", "[con][generic]")
{
	using Catch::Matchers::Equals;

	//	std::cout << "C++ std: " << __cplusplus << std::endl;
	typedef sf::TClosure<std::string, const char*, int> my_closure_type;
	//
	SECTION("sf::Closure Valid")
	{
		// Value passed to lambda function.
		int abc = 456;
		// Construct closure with lambda function.
		my_closure_type closure([abc](const char* fmt, int value) -> std::string {
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
		my_closure_type closure([](const char* fmt, int value) -> std::string {
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
		auto fn = [](const char* fmt, int value) -> std::string {
			return sf::string_format(fmt, value);
		};
		//
		my_closure_type c1, c2;
		//
		c2.assign(c1.assign(fn));
		//
		REQUIRE_THAT(c1("format (%d)", 123), Equals("format (123)"));
		REQUIRE_THAT(c2("format (%d)", 123), Equals("format (123)"));
		//
		c2.unassign();
		// Closure is not assigned using bool operator.
		REQUIRE(!c2);
	}

	SECTION("sf::Closure Assignment Class Member")
	{
		struct MyClass
		{
				std::string myFunc(const char* fmt, int value)
				{
					return sf::string_format(fmt, value);
				}

				static std::string myStaticFunc(const char* fmt, int value)
				{
					return sf::string_format(fmt, value);
				}
		} myClass;

		sf::TClosure<std::string, const char*, int> c2;

		// Test assigning a static method directly.
		c2.assign(&MyClass::myStaticFunc);
		REQUIRE_THAT(c2("format (%d)", 345), Equals("format (345)"));

		// Test assigning a function using std::bind().
		c2.assign(std::bind(&MyClass::myStaticFunc, std::placeholders::_1, std::placeholders::_2));// NOLINT(modernize-avoid-bind)
		REQUIRE_THAT(c2("format (%d)", 123), Equals("format (123)"));

		// Test assigning a member using std::bind().
		c2.assign(std::bind(&MyClass::myFunc, &myClass, std::placeholders::_1, std::placeholders::_2));// NOLINT(modernize-avoid-bind)
		REQUIRE_THAT(c2("format (%d)", 123), Equals("format (123)"));

		// Test assign template function using std::bind under the hood.
		c2.assign(&myClass, &MyClass::myFunc, std::placeholders::_1, std::placeholders::_2);
		REQUIRE_THAT(c2("format (%d)", 125), Equals("format (125)"));
	}
}
