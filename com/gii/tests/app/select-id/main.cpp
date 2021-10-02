#include <QApplication>
#include <QTimer>
#include <QDir>
#include <QSettings>
#include <misc/qt/qt_utils.h>
#include <misc/qt/Globals.h>
#include <misc/gen/IniProfile.h>
#include <gii/qt/InformationSelectDialog.h>
#include <gii/qt/InformationIdEdit.h>
#include <gii/gen/Variable.h>
#include <gen/ResultData.h>
#if IS_WIN
#include <Windows.h>
#endif
#include "test-ini-content.h"

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

}

int main(int argc, char* argv[])
{
#if IS_WIN
	if (!sf::IsDebug())
	{
		// Removes the console in windows application.
		FreeConsole();
	}
#endif
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
	//
	if (false)
	{
		QDialog dlg;
		appSettings.restoreWindowRect("Dialog", &dlg);
		dlg.setLayout(new QVBoxLayout(&dlg));
		auto edit = new sf::InformationIdEdit(&dlg);
		edit->setTypeId(sf::Gii::ResultData);
		dlg.layout()->addWidget(edit);
		dlg.exec();
		appSettings.saveWindowRect("Dialog", &dlg);
	}
	else
	{
		sf::InformationSelectDialog isd;
		auto ids = isd.execute(sf::Gii::Multiple);
		qDebug() << ids;
	}
	// Remove all entries before deinitializing.
	qDeleteAll(list);
	list.clear();
	//
	sf::Variable::deinitialize();
	//
	sf::setGlobalSettings(nullptr);
	return 0;
}

