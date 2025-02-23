#include "gii/gen/UnitConversionServerEx.h"
#include "gii/qt/VariableEdit.h"
#include "misc/gen/IniProfile.h"
#include "misc/gen/string.h"
#include "misc/qt/ApplicationSettings.h"
#include "misc/qt/FormDialog.h"
#include "test-ini-content.h"
#include <QApplication>
#include <QCheckBox>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <gii/qt/InformationSelectDialog.h>
#include <gii/qt/VariableListModel.h>
#include <gii/qt/VariableTreeView.h>
#include <misc/gen/TPointer.h>
#include <misc/gen/dbgutils.h>
#include <misc/qt/Globals.h>
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

void loadFromIni(InformationTypes::Vector& rv)
{
	std::istringstream is(VariableIniContent);
	IniProfile ini(is);
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

struct VarHandler final : VariableHandler
{
		void variableEventHandler(EEvent event, const sf::Variable& call_var, sf::Variable& link_var, bool same_inst) override
		{
			switch (event)
			{

				case veValueChange:
					SF_RTTI_NOTIFY(DO_DEFAULT, "Change of '" << sf::stringf("0x%llX", link_var.getId()) << "' to: " << link_var.getCurString());
					break;

				case veUserPrivate:
					SF_RTTI_NOTIFY(DO_DEFAULT, "Toggle of '" << sf::stringf("0x%llX", link_var.getId()));
					break;

				default:
					break;
			}
		}
};

QDialog* createDialog(QWidget* parent, const InformationTypes::IdVector& ids)
{
	// Set the file location to the resource.
	const auto dlg = new sf::FormDialog(parent);
	// Make it modeless.
	dlg->setModal(false);
	dlg->load(QFile(":/ui/dialog"));
	if (const auto vtv = dlg->findChild<sf::VariableTreeView*>("vtvTable"))
	{
		for (auto id: ids)
		{
			vtv->variableListModel()->addVariable(id);
		}
	}
	return dlg;
};

}// namespace sf

int main(int argc, char* argv[])
{
	std::istringstream is(sf::UnitConversionIniContent);
	const auto ucs = std::make_unique<sf::UnitConversionServerEx>();
	ucs->load(is);
	// Set the unit system to use.
	ucs->setUnitSystem(sf::UnitConversionServer::usMetric);
	// Enable unit conversion.
	ucs->setEnableId(0x5);
	// Removes the console in Windows application.
	sf::freeConsole();
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
	// Fill the list with the available ids.
	sf::InformationTypes::IdVector ids;
	// Attach the handler to all variables.
	for (const auto ib: list)
	{
		if (const auto var = dynamic_cast<sf::Variable*>(ib))
		{
			ids.push_back(var->getId());
			var->setHandler(&handler);
		}
	}
	// Create self cleanup smart pointer instance for the dialog.
	sf::TPointer<QDialog> dlg(new QDialog());
	// Set an icon on the window.
	dlg->setWindowIcon(QIcon(":logo/ico/scanframe"));
	settings.restoreWindowRect("Dialog", dlg);
	dlg->setLayout(new QVBoxLayout(dlg));
	// ReSharper disable once CppDFAMemoryLeak
	const auto btn = new QPushButton(dlg);
	btn->setText("Create Resource &Dialog");
	dlg->layout()->addWidget(btn);
	btn->setAutoDefault(false);
	// ReSharper disable once CppDFAMemoryLeak
	const auto cb = new QCheckBox(dlg);
	cb->setText("Enable Checkbox");
	dlg->layout()->addWidget(cb);
	// ReSharper disable once CppDFAMemoryLeak
	const auto sb = new QSpinBox(dlg);
	const auto lbl = new QLabel(dlg);
	lbl->setBuddy(sb);
	lbl->setText("Name level");
	sb->setValue(0);
	sb->setMinimum(-10);
	sb->setMaximum(+10);
	const auto sbl = new QHBoxLayout(dlg);
	dlg->layout()->addItem(sbl);
	sbl->addWidget(lbl);
	sbl->addWidget(sb);
	//
	for (const auto id: {0x5, 0x99003, 0x99004, 0xb0030, 0xb0038})
	{
		const auto ve = new sf::VariableEdit(dlg);
		ve->setId(id);
		ve->setNameLevel(2);
		ve->setConverted(true);
		dlg->layout()->addWidget(ve);
	}
	const auto vtv = new sf::VariableTreeView(dlg);
	// Get the pointer to the attached list model.
	const auto vlm = vtv->variableListModel();
	// Show 2 name levels only.
	vlm->setNameLevel(sb->value());
	vtv->setRowCheckBox(false);
	vlm->addVariables(list);
	dlg->layout()->addWidget(vtv);
	//
	btn->connect(btn, &QPushButton::clicked, [&]() {
		const auto d = createDialog(dlg, ids);
		d->show();
	});
	//
	cb->connect(cb, &QCheckBox::checkStateChanged, [&](int state) { vtv->setRowCheckBox(cb->isChecked()); });
	//
	sb->connect(sb, &QSpinBox::valueChanged, [&](int value) { vtv->setNameLevel(value); });
	//
	// Only can set the column width after the model has been set to the tree view.
	settings.restoreTreeViewColumns("VarList", vtv);
	// Show the dialog before executing since it does only shows child dialogs as modal.
	dlg->exec();
	//
	settings.saveTreeViewColumns("VarList", vtv);
	settings.saveWindowRect("Dialog", dlg);
	// Remove all entries before un-initializing.
	qDeleteAll(list);
	// Clear the list.
	list.clear();
	// Signal all okay.
	return 0;
}
