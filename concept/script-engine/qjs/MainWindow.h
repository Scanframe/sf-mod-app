#pragma once

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

	private Q_SLOTS:
		void onLoadScript();
		void onCallFunction();

	private:
		// Pointer to the main window.
		Ui::MainWindow* ui;
		// Script engine.
		QJSEngine* _engine;
		// Holds the targeted object having the callable functions.
		QJSValue _object;
};
