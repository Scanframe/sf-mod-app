#include "misc/qt/qt_utils.h"
#include <iostream>
#include <misc/qt/FileMapper.h>
#include <test/catch.h>

TEST_CASE("sf::qt::FileMapper", "[con][generic][qt][file]")
{
	using Catch::Matchers::Equals;

	SECTION("Create")
	{
		size_t sz = 256;
		// Characters to fill the file with.
		auto* str = "abcdefghijklmnopqrstuvwxyz0123456789\n";
		//
		auto fm = new sf::qt::FileMapper();
		//
		fm->initialize();
		//
		fm->createView(sz);
		//
		//std::clog << "File: " << fm->getFile().fileName() << std::endl;
		//
		fm->mapView();
		//
		auto ptr = (char*) fm->getPtr();
		//
		for (size_t i = 0; i < sz; ++i)
		{
			// Write entire space with data.
			ptr[i] = str[i % 37];
		}
		//
		fm->unmapView();

		//
		delete fm;
		/*
		REQUIRE();
		//
		REQUIRE_THAT(closure("format (%d)", 123), Equals("format (579)"));
*/
	}
}
