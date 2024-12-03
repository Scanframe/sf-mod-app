#include <QApplication>
#include <QDir>
#include <QTimer>
#include <misc/gen/dbgutils.h>
#include <misc/qt/Globals.h>
#include <misc/qt/qt_utils.h>
#include <gii/qt/VariableListModel.h>
#if IS_WIN
	#include <windows.h>
#endif

#include "gii/gen/InformationBase.h"
#include "gii/gen/Variable.h"
#include "misc/gen/IniProfile.h"
#include "misc/gen/string.h"
#include "test-ini-content.h"

namespace sf
{

void loadFromIni(InformationTypes::Vector& rv)
{
	std::istringstream is(IniContent);
	sf::IniProfile ini(is);
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
#if IS_WIN
	if (!sf::isDebug())
	{
		// Removes the console in windows application.
		FreeConsole();
	}
#endif
	// Ignore desktop settings because it gives the wrong icon colors.
	QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	// Initialize base using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	// Set the plugin/module directory. For now not configurable.
	sf::setPluginDir(QCoreApplication::applicationDirPath() + QDir::separator() + "lib");
	//
	sf::InformationTypes::Vector list;
	sf::loadFromIni(list);
	//
	QDialog dlg;
	settings.restoreWindowRect("Dialog", &dlg);
	dlg.setLayout(new QVBoxLayout(&dlg));
	auto tv = new QTreeView(&dlg);
	auto vlm = new sf::VariableListModel(&dlg);
	tv->setModel(vlm);
	tv->setAlternatingRowColors(true);
	dlg.layout()->addWidget(tv);
	dlg.exec();
	//
	settings.saveWindowRect("Dialog", &dlg);
	// Remove all entries before uninitializing.
	qDeleteAll(list);
	// Clear the list.
	list.clear();
	return 0;
}
