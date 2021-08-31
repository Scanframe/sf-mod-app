#include <QMainWindow>
#include <QToolBar>
#include <misc/gen/ConfigLocation.h>
#include <misc/qt/PropertySheetDialog.h>
#include <misc/qt/Resource.h>
#include <gii/qt/InformationMonitor.h>
#include <misc/qt/Globals.h>
#include "AcquisitionAppModule.h"
#include "AcquisitionPropertyPage.h"

namespace sf
{

AcquisitionAppModule::AcquisitionAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	 , _settings(params._settings)
	 , _serverUt(new RsaServer(0))
	 , _serverEt(new RsaServer(0))
{
	settingsReadWrite(false);
	//
	_actionMonitorVariable = new QAction(Resource::getSvgIcon(":icon/svg/variable", QPalette::ButtonText), "Monitor Variable", this);
	_actionMonitorResultData = new QAction(Resource::getSvgIcon(":icon/svg/resultdata", QPalette::ButtonText), "Monitor ResultData");
	//
	connect(_actionMonitorVariable, &QAction::triggered, [&]()
	{
		Q_ASSERT(getGlobalParent());
		auto im = new InformationMonitor(getGlobalParent());
		if (im->selectId(Gii::Variable))
		{
			im->show();
		}
		else
		{
			delete im;
		}
	});
	connect(_actionMonitorResultData, &QAction::triggered, [&]()
	{
		Q_ASSERT(getGlobalParent());
		auto im = new InformationMonitor(getGlobalParent());
		if (im->selectId(Gii::ResultData))
		{
			im->show();
		}
		else
		{
			delete im;
		}
	});
}

AcquisitionAppModule::~AcquisitionAppModule()
{
	delete _serverUt;
	delete _serverEt;
}

void AcquisitionAppModule::initialize(bool init)
{
	if (init)
	{
		createDevices();
	}
}

QString AcquisitionAppModule::getName() const
{
	return tr("Acquisition Server");
}

QString AcquisitionAppModule::getDescription() const
{
	return tr("Provides a data acquisition module.");
}

void AcquisitionAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new AcquisitionPropertyPage(*this, sheet));
}

QString AcquisitionAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString AcquisitionAppModule::getSvgIconResource() const
{
	return ":icon/svg/signal";
}

void AcquisitionAppModule::settingsReadWrite(bool save)
{
	if (!_settings)
	{
		return;
	}
	_settings->beginGroup("AppModule.Acquisition");
	QString keySwitchId("SwitchId");
	QString keyDeviceUt("DeviceUt");
	QString keyDeviceEt("DeviceEt");
	if (!save)
	{
		_vSwitch.setup(_settings->value(keySwitchId).toString().toLongLong(nullptr, 0));
		_serverUtName = _settings->value(keyDeviceUt).toString();
		_serverEtName = _settings->value(keyDeviceEt).toString();
	}
	else
	{
		_settings->setValue(keySwitchId, QString("0x%1").arg(_vSwitch.getDesiredId(), 0, 16));
		_settings->setValue(keyDeviceUt, _serverUtName);
		_settings->setValue(keyDeviceEt, _serverEtName);
	}
	_settings->endGroup();
}

void AcquisitionAppModule::createDevices()
{
	for (auto& e: std::vector<std::pair<const QString&, RsaServer*&>>
		{
			{_serverUtName, _serverUt},
			{_serverEtName, _serverEt}
		})
	{
		if (e.first.isEmpty())
		{
			// Delete previous implementation.
			e.second->destroyImplementation();
		}
		else
		{
			// When a device exists and the name is not the same delete it first.
			if (e.first.toStdString() != e.second->getImplementationName())
			{
				// Delete previous implementation.
				e.second->destroyImplementation();
			}
			// Create implementation using the given name.
			e.second->createImplementation(e.first.toStdString());
			// Check if device was created successfully.
			if (!e.second)
			{
				qWarning() << QString("Configured acquisition with name '%1'").arg(e.first);
			}
		}
	}
}

void AcquisitionAppModule::addMenuItems(AppModuleInterface::MenuType menuType, QMenu* menu)
{
	if (menuType == Tools)
	{
		auto subMenu = menu->addMenu(Resource::getSvgIcon(getSvgIconResource(), QPalette::ButtonText), getName());
		subMenu->setToolTip(getDescription());
		//
		subMenu->addAction(_actionMonitorVariable);
		subMenu->addAction(_actionMonitorResultData);
	}
}

void AcquisitionAppModule::addToolBars(QMainWindow* mainWindow)
{
	auto toolBar = mainWindow->addToolBar(getName());
	// A toolbar needs object name to store its state.
	toolBar->setObjectName(metaObject()->className());
	toolBar->setToolTip(getDescription());
	//
	toolBar->addAction(_actionMonitorVariable);
	toolBar->addAction(_actionMonitorResultData);
}

}
