#include "UnitConversionAppModule.h"
#include "ConversionDialog.h"
#include "UnitConversionPropertyPage.h"
#include <misc/qt/qt_utils.h>
#include <misc/qt/PropertySheetDialog.h>
#include <misc/gen/ConfigLocation.h>
#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>

namespace sf
{

UnitConversionAppModule::UnitConversionAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	 , _settings(params._settings)
	 ,_ucs()
{
	// Form the path to the configuration settings file.
	_configFilePath = QFileInfo(QString::fromStdString(getConfigLocation()), QFileInfo(getLibraryFilename()).baseName() + ".cfg").absoluteFilePath();
	//
	std::ifstream ifs(_configFilePath.toStdString());
	_ucs.load(ifs);
	//
	settingsReadWrite(false);
	// Install ask handler.
	_ucs.setAskHandler(UnitConversionServerClosure().assign(this, &UnitConversionAppModule::unitConversionHandler, std::placeholders::_1));
}

UnitConversionAppModule::~UnitConversionAppModule()
{
	if (_ucs.isDirty())
	{
		{
			QString bak_file = _configFilePath + ".bak";
			QFile file(_configFilePath);
			// When the config file exists and is almost zero do not replace it as a backup.
			if (file.exists() && file.size() > 30)
			{
				// Remove the old backup file.
				QFile(bak_file).remove();
				// Rename the config file to the backup file.
				file.rename(bak_file);
			}
		}
		// Create output stream and write the configuration.
		std::ofstream ofs(_configFilePath.toStdString());
		_ucs.save(ofs);
	}
}

void UnitConversionAppModule::initialize(InitializeStage stage)
{
	// Load the settings.
	if (stage)
	{
		//settingsReadWrite(false);
	}
	//_ucs.setEnable(true);
}

QString UnitConversionAppModule::getName() const
{
	return tr("Units Server");
}

QString UnitConversionAppModule::getDescription() const
{
	return tr("Provides a units of measurement server.");
}

void UnitConversionAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new UnitConversionPropertyPage(*this, sheet));
}

QString UnitConversionAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString UnitConversionAppModule::getSvgIconResource() const
{
	return ":icon/svg/units";
}

void UnitConversionAppModule::settingsReadWrite(bool save)
{
	if (!_settings)
	{
		return;
	}
	_settings->beginGroup("AppModule.UnitConversion");
	QString keyEnableId("EnableId");
	QString keyAsk("Ask");
	QString keyUnitSystem("UnitSystem");
	if (!save)
	{
		_ucs.setEnableId(_settings->value(keyEnableId).toString().toLongLong(nullptr, 0));
		_ucs.setUnitSystem(_settings->value(keyUnitSystem, PassThrough).value<UnitSystem>());
		_ask = _settings->value(keyAsk, _ask).toBool();
	}
	else
	{
		_settings->setValue(keyEnableId, QString("0x%1").arg(_ucs.getEnableId(), 0, 16));
		_settings->setValue(keyUnitSystem, enumToKey((UnitSystem) _ucs.getUnitSystem()));
		_settings->setValue(keyAsk, _ask);
	}
	_settings->endGroup();
}

bool UnitConversionAppModule::unitConversionHandler(UnitConversionEvent& ev) const
{
	if (_ask)
	{
		ConversionDialog dlg;
		return dlg.execute(ev);
	}
	return false;
}

}
