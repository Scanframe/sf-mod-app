#include <misc/qt/Resource.h>
#include <rsa/iface/RsaInterface.h>
#include <sto/iface/StorageInterface.h>
#include "ProjectPropertyPage.h"
#include "ui_ProjectPropertyPage.h"

namespace sf
{

ProjectPropertyPage::ProjectPropertyPage(ProjectAppModule& am, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::ProjectPropertyPage)
	 , _am(am)
{
	ui->setupUi(this);
	for (auto cb: {ui->cbDeviceUt, ui->cbDeviceEt, ui->cbDeviceMotion, ui->cbStorage})
	{
		cb->addItem(tr("None"));
	}
	for (size_t index = 0; index < RsaInterface::Interface().size(); index++)
	{
		auto display = QString(RsaInterface::Interface().getName(index)).append(": ").append(RsaInterface::Interface().getDescription(index));
		ui->cbDeviceUt->addItem(display, RsaInterface::Interface().getName(index));
		ui->cbDeviceEt->addItem(display, RsaInterface::Interface().getName(index));
	}
	for (size_t index = 0; index < StorageInterface::Interface().size(); index++)
	{
		auto display = QString(StorageInterface::Interface().getName(index)).append(": ").append(StorageInterface::Interface().getDescription(index));
		ui->cbStorage->addItem(display, StorageInterface::Interface().getName(index));
	}
}

ProjectPropertyPage::~ProjectPropertyPage()
{
	delete ui;
}

QString ProjectPropertyPage::getPageName() const
{
	return tr("Project");
}

QString ProjectPropertyPage::getPageDescription() const
{
	return tr("Configure the acquisition, motion controller devices and storage.");
}

QIcon ProjectPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/signal", QPalette::ColorRole::Text);
}

void ProjectPropertyPage::updatePage()
{
	ui->cbDeviceUt->setCurrentIndex(clip<int>(ui->cbDeviceUt->findData(_am._serverUtName), 0, 99));
	ui->cbDeviceEt->setCurrentIndex(clip<int>(ui->cbDeviceEt->findData(_am._serverEtName), 0, 99));
	ui->cbDeviceMotion->setCurrentIndex(clip<int>(ui->cbDeviceMotion->findData(_am._serverMotionName), 0, 99));
	ui->cbStorage->setCurrentIndex(clip<int>(ui->cbStorage->findData(_am._serverStorageName), 0, 99));
}

void ProjectPropertyPage::applyPage()
{
	_am._serverUtName = ui->cbDeviceUt->currentData().toString();
	_am._serverEtName = ui->cbDeviceEt->currentData().toString();
	_am._serverMotionName = ui->cbDeviceMotion->currentData().toString();
	_am._serverStorageName = ui->cbDeviceEt->currentData().toString();
}

bool ProjectPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= _am._serverUtName != ui->cbDeviceUt->currentData().toString();
	rv |= _am._serverEtName != ui->cbDeviceEt->currentData().toString();
	rv |= _am._serverMotionName != ui->cbDeviceMotion->currentData().toString();
	rv |= _am._serverStorageName != ui->cbStorage->currentData().toString();
	return rv;
}

void ProjectPropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_am.settingsReadWrite(true);
		_am.createDevices();
	}
}

void ProjectPropertyPage::stateSaveRestore(QSettings& settings, bool save)
{
}

}
