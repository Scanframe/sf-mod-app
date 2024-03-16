#include <iostream>
#include <misc/gen/ConfigLocation.h>
#include <misc/gen/gen_utils.h>
#include <test/catch.h>

namespace sf
{

struct ConfigHandler
{
		ConfigHandler()
		{
			// Install handler.
			setConfigLocationHandler(ConfigLocationClosure().assign(this, &ConfigHandler::Handler, std::placeholders::_1));
		}

		~ConfigHandler()
		{
			// Uninstall handler.
			setConfigLocationHandler();
		}

		std::string Handler(const std::string& option)
		{
			return option + "-directory";
		}
};

}// namespace sf

TEST_CASE("sf::ConfigLocation", "[con][generic]")
{

	SECTION("NoHandler", "No handler installed")
	{
		CHECK(sf::getConfigLocation() == sf::getWorkingDirectory());
	}

	SECTION("Handler", "Handler installed")
	{
		sf::ConfigHandler handler;
		CHECK(sf::getConfigLocation("option") == "option-directory");
	}
}
