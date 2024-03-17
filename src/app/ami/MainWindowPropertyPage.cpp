#include <QVariant>
#include <QMdiArea>
#include <QMetaEnum>
#include <misc/qt/Resource.h>
#include <misc/gen/Sustain.h>
#include "Application.h"
#include "MainWindowPropertyPage.h"
#include "ui_MainWindowPropertyPage.h"

namespace sf
{

MainWindowPropertyPage::MainWindowPropertyPage(MainWindow* mainWindow, PropertySheetDialog* parent)
	:PropertyPage(parent)
	 , ui(new Ui::MainWindowPropertyPage)
	 ,_mainWindow(mainWindow)
{
	ui->setupUi(this);
	// Fill the combo box.
	auto meta = QMetaEnum::fromType<QMdiArea::ViewMode>();
	for (auto i :{QMdiArea::SubWindowView, QMdiArea::TabbedView})
	{
		ui->cbViewMode->addItem(tr(meta.valueToKey(i)), QVariant(i));
	}
}

MainWindowPropertyPage::~MainWindowPropertyPage()
{
	delete ui;
}

QIcon MainWindowPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Application), QPalette::Text);
}

QString MainWindowPropertyPage::getPageName() const
{
	return tr("MDI Window");
}

QString MainWindowPropertyPage::getPageDescription() const
{
	return tr("Multi document interface main window settings.");
}

void MainWindowPropertyPage::updatePage()
{
	// Update controls with current values.
	ui->cbViewMode->setCurrentIndex(_mainWindow->getMdiArea()->viewMode());
}

bool MainWindowPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->cbViewMode->currentIndex() != _mainWindow->getMdiArea()->viewMode();
	return rv;
}

void MainWindowPropertyPage::applyPage()
{
	_mainWindow->getMdiArea()->setViewMode(static_cast<QMdiArea::ViewMode>(ui->cbViewMode->currentIndex()));
}

void MainWindowPropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_mainWindow->settingsReadWrite(true);
	}
}

}
