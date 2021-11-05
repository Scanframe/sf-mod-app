#include <gii/gen/Variable.h>
#include <gii/gen/UnitConversionServer.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/FormDialog.h>
#include <QApplication>
#include <QTimer>
#include <QDir>
#if IS_WIN
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
#if IS_WIN
	if (!sf::IsDebug())
	{
		// Removes the console in windows application.
		FreeConsole();
	}
#endif

	QApplication app(argc, argv);
	// Ignore desktop settings because it gives the wrong icon colors.
	QApplication::setDesktopSettingsAware(false);
	// Initialize base using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	//
	sf::Variable::initialize();
	//
	const char* IniContent = R"(
[Metric]
m/s,1=km/h,3.6,0,1
m/s,2=km/h,3.6,0,2
m/s,3=km/h,3.6,0,3

[Imperial]
m/s,0="/s,39.37007874015748031496062992126,0,0
m/s,1="/s,39.37007874015748031496062992126,0,1
m/s,2="/s,39.37007874015748031496062992126,0,2
°C,1=°F,1.8,32,1
)";
	// Initialize the unit conversion interface.
	std::istringstream is(IniContent);
	sf::UnitConversionServer ucs;
	ucs.load(is);
	// MAke clients get The imperial conversion values.
	//ucs.setUnitSystem(sf::UnitConversionServer::usImperial);
	ucs.setUnitSystem(sf::UnitConversionServer::usMetric);
	// Variable scope.
	{
		// Create a server instance.
		sf::Variable v_server(std::string("0x1,High Speed,m/s,A,High speed velocity setting,FLOAT,FLOAT,0.1,10,0,20"));
		// Makes the server instance retrieve new conversion values.
		v_server.setConvertValues(true);
		// Set the file location to the resource.
		sf::FormDialog dlg;
		dlg.Load(QFile(":/ui/dialog"));
		dlg.exec();
	}
	//
	sf::Variable::deinitialize();
	//
	return 0;
}
