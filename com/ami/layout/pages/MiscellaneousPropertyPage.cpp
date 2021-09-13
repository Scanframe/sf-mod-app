#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <misc/qt/qt_utils.h>
#include <misc/gen/dbgutils.h>
#include "MiscellaneousPropertyPage.h"
#include "ui_MiscellaneousPropertyPage.h"

namespace sf
{

MiscellaneousPropertyPage::MiscellaneousPropertyPage(QObject* target, QWidget* parent)
	:PropertyPage(parent)
	 , ui(new Ui::MiscellaneousPropertyPage)
	 , _target(target)
	 , _targetLineEdit(qobject_cast<QLineEdit*>(target))
	 , _targetLabel(qobject_cast<QLabel*>(target))
{
	ui->setupUi(this);
	//
	ui->gbAlignment->setVisible(!!_targetLineEdit);
	ui->gbLabel->setVisible(!!_targetLabel);
	//
	ui->leObjectName->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9_]+"), ui->leObjectName));
	//
	for (auto a: {Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter, Qt::AlignJustify, Qt::AlignAbsolute})
	{
		ui->cbAlignmentHorizontal->addItem(enumToKey<Qt::Alignment>(a), a);
	}
	for (auto a: {Qt::AlignTop, Qt::AlignBottom, Qt::AlignVCenter, Qt::AlignBaseline})
	{
		ui->cbAlignmentVertical->addItem(enumToKey<Qt::Alignment>(a), a);
	}
	if (_targetLabel)
	{
		// First entry is the 'None' for no buddy.
		ui->cbBuddy->addItem(tr("None"), QVariant::fromValue<QObject*>(nullptr));
		// Get the parents children.
		auto children = _targetLabel->parent()->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
		// Check if one of the children is also the current buddy.
		if (_targetLabel->buddy() && !children.contains(_targetLabel->buddy()))
		{
			// Add the current buddy to the front of the list.
			children.prepend(_targetLabel->buddy());
		}
		for (auto child: children)
		{
			if (!child->objectName().isEmpty() && child != _targetLabel && !qobject_cast<QLabel*>(child))
			{
				auto name = QString("%1 (%2)").arg(child->metaObject()->className()).arg(child->objectName());
				// When the current one was found add it at the start.
				if (_targetLabel->buddy() == child)
				{
					ui->cbBuddy->insertItem(0, name, QVariant::fromValue<QObject*>(child));
				}
				else
				{
					ui->cbBuddy->addItem(name, QVariant::fromValue<QObject*>(child));
				}
			}
		}
	}
}

MiscellaneousPropertyPage::~MiscellaneousPropertyPage()
{
	delete ui;
}

QString MiscellaneousPropertyPage::getPageName() const
{
	return tr("Miscellaneous");
}

bool MiscellaneousPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= ui->leObjectName->text() != _target->objectName();
	if (_targetLineEdit)
	{
		rv |= ui->cbAlignmentHorizontal->currentData().value<Qt::Alignment>() != (_targetLineEdit->alignment() & Qt::AlignHorizontal_Mask);
		rv |= ui->cbAlignmentVertical->currentData().value<Qt::Alignment>() != (_targetLineEdit->alignment() & Qt::AlignVertical_Mask);
	}
	if (_targetLabel)
	{
		rv |= _targetLabel->buddy() != ui->cbBuddy->currentData().value<QWidget*>();
		rv |= _targetLabel->text() != ui->leLabelText->text();
	}
	return rv;
}

void MiscellaneousPropertyPage::updatePage()
{
	ui->leObjectName->setText(_target->objectName());
	if (_targetLineEdit)
	{
		ui->cbAlignmentHorizontal->setCurrentIndex(indexFromComboBox(ui->cbAlignmentHorizontal, {_targetLineEdit->alignment() & Qt::AlignHorizontal_Mask}, 0));
		ui->cbAlignmentVertical->setCurrentIndex(indexFromComboBox(ui->cbAlignmentVertical, {_targetLineEdit->alignment() & Qt::AlignVertical_Mask}, 0));
	}
	if (_targetLabel)
	{
		ui->cbBuddy->setCurrentIndex(ui->cbBuddy->findData(QVariant::fromValue(_targetLabel->buddy())));
		ui->leLabelText->setText(_targetLabel->text());
	}
}

bool MiscellaneousPropertyPage::canApplyPage() const
{
	if (ui->leObjectName->text().isEmpty())
	{
		QMessageBox::warning((QWidget*) this, tr("Validation Error"), tr("Empty object name is not allowed."));
		return false;
	}
	return true;
}

void MiscellaneousPropertyPage::applyPage()
{
	_target->setObjectName(ui->leObjectName->text());
	if (_targetLineEdit)
	{
		_targetLineEdit->setAlignment(ui->cbAlignmentHorizontal->currentData().value<Qt::Alignment>());
	}
	if (_targetLabel)
	{
		_targetLabel->setBuddy(ui->cbBuddy->currentData().value<QWidget*>());
		_targetLabel->setText(ui->leLabelText->text());
	}
}

}
