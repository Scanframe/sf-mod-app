#include "test-Window.h"
#include <gii/gen/Variable.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/ModuleConfiguration.h>
#include <QApplication>
#include <QTimer>
#include <QDir>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	// Initialize base using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings app_settings;
	// Set the file path to the settings instance and make it watch changes.
	app_settings.setFilepath(fi.absoluteFilePath(), true);
	//
	auto settings = new QSettings(fi.absoluteFilePath(), QSettings::Format::IniFormat, QApplication::instance());
	//
	sf::ModuleConfiguration modConf(settings);
	//
	modConf.load(true);
	//
	sf::Variable vThreshold(std::string("0x10,Palette|Threshold,%,SAP,Test variable for amplitude palette.,FLOAT,,0.1,80.0,0.0,100.0"));
	//
	auto win = new Window(settings);
	//
	win->show();
	//
	auto rv = QCoreApplication::exec();
	//
	delete win;
	//
	return rv;
}

