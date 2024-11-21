#include "misc/gen/system.h"
#include <cstddef>
#include <gmi/iface/Controller.h>
#include <memory>
#include <test/catch.h>
#include <misc/gen/file.h>


TEST_CASE("sf::gmi", "[utils]")
{

	SECTION("Utilities")
	{
		REQUIRE_FALSE(false);
	}
}

SCENARIO("sf::gmi::Controller", "[controller]")
{
	GIVEN("Controller")
	{
		// Smart pointer when out of scope delete content.
		std::unique_ptr<sf::gmi::TController> controller;
		std::string plugin_dir = sf::getExecutableDirectory() + sf::getDirectorySeparator() + "lib";
		std::cout << "Plugins at: " << plugin_dir << std::endl;
		// Check if the directory exists.
		REQUIRE(sf::fileExists(plugin_dir));

		THEN("No implementations")
		{
			REQUIRE(sf::gmi::TController::Interface().size() == 0);
			REQUIRE(sf::gmi::TController::Interface().indexOf("emulator") == sf::gmi::TController::Interface().npos);
		}

		// Validate one use case for the GIVEN object
		WHEN("Load Module")
		{
			auto plugin = plugin_dir  + sf::getDirectorySeparator() + "libsf-gmi-emulator";
			// Append the appropriate extension.
#if IS_WIN
				plugin += ".dll";
#else
				plugin += ".so";
#endif
			REQUIRE(sf::loadDynamicLibrary(plugin));

			THEN("Implementations are available")
			{
				// Need at least one controller implementation.
				REQUIRE(sf::gmi::TController::Interface().size() > 0);
				REQUIRE(sf::gmi::TController::Interface().indexOf("emulator") != sf::gmi::TController::Interface().npos);
			}

			THEN("Create emulator implementation from name")
			{
				// Need at least one controller implementation.
				controller.reset(sf::gmi::TController::Interface().create("emulator"));
				REQUIRE(controller);
				WHEN("")
				{

				}
			}

		}

/*
		// Need at least one controller implementation.
		controller = std::unique_ptr<sf::gmi::TController>(sf::gmi::TController::Interface().create("emulator", sf::gmi::TController::Parameters(0)));
		// Check if the controller was created.
		REQUIRE(controller);
*/

	}
}
