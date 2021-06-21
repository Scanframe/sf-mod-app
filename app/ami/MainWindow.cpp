
#include <QtWidgets>

#include "MainWindow.h"
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

MainWindow::MainWindow(QSettings& settings)
	:_mdiArea(new QMdiArea), _settings(settings)
{
	_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	_mdiArea->setViewMode(QMdiArea::TabbedView);

	setCentralWidget(_mdiArea);
	connect(_mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);

	createActions();
	createStatusBar();
	updateMenus();

	readSettings();

	setWindowTitle(QCoreApplication::applicationName());
	setWindowIcon(QIcon(":logo/ico/scanframe"));
	setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	_mdiArea->closeAllSubWindows();
	if (_mdiArea->currentSubWindow())
	{
		event->ignore();
	}
	else
	{
		writeSettings();
		event->accept();
	}
}

void MainWindow::newFile()
{
	// TODO: Open dialog to create a new file.
	auto child = createMdiChild();
	if (child)
	{
		child->newFile();
		if (auto widget = dynamic_cast<QWidget*>(child))
		{
			//child->show();
			widget->showMaximized();
		}
	}
}

void MainWindow::open()
{
/*
	QProcess p;
	p.start("kdialog", QStringList() << "--getopenfilename" << "$HOME");
	p.waitForFinished();
	QMessageBox msgBox;
	msgBox.setWindowTitle("Selected file...");
	msgBox.setText(p.readAllStandardOutput());
	msgBox.exec();
	qWarning() << p.readAllStandardOutput();
*/
	QFileDialog fd(this, "Open editor file", QString(), "Javascript (*.js);;All Files (*)");
	auto pc = qvariant_cast<sf::PaletteColors*>(QApplication::instance()->property("systemColors"));
	if (pc)
	{
		pc->styleFileDialog(fd);
	}
	if (fd.exec())
	{
		openFile(fd.selectedFiles().first());
	}
}

bool MainWindow::openFile(const QString& fileName)
{
	if (QMdiSubWindow* existing = findMdiChild(fileName))
	{
		_mdiArea->setActiveSubWindow(existing);
		return true;
	}
	const bool succeeded = loadFile(fileName);
	if (succeeded)
	{
		statusBar()->showMessage(tr("File loaded"), 2000);
	}
	return succeeded;
}

bool MainWindow::loadFile(const QString& fileName)
{
	auto child = createMdiChild(fileName);
	if (child)
	{
		const bool succeeded = child->loadFile(fileName);
		auto widget = dynamic_cast<QWidget*>(child);
		if (succeeded)
		{
			if (widget)
			{
				//child->show();
				widget->showMaximized();
			}
		}
		else
		{
			if (widget)
			{
				widget->close();
			}
		}
		MainWindow::prependToRecentFiles(fileName);
		return succeeded;
	}
	return false;
}

static inline QString recentFilesKey()
{
	return QStringLiteral("recentFileList");
}

static inline QString fileKey()
{
	return QStringLiteral("file");
}

static QStringList readRecentFiles(QSettings& settings)
{
	QStringList result;
	const int count = settings.beginReadArray(recentFilesKey());
	for (int i = 0; i < count; ++i)
	{
		settings.setArrayIndex(i);
		result.append(settings.value(fileKey()).toString());
	}
	settings.endArray();
	return result;
}

static void writeRecentFiles(const QStringList& files, QSettings& settings)
{
	const int count = files.size();
	settings.beginWriteArray(recentFilesKey());
	for (int i = 0; i < count; ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue(fileKey(), files.at(i));
	}
	settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const int count = settings.beginReadArray(recentFilesKey());
	settings.endArray();
	return count > 0;
}

void MainWindow::prependToRecentFiles(const QString& filename)
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

	const QStringList oldRecentFiles = readRecentFiles(settings);
	QStringList recentFiles = oldRecentFiles;
	recentFiles.removeAll(filename);
	recentFiles.prepend(filename);
	if (oldRecentFiles != recentFiles)
	{
		writeRecentFiles(recentFiles, settings);
	}

	setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
	_recentFileSubMenuAct->setVisible(visible);
	_recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

	const QStringList recentFiles = readRecentFiles(settings);
	const int count = qMin(int(MaxRecentFiles), recentFiles.size());
	int i = 0;
	for (; i < count; ++i)
	{
		const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
		_recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
		_recentFileActs[i]->setData(recentFiles.at(i));
		_recentFileActs[i]->setVisible(true);
	}
	for (; i < MaxRecentFiles; ++i)
	{
		_recentFileActs[i]->setVisible(false);
	}
}

void MainWindow::openRecentFile()
{
	if (auto action = qobject_cast<const QAction*>(sender()))
	{
		openFile(action->data().toString());
	}
}

void MainWindow::save()
{
	if (activeMdiChild() && activeMdiChild()->save())
	{
		statusBar()->showMessage(tr("File saved"), 2000);
	}
}

void MainWindow::saveAs()
{
	auto child = activeMdiChild();
	if (child && child->saveAs())
	{
		statusBar()->showMessage(tr("File saved"), 2000);
		MainWindow::prependToRecentFiles(child->currentFile());
	}
}

void MainWindow::cut()
{
	if (activeMdiChild())
	{
		activeMdiChild()->cut();
	}
}

void MainWindow::copy()
{
	if (activeMdiChild())
	{
		activeMdiChild()->copy();
	}
}

void MainWindow::paste()
{
	if (activeMdiChild())
	{
		activeMdiChild()->paste();
	}
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About MDI"),
		tr("The <b>MDI</b> example demonstrates how to write multiple "
			 "document interface applications using Qt."));
}

void MainWindow::configModules()
{
	if (QCoreApplication::instance()->property("SettingsFile").isValid())
	{
		sf::AppModuleInterface::openConfiguration(_settings, this);
	}
	else
	{
		QMessageBox::information(this, tr("Configure Modules"),
			tr("Application property 'SettingsFile' is missing!"));
	}
}

void MainWindow::updateMenus()
{
	bool hasMdiChild = (activeMdiChild() != nullptr);
	_saveAct->setEnabled(hasMdiChild);
	_saveAsAct->setEnabled(hasMdiChild);
	_pasteAct->setEnabled(hasMdiChild);
	_closeAct->setEnabled(hasMdiChild);
	_closeAllAct->setEnabled(hasMdiChild);
	_tileAct->setEnabled(hasMdiChild);
	_cascadeAct->setEnabled(hasMdiChild);
	_nextAct->setEnabled(hasMdiChild);
	_previousAct->setEnabled(hasMdiChild);
	_windowMenuSeparatorAct->setVisible(hasMdiChild);
	bool hasSelection = (activeMdiChild() && activeMdiChild()->hasSelection());
	_cutAct->setEnabled(hasSelection);
	_copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu()
{
	_windowMenu->clear();
	_windowMenu->addAction(_closeAct);
	_windowMenu->addAction(_closeAllAct);
	_windowMenu->addSeparator();
	_windowMenu->addAction(_tileAct);
	_windowMenu->addAction(_cascadeAct);
	_windowMenu->addSeparator();
	_windowMenu->addAction(_nextAct);
	_windowMenu->addAction(_previousAct);
	_windowMenu->addAction(_windowMenuSeparatorAct);

	QList<QMdiSubWindow*> windows = _mdiArea->subWindowList();
	_windowMenuSeparatorAct->setVisible(!windows.isEmpty());

	for (int i = 0; i < windows.size(); ++i)
	{
		QMdiSubWindow* mdiSubWindow = windows.at(i);
		//auto child = qobject_cast<TextEditor*>(mdiSubWindow->widget());
		auto child = dynamic_cast<sf::MultiDocInterface*>(mdiSubWindow->widget());
		QString text;
		if (i < 9)
		{
			text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
		}
		else
		{
			text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
		}
		QAction* action = _windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]()
		{
			_mdiArea->setActiveSubWindow(mdiSubWindow);
		});
		action->setCheckable(true);
		action->setChecked(child == activeMdiChild());
	}
}

sf::MultiDocInterface* MainWindow::createMdiChild(const QString& filename)
{
#if false
	if (1)
	{
		auto child = new CodeEditor;
		mdiArea->addSubWindow(child);
		connect(child, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
		connect(child, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
		return child;
	}
	else
	{
		auto child = new TextEditor;
		mdiArea->addSubWindow(child);
		connect(child, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
		connect(child, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
		return child;
	}
#else
	return nullptr;
#endif
}

void MainWindow::createActions()
{
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QToolBar* fileToolBar = addToolBar(tr("File"));
	// Color for the icons.
	const QColor iconColor = QApplication::palette().color(QApplication::palette().currentColorGroup(),
		QPalette::ColorRole::ButtonText);

	const QIcon newIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::New), iconColor);

	_newAct = new QAction(newIcon, tr("&New"), this);
	_newAct->setShortcuts(QKeySequence::New);
	_newAct->setStatusTip(tr("Create a new file"));
	connect(_newAct, &QAction::triggered, this, &MainWindow::newFile);
	fileMenu->addAction(_newAct);
	fileToolBar->addAction(_newAct);

	const QIcon openIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Open),
		iconColor);

	auto openAct = new QAction(openIcon, tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	fileMenu->addAction(openAct);
	fileToolBar->addAction(openAct);

	const QIcon saveIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Save),
		iconColor);

	_saveAct = new QAction(saveIcon, tr("&Save"), this);
	_saveAct->setShortcuts(QKeySequence::Save);
	_saveAct->setStatusTip(tr("Save the document to disk"));
	connect(_saveAct, &QAction::triggered, this, &MainWindow::save);
	fileToolBar->addAction(_saveAct);

	const QIcon saveAsIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Save),
		iconColor);

	_saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
	_saveAsAct->setShortcuts(QKeySequence::SaveAs);
	_saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(_saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
	fileMenu->addAction(_saveAsAct);

	fileMenu->addSeparator();

	QMenu* recentMenu = fileMenu->addMenu(tr("Recent..."));
	connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
	_recentFileSubMenuAct = recentMenu->menuAction();

	for (auto& recentFileAct : _recentFileActs)
	{
		recentFileAct = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileAct->setVisible(false);
	}

	_recentFileSeparator = fileMenu->addSeparator();

	setRecentFilesVisible(MainWindow::hasRecentFiles());

	fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);

	fileMenu->addSeparator();

	const QIcon exitIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Exit),
		iconColor);

	QAction* exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), QCoreApplication::instance(),
		&QApplication::closeAllWindows);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Exit the application"));
	fileMenu->addAction(exitAct);

	QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
	QToolBar* editToolBar = addToolBar(tr("Edit"));

	const QIcon cutIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Cut), iconColor);

	_cutAct = new QAction(cutIcon, tr("Cu&t"), this);
	_cutAct->setShortcuts(QKeySequence::Cut);
	_cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
	connect(_cutAct, &QAction::triggered, this, &MainWindow::cut);
	editMenu->addAction(_cutAct);
	editToolBar->addAction(_cutAct);

	const QIcon copyIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Copy),
		iconColor);

	_copyAct = new QAction(copyIcon, tr("&Copy"), this);
	_copyAct->setShortcuts(QKeySequence::Copy);
	_copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
	connect(_copyAct, &QAction::triggered, this, &MainWindow::copy);
	editMenu->addAction(_copyAct);
	editToolBar->addAction(_copyAct);

	const QIcon pasteIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Paste),
		iconColor);

	_pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
	_pasteAct->setShortcuts(QKeySequence::Paste);
	_pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
	connect(_pasteAct, &QAction::triggered, this, &MainWindow::paste);
	editMenu->addAction(_pasteAct);
	editToolBar->addAction(_pasteAct);

	_windowMenu = menuBar()->addMenu(tr("&Window"));
	connect(_windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

	_closeAct = new QAction(tr("Cl&ose"), this);
	_closeAct->setStatusTip(tr("Close the active window"));
	connect(_closeAct, &QAction::triggered, _mdiArea, &QMdiArea::closeActiveSubWindow);

	_closeAllAct = new QAction(tr("Close &All"), this);
	_closeAllAct->setStatusTip(tr("Close all the windows"));
	connect(_closeAllAct, &QAction::triggered, _mdiArea, &QMdiArea::closeAllSubWindows);

	_tileAct = new QAction(tr("&Tile"), this);
	_tileAct->setStatusTip(tr("Tile the windows"));
	connect(_tileAct, &QAction::triggered, _mdiArea, &QMdiArea::tileSubWindows);

	_cascadeAct = new QAction(tr("&Cascade"), this);
	_cascadeAct->setStatusTip(tr("Cascade the windows"));
	connect(_cascadeAct, &QAction::triggered, _mdiArea, &QMdiArea::cascadeSubWindows);

	_nextAct = new QAction(tr("Ne&xt"), this);
	_nextAct->setShortcuts(QKeySequence::NextChild);
	_nextAct->setStatusTip(tr("Move the focus to the next window"));
	connect(_nextAct, &QAction::triggered, _mdiArea, &QMdiArea::activateNextSubWindow);

	_previousAct = new QAction(tr("Pre&vious"), this);
	_previousAct->setShortcuts(QKeySequence::PreviousChild);
	_previousAct->setStatusTip(tr("Move the focus to the previous window"));
	connect(_previousAct, &QAction::triggered, _mdiArea, &QMdiArea::activatePreviousSubWindow);

	_windowMenuSeparatorAct = new QAction(this);
	_windowMenuSeparatorAct->setSeparator(true);

	updateWindowMenu();

	menuBar()->addSeparator();

	QMenu* configMenu = menuBar()->addMenu(tr("&Config"));
	_moduleConfigAct = configMenu->addAction(tr("&Modules"), this, &MainWindow::configModules);
	_moduleConfigAct->setStatusTip(tr("Configure the modules the application to load"));

	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

	QAction* aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
	aboutAct->setStatusTip(tr("Show the application's About box"));

	QAction* aboutQtAct = helpMenu->addAction(tr("About &Qt"), QCoreApplication::instance(), &QApplication::aboutQt);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
/*
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
	if (geometry.isEmpty())
	{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
		auto availableGeometry = screen()->availableGeometry();
#else
		auto availableGeometry = QApplication::desktop()->availableGeometry(this);
#endif
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
			(availableGeometry.height() - height()) / 2);
	}
	else
	{
		restoreGeometry(geometry);
	}
*/
}

void MainWindow::writeSettings()
{
/*
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("geometry", saveGeometry());
*/
}

sf::MultiDocInterface* MainWindow::activeMdiChild() const
{
	if (QMdiSubWindow* activeSubWindow = _mdiArea->activeSubWindow())
	{
		//return qobject_cast<TextEditor*>(activeSubWindow->widget());
		return dynamic_cast<sf::MultiDocInterface*>(activeSubWindow->widget());
	}
	return nullptr;
}

QMdiSubWindow* MainWindow::findMdiChild(const QString& fileName) const
{
	QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
	const QList<QMdiSubWindow*> subWindows = _mdiArea->subWindowList();
	for (QMdiSubWindow* window: subWindows)
	{
		//auto child = qobject_cast<TextEditor*>(window->widget());
		auto child = dynamic_cast<sf::MultiDocInterface*>(window->widget());
		if (child->currentFile() == canonicalFilePath)
		{
			return window;
		}
	}
	return nullptr;
}

void MainWindow::switchLayoutDirection()
{
	if (layoutDirection() == Qt::LeftToRight)
	{
		QGuiApplication::setLayoutDirection(Qt::RightToLeft);
	}
	else
	{
		QGuiApplication::setLayoutDirection(Qt::LeftToRight);
	}
}

void MainWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);
	// Use timer otherwise focus does not follow.
	QTimer::singleShot(200, [&]
	{
		_moduleConfigAct->trigger();
	});
}

}