#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../../iface/src/rt-iface.h"

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
		void onLoadA();
		void onLoadB();
		void onLoadLibAndExecuteFunction();

	private:
		// Pointer to the main window.
		Ui::MainWindow* ui;
		// Pointer to an implementation of RuntimeIface.
		RuntimeIface* interfaceInstance;
};

#endif // MAINWINDOW_H
