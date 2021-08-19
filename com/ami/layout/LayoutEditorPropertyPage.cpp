#include <misc/qt/Resource.h>
#include "LayoutEditorPropertyPage.h"
#include "ui_LayoutEditorPropertyPage.h"

namespace sf
{

LayoutEditorPropertyPage::LayoutEditorPropertyPage(LayoutEditorAppModule& ucm, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::LayoutEditorPropertyPage)
	 , _lem(ucm)
{
	ui->setupUi(this);
}

LayoutEditorPropertyPage::~LayoutEditorPropertyPage()
{
	delete ui;
}

QString LayoutEditorPropertyPage::getPageName() const
{
	return tr("Layout Editor");
}

QString LayoutEditorPropertyPage::getPageDescription() const
{
	return tr("Configure the layout editor.");
}

QIcon LayoutEditorPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/layout", QPalette::ColorRole::Text);
}

void LayoutEditorPropertyPage::updatePage()
{
	ui->cbReadOnly->setChecked(_lem._readOnly);
}

void LayoutEditorPropertyPage::applyPage()
{
	_lem._readOnly = ui->cbReadOnly->isChecked();

}

bool LayoutEditorPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= _lem._readOnly != ui->cbReadOnly->isChecked();
	return rv;
}

void LayoutEditorPropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_lem.settingsReadWrite(true);
	}
}

void LayoutEditorPropertyPage::stateSaveRestore(QSettings& settings, bool save)
{
/*
	if (save)
	{
		settings.setValue(ui->tabWidget->objectName(), ui->tabWidget->currentIndex());
	}
	else
	{
		auto index = settings.value(ui->tabWidget->objectName(), ui->tabWidget->currentIndex()).toInt();
		ui->tabWidget->setCurrentIndex(index);
	}
*/
}

}
