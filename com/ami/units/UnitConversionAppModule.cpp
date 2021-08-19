#include <QSettings>
#include <QMetaEnum>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <misc/qt/PropertySheetDialog.h>
#include "UnitConversionAppModule.h"
#include "ConversionDialog.h"
#include "UnitConversionPropertyPage.h"

namespace sf
{

UnitConversionAppModule::UnitConversionAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	,_settings(params._settings)
{

	// TODO: Resolving path needs to be fixed globally.
	// Form the path to the configuration settings file.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", QFileInfo(getLibraryFilename()).baseName() + ".cfg");
	_configFilePath = fi.absoluteFilePath();
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

void UnitConversionAppModule::initialize()
{
		// Load the settings.
//		settingsReadWrite(false);
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

template<typename T>
static const char* enumToKey(const T value)
{
	return QMetaEnum::fromType<T>().valueToKey(value);
}

template<typename T>
static T keyToEnum(const char* key)
{
	return QMetaEnum::fromType<T>().keyToValue(key);
}

template<typename T>
static T keyToEnum(QString key)
{
	return QMetaEnum::fromType<T>().keyToValue(key);
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
		_settings->setValue(keyUnitSystem, enumToKey((UnitSystem)_ucs.getUnitSystem()));
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
