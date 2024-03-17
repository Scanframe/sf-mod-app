#include <QUiLoader>
#include <misc/qt/Globals.h>
#include <misc/qt/qt_utils.h>
#include "ContainerPropertyPage.h"
#include "ui_ContainerPropertyPage.h"

namespace sf
{

ContainerPropertyPage::ContainerPropertyPage(QWidget* target, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::ContainerPropertyPage)
	 , _target(target)
{
	ui->setupUi(this);
	//
	ui->cbLayoutType->addItem(tr("None"), "");
	for (auto& name: getGlobalUiLoader()->availableLayouts())
	{
		ui->cbLayoutType->addItem(name.mid(1), name);
	}
}

ContainerPropertyPage::~ContainerPropertyPage()
{
	delete ui;
}

QString ContainerPropertyPage::getPageName() const
{
	return "Container";
}

QString ContainerPropertyPage::getPageDescription() const
{
	return "Container configuration for child widgets in a layout.";
}

void ContainerPropertyPage::updatePage()
{
	ui->cbLayoutType->setCurrentIndex(indexFromComboBox(ui->cbLayoutType, _target->layout() ? _target->layout()->metaObject()->className() : ""));
	ui->gbMargin->setVisible(!!_target->layout());
	if (_target->layout())
	{
		auto margins = _target->layout()->contentsMargins();
		ui->sbMarginLeft->setValue(margins.left());
		ui->sbMarginRight->setValue(margins.right());
		ui->sbMarginTop->setValue(margins.top());
		ui->sbMarginBottom->setValue(margins.bottom());
	}
}

bool ContainerPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->cbLayoutType->currentIndex() != indexFromComboBox(ui->cbLayoutType, _target->layout() ? _target->layout()->metaObject()->className() : "");
	if (_target->layout())
	{
		auto margins = _target->layout()->contentsMargins();
		rv |= ui->sbMarginLeft->value() != margins.left();
		rv |= ui->sbMarginRight->value() != margins.right();
		rv |= ui->sbMarginTop->value() != margins.top();
		rv |= ui->sbMarginBottom->value() != margins.bottom();
	}
	return rv;
}

void ContainerPropertyPage::applyPage()
{
	// Only when changed layout changes set a new one.
	if (ui->cbLayoutType->currentIndex() != indexFromComboBox(ui->cbLayoutType, _target->layout() ? _target->layout()->metaObject()->className() : ""))
	{
		auto name = ui->cbLayoutType->currentData().toString();
		QList<QLayoutItem*> items;
		if (_target->layout())
		{
			for (int i = _target->layout()->count() - 1; i >= 0; i--)
			{
				items.append(_target->layout()->takeAt(i));
			}
		}
		delete _target->layout();
		auto layout = getGlobalUiLoader()->createLayout(name, _target);
		// Iterate reverse as the items were recorded.
		for (auto it = items.rbegin(); it != items.rend(); it++)
		{
			layout->addItem(*it);
		}
	}
	// Only when the target has a
	if (_target->layout())
	{
		_target->layout()->setContentsMargins(
			ui->sbMarginLeft->value(),
			ui->sbMarginRight->value(),
			ui->sbMarginTop->value(),
			ui->sbMarginBottom->value()
		);
	}
}

}
