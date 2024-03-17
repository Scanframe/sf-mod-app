#include <iostream>
#include <misc/gen/target.h>
#include <test/catch.h>

// Some user variable you want to be able to set
int debug_level = 0;

#if IS_QT
	#if IS_WIN
		#include "misc/win/win_utils.h"
	#endif
	#include <QApplication>
	#include <QDir>
	#include <QTimer>
	#include <misc/qt/Globals.h>
	#include <misc/qt/qt_utils.h>
#endif

int main(int argc, char* argv[])
{
#if IS_QT
	// Display environment variable in only available at a Linux OS.
	// Get the DISPLAY environment
	auto display = qgetenv("DISPLAY");
	#if !IS_WIN
	std::unique_ptr<QCoreApplication> app((display.length() > 0) ? new QApplication(argc, argv) : new QCoreApplication(argc, argv));
	#else
	std::unique_ptr<QCoreApplication> app((sf::isRunningWine() && display.length() > 0) ? new QApplication(argc, argv) : new QCoreApplication(argc, argv));
	#endif
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Make settings file available through a property.
	QCoreApplication::instance()->setProperty("SettingsFile", fi.absoluteFilePath());
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	// Set the plugin/module directory. For now not configurable.
	sf::setPluginDir(QCoreApplication::applicationDirPath() + QDir::separator() + "lib");
#endif
	// Function calling catch command line processor.
	auto func = [&]() -> int {
		// There must be exactly one instance
		Catch::Session session;
		// Build a new parser on top of Catch's
		using namespace Catch::Clara;
		auto cli
			// Get Catch's composite command line parser
			= session.cli()
			// bind variable to a new option, with a hint string
			| Opt(debug_level, "level")
					// the option names it will respond to
					["--debug"]
			// description string for the help output
			("Custom option for a debug level.");
		// Now pass the new composite back to Catch, so it uses that.
		session.cli(cli);
		// Let Catch (using Clara) parse the command line
		int returnCode = session.applyCommandLine(argc, argv);
		if (returnCode != 0)
		{
			// Indicates a command line error
#if IS_QT
			QCoreApplication::instance()->exit(returnCode);
#endif
			return returnCode;
		}
		else
		{
			auto rv = session.run();
#if IS_QT
			QCoreApplication::instance()->exit(rv);
#endif
			return rv;
		}
	};
#if IS_QT
	QTimer::singleShot(0, func);
	//
	QCoreApplication::instance()->exec();
#else
	return func();
#endif
}
