#include "InformationServiceAppModule.h"
#include "InformationServicePropertyPage.h"
#include <misc/qt/qt_utils.h>
#include <misc/qt/PropertySheetDialog.h>
#include <misc/gen/ConfigLocation.h>
#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>

namespace sf
{

InformationServiceAppModule::InformationServiceAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	 , _settings(params._settings)
	 , _informationService(*new InformationService(this))
{
	settingsReadWrite(false);
}

InformationServiceAppModule::~InformationServiceAppModule()
{
}

void InformationServiceAppModule::initialize(InitializeStage stage)
{
	// Load the settings.
	if (stage)
	{
		//settingsReadWrite(false);
	}
}

QString InformationServiceAppModule::getName() const
{
	return tr("Information Service");
}

QString InformationServiceAppModule::getDescription() const
{
	return tr("Provides a client or server service for generic information.");
}

void InformationServiceAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new InformationServicePropertyPage(*this, sheet));
}

QString InformationServiceAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString InformationServiceAppModule::getSvgIconResource() const
{
	return ":icon/svg/units";
}

void InformationServiceAppModule::settingsReadWrite(bool save)
{
	if (!_settings)
	{
		return;
	}
	_settings->beginGroup("AppModule.InformationService");
	QString keyServiceType("ServiceType");
	QString keyIdOffset("IdOffset");
	if (save)
	{
		_settings->setValue(keyServiceType, (int) _informationService.getType());
		_settings->setValue(keyIdOffset, _informationService.getIdOffset());
	}
	else
	{
		_informationService.setType((InformationService::EServiceType) _settings->value(keyServiceType).toInt());
		_informationService.setIdOffset(_settings->value(keyIdOffset).toULongLong());
	}
	_settings->endGroup();
}

}
