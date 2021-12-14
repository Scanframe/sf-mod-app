#include <test/catch.h>
#include <misc/gen/gen_utils.h>

namespace sf
{

//template<typename S>
typedef std::string S;
strings split(const S& s, S::value_type sep, S::value_type delimiter = 0)
{
	strings rv;
	bool d = false;
	S::size_type p = 0, l = 0, i = 0;
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

}

TEST_CASE("sf::StringSplit", "[generic][strings]")
{
	using Catch::Matchers::Equals;

	SECTION("Splitter")
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