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
	QCoreApplication::setOrganizationName("ScanFrame");
	QCoreApplication::setApplicationName("Multi Document Concept");
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	// InitializeBase using the application file path.
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
	QCommandLineParser parser;
	parser.setApplicationDescription("ScanFrame's MDI concept example.");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file", "The file to open.");
	parser.process(app);
	//
	MainWindow win;
	const QStringList posArgs = parser.positionalArguments();
	for (const QString& fileName: posArgs)
	{
		win.openFile(fileName);
	}
	win.show();
	return QApplication::exec();
}
