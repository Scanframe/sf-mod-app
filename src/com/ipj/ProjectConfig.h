#pragma once

#include <rsa/iface/RsaServer.h>
#include <sto/iface/StorageServer.h>
#include "global.h"

namespace sf
{

class RsaServer;

class PropertySheetDialog;

class _IPJ_CLASS ProjectConfig
{
	public:

		class _IPJ_CLASS Settings
		{
			public:
				virtual bool load(const std::string& filepath) = 0;

				virtual bool read(const std::string& filepath) = 0;

				[[nodiscard]] virtual const std::string& getSettingsFileSuffix() const = 0;
		};

		ProjectConfig();

		~ProjectConfig();

		Settings& settings();

		RsaServer* getServerAcquisitionUt();

		RsaServer* getServerAcquisitionEt();

		StorageServer* getServerStorage();

		void addPropertyPages(PropertySheetDialog* sheet);

	private:

		Settings* _settings{nullptr};
		RsaServer* _serverAcquisitionUt{nullptr};
		RsaServer* _serverAcquisitionEt{nullptr};
		StorageServer* _serverStorage{nullptr};

};

}
