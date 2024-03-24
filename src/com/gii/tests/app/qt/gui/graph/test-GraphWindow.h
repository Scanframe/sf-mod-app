#pragma once

#include <QDialog>

namespace Ui {class GraphWindow;}

class GraphWindow :public QDialog
{
	Q_OBJECT

	public:
		explicit GraphWindow(QWidget* parent = nullptr);

	private Q_SLOTS:

		void onPaint(QPaintEvent* event);

	private:
		// Pointer to the main window.
		Ui::GraphWindow* ui;

		void onSlider(int pos);
};
