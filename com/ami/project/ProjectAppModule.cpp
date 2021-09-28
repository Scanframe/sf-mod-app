#include <QMainWindow>
#include <QToolBar>
#include <misc/gen/ConfigLocation.h>
#include <misc/qt/PropertySheetDialog.h>
#include <misc/qt/Resource.h>
#include <misc/qt/Globals.h>
#include <gii/qt/InformationMonitor.h>
#include <QFileDialog>
#include <QMessageBox>
#include "ProjectAppModule.h"
#include "ProjectPropertyPage.h"

namespace sf
{

ProjectAppModule::ProjectAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	 , _settings(params._settings)
	 ,	_projectConfig(new ProjectConfig)
	 // TODO: Must be read from QSettings and option for auto read at start up.
	 , _currentSettingsFile("default")
{
	settingsReadWrite(false);
	// TODO: Needs AMI interface change to make settings files open.
	//addFileType(tr("Settings File"), QString::fromStdString(_projectConfig->settings().getSettingsFileSuffix()));
	//
	_actionMonitorVariable = new QAction(Resource::getSvgIcon(":icon/svg/variable", QPalette::ButtonText), "Monitor Variable", this);
	_actionMonitorResultData = new QAction(Resource::getSvgIcon(":icon/svg/resultdata", QPalette::ButtonText), "Monitor ResultData");
	_actionSaveSettings = new QAction(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Save), QPalette::ButtonText), "&Save");
	_actionLoadSettings = new QAction(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::OpenFolder), QPalette::ButtonText), "&Load");
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
	//
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
	//
	connect(_actionSaveSettings, &QAction::triggered, [&]()
	{
		Q_ASSERT(getGlobalParent());
		auto location = getConfigLocation("settings");
		QFileDialog dlg(getGlobalParent(), tr("Save Setting"), QString::fromStdString(location),
			tr("Settings Files (*.%1)").arg(QString::fromStdString(_projectConfig->settings().getSettingsFileSuffix())));
		dlg.setDefaultSuffix(QString::fromStdString(_projectConfig->settings().getSettingsFileSuffix()));
		dlg.setAcceptMode(QFileDialog::AcceptSave);
		dlg.selectFile(_currentSettingsFile);
		if (!_currentSettingsFile.isEmpty())
		{
			dlg.fileSelected(_currentSettingsFile);
		}
		if (dlg.exec() == QFileDialog::Accepted)
		{
			auto fl = dlg.selectedFiles();
			if (!fl.empty())
			{
				_currentSettingsFile = fl.first();
				if (!_projectConfig->settings().read(_currentSettingsFile.toStdString()))
				{
					QMessageBox::information(getGlobalParent(), tr("Save Failed"), tr("Saving file '%1' failed!").arg(_currentSettingsFile));
				}
			}
		}
	});
	//
	connect(_actionLoadSettings, &QAction::triggered, [&]()
	{
		Q_ASSERT(getGlobalParent());
		auto location = getConfigLocation("settings");
		QFileDialog dlg(getGlobalParent(), tr("Save Setting"), QString::fromStdString(location),
			tr("Settings Files (*.%1)").arg(QString::fromStdString(_projectConfig->settings().getSettingsFileSuffix())));
		dlg.setDefaultSuffix(QString::fromStdString(_projectConfig->settings().getSettingsFileSuffix()));
		dlg.setAcceptMode(QFileDialog::AcceptOpen);
		dlg.selectFile(_currentSettingsFile);
		if (!_currentSettingsFile.isEmpty())
		{
			dlg.fileSelected(_currentSettingsFile);
		}
		if (dlg.exec() == QFileDialog::Accepted)
		{
			auto fl = dlg.selectedFiles();
			if (!fl.empty())
			{
				_currentSettingsFile = fl.first();
				if (!_projectConfig->settings().load(_currentSettingsFile.toStdString()))
				{
					QMessageBox::information(getGlobalParent(), tr("Load Failed"), tr("Loading file '%1' failed!").arg(_currentSettingsFile));
				}
			}
		}
	});
}

ProjectAppModule::~ProjectAppModule()
{
	delete _projectConfig;
}

void ProjectAppModule::initialize(bool init)
{
	if (init)
	{
		createDevices();
	}
}

QString ProjectAppModule::getName() const
{
	return tr("Project");
}

QString ProjectAppModule::getDescription() const
{
	return tr("Provides project module for configuring acquisition and motion devices.");
}

void ProjectAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new ProjectPropertyPage(*this, sheet));
}

QString ProjectAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString ProjectAppModule::getSvgIconResource() const
{
	return ":icon/svg/project";
}

void ProjectAppModule::settingsReadWrite(bool save)
{
	if (!_settings)
	{
		return;
	}
	_settings->beginGroup("AppModule.Acquisition");
	QString keyDeviceUt("DeviceUt");
	QString keyDeviceEt("DeviceEt");
	QString keyDeviceMotion("DeviceMotion");
	if (!save)
	{
		_serverUtName = _settings->value(keyDeviceUt).toString();
		_serverEtName = _settings->value(keyDeviceEt).toString();
		_serverMotionName = _settings->value(keyDeviceMotion).toString();
	}
	else
	{
		_settings->setValue(keyDeviceUt, _serverUtName);
		_settings->setValue(keyDeviceEt, _serverEtName);
		_settings->setValue(keyDeviceMotion, _serverMotionName);
	}
	_settings->endGroup();
}

void ProjectAppModule::createDevices()
{
	for (auto& e: std::vector<std::pair<const QString&, RsaServer*>>
		{
			{_serverUtName, _projectConfig->getServerAcquisitionUt()},
			{_serverEtName, _projectConfig->getServerAcquisitionEt()}
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
	// TODO: Creation of motion controller needs implementation.
//	// Delete previous implementation.
//	_projectConfig->getServerMotion()->destroyImplementation();
//	// Create implementation using the given name.
//	_projectConfig->getServerMotion()->createImplementation(_serverMotionName.toStdString());

	// Delete previous implementation.
	_projectConfig->getServerStorage()->destroyImplementation();
	// Create implementation using the given name.
	_projectConfig->getServerStorage()->createImplementation(_serverStorageName.toStdString());
}

void ProjectAppModule::addMenuItems(AppModuleInterface::MenuType menuType, QMenu* menu)
{
	if (menuType == Tools)
	{
		auto subMenu = menu;
/*
		auto subMenu = menu->addMenu(Resource::getSvgIcon(getSvgIconResource(), QPalette::ButtonText), getName());
		subMenu->setToolTip(getDescription());
*/
		//
		subMenu->addAction(_actionMonitorVariable);
		subMenu->addAction(_actionMonitorResultData);
	}
	if (menuType == Settings)
	{
		menu->addAction(_actionSaveSettings);
		menu->addAction(_actionLoadSettings);
	}
}

void ProjectAppModule::addToolBars(QMainWindow* mainWindow)
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
