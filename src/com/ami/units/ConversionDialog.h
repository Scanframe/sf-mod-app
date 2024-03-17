#pragma once

#include <QDialog>

namespace sf
{

class UnitConversionEvent;

namespace Ui {class ConversionDialog;}

class ConversionDialog :public QDialog
{
	Q_OBJECT

	public:
		explicit ConversionDialog(QWidget* parent = nullptr);

		~ConversionDialog() override;

		bool execute(UnitConversionEvent& ev, bool restricted = true);

	private:
		Ui::ConversionDialog* ui;

		void okay();

		bool checkContent();
};

}
