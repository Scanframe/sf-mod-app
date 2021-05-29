#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {class GraphWindow;}
QT_END_NAMESPACE

class GraphWindow :public QMainWindow
{
	Q_OBJECT

	public:
		explicit GraphWindow(QWidget* parent = nullptr);

		~GraphWindow() override;

	private slots:

		void onAction1();

		void onAction2();

		void onPaint(QPaintEvent* event);

	private:
		// Pointer to the main window.
		Ui::GraphWindow* ui;
};
