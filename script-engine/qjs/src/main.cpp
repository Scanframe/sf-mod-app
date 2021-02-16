#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include <misc/qt_utils.h>
#include <misc/dbgutils.h>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
	//QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	// Initialize using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath(), fi.completeBaseName() + ".ini");
	// Make settings file available through a property..
	QCoreApplication::instance()->setProperty("SettingsFile", fi.absoluteFilePath());
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	//
	MainWindow w;
	w.show();
	qDebug() << "QT Ver: " << QT_VERSION_STR;
	return QApplication::exec();
}