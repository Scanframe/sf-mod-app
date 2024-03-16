#include <iostream>
#include <misc/gen/IniProfile.h>
#include <misc/gen/gen_utils.h>
#include <test/catch.h>

TEST_CASE("sf::IniProfile", "[con][generic][ini]")
{
	using Catch::Matchers::Equals;

	SECTION("sf::Create")
	{
		// Use test file in the working directory.
		auto ini_path = sf::getWorkingDirectory() + sf::getDirectorySeparator() + "default.ini";
		// Keys and values to compare.
		std::string key1("StringKey1");
		std::string value1("This is micro [\U000000B5] and this is squared [\U000000B2]");
		std::string key2("StringKey3");
		std::string value2("This is a half [\U000000BD] and this is plus-minus [\U000000B1]");

		// Scope for ini to get out of scope.
		{
			// Remove the file when it exists.
			if (sf::fileExists(ini_path))
			{
				sf::fileUnlink(ini_path);
			}

			sf::IniProfile ini(ini_path);

			REQUIRE(ini.setSection("Section 1"));
			REQUIRE_FALSE(ini.keyExists(key1));

			REQUIRE(ini.setString(key1, value1));
			REQUIRE(ini.insertComment(key1, "String having some utf8 encoded"));
			REQUIRE(ini.setInt("Integer", 123456789));

			REQUIRE(ini.setSection("Section 2"));
			REQUIRE(ini.setString(key2, value2));
			REQUIRE(ini.insertComment(key2, "Another section and string key having UTF-8 encoded stuff in it."));
			REQUIRE(ini.setInt("Integer", 987654321));

			// Goto section null
			REQUIRE(ini.setSection(0));
			REQUIRE(ini.getString(key1) == value1);
			REQUIRE(ini.getInt("Integer") == 123456789);
			/*
			// Write file.
			REQUIRE(ini.sync());
*/
			// Out of scope should write the file.
		}

		// File needs to exist.
		REQUIRE(sf::fileExists(ini_path));
		// Open the ini-file again.
		sf::IniProfile ini(ini_path);

		// Goto section null
		REQUIRE(ini.setSection(0));
		REQUIRE(ini.getString(key1) == value1);
		REQUIRE(ini.getInt("Integer") == 123456789);

		REQUIRE(ini.setSection(1));
		REQUIRE(ini.getString(key2) == value2);
		REQUIRE(ini.getInt("Integer") == 987654321);
	}

	SECTION("sf::InputStream")
	{
		const char* IniContent = R"(

[Section A]
;String having some ...
Key1=Value1
Integer=123456789

[Section B]
;Another key here
Key2=Value2
Integer=987654321

)";
		std::istringstream is(IniContent);
		sf::IniProfile ini(is);

		REQUIRE(ini.sectionExists("Section A"));
		REQUIRE(ini.sectionExists("Section B"));
		REQUIRE(ini.setSection("Section B"));
		REQUIRE(ini.getString("Key2") == "Value2");
		REQUIRE(ini.getInt("Integer") == 987654321);
		REQUIRE(ini.getKeys() == sf::strings{"Key2", "Integer"});

		//ini.write(std::clog);
	}
}
