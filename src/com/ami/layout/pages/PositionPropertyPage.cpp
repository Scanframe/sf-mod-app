#include <QFormLayout>
#include <misc/qt/qt_utils.h>
#include <misc/qt/Resource.h>
#include "PositionPropertyPage.h"
#include "ui_PositionPropertyPage.h"

namespace sf
{

PositionPropertyPage::PositionPropertyPage(QObject* target, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::PositionPropertyPage)
	 , _target(target)
	 , _targetLayout(dynamic_cast<QLayout*>(target))
	 , _targetWidget(dynamic_cast<QWidget*>(target))
{
	ui->setupUi(this);
	// When a widget and it has a layout.has a layout.
	if (auto p = dynamic_cast<QWidget*>(target->parent()))
	{
		// When having also a layout.
		if (p->layout())
		{
			_layout = p->layout();
			_formLayout = dynamic_cast<QFormLayout*>(_layout);
			_boxLayout = dynamic_cast<QBoxLayout*>(_layout);
			_gridLayout = dynamic_cast<QGridLayout*>(_layout);
		}
	}
	// Hide group boxes.
	ui->gbFormLayout->setVisible(!!_formLayout);
	ui->gbBoxLayout->setVisible(!!_boxLayout);
	//
	if (_formLayout)
	{
		// Fill the size policy combo boxes.
		for (auto p: {QFormLayout::ItemRole::FieldRole, QFormLayout::ItemRole::LabelRole, QFormLayout::ItemRole::SpanningRole})
		{
			ui->cbFormLayoutRole->addItem(enumToKey(p), p);
		}
	}
}

PositionPropertyPage::~PositionPropertyPage()
{
	delete ui;
}

QString PositionPropertyPage::getPageName() const
{
	return tr("Position");
}

QString PositionPropertyPage::getPageDescription() const
{
	return tr("Manipulation of the items position in the layout or widget.");
}

QIcon PositionPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Position), QPalette::Text);
}

bool PositionPropertyPage::isPageModified() const
{
	bool rv = false;
	if (_formLayout)
	{
		auto pos = getLayoutPosition(_formLayout, _target);
		rv |= ui->cbFormLayoutRole->currentIndex() != indexFromComboBox(ui->cbFormLayoutRole, pos.second, ui->cbFormLayoutRole->currentIndex());
		rv |= ui->sbFormLayoutRow->value() != pos.first;
	}
	if (_boxLayout)
	{
		rv |= getLayoutIndex(_boxLayout, _target) != ui->sbBoxIndex->value();
	}
	return rv;
}

void PositionPropertyPage::updatePage()
{
	if (_formLayout)
	{
		auto pos = getLayoutPosition(_formLayout, _target);
		ui->cbFormLayoutRole->setCurrentIndex(indexFromComboBox(ui->cbFormLayoutRole, pos.second, ui->cbFormLayoutRole->currentIndex()));
		ui->sbFormLayoutRow->setValue(pos.first);
		ui->sbFormLayoutRow->setMaximum(_formLayout->rowCount());
	}
	if (_boxLayout)
	{
		ui->sbBoxIndex->setValue(getLayoutIndex(_boxLayout, _target));
		ui->sbBoxIndex->setMaximum(_boxLayout->count() - 1);
	}
}

bool moveFormRow(QFormLayout* layout, int fromRow, int toRow, bool span)
{
	if (fromRow >= layout->rowCount() || toRow >= layout->rowCount() || fromRow < 0 || toRow < 0)
	{
		return false;
	}
	auto empty = [](QLayoutItem* item) -> bool
	{
		return item ? item->isEmpty() : true;
	};
	// Remove the row without deleting the items.
	auto result = layout->takeRow(fromRow);
	// When field only.
	if (empty(result.labelItem) && !empty(result.fieldItem))
	{
		if (result.fieldItem->widget())
		{
			layout->insertRow(toRow, nullptr, result.fieldItem->widget());
			// Only when spanning was the role.
			if (span)
			{
				layout->setWidget(toRow, QFormLayout::SpanningRole, result.fieldItem->widget());
			}
		}
		else if (result.fieldItem->layout())
		{
			layout->insertRow(toRow, nullptr, result.fieldItem->layout());
			// Only when spanning was the role.
			if (span)
			{
				layout->setLayout(toRow, QFormLayout::SpanningRole, result.fieldItem->layout());
			}
		}
	}
		// When label only.
	else if (!empty(result.labelItem) && empty(result.fieldItem))
	{
		if (result.labelItem->widget())
		{
			layout->insertRow(toRow, result.labelItem->widget(), (QWidget*) nullptr);
		}
		else if (result.labelItem->layout())
		{
			layout->insertRow(toRow, result.labelItem->layout());
		}
	}
	else
	{
		if (result.labelItem->widget() && result.fieldItem->widget())
		{
			layout->insertRow(toRow, result.labelItem->widget(), result.fieldItem->widget());
		}
		if (result.labelItem->widget() && result.fieldItem->layout())
		{
			layout->insertRow(toRow, result.labelItem->widget(), result.fieldItem->layout());
		}
	}
	return true;
}

void PositionPropertyPage::applyPage()
{
	if (_boxLayout)
	{
		auto fromIndex = getLayoutIndex(_boxLayout, _target);
		auto toIndex = ui->sbBoxIndex->value();
		if (fromIndex != toIndex)
		{
			auto item = _boxLayout->takeAt(fromIndex);
			_boxLayout->insertItem(toIndex, item);
		}
	}
	if (_formLayout)
	{
		auto pos = getLayoutPosition(_formLayout, _target);
		//auto item = _formLayout->itemAt(pos.first, pos.second);
		auto newRow = ui->sbFormLayoutRow->value();
		auto newRole = ui->cbFormLayoutRole->currentData().value<QFormLayout::ItemRole>();
		// Check for a role and row change.
		if (newRole != pos.second || newRow != pos.first)
		{
			bool flagMoveRow = true;
			// When the current role is spanning, and it does not change.
			if (pos.second == QFormLayout::SpanningRole && newRole == pos.second)
			{
				auto trgItemLabel = _formLayout->itemAt(newRow, QFormLayout::LabelRole);
				auto trgItemField = _formLayout->itemAt(newRow, QFormLayout::FieldRole);
				// Check if both target items are empty.
				if ((!trgItemLabel || trgItemLabel->isEmpty()) && (!trgItemField || trgItemField->isEmpty()))
				{
					flagMoveRow = false;
				}
			}
			else
			{
				auto trgItem = _formLayout->itemAt(newRow, newRole);
				// When targeted position is empty it can the setWidget or setLayout function can be used.
				if (!trgItem || trgItem->isEmpty())
				{
					flagMoveRow = false;
				}
			}
			// When moving the row is the only option.
			if (flagMoveRow)
			{
				// Remove the row by taking it and inserting it again.
				moveFormRow(_formLayout, pos.first, newRow, newRole == QFormLayout::SpanningRole);
			}
			else
			{
				if (_targetWidget)
				{
					_formLayout->setWidget(newRow, newRole, _targetWidget);
				}
				if (_targetLayout)
				{
					_formLayout->setLayout(newRow, newRole, _targetLayout);
				}
			}
			// When the same row and changing from spanning.
			if (newRow == pos.first && newRole != QFormLayout::SpanningRole && newRole == QFormLayout::FieldRole)
			{
				// Remove the row by taking it.
				auto rowResult = _formLayout->takeRow(pos.first);
				//
				if (rowResult.fieldItem->widget())
				{
					_formLayout->insertRow(newRow, nullptr, rowResult.fieldItem->widget());
				}
				if (rowResult.fieldItem->layout())
				{
					_formLayout->insertRow(newRow, nullptr, rowResult.fieldItem->layout());
				}
			}
			_formLayout->update();
		}
	}
}

}
