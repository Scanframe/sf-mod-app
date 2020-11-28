#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "com/misc/dbgutils.h"

#include <QAbstractItemView>
#include <QDate>
#include <QDebug>
#include <QMessageBox>
#include <QStringList>
#include <QStringListModel>
#include <iostream>
#include <QtCore/QDir>
#include <QtCore/QLibrary>

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
		QString file = QCoreApplication::applicationDirPath() + QDir::separator() + "librt-impl-a.so";
		if (!QFile::exists(file))
		{
			qDebug() << "File (" << file << ") does not exist";
		}
		else
		{
			_NORM_NOTIFY(DO_DEFAULT, "Loading: " << file.toStdString())
			QLibrary lib(file);
			if (!lib.load())
			{
				qDebug() << "QLibrary(" << file << ")" << lib.errorString();
			}
			else
			{
				FRuntimeIface = RuntimeIface::NewRegisterObject("Implementation_Alias", RuntimeIface::Parameters(100));
			}
		}
	}
	else
	{
		qDebug() << FRuntimeIface->getString();
	}

}
