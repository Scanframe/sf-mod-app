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
	//QApplication::setApplicationName("Modular Application");
	QApplication::setApplicationDisplayName("Modular Base Application");
	QApplication::setApplicationVersion(QT_VERSION_STR);
	QApplication::setWindowIcon(QIcon(":logo/ico/scanframe"));
	QApplication::setDesktopSettingsAware(false);
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Make settings file available through a property.
	QApplication::instance()->setProperty("ConfigDir", fi.absolutePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath(), fi.completeBaseName() + ".ini");
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

	win.show();
	// Open files from the command line after the window is shown otherwise the MDI dependent menu is not updated.
	for (const QString& fileName: parser.positionalArguments())
	{
		win.openFile(fileName);
	}
	return QApplication::exec();
}
