#include <QFileInfo>
#include <QDir>
#include <misc/gen/ConfigLocation.h>
#include <QCommandLineParser>
#include <misc/gen/gen_utils.h>
#include <misc/qt/Globals.h>
#include <misc/gen/Sustain.h>
#include "Application.h"

namespace sf
{

Application::Application(int& argc, char** argv, int flags)
	:QApplication(argc, argv, flags)
{
	// Call some static methods.
	setWindowIcon(QIcon(":logo/ico/scanframe"));
	// Ignore desktop settings because it gives the wrong icon colors.
	setDesktopSettingsAware(false);
	// Install a handler.
	setConfigLocationHandler(ConfigLocationClosure().assign(this, &Application::ConfigLocationHandler, std::placeholders::_1));
	// InitializeBase using the application file path.
	QFileInfo fi(sf::Application::applicationFilePath());;
	// Set the instance to change the extension only.
	fi.setFile(QString::fromStdString(sf::getConfigLocation()), fi.completeBaseName() + ".ini");
	// Create settings instance.
	_settings = new QSettings(fi.absoluteFilePath(), QSettings::Format::IniFormat, QApplication::instance());
	// Set the global settings for indirect opened dialogs to save and restore size and/or position.
	setGlobalSettings(_settings);
	//_settings = new QSettings(QSettings::Scope::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
	_moduleConfiguration = new ModuleConfiguration(_settings, this);
	// When libraries are loaded create the module instances.
	connect(_moduleConfiguration, &ModuleConfiguration::libraryLoaded, [&]()
	{
		// Create the interface implementations (that are missing).
		AppModuleInterface::instantiate(_settings, this);
	});
	// Set the file path to the settings instance.
	appSettings.setFilepath(fi.absoluteFilePath());
	// Install the sustain timer.
	setSustainTimer(_sustainInterval);
}

Application::~Application()
{
	// Uninstall the sustain timer by assigning zero as the interval.
	setSustainTimer(0);
	// Uninstall handler.
	setConfigLocationHandler();
	for (auto mod: AppModuleInterface::getMap().values())
	{
		delete mod;
	}
	delete_null(_mainWindow);
	delete_null(_moduleConfiguration);
	// Reset the global settings.
	setGlobalSettings(nullptr);
}

std::string Application::ConfigLocationHandler(const std::string& option)
{
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	auto rv = fi.absolutePath() + QDir::separator() + "config";
	if (!option.empty())
	{
		rv +=  QDir::separator() + QString::fromStdString(option);
	}
	return rv.toStdString();
}

void Application::initialize()
{
	// Load the missing modules from the configuration.
	_moduleConfiguration->load();
	// Create the main window when not done already.
	if (!_mainWindow)
	{
		_mainWindow = new MainWindow(_settings, this);
	}
	// Initializes all uninitialized module instances.
	AppModuleInterface::initializeInstances();
}

void Application::parseCommandline()
{
	_commandLineParser.setApplicationDescription("ScanFrame Modular Application");
	_commandLineParser.addHelpOption();
	_commandLineParser.addVersionOption();
	_commandLineParser.addPositionalArgument("file", "The file to open.");
	_commandLineParser.addOption(QCommandLineOption("debug", "Enables debugging messages."));
	_commandLineParser.process(*this);
}

void Application::processCommandLine()
{
// Open files from the command line after the window is shown otherwise the MDI dependent menu is not updated.
	if (_mainWindow)
	{
		for (const QString& fileName: _commandLineParser.positionalArguments())
		{
			_mainWindow->openFile(fileName);
		}
	}
}

MainWindow* Application::getMainWindow()
{
	return _mainWindow;
}

ModuleConfiguration& Application::getModuleConfiguration()
{
	return *_moduleConfiguration;
};

}
