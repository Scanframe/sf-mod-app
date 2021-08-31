#include <misc/qt/Resource.h>
#include "AcquisitionPropertyPage.h"
#include <rsa/iface/RsaInterface.h>
#include "ui_AcquisitionPropertyPage.h"

namespace sf
{

AcquisitionPropertyPage::AcquisitionPropertyPage(AcquisitionAppModule& am, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::AcquisitionPropertyPage)
	 , _am(am)
{
	ui->setupUi(this);

	ui->cbDeviceUt->addItem(tr("None"));
	ui->cbDeviceEt->addItem(tr("None"));
	for (size_t index = 0; index < RsaInterface::Interface().size(); index++)
	{
		auto display = QString(RsaInterface::Interface().getName(index)).append(": ").append(RsaInterface::Interface().getDescription(index));
		ui->cbDeviceUt->addItem(display, RsaInterface::Interface().getName(index));
		ui->cbDeviceEt->addItem(display, RsaInterface::Interface().getName(index));
	}
}

AcquisitionPropertyPage::~AcquisitionPropertyPage()
{
	delete ui;
}

QString AcquisitionPropertyPage::getPageName() const
{
	return tr("Acquisition");
}

QString AcquisitionPropertyPage::getPageDescription() const
{
	return tr("Configure of the acquisition device(s).");
}

QIcon AcquisitionPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/signal", QPalette::ColorRole::Text);
}

void AcquisitionPropertyPage::updatePage()
{
	ui->iieSwitchId->setText(QString("0x%1").arg(_am._vSwitch.getDesiredId(), 0, 16));
	ui->cbDeviceUt->setCurrentIndex(clip<int>(ui->cbDeviceUt->findData(_am._serverUtName), 0, 99));
	ui->cbDeviceEt->setCurrentIndex(clip<int>(ui->cbDeviceEt->findData(_am._serverEtName), 0, 99));
}

void AcquisitionPropertyPage::applyPage()
{
	_am._vSwitch.setup(ui->iieSwitchId->text().toLongLong(nullptr, 0));
	_am._serverUtName = ui->cbDeviceUt->currentData().toString();
	_am._serverEtName = ui->cbDeviceEt->currentData().toString();
}

bool AcquisitionPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= _am._vSwitch.getDesiredId() != ui->iieSwitchId->text().toLongLong(nullptr, 0);
	rv |= _am._serverUtName != ui->cbDeviceUt->currentData().toString();
	rv |= _am._serverEtName != ui->cbDeviceEt->currentData().toString();
	return rv;
}

void AcquisitionPropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_am.settingsReadWrite(true);
		_am.createDevices();
	}
}

void AcquisitionPropertyPage::stateSaveRestore(QSettings& settings, bool save)
{

}

}
