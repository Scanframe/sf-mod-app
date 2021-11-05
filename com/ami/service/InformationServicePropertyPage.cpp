#include "InformationServicePropertyPage.h"
#include "ui_InformationServicePropertyPage.h"
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

InformationServicePropertyPage::InformationServicePropertyPage(InformationServiceAppModule& ism, QWidget* parent)
	:PropertyPage(parent), ui(new Ui::InformationServicePropertyPage)
	 , _ism(ism)
{
	ui->setupUi(this);
	ui->cbServiceType->addItems({"None", "Client", "Server", "Client + Server (debug)"});
}

InformationServicePropertyPage::~InformationServicePropertyPage()
{
	delete ui;
}

QString InformationServicePropertyPage::getPageName() const
{
	return tr("Information Service");
}

QString InformationServicePropertyPage::getPageDescription() const
{
	return tr("Configure the information service (client or server).");
}

QIcon InformationServicePropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/service", QPalette::ColorRole::Text);
}

void InformationServicePropertyPage::updatePage()
{
	ui->cbServiceType->setCurrentIndex(_ism._informationService.getType());
	ui->iieIdOffset->setId(_ism._informationService.getIdOffset());
}

void InformationServicePropertyPage::applyPage()
{
	_ism._informationService.setType((InformationService::EServiceType)ui->cbServiceType->currentIndex());
	_ism._informationService.setIdOffset(ui->iieIdOffset->getId());
}

bool InformationServicePropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->cbServiceType->currentIndex() != _ism._informationService.getType();
	rv |= ui->iieIdOffset->getId() != _ism._informationService.getIdOffset();
	return rv;
}

void InformationServicePropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_ism.settingsReadWrite(true);
	}
}

}
