#include <QApplication>
#include <QTimer>
#include <QDir>
#include <misc/qt/qt_utils.h>
#include <gen/Variable.h>
#include <gen/UnitConversionServer.h>
#include <qt/FormDialog.h>
#include <test-GraphWindow.h>
#if IS_WIN
#include <Windows.h>
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
	// Initialize base using the application file path.
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
	sf::Variable::initialize();
	//
	QWidget* win{nullptr};
	//
	QTimer::singleShot(0, [&]
	{
		if (false)
		{
			const char* IniContent = R"(
[System Metric]
m/s|1=km/h,3.6,0,1
m/s|2=km/h,3.6,0,2
m/s|3=km/h,3.6,0,3

[System Imperial]
m/s|0="/s,39.37007874015748031496062992126,0,0
m/s|1="/s,39.37007874015748031496062992126,0,1
m/s|2="/s,39.37007874015748031496062992126,0,2
°C|1=°F,1.8,32,1
)";
			// Initialize the unit conversion interface.
			std::istringstream is(IniContent);
			sf::UnitConversionServer ucs(is);
			// MAke clients get The imperial conversion values.
			//ucs.setUnitSystem(sf::UnitConversionServer::usImperial);
			ucs.setUnitSystem(sf::UnitConversionServer::usMetric);
			// Create a server instance.
			sf::Variable v_server(std::string("0x1,High Speed,m/s,A,High speed velocity setting,FLOAT,FLOAT,0.1,10,0,20"));
			// Makes the server instance retrieve new conversion values.
			v_server.setConvertValues(true);
			// Set the file location to the resource.
			QFileInfo fi(":/ui/dialog");
			fi.exists();
			//
			sf::FormDialog dlg;
			dlg.Load(QFile(fi.absoluteFilePath()));
			dlg.exec();
		}
		else
		{
			win = new GraphWindow();
			settings.restoreWindowRect("MainWindow", win);
			win->show();
		}
	});
	//
	auto rv = QCoreApplication::exec();
	//
	settings.saveWindowRect("MainWindow", win);
	//
	sf::delete_null(win);
	//
	sf::Variable::deinitialize();
	//
	return rv;
}

