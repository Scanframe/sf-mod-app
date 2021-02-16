#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QStyledItemDelegate>
#include <iostream>
#include <misc/capturelistmodel.h>
#include <misc/resource.h>
#include <QJSValueIterator>
#include <QDir>
#include <QDirIterator>
#include "misc/dbgutils.h"
#include "ExposedObject.h"

MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent)
	 , ui(new Ui::MainWindow)
	 , _engine(nullptr)
{
	ui->setupUi(this);
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/scanframe"));
	centralWidget()->setAttribute(Qt::WA_AlwaysShowToolTips, true);
	// Assign the model to the listView.
	auto clm = new sf::CaptureListModel(ui->listViewLog);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog);
	// Assign the modal to the listview.
	ui->listViewLog->setModel(clm);
	// Method of connecting Qt5 style
	connect(ui->pushButtonLoad, &QPushButton::clicked, this, &MainWindow::onLoadScript);
	connect(ui->pushButtonCall, &QPushButton::clicked, this, &MainWindow::onCallFunction);
	// Needs to be set to be able to style the popup list from the QComboBox from the qss file.
	for (auto cb: {ui->comboBoxScripts, ui->comboBoxFunctions})
	{
		cb->setItemDelegate(new QStyledItemDelegate(cb));
	}
	for (auto pb: {ui->pushButtonCall, ui->pushButtonLoad})
	{
		pb->setEnabled(false);
	}
	// Fill the the combobox with usable scripts.
	for(auto& entry: QDir( ":/js" ).entryList())
	{
		ui->comboBoxScripts->addItem("Resource: " + entry, QVariant(":/js/" + entry));
	}
	// Add the js-files in the application directory
	QDirIterator it(QCoreApplication::applicationDirPath(), {"*.js"}, QDir::Files);
	while (it.hasNext())
	{
		it.next();
		ui->comboBoxScripts->addItem("File: " + it.fileInfo().baseName(), QVariant(it.filePath()));
	}
	// When scripts are there enable the run push button.
	if (ui->comboBoxScripts->count())
	{
		ui->pushButtonLoad->setEnabled(true);
	}
}

MainWindow::~MainWindow()
{
	delete _engine;
	delete ui;
}

void MainWindow::onLoadScript()
{
	// Only create the object once.
	static auto obj = new ExposedObject(this);
	// Delete JS engine in order to reset it. A method for doing that is not available.
	delete _engine;
	_engine = new QJSEngine(this);
	// Assign a global exposed object.
	_engine->globalObject().setProperty("ExposedObject", _engine->toScriptValue(obj));
	// Get the current selected script.
	auto script = ui->comboBoxScripts->currentData().toString();
	// Evaluate the script.
	auto result = _engine->evaluate(sf::Resource::getString(script), script);
	// Check for an error.
	if (result.isError())
	{
		std::clog << result.toString() << std::endl;
	}
	else
	{
		// Clear the current entries.
		ui->comboBoxFunctions->clear();
		// The script returns the object global name to query for callables.
		QJSValueIterator it(_object = _engine->globalObject().property(result.toString()));
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
	// Enable the call button when functions are available.
	if (ui->comboBoxFunctions->count())
	{
		ui->pushButtonCall->setEnabled(true);
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
