#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include <misc/qt_utils.h>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QFileInfo fi(QCoreApplication::applicationFilePath());
	QString path = fi.absoluteDir().absolutePath() + QDir::separator() + fi.baseName() + ".ini";
	sf::setApplicationStyle(path);
	MainWindow w;
	w.show();
	return QApplication::exec();
}



