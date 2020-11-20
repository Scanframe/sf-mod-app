#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "common/dbgutils.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QDate>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowIcon(QIcon(":/images/icon.ico"));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionLoad_A_triggered()
{
	//UserOutputDebugString(DO_MSGBOX, "Hello world...");
//	_RTTI_NOTIFY(DO_COUT, "out: Hello world...")
	_RTTI_NOTIFY(DO_CLOG, "Hello world... CLOCKS_PER_SEC: " << CLOCKS_PER_SEC)
	_RTTI_NOTIFY(DO_MSGBOX|DO_DBGBRK, "A debug break?")

	qDebug() << "Date:" << QDate::currentDate();

}

