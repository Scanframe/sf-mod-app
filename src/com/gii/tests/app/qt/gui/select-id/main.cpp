#include "misc/qt/ApplicationSettings.h"
#include "test-ini-content.h"
#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QTimer>
#include <gii/gen/ResultData.h>
#include <gii/gen/Variable.h>
#include <gii/qt/InformationIdEdit.h>
#include <gii/qt/InformationSelectDialog.h>
#include <misc/gen/IniProfile.h>
#include <misc/gen/dbgutils.h>
#include <misc/qt/Globals.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

void loadFromIni(InformationTypes::Vector& rv)
{
	std::istringstream is(IniContent);
	sf::IniProfile ini(is);
	TVector<ResultData*> results;
	TVector<Variable*> variables;
	if (ini.selectSection("GenericParamInfo"))
	{
		int count = ini.getInt("Entries");
		for (int i = 0; i < count; i++)
		{
			auto sd = ini.getString(std::to_string(i));
			if (sd.empty())
			{
				continue;
			}
			auto def = Variable::getDefinition(sd);
			if (def._valid)
			{
				auto v = new Variable(def);
				variables.add(v);
				rv.append(v);
			}
		}
	}
	if (ini.selectSection("GenericResultInfo"))
	{
		int count = ini.getInt("Entries");
		for (int i = 0; i < count; i++)
		{
			auto sd = ini.getString(std::to_string(i));
			if (sd.empty())
			{
				continue;
			}
			auto def = ResultData::getDefinition(sd);
			if (def._valid)
			{
				auto r = new ResultData(def);
				results.add(r);
				rv.append(r);
			}
		}
	}
	if (ini.selectSection("GenericParamValue"))
	{
		for (auto v: variables)
		{
			auto key = "0x" + toUpper(itostr(v->getId(), 16));
			if (ini.keyExists(key))
			{
				auto value = ini.getString(key);
				if (!value.empty())
				{
					v->setCur(Value(unescape(value)));
				}
			}
		}
	}
}

}// namespace sf

int main(int argc, char* argv[])
{
	// Removes the console in Windows application.
	sf::freeConsole();
	// Ignore desktop settings because it gives the wrong icon colors.
	QApplication::setDesktopSettingsAware(false);
	//
	QApplication app(argc, argv);
	// Initialize base using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings appSettings;
	// Set the file path to the settings instance and make it watch changes.
	appSettings.setFilepath(fi.absoluteFilePath(), true);
	//
	auto settings = new QSettings(fi.absoluteFilePath(), QSettings::Format::IniFormat, &app);
	//
	sf::setGlobalSettings(settings);
	//
	sf::Variable::initialize();

	sf::InformationTypes::Vector list;
	sf::loadFromIni(list);
	// Just any argument will trigger a single select dialog.
	if (argc > 1)
	{
		QDialog dlg;
		// Set an icon on the window.
		dlg.setWindowIcon(QIcon(":logo/ico/scanframe"));
		appSettings.restoreWindowRect("Dialog", &dlg);
		dlg.setLayout(new QVBoxLayout(&dlg));
		auto edit = new sf::InformationIdEdit(&dlg);
		edit->setTypeId(sf::gii::ResultData);
		dlg.layout()->addWidget(edit);
		dlg.exec();
		appSettings.saveWindowRect("Dialog", &dlg);
	}
	else
	{
		sf::InformationSelectDialog isd;
		auto ids = isd.execute(sf::gii::Multiple);
		qDebug() << ids;
	}
	// Remove all entries before uninitializing.
	qDeleteAll(list);
	list.clear();
	//
	sf::Variable::uninitialize();
	//
	sf::setGlobalSettings(nullptr);
	return 0;
}
