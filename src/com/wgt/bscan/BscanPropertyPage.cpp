#include "BscanPropertyPage.h"
#include "ui_BscanPropertyPage.h"
#include <misc/qt/Resource.h>

namespace sf
{

BscanPropertyPage::BscanPropertyPage(BscanGraph* target, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::BscanPropertyPage)
	 , _target(target)
	 ,_idPropertyList(target)
{
	ui->setupUi(this);
	_idPropertyList.add("idAmplitudeUnit", ui->iieIdAmplitudeUnit);
	_idPropertyList.add("idAttenuation", ui->iieIdAttenuation);
	_idPropertyList.add("idTimeUnit", ui->iieIdTimeUnit);
	_idPropertyList.add("idDelay", ui->iieIdDelay);
	_idPropertyList.add("idRange", ui->iieIdRange);
	_idPropertyList.add("idData", ui->iieIdData);
}

BscanPropertyPage::~BscanPropertyPage()
{
	delete ui;
}

QString BscanPropertyPage::getPageName() const
{
	return tr("Bscan Graph");
}

QIcon BscanPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Graph), QPalette::Text);
}

bool BscanPropertyPage::isPageModified() const
{
	return _idPropertyList.isModified();
}

void BscanPropertyPage::updatePage()
{
	_idPropertyList.update();
}

void BscanPropertyPage::applyPage()
{
	_idPropertyList.apply();
}

}
