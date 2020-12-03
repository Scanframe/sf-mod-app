#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui{class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget* parent = nullptr);
		~MainWindow() override;

	private slots:
		void onReadFromFile();
		void onWriteToFile();

	private:
		Ui::MainWindow* ui;
		//
		void backgroundHandler();
};

#endif // MAINWINDOW_H
