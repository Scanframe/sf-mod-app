#include <misc/qt/Resource.h>
#include "CodeEditorPropertyPage.h"
#include "ui_CodeEditorPropertyPage.h"

namespace sf
{

CodeEditorPropertyPage::CodeEditorPropertyPage(CodeEditorConfiguration& cfg, PropertySheetDialog* parent)
	:PropertyPage(parent)
	 , ui(new Ui::CodeEditorPropertyPage)
	, _configuration(cfg)
{
	ui->setupUi(this);
	// Update the control widgets before connecting signals.
	updatePage();
	// Connect the signals from the control widgets.
	connectControls();
}

CodeEditorPropertyPage::~CodeEditorPropertyPage()
{
	delete ui;
}

QIcon CodeEditorPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/code-editor", QPalette::ColorRole::Text);
}

QString CodeEditorPropertyPage::getPageName() const
{
	return tr("Code Editor");
}

QString CodeEditorPropertyPage::getPageDescription() const
{
	return tr("Syntax highlighted code editor.");
}

void CodeEditorPropertyPage::updatePage()
{
	ui->cbDarkMode->setChecked(_configuration.darkMode);
	ui->cbShowGutter->setChecked(_configuration.showGutter);
	ui->cbFontType->setEditText(_configuration.fontType);
	ui->sbFontSize->setValue(_configuration.fontSize);
	ui->cbUseHighLighting->setChecked(_configuration.useHighLighting);
}

bool CodeEditorPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= _configuration.darkMode != ui->cbDarkMode->isChecked();
	rv |= _configuration.showGutter != ui->cbShowGutter->isChecked();
	rv |= _configuration.fontType != ui->cbFontType->currentText();
	rv |= _configuration.fontSize != ui->sbFontSize->value();
	rv |= _configuration.useHighLighting != ui->cbUseHighLighting->isChecked();
	return rv;
}

void CodeEditorPropertyPage::applyPage()
{
	_configuration.darkMode = ui->cbDarkMode->isChecked();
	_configuration.showGutter = ui->cbShowGutter->isChecked();
	_configuration.fontType = ui->cbFontType->currentText();
	_configuration.fontSize = ui->sbFontSize->value();
	_configuration.save();
}

void CodeEditorPropertyPage::afterPageApply()
{
}

}
