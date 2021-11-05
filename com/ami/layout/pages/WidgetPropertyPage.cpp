#include <QMetaEnum>
#include <misc/qt/qt_utils.h>
#include <misc/qt/Resource.h>
#include "WidgetPropertyPage.h"
#include "ui_WidgetPropertyPage.h"

namespace sf
{

WidgetPropertyPage::WidgetPropertyPage(QWidget* target, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::WidgetPropertyPage)
	 , _target(target)
{
	ui->setupUi(this);
	// Fill the size policy combo boxes.
	for (auto p: {QSizePolicy::Fixed, QSizePolicy::Minimum, QSizePolicy::Maximum, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding, QSizePolicy::Ignored})
	{
		ui->cbHorizontalPolicy->addItem(enumToKey(p), p);
		ui->cbVerticalPolicy->addItem(enumToKey(p), p);
	}
}

WidgetPropertyPage::~WidgetPropertyPage()
{
	delete ui;
}

QString WidgetPropertyPage::getPageName() const
{
	return tr("Widget");
}

QIcon WidgetPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Widget), QPalette::Text);
}

bool WidgetPropertyPage::isPageModified() const
{
	bool rv = false;
	QSizePolicy sp(_target->sizePolicy());
	rv |= ui->cbHorizontalPolicy->currentData() != sp.horizontalPolicy();
	rv |= ui->cbVerticalPolicy->currentData() != sp.verticalPolicy();
	rv |= ui->sbHorizontalStretch->value() != sp.horizontalStretch();
	rv |= ui->sbVerticalStretch->value() != sp.verticalStretch();

	rv |= ui->sbMinWidth->value() != _target->minimumSize().width();
	rv |= ui->sbMinHeight->value() != _target->minimumSize().height();
	rv |= ui->sbMaxWidth->value() != _target->maximumSize().width();
	rv |= ui->sbMaxHeight->value() != _target->maximumSize().height();
	rv |= QSize(ui->sbIncrementWidth->value(), ui->sbIncrementWidth->value()) != _target->sizeIncrement();

	rv |= ui->pteStyleSheet->toPlainText() != _target->styleSheet();
	return rv;
}

void WidgetPropertyPage::updatePage()
{
	QSizePolicy sp(_target->sizePolicy());
	ui->cbHorizontalPolicy->setCurrentIndex(indexFromComboBox(ui->cbHorizontalPolicy, sp.horizontalPolicy(), ui->cbHorizontalPolicy->currentIndex()));
	ui->cbVerticalPolicy->setCurrentIndex(indexFromComboBox(ui->cbHorizontalPolicy, sp.verticalPolicy(), ui->cbVerticalPolicy->currentIndex()));
	ui->sbHorizontalStretch->setValue(sp.horizontalStretch());
	ui->sbVerticalStretch->setValue(sp.verticalStretch());

	ui->sbMinWidth->setValue(_target->minimumSize().width());
	ui->sbMinHeight->setValue(_target->minimumSize().height());
	ui->sbMaxWidth->setValue(_target->maximumSize().width());
	ui->sbMaxHeight->setValue(_target->maximumSize().height());

	ui->sbIncrementWidth->setValue(_target->sizeIncrement().width());
	ui->sbIncrementWidth->setValue(_target->sizeIncrement().height());

	ui->pteStyleSheet->setPlainText(_target->styleSheet());
}

void WidgetPropertyPage::applyPage()
{
	QSizePolicy sp(_target->sizePolicy());
	sp.setHorizontalPolicy(ui->cbHorizontalPolicy->currentData().value<QSizePolicy::Policy>());
	sp.setVerticalPolicy(ui->cbVerticalPolicy->currentData().value<QSizePolicy::Policy>());
	sp.setHorizontalStretch(ui->sbHorizontalStretch->value());
	sp.setVerticalStretch(ui->sbVerticalStretch->value());

	_target->setSizePolicy(sp);

	_target->setMinimumSize(ui->sbMinWidth->value(), ui->sbMinHeight->value());
	_target->setMaximumSize(ui->sbMaxWidth->value(), ui->sbMaxHeight->value());
	_target->setSizeIncrement(ui->sbIncrementWidth->value(), ui->sbIncrementWidth->value());

	_target->setStyleSheet(ui->pteStyleSheet->toPlainText());
}

void WidgetPropertyPage::stateSaveRestore(QSettings& settings, bool save)
{
	if (save)
	{
		settings.setValue("TabIndex", ui->tabWidget->currentIndex());
	}
	else
	{
		ui->tabWidget->setCurrentIndex(settings.value("TabIndex", ui->tabWidget->currentIndex()).toInt());
	}
}

}
