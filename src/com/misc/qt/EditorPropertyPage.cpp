#include "Resource.h"
#include "PropertySheetDialog.h"
#include "EditorPropertyPage.h"
#include "ui_EditorPropertyPage.h"

namespace sf
{

EditorPropertyPage::EditorPropertyPage(Editor::Configuration& cfg, PropertySheetDialog* parent)
	:PropertyPage(parent)
	 , ui(new Ui::EditorPropertyPage)
	, _configuration(cfg)
{
	ui->setupUi(this);
}

EditorPropertyPage::~EditorPropertyPage()
{
	delete ui;
}

QIcon EditorPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/code-editor", QPalette::ColorRole::Text);
}

QString EditorPropertyPage::getPageName() const
{
	return tr("Code Editor");
}

QString EditorPropertyPage::getPageDescription() const
{
	return tr("Syntax highlighted code editor.");
}

void EditorPropertyPage::updatePage()
{
	ui->cbDarkMode->setChecked(_configuration.darkMode);
	ui->cbShowGutter->setChecked(_configuration.showGutter);
	ui->cbFontType->setEditText(_configuration.fontType);
	ui->sbFontSize->setValue(_configuration.fontSize);
	ui->cbUseHighLighting->setChecked(_configuration.useHighLighting);
}

bool EditorPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= _configuration.darkMode != ui->cbDarkMode->isChecked();
	rv |= _configuration.showGutter != ui->cbShowGutter->isChecked();
	rv |= _configuration.fontType != ui->cbFontType->currentText();
	rv |= _configuration.fontSize != ui->sbFontSize->value();
	rv |= _configuration.useHighLighting != ui->cbUseHighLighting->isChecked();
	return rv;
}

void EditorPropertyPage::applyPage()
{
	_configuration.darkMode = ui->cbDarkMode->isChecked();
	_configuration.showGutter = ui->cbShowGutter->isChecked();
	_configuration.fontType = ui->cbFontType->currentText();
	_configuration.fontSize = ui->sbFontSize->value();
	_configuration.settingsReadWrite(true);
}

void EditorPropertyPage::afterPageApply(bool was_modified)
{
}

}
