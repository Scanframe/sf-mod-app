#include <misc/gen/gen_utils.h>
#include <test/catch.h>

#if 0
namespace sf
{

strings split(const std::string& s, std::string::value_type sep, std::string::value_type delimiter = 0)
{
	strings rv;
	bool d = false;
	std::string::size_type p = 0, l = 0, i = 0;
	for (; i < s.length(); i++)
	{
		// When the character is a separator
		if (s[i] == sep && !d)
		{
			// First field is empty.
			if (i == 0)
			{
				rv.append({});
			}
			else
			{
				p += i - l;
				rv.append(s.substr(p, l));
				l = 0;
				p = 0;
			}
		}
		else
		{
			// if text delimiter was given.
			if (delimiter && s[i] == delimiter)
			{
				// Toggle delimiter flag when flag is encountered.
				d = !d;
				// Skip
				if (d)
				{
					p--;
				}
			}
			else
			{
				// Count the number of characters for the field.
				l++;
			}
		}
	}
	// Check for characters after last separator;
	if (l)
	{
		p += i - l;
		rv.append(s.substr(p, l));
	}
	return rv;
}

}// namespace sf
#endif

TEST_CASE("sf::String", "[con][generic][strings]")
{
	using Catch::Matchers::Equals;

	SECTION("Unique")
	{
		std::string_view s("My String");
		std::string s1(s);
		std::string s2;
		s2.assign(s1);
		auto* p1 = static_cast<const void*>(s1.c_str());
		auto* p2 = static_cast<const void*>(s2.c_str());
		// CLion complains when CHECK is not in its own scope?
		{
			// Proof that a string makes a real copy of the string.
			CHECK(p1 != p2);
		}
	}

	SECTION("Explode")
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