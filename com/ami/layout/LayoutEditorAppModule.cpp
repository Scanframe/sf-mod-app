#include <misc/qt/PropertySheetDialog.h>
#include "LayoutEditorAppModule.h"
#include "LayoutEditor.h"
#include "LayoutEditorPropertyPage.h"

namespace sf
{

LayoutEditorAppModule::LayoutEditorAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
{
	addFileType(tr("UI Layout File"), "ui");
}

QString LayoutEditorAppModule::getName() const
{
	return tr("Layout Editor");
}

QString LayoutEditorAppModule::getDescription() const
{
	return tr("UI layout editor.");
}

void LayoutEditorAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new LayoutEditorPropertyPage(*this, sheet));
}

MultiDocInterface* LayoutEditorAppModule::createWidget(QWidget* parent) const
{
	return new LayoutEditor(getSettings(), parent);
}

QString LayoutEditorAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString LayoutEditorAppModule::getSvgIconResource() const
{
	return ":icon/svg/layout";
}

void LayoutEditorAppModule::settingsReadWrite(bool save)
{

}

}
