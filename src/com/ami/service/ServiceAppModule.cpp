#include "ServiceAppModule.h"
#include "ServicePropertyPage.h"
#include <misc/qt/qt_utils.h>
#include <misc/qt/PropertySheetDialog.h>
#include <misc/gen/ConfigLocation.h>
#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>

namespace sf
{

ServiceAppModule::ServiceAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	 , _settings(params._settings)
	 , _informationService(nullptr)
{
	_informationService = new InformationService(this);
	settingsReadWrite(false);
}

ServiceAppModule::~ServiceAppModule()
{
	(void)this;
}

void ServiceAppModule::initialize(InitializeStage stage)
{
	// Load the settings.
	if (stage)
	{
		//settingsReadWrite(false);
	}
}

QString ServiceAppModule::getName() const
{
	return tr("Information Service");
}

QString ServiceAppModule::getDescription() const
{
	return tr("Provides a client or server service for generic information.");
}

void ServiceAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new ServicePropertyPage(*this, sheet));
}

QString ServiceAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString ServiceAppModule::getSvgIconResource() const
{
	return ":icon/svg/units";
}

void ServiceAppModule::settingsReadWrite(bool save)
{
	if (!_settings || !_informationService)
	{
		return;
	}
	_settings->beginGroup("AppModule.InformationService");
	QString keyServiceType("ServiceType");
	QString keyIdOffset("IdOffset");
	if (save)
	{
		_settings->setValue(keyServiceType, (int) _informationService->getType());
		_settings->setValue(keyIdOffset, _informationService->getIdOffset());
	}
	else
	{
		_informationService->setType((InformationService::EServiceType) _settings->value(keyServiceType).toInt());
		_informationService->setIdOffset(_settings->value(keyIdOffset).toULongLong());
	}
	_settings->endGroup();
}

}
