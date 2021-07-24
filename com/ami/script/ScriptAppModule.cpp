#include <misc/qt/ScriptHighlighter.h>
#include "ScriptAppModule.h"
#include "ScriptManagerPropertyPage.h"
#include "ScriptEditor.h"

namespace sf
{

ScriptAppModule::ScriptAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	 , _scriptManager(*new ScriptManager(params._settings, this))
{
	addFileType(tr("Script file"), ScriptManager::getFileSuffix());
	// Load the script manager configuration.
	_scriptManager.settingsReadWrite(false);
}

QString ScriptAppModule::getName() const
{
	return tr("Script");
}

QString ScriptAppModule::getDescription() const
{
	return tr("Script manager and editor.");
}

void ScriptAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	auto pp = new ScriptManagerPropertyPage(&_scriptManager, sheet);
	connect(pp, &ScriptManagerPropertyPage::openEditor, this, &ScriptAppModule::openEditor);
	sheet->addPage(pp);
}

QString ScriptAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

void ScriptAppModule::openEditor(int index)
{
	if (auto entry = _scriptManager.getEntry(index))
	{
		auto mdi = openFile(_scriptManager.getScriptFilePath(entry->getFilename()), this);
		if (auto se = dynamic_cast<ScriptEditor*>(mdi))
		{
			se->setInterpreter(entry->getInterpreter());
		}
	}
}

QString ScriptAppModule::getSvgIconResource() const
{
	return ":icon/svg/scripting";
}

MultiDocInterface* ScriptAppModule::createWidget(QWidget* parent) const
{
	auto se = new ScriptEditor(getSettings(), parent);
	se->getEditor()->setConfiguration(_configuration);
	return se;
}



}

