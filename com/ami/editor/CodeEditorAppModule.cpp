#include "CodeEditorAppModule.h"
#include "CodeEditor.h"
#include "CodeEditorConfiguration.h"
#include "CodeEditorPropertyPage.h"

namespace sf
{

CodeEditorAppModule::CodeEditorAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	,_configuration(getSettings())
{
	addFileType("text/javascript");
	addFileType(tr("Javascript"), "js");
	// Load the configuration settings.
	_configuration.load();
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
	sheet->addPage(new CodeEditorPropertyPage(_configuration, sheet));
}

MultiDocInterface* CodeEditorAppModule::createWidget(QWidget* parent) const
{
	return new CodeEditor(_configuration, parent);
}

QString CodeEditorAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString CodeEditorAppModule::getSvgIconResource() const
{
	return ":icon/svg/code-editor";
}


}

