#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "misc/dbgutils.h"

#include <QAbstractItemView>
#include <QDate>
#include <QDebug>
#include <QMessageBox>
#include <QStringList>
#include <QStringListModel>
#include <iostream>
#include <QtCore/QDir>
#include <QtCore/QLibrary>
#include <QtCore/QCoreApplication>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, FRuntimeIface(nullptr)
{
	ui->setupUi(this);
	this->setWindowIcon(QIcon(":/img/icon.ico"));
	// Create model
#if 0
	auto model = new QStringListModel(this);
	// Make data
	QStringList List;
	List << "Clair de Lune" << "Reverie" << "Prelude";
	// Populate our model
	model->setStringList(List);
	// Glue model and view together
	ui->listView->setModel(model);
#else
//	auto model = new MyListModel(this);
	listModal.insert(QList<Message>
		{
			Message("a", "first message", "this is a sample text message of the first message"), Message("b",
			"second message", "another sample text message of the second message")
		});

	listModal.dataChanged(listModal.index(0), listModal.index(3));
	// Glue model and view together
	ui->treeView->setModel(&listModal);
#endif
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionLoad_A_triggered()
{
	_RTTI_NOTIFY(DO_COUT, "out: Hello world...")
	_RTTI_NOTIFY(DO_CLOG, "log: Hello world...")
	_RTTI_NOTIFY(DO_CERR, "err: Hello world...")
	_RTTI_NOTIFY(DO_MSGBOX | DO_DBGBRK, "A debug break?")
	//ui->listView->model()->isWidgetType() << "Date:" << QDate::currentDate();
}

void MainWindow::on_actionLoad_B_triggered()
{
	static int counter = 0;
	auto cnt = QString::asprintf("%d", counter++);
	listModal.insert(QList<Message>{Message(cnt, "" + cnt, "This is a sample text message " + cnt)},
		listModal.rowCount(QModelIndex()));
	if (!FRuntimeIface)
	{
		// Do not specify the extension because this is different in Windows then it is in Linux and QLibrary takes care of it.
		QString file = QCoreApplication::applicationDirPath() + QDir::separator() + "librt-impl-a";
		qInfo() << "Loading: " << file;
		QLibrary lib(file);
		if (!lib.load())
		{
			qCritical() << "QLibrary(" << file << ")" << lib.errorString();
		}
		else
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Size() : " << RuntimeIface::Interface().Size())
			//
			std::string names[] = {"Class", "Class-A", "Class-B"};
			for (auto& name: names)
			{
				qDebug() << "IndexOf(" << name.c_str() << ") >> " << RuntimeIface::Interface().IndexOf(name);
			}
			//
			FRuntimeIface = RuntimeIface::Interface().Create("Class-A", RuntimeIface::Parameters(123));
		}
	}
	else
	{
		qInfo() << FRuntimeIface->getString();
	}

}
