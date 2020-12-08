#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDate>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QStringList>
#include <QLibrary>
#include <QDir>
#include <QDirIterator>

#include <iostream>
#include <misc/qcapturelistmodel.h>
#include <misc/qresource.h>
#include "misc/dbgutils.h"
#include "misc/genutils.h"
#include "misc/qt_utils.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, _interfaceInstance(nullptr)
{
	ui->setupUi(this);
	// Set an icon on the window.
	this->setWindowIcon(QIcon(":logo/scanframe"));
	// Assign the model to the listView.
	auto clm = new sf::QCaptureListModel(ui->listViewLog);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog);
	// Assign the modal to the listview.
	ui->listViewLog->setModel(clm);
	// Connecting ScanFrame style using strings only so connection can read from a config file.
	//sf::QObject_connect(ui->pushButtonLoad, "triggered", this, "onLoadLib");
	// Method of connecting Qt5 style
	connect(ui->pushButtonFind, &QPushButton::clicked, this, &MainWindow::onFindLibs);
	connect(ui->pushButtonLoad, &QPushButton::clicked, this, &MainWindow::onLoadLib);
	connect(ui->pushButtonCreate, &QPushButton::clicked, this, &MainWindow::onCreateInstance);
	connect(ui->pushButtonCall, &QPushButton::clicked, this, &MainWindow::onCallMethod);
	// Set the same icon on all push buttons.
	for (auto pb: {ui->pushButtonFind, ui->pushButtonLoad, ui->pushButtonCreate, ui->pushButtonCall})
	{
		pb->setIcon(sf::QResource::getIcon(sf::QResource::Icon::Submit));
	}
}

MainWindow::~MainWindow()
{
	delete ui;
	delete _interfaceInstance;
}

void MainWindow::onFindLibs()
{
	// clear the combo entries.
	ui->comboBoxLibrary->clear();
	// clear the info list entries.
	_libraryInfoList.clear();
	QDirIterator it(QCoreApplication::applicationDirPath(), {"*.so", "*.dll"}, QDir::Files);
	while (it.hasNext())
	{
		sf::DynamicLibraryInfo dld;
		dld.read(it.next().toStdString());
		if (dld.Path.length())
		{
			_libraryInfoList.append(dld);
			qDebug() << "Path:" << dld.Path;
			qDebug() << "Name: " << dld.Name;
			qDebug() << "Description: " << dld.Description;
		}
	}
	// Sort the list of libraries by name.
	std::sort(_libraryInfoList.begin(), _libraryInfoList.end(),
		[](const sf::DynamicLibraryInfo& a, const sf::DynamicLibraryInfo& b) {return a.Name < b.Name;});
	QVectorIterator<sf::DynamicLibraryInfo> dli(_libraryInfoList);
	for (int i = 0; i < _libraryInfoList.count(); i++)
	{
		auto filename = QDir(QCoreApplication::applicationDirPath()).relativeFilePath(
			QString::fromStdString(_libraryInfoList[i].Path));
		QString name = QString("%1 (%2)").arg(QString::fromStdString(_libraryInfoList[i].Name)).arg(filename);
		ui->comboBoxLibrary->addItem(name, QVariant(i));
	}
}

void MainWindow::onLoadLib()
{
	// Sanity check.
	if (ui->comboBoxLibrary->currentIndex() < 0)
	{
		QMessageBox mb;
		mb.setWindowTitle("Information");
		mb.setText("No library selected to load.");
		mb.setIcon(QMessageBox::Information);
		mb.exec();
		return;
	}
	// Get the index from the user data in the combobox.
	auto idx = ui->comboBoxLibrary->currentData().toInt();
	// Get the path to the library.
	auto& file = _libraryInfoList[idx].Path;
	qDebug() << "Loading: " << file;
	QLibrary lib(QString::fromStdString(file));
	if (!lib.load())
	{
		qWarning() << "QLibrary(" << file << ")" << lib.errorString();
	}
	// clear the current entries.
	ui->comboBoxClass->clear();
	// Iterate through registered classes and fill
	for (size_t i = 0; i < RuntimeIface::Interface().Size(); ++i)
	{
		QString name = QString("%1 (%2)").arg(RuntimeIface::Interface().Name(i)).arg(
			QDir(QCoreApplication::applicationDirPath()).relativeFilePath(QString::fromStdString(file)));
		ui->comboBoxClass->addItem(name, QVariant(QString(RuntimeIface::Interface().Name(i))));
	}
}

void MainWindow::onCreateInstance()
{
	// Delete the current instance.
	delete _interfaceInstance;
	_interfaceInstance = RuntimeIface::Interface().Create(ui->comboBoxClass->currentData().toString().toStdString(),
		RuntimeIface::Parameters(123));
}

void MainWindow::onCallMethod()
{
	if (_interfaceInstance)
	{
		auto result = _interfaceInstance->getString();
		std::clog << "Call Result: " << result << std::endl;
	}
	else
	{
		QMessageBox mb;
		mb.setWindowTitle("Information");
		mb.setText("No instance created for calling a method on.");
		mb.setIcon(QMessageBox::Information);
		mb.exec();
	}
}
