#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <iostream>
#include <misc/capturelistmodel.h>
#include <misc/resource.h>
#include <QStyledItemDelegate>
#include "misc/dbgutils.h"
#include "tutorial.h"

MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent)
	 , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/scanframe"));
	// Assign the model to the listView.
	auto clm = new sf::CaptureListModel(ui->listViewLog);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog | clm->ssCout);
	// Assign the modal to the listview.
	ui->listViewLog->setModel(clm);
	// Method of connecting Qt5 style
	connect(ui->pushButtonCall, &QPushButton::clicked, this, &MainWindow::onCallFunction);
	connect(ui->pushButtonRun, &QPushButton::clicked, this, &MainWindow::onLoadScript);
	// Needs to be set to be able to style the popup list from the QComboBox from the qss file.
	for (auto cb: {ui->comboBoxScripts, ui->comboBoxFunctions})
	{
		cb->setItemDelegate(new QStyledItemDelegate(cb));
	}
	//centralWidget()->setAttribute(Qt::WA_AlwaysShowToolTips, true);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onLoadScript()
{
	RunApplication();
}

void MainWindow::onCallFunction()
{
	//std::clog << result.toString() << std::endl;
}
