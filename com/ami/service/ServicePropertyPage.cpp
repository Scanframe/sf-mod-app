#include "ServicePropertyPage.h"
#include "InformationService.h"
#include "ui_ServicePropertyPage.h"
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

ServicePropertyPage::ServicePropertyPage(ServiceAppModule& ism, QWidget* parent)
	:PropertyPage(parent), ui(new Ui::ServicePropertyPage)
	 , _ism(ism)
{
	ui->setupUi(this);
	ui->cbServiceType->addItems({"None", "Client", "Server", "Client + Server (debug)"});
}

ServicePropertyPage::~ServicePropertyPage()
{
	delete ui;
}

QString ServicePropertyPage::getPageName() const
{
	return tr("Information Service");
}

QString ServicePropertyPage::getPageDescription() const
{
	return tr("Configure the information service (client or server).");
}

QIcon ServicePropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/service", QPalette::ColorRole::Text);
}

void ServicePropertyPage::updatePage()
{
	if (_ism._informationService)
	{
		ui->cbServiceType->setCurrentIndex(_ism._informationService->getType());
		ui->iieIdOffset->setId(_ism._informationService->getIdOffset());
	}
}

void ServicePropertyPage::applyPage()
{
	if (_ism._informationService)
	{
		_ism._informationService->setType((InformationService::EServiceType) ui->cbServiceType->currentIndex());
		_ism._informationService->setIdOffset(ui->iieIdOffset->getId());
	}
}

bool ServicePropertyPage::isPageModified() const
{
	bool rv = false;
	if (_ism._informationService)
	{
		rv |= ui->cbServiceType->currentIndex() != _ism._informationService->getType();
		rv |= ui->iieIdOffset->getId() != _ism._informationService->getIdOffset();
	}
	return rv;
}

void ServicePropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_ism.settingsReadWrite(true);
	}
}

}
