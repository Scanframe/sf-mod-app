#pragma once

#include <QMainWindow>
#include "../iface/RuntimeIface.h"

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

	private Q_SLOTS:
		void onFindLibs();
		void onLoadLib();
		void onCreateInstance();
		void onCallMethod();

	private:
		// Pointer to the main window.
		Ui::MainWindow* ui;
		// Holds the list of libraries.
		QList<sf::DynamicLibraryInfo> _libraryInfoList;
		// Pointer to an implementation of RuntimeIface.
		RuntimeIface* _interfaceInstance;
};
