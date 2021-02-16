#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJSEngine>

QT_BEGIN_NAMESPACE
namespace Ui{class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		// Constructor
		explicit MainWindow(QWidget* parent = nullptr);
		// Destructor.
		~MainWindow() override;

	private slots:
		void onLoadScript();
		void onCallFunction();

	private:
		// Pointer to the main window.
		Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
