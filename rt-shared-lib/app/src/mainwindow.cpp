#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDate>
#include <QDebug>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QLibrary>

#include <iostream>
#include <misc/qcapturelistmodel.h>
#include "misc/dbgutils.h"
#include "misc/qt_utils.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, interfaceInstance(nullptr)
{
	ui->setupUi(this);
	// Set an icon on the window.
	this->setWindowIcon(QIcon(":/img/icon.ico"));
	// Assign the model to the listView.
	auto clm = new sf::QCaptureListModel(ui->listView);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog);
	// Assign the modal to the listview.
	ui->listView->setModel(clm);
	// Method of connecting Qt5 style
	//connect(ui->actionLoad_C, &QAction::triggered, this, &MainWindow::onLoadLib);
	// Method connection pre Qt5 style.
	//connect(ui->actionLoad_C, SIGNAL(triggered()), this, SLOT(onLoadLib()));
	// Connecting ScanFrame style using strings only so connection can read from a config file.
	sf::QObject_connect(ui->actionLoad_A, "triggered", this, "onLoadA");
	sf::QObject_connect(ui->actionLoad_B, "triggered", this, "onLoadB");
	sf::QObject_connect(ui->actionLoad_C, "triggered", this, "onLoadLibAndExecuteFunction");
}

MainWindow::~MainWindow()
{
	delete ui;
	delete interfaceInstance;
}

void MainWindow::onLoadA() // NOLINT(readability-convert-member-functions-to-static)
{
	_RTTI_NOTIFY(DO_DEFAULT, "Called: " << __FUNCTION__)
}

void MainWindow::onLoadB() // NOLINT(readability-convert-member-functions-to-static)
{
	std::clog << "This message streamed to 'std::clog'." << std::endl;
}

void MainWindow::onLoadLibAndExecuteFunction()
{
	// Sentry for loading.
	if (!interfaceInstance)
	{
		// Do not specify the extension because this is different in Windows then it is in Linux and QLibrary takes care of it.
		QString file = QCoreApplication::applicationDirPath() + QDir::separator() + "librt-impl-a";
		_RTTI_NOTIFY(DO_DEFAULT, "Loading: " << file);
		QLibrary lib(file);
		if (!lib.load())
		{
			_RTTI_NOTIFY(DO_DEFAULT, "QLibrary(" << file << ")" << lib.errorString())
		}
		else
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Size(): " << RuntimeIface::Interface().Size())
			//
			std::string names[] = {"Class", "Class-A", "Class-B"};
			for (auto& name: names)
			{
				_RTTI_NOTIFY(DO_DEFAULT, "IndexOf(" << name.c_str() << ") >> " << RuntimeIface::Interface().IndexOf(name))
			}
			//
			interfaceInstance = RuntimeIface::Interface().Create("Class-A", RuntimeIface::Parameters(123));
		}
	}
	else
	{
		_RTTI_NOTIFY(DO_DEFAULT, "getString(): " << interfaceInstance->getString())
	}
}
