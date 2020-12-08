#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDate>
#include <QMessageBox>
#include <QStringList>
#include <QTimer>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QFormBuilder>

#include <misc/qcapturelistmodel.h>
#include "misc/dbgutils.h"
#include "misc/qt_utils.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowIcon(QIcon(":logo/scanframe"));
	// Assign the model to the listView.
	auto clm = new sf::QCaptureListModel(ui->listView);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog);
	// Assign the modal to the listview.
	ui->listView->setModel(clm);
	// Connecting ScanFrame style using strings only so connection can read from a config file.
	sf::QObject_connect(ui->actionWriteUI, "triggered", this, "onWriteToFile");
	sf::QObject_connect(ui->actionReadUI, "triggered", this, "onReadFromFile");
	// Start timer and call a member function each second.
	auto *timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &MainWindow::backgroundHandler);
	timer->start(3000);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onWriteToFile()
{
	_RTTI_NOTIFY(DO_DEFAULT, "Called: " << __FUNCTION__)
	QString fileName = QCoreApplication::applicationDirPath() + QDir::separator() + "the_form.ui";
	QFile file(fileName);
	QFormBuilder builder;
	file.open(QFileDevice::ReadWrite);
	builder.save(&file, this);
	//builder.computeProperties()
}

void MainWindow::onReadFromFile()
{
	_RTTI_NOTIFY(DO_DEFAULT, "Called: " << __FUNCTION__)
}

void MainWindow::backgroundHandler()
{
	_RTTI_NOTIFY(DO_DEFAULT, "Called: " << __FUNCTION__)
}
