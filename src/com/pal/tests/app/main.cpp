#include "misc/qt/ApplicationSettings.h"
#include "misc/qt/Globals.h"
#include "test-Window.h"
#include <QApplication>
#include <QDir>
#include <gii/gen/Variable.h>
#include <misc/qt/ModuleConfiguration.h>

int main(int argc, char* argv[])
{
	// Removes the console in Windows application.
	sf::freeConsole();
	// Ignore desktop settings because it gives the wrong icon colors.
	QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	// Initialize base using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings app_settings;
	// Set the file path to the settings instance and make it watch changes.
	app_settings.setFilepath(fi.absoluteFilePath(), true);
	// Set the plugin/module directory. For now not configurable.
	sf::setPluginDir(QCoreApplication::applicationDirPath() + QDir::separator() + "lib");
	//
	auto settings = app_settings.getSettings();
	//
	sf::ModuleConfiguration modConf(&settings);
	//
	modConf.load(true);
	//
	sf::Variable vThreshold(std::string("0x10,Palette|Threshold,%,SAP,Test variable for amplitude palette.,FLOAT,,0.1,80.0,0.0,100.0"));
	//
	const auto win = new Window(&settings);
	//
	win->show();
	//
	const auto rv = QCoreApplication::exec();
	//
	delete win;
	//
	return rv;
}
