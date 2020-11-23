#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include "messagelist.h"

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow() override;

	private slots:
		void on_actionLoad_A_triggered();
		void on_actionLoad_B_triggered();

	private:
		Ui::MainWindow *ui;
		//
		MessageListModal listModal;

};
#endif // MAINWINDOW_H
