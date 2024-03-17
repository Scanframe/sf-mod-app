#include <QApplication>
#include <QDir>
#include <QTimer>
#include <misc/gen/gen_utils.h>
#include <misc/qt/qt_utils.h>
#include <test-ScriptWindow.h>
#if IS_WIN
	#include <windows.h>
#endif

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
	auto win = new ScriptWindow(settings);
	//
	win->show();
	//
	auto rv = QCoreApplication::exec();
	//
	delete win;
	//
	return rv;
}
