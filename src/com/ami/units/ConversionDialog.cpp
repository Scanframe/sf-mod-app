#include <gii/gen/UnitConversion.h>
#include <misc/qt/Resource.h>
#include <QPushButton>
#include <QTimer>
#include "ConversionDialog.h"
#include "ui_ConversionDialog.h"

namespace sf
{

ConversionDialog::ConversionDialog(QWidget* parent)
:QDialog(parent)
, ui(new Ui::ConversionDialog)
{
	ui->setupUi(this);
	//
	ui->buttonBox->button(QDialogButtonBox::Cancel)->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Cancel), QPalette::ButtonText));
	ui->buttonBox->button(QDialogButtonBox::Ok)->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Okay), QPalette::ButtonText));
	//
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ConversionDialog::close);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConversionDialog::okay);
	//
	QPalette palette = ui->lblMessage->palette();
	palette.setColor(ui->lblMessage->backgroundRole(), QColorConstants::Svg::red);
	palette.setColor(ui->lblMessage->foregroundRole(), QColorConstants::Svg::red);
	ui->lblMessage->setPalette(palette);
	ui->lblMessage->setVisible(false);
	//
	ui->sbFromPrecision->setRange(-20, 20);
	ui->sbPrecisionShift->setRange(-20, 20);
	//
	setModal(true);
}

ConversionDialog::~ConversionDialog()
{
	delete ui;
}

bool ConversionDialog::checkContent()
{
	// Remove unwanted characters.
	ui->leFromUnit->setText(ui->leFromUnit->text().remove(' ').remove(','));
	ui->leToUnit->setText(ui->leToUnit->text().remove(' ').remove(','));
	ui->leMultiplier->setText(ui->leMultiplier->text().remove(' '));
	ui->leOffset->setText(ui->leOffset->text().remove(' '));

	if (ui->leFromUnit->text().isEmpty() || ui->leToUnit->text().isEmpty())
	{
		ui->lblMessage->setText("Unit cannot be empty!");
		ui->lblMessage->setVisible(true);
		QTimer::singleShot(5000, [&]()
		{
			ui->lblMessage->setVisible(false);
		});
		return false;
	}
	return true;
}

void ConversionDialog::okay()
{
	if (checkContent())
	{
		done(QDialog::Accepted);
	}
}

bool ConversionDialog::execute(UnitConversionEvent& ev, bool restricted)
{
	if (restricted)
	{
		ui->leFromUnit->setReadOnly(true);
		ui->sbFromPrecision->setReadOnly(true);
		ui->leToUnit->setFocus();
	}
	// Initialize the edit fields.
	ui->leFromUnit->setText(QString::fromStdString(ev._from_unit));
	ui->sbFromPrecision->setValue(ev._from_precision);
	ui->leToUnit->setText(QString::fromStdString(ev._to_unit));
	ui->leMultiplier->setText(QString::fromStdString(trimRight(trimRight(std::to_string(ev._multiplier), "0"), ".")));
	ui->leOffset->setText(QString::fromStdString(trimRight(trimRight(std::to_string(ev._offset), "0"), ".")));
	ui->sbPrecisionShift->setValue(ev._to_precision - ev._from_precision);
	//
	if (exec() == QDialog::Accepted)
	{
		if (!restricted)
		{
			ev._from_unit = ui->leFromUnit->text().trimmed().toStdString();
			ev._from_precision = ui->sbFromPrecision->value();
		}
		ev._to_unit = ui->leToUnit->text().trimmed().toStdString();
		ev._to_precision = ui->sbFromPrecision->value() + ui->sbPrecisionShift->value();
		ev._multiplier = ui->leMultiplier->text().toDouble();
		ev._offset = ui->leOffset->text().toDouble();
		return true;
	}
	return false;
}

}
