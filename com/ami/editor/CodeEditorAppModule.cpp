#include "CodeEditorAppModule.h"
#include "CodeEditor.h"

namespace sf
{

CodeEditorAppModule::CodeEditorAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
{
	addFileType("text/javascript");
	addFileType(tr("Javascript"), "js");
}

QString CodeEditorAppModule::getName() const
{
	return tr("Code Editor");
}

QString CodeEditorAppModule::getDescription() const
{
	return tr("Syntax highlighted code editor.");
}

void CodeEditorAppModule::addPropertySheets(QWidget*)
{
}

MultiDocInterface* CodeEditorAppModule::createWidget(QWidget* parent) const
{
	return new CodeEditor(parent);
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

