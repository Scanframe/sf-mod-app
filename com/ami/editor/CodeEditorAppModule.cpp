#include <misc/qt/EditorPropertyPage.h>
#include "CodeEditorAppModule.h"
#include "JsHighlighter.h"
#include "CodeEditor.h"

namespace sf
{

CodeEditorAppModule::CodeEditorAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	,_configuration(getSettings())
{
	addFileType("text/javascript");
	addFileType(tr("Javascript"), "js");
	// Load the configuration settings.
	_configuration.settingsReadWrite(false);
}

QString CodeEditorAppModule::getName() const
{
	return tr("Code Editor");
}

QString CodeEditorAppModule::getDescription() const
{
	return tr("Syntax highlighted code editor settings.");
}

void CodeEditorAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new EditorPropertyPage(_configuration, sheet));
}

MultiDocInterface* CodeEditorAppModule::createWidget(QWidget* parent) const
{
	auto ce = new CodeEditor(parent);
	ce->setConfiguration(_configuration);
	// Assign highlighter to the underlying document.
	if (_configuration.useHighLighting)
	{
		new JsHighlighter(ce->document(), _configuration.darkMode);
	}
	return ce;
}

QString CodeEditorAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString CodeEditorAppModule::getSvgIconResource() const
{
	return ":icon/svg/code-editor";
}

void CodeEditorAppModule::initialize(bool)
{
}

}

