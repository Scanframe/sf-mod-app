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
		void onFindLibs();
		void onLoadLib();
		void onCreateInstance();
		void onCallMethod();

	private:
		void loadLibAndExecuteFunction();
		// Pointer to the main window.
		Ui::MainWindow* ui;
		// Holds the list of libraries.
		QVector<sf::DynamicLibraryInfo> _libraryInfoList;
		// Pointer to an implementation of RuntimeIface.
		RuntimeIface* _interfaceInstance;
};

#endif // MAINWINDOW_H
