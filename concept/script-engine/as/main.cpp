#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include <misc/qt/qt_utils.h>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
	qDebug() << "QT Ver: " << QT_VERSION_STR;
	//QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	//
	MainWindow w;
	w.show();
	return QApplication::exec();}
