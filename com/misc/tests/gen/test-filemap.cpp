#include <catch2/catch.hpp>
#include <iostream>
#include <misc/gen/gen_utils.h>
#include <misc/gen/IFileMapper.h>
#if IS_WIN
#include <misc/win/FileMapper.h>
#else
#include <misc/lnx/FileMapper.h>
#endif

TEST_CASE("sf::FileMapper", "[generic][file]")
{
	using Catch::Equals;

	SECTION("sf::Create")
	{
#if IS_WIN
		sf::IFileMapper* fm  = new sf::win::FileMapper;
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

