#include "misc/gen/IniProfile.h"
#include "misc/gen/string.h"
#include "test-ini-content.h"
#include <QApplication>
#include <QDir>
#include <QHeaderView>
#include <QTimer>
#include <gii/qt/VariableListModel.h>
#include <misc/gen/dbgutils.h>
#include <misc/qt/Globals.h>
#include <misc/qt/qt_utils.h>
#if IS_WIN
	#include <windows.h>
#endif

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

struct VarHandler : sf::VariableHandler
{

		void variableEventHandler(sf::Variable::EEvent event, const sf::Variable& call_var, sf::Variable& link_var, bool same_inst) override
		{
			switch (event)
			{
				case veValueChange:
					SF_RTTI_NOTIFY(DO_DEFAULT, "Change of '" << sf::stringf("0x%llX", link_var.getId()) << "' to: " << link_var.getCurString());
					break;
					/*

				case veFlagsChange:
					break;

				case veDesiredId:
					break;

				case veConverted:
					break;

*/
				default:
					break;
			}
		}
};

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
	sf::VarHandler handler;
	//
	for (auto ib: list)
	{
		auto var = dynamic_cast<sf::Variable*>(ib);
		if (var)
		{
			var->setHandler(&handler);
		}
	}

	//
	QDialog dlg;
	// Set an icon on the window.
	dlg.setWindowIcon(QIcon(":logo/ico/scanframe"));
	settings.restoreWindowRect("Dialog", &dlg);
	dlg.setLayout(new QVBoxLayout(&dlg));
	auto tv = new QTreeView(&dlg);
	//tv->setSelectionBehavior(QAbstractItemView::SelectRows);
	tv->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tv->setIndentation(0);
	//tv->header()->hide();
	auto vlm = new sf::VariableListModel(&dlg);
	vlm->addVariables(list);
	tv->setModel(vlm);
	// Only can set the column width after the model has been set to the tree view.
	settings.restoreTreeViewColumns("VarList", tv);
	// Set the delegates of the model onto the view for editing.
	vlm->setDelegates(tv);
	tv->setAlternatingRowColors(true);
	dlg.layout()->addWidget(tv);
	dlg.exec();
	//
	settings.saveWindowRect("Dialog", &dlg);
	settings.saveTreeViewColumns("VarList", tv);
	// Remove all entries before un-initializing.
	qDeleteAll(list);
	// Clear the list.
	list.clear();
	return 0;
}
