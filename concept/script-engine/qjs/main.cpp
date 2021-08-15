#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include <misc/qt/qt_utils.h>
#include <misc/gen/dbgutils.h>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
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
	MainWindow win;
	win.show();
	qDebug() << "QT Ver: " << QT_VERSION_STR;
	return QApplication::exec();
}
