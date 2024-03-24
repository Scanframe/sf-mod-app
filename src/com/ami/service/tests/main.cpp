#include <test/catch.h>
#include <misc/gen/target.h>

// Some user variable you want to be able to set
int debug_level = 0;

#if IS_QT
#include <QApplication>
#include <QTimer>
#include <QDir>
#include <misc/qt/qt_utils.h>
#endif

int main(int argc, char* argv[])
{
#if IS_QT
	QApplication app(argc, argv);
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
#endif
	// Function calling catch command line processor.
	auto func = [&]()->int
	{
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
		// Now pass the new composite back to Catch so it uses that
		session.cli(cli);
		// Let Catch (using Clara) parse the command line
		int returnCode = session.applyCommandLine(argc, argv);
		if (returnCode != 0)
		{
			// Indicates a command line error
#if IS_QT
			QApplication::exit(returnCode);
#endif
			return returnCode;
		}
		else
		{
			auto rv = session.run();
#if IS_QT
		QApplication::exit(rv);
#endif
			return rv;
		}
	};
#if IS_QT
	QTimer::singleShot(0, func);
	//
	return QCoreApplication::exec();
#else
	return func();
#endif

}

