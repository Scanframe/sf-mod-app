#include <QVariant>
#include <QMdiArea>
#include <QMetaEnum>
#include "ApplicationPropertyPage.h"
#include "ui_ApplicationPropertyPage.h"
#include "Application.h"
#include <misc/qt/Resource.h>
#include <misc/gen/Sustain.h>

namespace sf
{
ApplicationPropertyPage::ApplicationPropertyPage(Application* app, PropertySheetDialog* parent)
	:PropertyPage(parent)
	 , ui(new Ui::ApplicationPropertyPage)
	 ,_app(app)
{
	ui->setupUi(this);
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
	// Update controls with current values.
	ui->leAppDisplayName->setText(QApplication::applicationDisplayName());
	ui->sbSustain->setValue(_app->_sustainInterval);
}

bool ApplicationPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->leAppDisplayName->text() != QApplication::applicationDisplayName();
	rv |= ui->sbSustain->value() != _app->_sustainInterval;
	return rv;
}

void ApplicationPropertyPage::applyPage()
{
	QApplication::setApplicationDisplayName(ui->leAppDisplayName->text());
	_app->_sustainInterval = ui->sbSustain->value();
}

void ApplicationPropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_app->settingsReadWrite(true);
		setSustainTimer(_app->_sustainInterval);
	}
}

}
