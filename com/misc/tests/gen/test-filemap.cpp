#include <test/catch.h>

#include <iostream>
#include <misc/gen/IFileMapper.h>
#include <misc/gen/gen_utils.h>
#if IS_WIN
	#include <misc/win/FileMapper.h>
#else
	#include <misc/lnx/FileMapper.h>
#endif

TEST_CASE("sf::FileMapper", "[con][generic][file]")
{
	using Catch::Matchers::Equals;

	SECTION("sf::Create")
	{
#if IS_WIN
		sf::IFileMapper* fm = new sf::win::FileMapper;
#else
		auto* fm = new sf::lnx::FileMapper;
//		sf::IFileMapper* fm = new sf::lnx::FileMapper;
#endif

		/*
		fm->initialize();
		fm->createView(10*1024);
		// Closure assignment check.
		REQUIRE(closure.isAssigned());
		//
		REQUIRE_THAT(closure("format (%d)", 123), Equals("format (579)"));
*/
	}
}
