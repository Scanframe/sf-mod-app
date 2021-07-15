#include <QVariant>
#include <misc/qt/Resource.h>
#include "ApplicationPropertyPage.h"
#include "ui_ApplicationPropertyPage.h"

namespace sf
{
ApplicationPropertyPage::ApplicationPropertyPage(PropertySheetDialog* parent)
	:PropertyPage(parent)
	 , ui(new Ui::ApplicationPropertyPage)
{
	ui->setupUi(this);
	ui->comboBox->addItem(tr("Off"), QVariant(false));
	ui->comboBox->addItem(tr("On"), QVariant(true));
	ui->comboBox->setCurrentIndex(0);

	ui->comboBox_2->addItem("Text Item 1");
	ui->comboBox_2->addItem("Text Item 2");
	ui->comboBox_2->setEditText("Initial..");

	// Connect signals when the state of a control widget changes.
	connectControls();
}

ApplicationPropertyPage::~ApplicationPropertyPage()
{
	delete ui;
}

QIcon ApplicationPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Application), QPalette::Text);
}

QString ApplicationPropertyPage::getPageName() const
{
	return tr("Application");
}

QString ApplicationPropertyPage::getPageDescription() const
{
	return tr("Application settings.");
}

void ApplicationPropertyPage::updatePage()
{
}

bool ApplicationPropertyPage::isPageModified() const
{
	return false;
}

void ApplicationPropertyPage::applyPage()
{
}

void ApplicationPropertyPage::afterPageApply()
{
}

}
