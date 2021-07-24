#include <QVariant>
#include <QMdiArea>
#include <QMetaEnum>
#include "ApplicationPropertyPage.h"
#include "ui_ApplicationPropertyPage.h"
#include <misc/qt/Resource.h>

namespace sf
{
ApplicationPropertyPage::ApplicationPropertyPage(MainWindow* main_win, PropertySheetDialog* parent)
	:PropertyPage(parent)
	 , ui(new Ui::ApplicationPropertyPage)
	 ,_mainWindow(main_win)
{
	ui->setupUi(this);
	// Fill the combo box.
	auto meta = QMetaEnum::fromType<QMdiArea::ViewMode>();
	for (auto i :{QMdiArea::SubWindowView, QMdiArea::TabbedView})
	{
		ui->cbViewMode->addItem(tr(meta.valueToKey(i)), QVariant(i));
	}
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
	ui->cbViewMode->setCurrentIndex(_mainWindow->getMdiArea()->viewMode());
	ui->leAppDisplayName->setText(QApplication::applicationDisplayName());
}

bool ApplicationPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->cbViewMode->currentIndex() != _mainWindow->getMdiArea()->viewMode();
	rv |= ui->leAppDisplayName->text() != QApplication::applicationDisplayName();
	return rv;
}

void ApplicationPropertyPage::applyPage()
{
	_mainWindow->getMdiArea()->setViewMode(static_cast<QMdiArea::ViewMode>(ui->cbViewMode->currentIndex()));
	QApplication::setApplicationDisplayName(ui->leAppDisplayName->text());
}

void ApplicationPropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_mainWindow->settingsReadWrite(true);
	}
}

}
