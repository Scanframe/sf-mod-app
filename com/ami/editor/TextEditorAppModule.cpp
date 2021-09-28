#include "TextEditorAppModule.h"
#include "TextEditor.h"

namespace sf
{

TextEditorAppModule::TextEditorAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
{
	// The order determines which double entry gets into open file dialog filter list.
	addFileType("text/plain");
	addFileType(tr("Text Document"), "txt");
	addFileType("text/ini");
	addFileType(tr("INI Config"), "ini");
}

QString TextEditorAppModule::getName() const
{
	return tr("Text Editor");
}

QString TextEditorAppModule::getDescription() const
{
	return tr("Simple text editor.");
}

void TextEditorAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
}

MultiDocInterface* TextEditorAppModule::createWidget(QWidget* parent) const
{
	return new TextEditor(parent);
}

QString TextEditorAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString TextEditorAppModule::getSvgIconResource() const
{
	return ":icon/svg/text-editor";
}

void TextEditorAppModule::initialize(InitializeStage stage)
{
}

}
