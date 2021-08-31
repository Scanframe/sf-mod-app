#include "VariableIdPropertyPage.h"
#include "ui_VariableIdPropertyPage.h"

namespace sf
{

VariableIdPropertyPage::VariableIdPropertyPage(VariableWidgetBase* target, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::VariableIdPropertyPage)
	 , _target(target)
	 , _hasNameLevel(target->property("nameLevel").isValid())
{
	ui->setupUi(this);
	ui->sbNameLevel->setVisible(_hasNameLevel);
}

VariableIdPropertyPage::~VariableIdPropertyPage()
{
	delete ui;
}

QString VariableIdPropertyPage::getPageName() const
{
	return tr("Variable");
}

bool VariableIdPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->iieId->text() != _target->getVariableId();
	rv |= ui->cbReadOnly->isChecked() != _target->getReadOnly();
	rv |= ui->cbConverted->isChecked() != _target->getConverted();
	if (_hasNameLevel)
	{
		rv |= ui->sbNameLevel->value() != _target->property("nameLevel").toInt();
	}
	return rv;
}

void VariableIdPropertyPage::updatePage()
{
	ui->iieId->setText(_target->getVariableId());
	ui->cbReadOnly->setChecked(_target->getReadOnly());
	ui->cbConverted->setChecked(_target->getConverted());
	if (_hasNameLevel)
	{
		ui->sbNameLevel->setValue(_target->property("nameLevel").toInt());
	}
}

void VariableIdPropertyPage::applyPage()
{
	_target->setVariableId(ui->iieId->text());
	_target->setReadOnly(ui->cbReadOnly->isChecked());
	_target->setConverted(ui->cbConverted->isChecked());
	if (_hasNameLevel)
	{
		_target->setProperty("nameLevel", ui->sbNameLevel->value());
	}
}

}
