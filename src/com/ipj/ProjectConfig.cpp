#include "ProjectConfig.h"
#include <gii/gen/GiiUtils.h>
#include "IniProfileSettings.h"
#include <rsa/iface/MakeIds.h>

namespace sf
{

ProjectConfig::ProjectConfig()
	:_serverAcquisitionUt(new RsaServer(0, DEVICENR_UT, "Ultrasonic"))
	 , _serverAcquisitionEt(new RsaServer(0, DEVICENR_EC, "Eddy current"))
	 , _serverStorage(new StorageServer(DEVICENR_STO, "Storage"))
{
}

ProjectConfig::~ProjectConfig()
{
	delete _serverAcquisitionUt;
	delete _serverAcquisitionEt;
	delete _serverStorage;
}

void ProjectConfig::addPropertyPages(PropertySheetDialog* sheet)
{
	(void)sheet;
}

RsaServer* ProjectConfig::getServerAcquisitionUt()
{
	return _serverAcquisitionUt;
}

RsaServer* ProjectConfig::getServerAcquisitionEt()
{
	return _serverAcquisitionEt;
}

StorageServer* ProjectConfig::getServerStorage()
{
	return _serverStorage;
}

ProjectConfig::Settings& ProjectConfig::settings()
{
	if (!_settings)
	{
		_settings = new IniProfileSettings();
	}
	return *_settings;
}


}