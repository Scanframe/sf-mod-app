#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "misc/qt/qt_utils.h"
#include "misc/qt/Globals.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
	//QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Make settings file available through a property.
	QCoreApplication::instance()->setProperty("SettingsFile", fi.absoluteFilePath());
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	// Set the plugin/module directory. For now not configurable.
	sf::setPluginDir(QCoreApplication::applicationDirPath() + QDir::separator() + "lib");

	//
	MainWindow win;
	win.show();
	qDebug() << "QT Ver: " << QT_VERSION_STR;
	return QApplication::exec();
}
