#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QStyledItemDelegate>
#include <iostream>
#include <misc/qcapturelistmodel.h>
#include <misc/qresource.h>
#include <QJSValueIterator>
#include "misc/dbgutils.h"
#include "ExposedObject.h"

MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent)
	 , ui(new Ui::MainWindow)
	 , _engine(this)
{
	ui->setupUi(this);
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/scanframe"));
	// Assign the model to the listView.
	auto clm = new sf::QCaptureListModel(ui->listViewLog);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog);
	// Assign the modal to the listview.
	ui->listViewLog->setModel(clm);
	// Method of connecting Qt5 style
	connect(ui->pushButtonCall, &QPushButton::clicked, this, &MainWindow::onCallFunction);
	connect(ui->pushButtonRun, &QPushButton::clicked, this, &MainWindow::onRunScript);
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

void MainWindow::onRunScript()
{
	static auto obj = new ExposedObject(this);
	//
	_engine.globalObject().setProperty("ExposedObject", _engine.toScriptValue(obj));
	//
	//auto result = _engine.importModule(":/js/expose");
	//auto result = _engine.evaluate(sf::Resource::getString(":/js/expose"), ":/js/expose");
	auto result = _engine.evaluate(sf::Resource::getString(":/js/Main"), ":/js/Main.js");
	//
	if (result.isError())
	{
		std::clog << result.toString() << std::endl;
	}
	else
	{
		// Clear the current entries.
		ui->comboBoxFunctions->clear();
		// The script returns the object global name to query for callables.
		QJSValueIterator it(_object = _engine.globalObject().property(result.toString()));
		// Iterate and fill the combo list.
		while (it.hasNext())
		{
			it.next();
			if (it.value().isCallable())
			{
				ui->comboBoxFunctions->addItem(it.name(), QVariant(it.name()));
			}
		}
	}
}

void MainWindow::onCallFunction()
{
	if (_object.isObject())
	{
		auto prop = _object.property(ui->comboBoxFunctions->currentData().toString());
		if (prop.isCallable())
		{
			prop.call();
		}
	}
}


