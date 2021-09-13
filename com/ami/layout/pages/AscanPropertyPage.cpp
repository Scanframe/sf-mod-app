#include "AscanPropertyPage.h"
#include "ui_AscanPropertyPage.h"

namespace sf
{

AscanPropertyPage::AscanPropertyPage(AscanGraph* target, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::AscanPropertyPage)
	 , _target(target)
	 ,_idPropertyList(target)
{
	ui->setupUi(this);
	_idPropertyList.add("idTimeUnit", ui->iieIdTimeUnit);
	_idPropertyList.add("idAmplitudeUnit", ui->iieIdAmplitudeUnit);
	_idPropertyList.add("idAttenuation", ui->iieIdAttenuation);
	_idPropertyList.add("idData", ui->iieIdData);
}

AscanPropertyPage::~AscanPropertyPage()
{
	delete ui;
}

QString AscanPropertyPage::getPageName() const
{
	return tr("Ascan Graph");
}

bool AscanPropertyPage::isPageModified() const
{
	return _idPropertyList.isModified();
}

void AscanPropertyPage::updatePage()
{
	_idPropertyList.update();
}

void AscanPropertyPage::applyPage()
{
	_idPropertyList.apply();
}

}
