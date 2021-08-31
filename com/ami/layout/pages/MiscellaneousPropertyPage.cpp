#include <QLineEdit>
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
{
	ui->setupUi(this);
	//
	ui->gbAlignment->setVisible(!!_targetLineEdit);
	//
	for (auto a: {Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter, Qt::AlignJustify, Qt::AlignAbsolute})
	{
		ui->cbAlignmentHorizontal->addItem(enumToKey<Qt::Alignment>(a), a);
	}
	for (auto a: {Qt::AlignTop, Qt::AlignBottom, Qt::AlignVCenter, Qt::AlignBaseline})
	{
		ui->cbAlignmentVertical->addItem(enumToKey<Qt::Alignment>(a), a);
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
	if (_targetLineEdit)
	{
		rv |= ui->cbAlignmentHorizontal->currentData().value<Qt::Alignment>() != (_targetLineEdit->alignment() & Qt::AlignHorizontal_Mask);
		rv |= ui->cbAlignmentVertical->currentData().value<Qt::Alignment>() != (_targetLineEdit->alignment() & Qt::AlignVertical_Mask);
	}
	return rv;
}

void MiscellaneousPropertyPage::updatePage()
{
	if (_targetLineEdit)
	{
		ui->cbAlignmentHorizontal->setCurrentIndex(indexFromComboBox(ui->cbAlignmentHorizontal, {_targetLineEdit->alignment() & Qt::AlignHorizontal_Mask}, 0));
		ui->cbAlignmentVertical->setCurrentIndex(indexFromComboBox(ui->cbAlignmentVertical, {_targetLineEdit->alignment() & Qt::AlignVertical_Mask}, 0));
	}
}

void MiscellaneousPropertyPage::applyPage()
{
	if (_targetLineEdit)
	{
		_targetLineEdit->setAlignment(ui->cbAlignmentHorizontal->currentData().value<Qt::Alignment>() /*|ui->cbAlignmentVertical->currentData().value<Qt::Alignment>()*/);
	}
}

}
