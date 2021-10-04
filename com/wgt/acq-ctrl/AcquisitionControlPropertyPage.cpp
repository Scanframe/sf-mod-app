#include "AcquisitionControlPropertyPage.h"
#include "ui_AcquisitionControlPropertyPage.h"

namespace sf
{

AcquisitionControlPropertyPage::AcquisitionControlPropertyPage(AcquisitionControl* target, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::AcquisitionControlPropertyPage)
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

AcquisitionControlPropertyPage::~AcquisitionControlPropertyPage()
{
	delete ui;
}

QString AcquisitionControlPropertyPage::getPageName() const
{
	return tr("Acquisition Control");
}

bool AcquisitionControlPropertyPage::isPageModified() const
{
	return _idPropertyList.isModified();
}

void AcquisitionControlPropertyPage::updatePage()
{
	_idPropertyList.update();
}

void AcquisitionControlPropertyPage::applyPage()
{
	_idPropertyList.apply();
}

}
