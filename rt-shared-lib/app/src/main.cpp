#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include <misc/qt_utils.h>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
	//QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	//
	QFileInfo fi(QCoreApplication::applicationFilePath());
	sf::QApplicationSettings settings;
	settings.setFilepath(fi.absoluteDir().absolutePath() + QDir::separator() + fi.completeBaseName() + ".ini", true);
	//
	MainWindow w;
	w.show();
	return QApplication::exec();
}



