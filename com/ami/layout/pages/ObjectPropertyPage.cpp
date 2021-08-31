#include "ObjectPropertyPage.h"
#include "ui_ObjectPropertyPage.h"

namespace sf
{

ObjectPropertyPage::ObjectPropertyPage(QObject* target, QWidget* parent)
:PropertyPage(parent)
,	ui(new Ui::ObjectPropertyPage)
,_target(target)
{
	ui->setupUi(this);
}

ObjectPropertyPage::~ObjectPropertyPage()
{
	delete ui;
}

QString ObjectPropertyPage::getPageName() const
{
	return tr("Object");
}

bool ObjectPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->leObjectName->text() != _target->objectName();
	return rv;
}

void ObjectPropertyPage::updatePage()
{
	ui->leObjectName->setText(_target->objectName());
}

void ObjectPropertyPage::applyPage()
{
	_target->setObjectName(ui->leObjectName->text());
}

}
