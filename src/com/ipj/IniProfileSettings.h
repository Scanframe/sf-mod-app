#pragma once
#include "ProjectConfig.h"

namespace sf
{

class IniProfileSettings :public ProjectConfig::Settings
{
	public:
		bool load(const std::string& filepath) override;

		bool read(const std::string& filepath) override;

		[[nodiscard]] const std::string& getSettingsFileSuffix() const override;

	private:
		std::string _settingsFileSuffix{"gisf"};

};

}
