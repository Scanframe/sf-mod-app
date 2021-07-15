#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QTimer>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <misc/qt/qt_utils.h>
#include "MainWindow.h"

using namespace sf;

int main(int argc, char* argv[])
{
	Q_INIT_RESOURCE(resource);
	QApplication app(argc, argv);
	QApplication::setOrganizationName("ScanFrame");
	QApplication::setApplicationName("Modular Base Application");
	QApplication::setApplicationVersion(QT_VERSION_STR);
	QApplication::setWindowIcon(QIcon(":logo/ico/scanframe"));
	QApplication::setDesktopSettingsAware(false);
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath(), fi.completeBaseName() + ".ini");
	// Make settings file available through a property..
	QApplication::instance()->setProperty("SettingsFile", fi.absoluteFilePath());
	// Create instance to handle settings.
	ApplicationSettings app_settings;
	// Set the file path to the settings instance.
	app_settings.setFilepath(fi.absoluteFilePath());
	//
	QCommandLineParser parser;
	parser.setApplicationDescription("ScanFrame's MDI concept example.");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file", "The file to open.");
	parser.process(app);
	//
	auto settings = new QSettings(fi.absoluteFilePath(), QSettings::Format::IniFormat, QApplication::instance());
	//auto settings = new QSettings(QSettings::Scope::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
	MainWindow win(settings);
	app_settings.restoreWindowRect("MainWindow", &win);
	win.show();
	// Open files from the command line after the window is shown other wise the MDI depend menu is not updated.
	for (const QString& fileName: parser.positionalArguments())
	{
		win.openFile(fileName);
	}
	auto exit_code = QApplication::exec();
	app_settings.saveWindowRect("MainWindow", &win);
	return exit_code;
}
