#include <gii/gen/Variable.h>
#include <gii/gen/UnitConversionServer.h>
#include <misc/qt/FormDialog.h>
#include <test/catch.h>
#include <QFileDialog>
#include <QCoreApplication>
#include <QTimer>
#include <QTemporaryDir>
#include <QStandardPaths>

extern int debug_level;

namespace
{

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

}

TEST_CASE("sf::FormDialog", "[debug]")
{

	sf::Variable::initialize();

	SECTION("Create UI Form")
	{
		// Initialize the unit conversion interface.
		std::istringstream is(IniContent);
		sf::UnitConversionServer ucs;
		ucs.load(is);
		// MAke clients get the Imperial conversion values.
		//ucs.setUnitSystem(sf::UnitConversionServer::usImperial);
		ucs.setUnitSystem(sf::UnitConversionServer::usMetric);
		// Create a server instance.
		sf::Variable v_server(std::string("0x1,High Speed,m/s,A,High speed velocity setting,FLOAT,FLOAT,0.1,10,0,20"));
		// Makes the server instance retrieve new conversion values.
		CHECK(v_server.setConvertValues(true));
		// Set the file location to the resource.
		QFileInfo fi(":/ui/test-formdialog");
		REQUIRE(fi.exists());
		//
		sf::FormDialog dlg;
		dlg.Load(QFile(fi.absoluteFilePath()));
		QTimer::singleShot(2000, [&]
		{
			QString fp = QDir::temp().filePath(QFileInfo(__FILE__).baseName() + ".png");
			if (debug_level)
			{
				qDebug() << "Saving grabbed content to: " << "file://" + fp;
			}
			CHECK(dlg.grab().save(fp));
			if (!debug_level)
			{
				dlg.close();
			}
		});
		dlg.exec();
	}

	sf::Variable::deinitialize();

}
