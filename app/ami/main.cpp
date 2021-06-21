#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <misc/qt/qt_utils.h>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
	Q_INIT_RESOURCE(resource);

	QApplication app(argc, argv);
	QApplication::setOrganizationName("ScanFrame");
	QApplication::setApplicationName("Modular Base Application");
	QApplication::setApplicationVersion(QT_VERSION_STR);
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath(), fi.completeBaseName() + ".ini");
	// Make settings file available through a property..
	QApplication::instance()->setProperty("SettingsFile", fi.absoluteFilePath());
	// Create instance to handle settings.
	sf::ApplicationSettings app_settings;
	// Set the file path to the settings instance and make it watch changes.
	app_settings.setFilepath(fi.absoluteFilePath(), true);
	//
	QCommandLineParser parser;
	parser.setApplicationDescription("ScanFrame's MDI concept example.");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file", "The file to open.");
	parser.process(app);
	//
	QSettings settings(fi.absoluteFilePath(), QSettings::IniFormat);
	//
	sf::MainWindow win(settings);
	const QStringList posArgs = parser.positionalArguments();
	for (const QString& fileName: posArgs)
	{
		win.openFile(fileName);
	}
	app_settings.restoreWindowRect("MainWindow", &win);
	win.show();
	auto exit_code = QApplication::exec();
	app_settings.saveWindowRect("MainWindow", &win);
	return exit_code;
}
