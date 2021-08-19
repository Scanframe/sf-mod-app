#include "WidgetPropertyPage.h"
#include "ui_WidgetPropertyPage.h"

namespace sf
{

WidgetPropertyPage::WidgetPropertyPage(QWidget* target, QWidget* parent)
:PropertyPage(parent)
,	ui(new Ui::WidgetPropertyPage)
,_target(target)
{
	ui->setupUi(this);
}

WidgetPropertyPage::~WidgetPropertyPage()
{
	delete ui;
}

QString WidgetPropertyPage::getPageName() const
{
	return tr("Widget");
}

bool WidgetPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->pteStyleSheet->toPlainText() != _target->styleSheet();
	rv |= ui->leObjectName->text() != _target->objectName();
	return rv;
}

void WidgetPropertyPage::updatePage()
{
	ui->pteStyleSheet->setPlainText(_target->styleSheet());
	ui->leObjectName->setText(_target->objectName());
}

void WidgetPropertyPage::applyPage()
{
	_target->setStyleSheet(ui->pteStyleSheet->toPlainText());
	_target->setObjectName(ui->leObjectName->text());
}

}
