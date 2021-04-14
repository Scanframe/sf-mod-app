#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QDate>
#include <QStringList>
#include <QTimer>
#include <QCoreApplication>
#include <QDir>
#include <QFormBuilder>
#include <QStandardPaths>
#include <QScrollArea>
#include <QPushButton>
//#include <QDesktopWidget>
#include <QResizeEvent>
#include <QFormLayout>
#include <QSettings>
#include <QFileDialog>
#include <QWidget>
#include <QScreen>

#include <misc/qt/CaptureListModel.h>
#include <misc/qt/FormWriter.h>
#include "misc/qt/qt_utils.h"
#include "misc/gen/dbgutils.h"
#include "FormDialog.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, rootName("FormRoot")
{
	ui->setupUi(this);
	setWindowIcon(QIcon(":logo/scanframe"));
	// Assign the model to the listView.
	auto clm = new sf::CaptureListModel(ui->listView);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog);
	// Assign the modal to the listview.
	ui->listView->setModel(clm);
	// Connecting ScanFrame style using strings only so connection can read from a config file.
	connect(ui->actionWriteUI, &QAction::triggered, this, &MainWindow::onWriteToFile);
	connect(ui->actionReadUI, &QAction::triggered, this, &MainWindow::onReadFromFile);
	//
	connect(ui->actionReadFile, &QAction::triggered, this, &MainWindow::onCreateFormDialog);
	// For now put the application main window in the top right corner of the screen.
	// if (QGuiApplication::screens().count())
	// {
		// auto scr = QGuiApplication::screens().at(0);
		// QRect rc = scr->geometry();
		// QSize sz(600, 400);
		// setGeometry(QRect(rc.topRight() - sz, sz));
	// }
	// Start timer and call a member function each second.
	auto* timer = new QTimer(this);
	// Every 10 seconds.
	timer->start(10000);
	connect(timer, &QTimer::timeout, [this]()->void
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Lambda function .. Timer event...")
		//qDebug() << _Q_RTTI_TYPENAME << "Timer event...";
	});
}

MainWindow::~MainWindow()
{
	delete ui;
}

QString MainWindow::uiFilepath()
{
	QFileInfo fi;
	//
	if (QCoreApplication::instance()->property("SettingsFile").isValid())
	{
		// Setting file information.
		QFileInfo sfi(QCoreApplication::instance()->property("SettingsFile").toString());
		// Get access to the settings.
		// Create the settings instance.
		QSettings settings(sfi.absoluteFilePath(), QSettings::IniFormat);
		// Start the ini-section.
		settings.beginGroup("UI");
		// Check if settings can be written and the key does not exist.
		QString key("Filepath");
		// Check if the key exists.
		if (settings.isWritable() && !settings.childKeys().contains(key))
		{
			// Set the default value.
			settings.setValue(key, "created.ui");
		}
		// Get the configured UI file.
		auto value = settings.value(key).toString();
		// End the section.
		settings.endGroup();
		// When the value was not empty.
		if (!value.isEmpty())
		{
			// Set the file using a directory and filename.
			fi.setFile(sfi.absoluteDir(), value);
		}
		else
		{
			fi.setFile(QFileDialog::getOpenFileName(this,
				"Open UI File", sfi.absolutePath(), tr("UI Files (*.ui)")));
		}
	}
	// When the file exists.
	if (fi.exists())
	{
		// Return the abs file path.
		return fi.absoluteFilePath();
	}
	// When the file was not found return an empty string.
	return "";
}

void MainWindow::onCreateFormDialog()
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "Called: " << __FUNCTION__)
	qDebug() << "QCoreApplication::libraryPaths()" << QCoreApplication::libraryPaths();

	auto dlg = new FormDialog(this);
	//dlg->setModal(true);
	dlg->setSizeGripEnabled(true);
	dlg->setMinimumSize(100, 50);
	auto scrollArea = new QScrollArea(dlg);
	scrollArea->setFrameShape(QFrame::Shape::Box);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	//scrollArea->setSizeAdjustPolicy(QScrollArea::SizeAdjustPolicy::AdjustToContentsOnFirstShow);
	QFile file(uiFilepath());
	// Open the file readonly.
	if (file.open(QFile::ReadOnly))
	{
		QFormBuilder builder;
		// Add the application directory as the plugin directory to find custom plugins.
		builder.addPluginPath(QCoreApplication::applicationDirPath());
		// Create widget from the loaded ui-file.
		auto widget = builder.load(&file);
		// When loading is successful.
		if (widget)
		{
			// The widget min size is also the actual size.
			widget->setMinimumSize(widget->size());
			// Copy the title from the widget.
			dlg->setWindowTitle(widget->windowTitle());
			// Set the widget onto the layout.
			scrollArea->setWidget(widget);
		}
	}
	// Connect the resizing signal from the FormDialog to the scroll area.
	connect(dlg, &FormDialog::resizing, [scrollArea](QResizeEvent* event)->void
	{
		scrollArea->setGeometry(QRect(QPoint(), event->size()));
	});
	dlg->adjustSize();
	dlg->show();
}

void MainWindow::onReadFromFile()
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "Called: " << __FUNCTION__)
	QFormBuilder builder;
	// Add the application directory as the plugin directory to find custom plugins.
	builder.addPluginPath(QCoreApplication::applicationDirPath());
	// Show the plugin paths.
	qDebug() << "builder.pluginPaths(): " << builder.pluginPaths();
	//
	QFile file(uiFilepath());
	//
	if (file.exists() && file.open(QFile::ReadOnly))
	{
		QWidget* widget = builder.load(&file, ui->frameEmbedded);
		if (widget)
		{
			// Set the root name.
			widget->setObjectName(rootName);
			// Adept to the size of the loaded form.
			widget->setMinimumSize(widget->size());
		}
	}
}

void MainWindow::onWriteToFile()
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "Called: " << __FUNCTION__)
	if (QCoreApplication::instance()->property("SettingsFile").isValid())
	{
		// Setting file information.
		QFileInfo sfi(QCoreApplication::instance()->property("SettingsFile").toString());
		QString filepath = QFileDialog::getSaveFileName(this,
			"Save UI File", sfi.absolutePath(), tr("UI Files (*.ui)"));
		// InitializeBase file object.
		QFile file(filepath);
		// Open file write only.
		file.open(QFileDevice::WriteOnly);
		//
		sf::FormWriter builder;
		//
		auto wgt = ui->frameEmbedded->findChild<QWidget*>(rootName, Qt::FindChildrenRecursively);
		if (wgt)
		{
			builder.save(&file, wgt);
		}
		file.close();
	}
}
