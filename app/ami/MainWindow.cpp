#include <QtWidgets>
#include <QPlainTextEdit>

#include "MainWindow.h"
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/ModuleConfiguration.h>
#include <ami/iface/AppModuleInterface.h>

namespace sf
{

MainWindow::MainWindow(QSettings* settings)
	:_mdiArea(new QMdiArea)
	 , _settings(settings)
	 , _moduleConfiguration(new ModuleConfiguration(settings, this))
{
	setWindowTitle(QCoreApplication::applicationName());

	_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	//_mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
	_mdiArea->setViewMode(QMdiArea::ViewMode::TabbedView);

	setCentralWidget(_mdiArea);
	connect(_mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);

	createActions();
	createStatusBar();
	updateMenus();

	setUnifiedTitleAndToolBarOnMac(true);

	readSettings();
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
	// Make the application quit after the main window closes.
	QCoreApplication::quit();
}

void MainWindow::newFile()
{
	auto child = createMdiChild();
	if (child)
	{
		child->newFile();
		// Get the widget from the MultiDocInterface child.
		if (auto widget = dynamic_cast<QWidget*>(child))
		{
			//widget->show();
			widget->showMaximized();
		}
	}
}

void MainWindow::open()
{
	QFileDialog fd(this, "Open editor file", QString(), AppModuleInterface::getFileTypeFilters(true));
	// When desktop aware use the colors of the desktop so icon colors are matching.
	if (QApplication::desktopSettingsAware())
	{
		auto pc = qvariant_cast<PaletteColors*>(QApplication::instance()->property("systemColors"));
		if (pc)
		{
			pc->styleFileDialog(fd);
		}
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
	const qsizetype count = files.size();
	settings.beginWriteArray(recentFilesKey());
	for (qsizetype i = 0; i < count; ++i)
	{
		settings.setArrayIndex((int) i);
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
	_recentFileSubMenuAction->setVisible(visible);
	_recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

	const QStringList recentFiles = readRecentFiles(settings);
	const qsizetype count = qMin<qsizetype>(MaxRecentFiles, recentFiles.size());
	int i = 0;
	for (; i < count; ++i)
	{
		const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
		_recentFileActions[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
		_recentFileActions[i]->setData(recentFiles.at(i));
		_recentFileActions[i]->setVisible(true);
	}
	for (; i < MaxRecentFiles; ++i)
	{
		_recentFileActions[i]->setVisible(false);
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

void MainWindow::undo()
{
	if (activeMdiChild())
	{
		activeMdiChild()->undo();
	}
}

void MainWindow::redo()
{
	if (activeMdiChild())
	{
		activeMdiChild()->redo();
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
	if (_moduleConfiguration)
	{
		_moduleConfiguration->openDialog(this);
	}
}

void MainWindow::updateWindowMenu()
{
	_windowMenu->clear();
	_windowMenu->addAction(_closeWindowAction);
	_windowMenu->addAction(_closeWindowsAction);
	_windowMenu->addSeparator();
	_windowMenu->addAction(_tileAction);
	_windowMenu->addAction(_cascadeAction);
	_windowMenu->addSeparator();
	_windowMenu->addAction(_nextAction);
	_windowMenu->addAction(_previousAction);
	_windowMenu->addAction(_windowMenuSeparatorAction);

	QList<QMdiSubWindow*> windows = _mdiArea->subWindowList();
	_windowMenuSeparatorAction->setVisible(!windows.isEmpty());

	for (int i = 0; i < windows.size(); ++i)
	{
		QMdiSubWindow* mdiSubWindow = windows.at(i);
		//auto child = qobject_cast<TextEditor*>(mdiSubWindow->widget());
		auto child = dynamic_cast<MultiDocInterface*>(mdiSubWindow->widget());
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

void MainWindow::updateMenus()
{
	bool hasMdiChild = (activeMdiChild() != nullptr);
	_saveAction->setEnabled(hasMdiChild && activeMdiChild()->isModified());
	_saveAsAction->setEnabled(hasMdiChild);
	_pasteAction->setEnabled(hasMdiChild);
	_closeWindowAction->setEnabled(hasMdiChild);
	_closeWindowsAction->setEnabled(hasMdiChild);
	_tileAction->setEnabled(hasMdiChild);
	_cascadeAction->setEnabled(hasMdiChild);
	_nextAction->setEnabled(hasMdiChild);
	_previousAction->setEnabled(hasMdiChild);
	_windowMenuSeparatorAction->setVisible(hasMdiChild);
	bool hasSelection = (hasMdiChild && activeMdiChild()->hasSelection());
	_cutAction->setEnabled(hasSelection);
	_copyAction->setEnabled(hasSelection);
	bool isUndoRedo = (hasMdiChild && activeMdiChild()->isUndoRedoEnabled());
	_undoAction->setEnabled(isUndoRedo && activeMdiChild()->isUndoAvailable());
	_redoAction->setEnabled(isUndoRedo && activeMdiChild()->isUndoAvailable());
}

void MainWindow::createActions()
{
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
	_windowMenu = menuBar()->addMenu(tr("&Window"));
	QMenu* configMenu = menuBar()->addMenu(tr("&Config"));
	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	//
	connect(_windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);
	// Color for the icons.
	auto iconColor = QApplication::palette().color(QPalette::ColorRole::ButtonText);

	auto newIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::New), iconColor);
	_newAction = new QAction(newIcon, tr("&New"), this);
	_newAction->setShortcuts(QKeySequence::New);
	_newAction->setStatusTip(tr("Create a new file"));
	connect(_newAction, &QAction::triggered, this, &MainWindow::newFile);

	auto openIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Open), iconColor);
	_openAction = new QAction(openIcon, tr("&Open..."), this);
	_openAction->setShortcuts(QKeySequence::Open);
	_openAction->setStatusTip(tr("Open an existing file"));
	connect(_openAction, &QAction::triggered, this, &MainWindow::open);

	auto saveIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Save), iconColor);
	_saveAction = new QAction(saveIcon, tr("&Save"), this);
	_saveAction->setShortcuts(QKeySequence::Save);
	_saveAction->setStatusTip(tr("Save the document to disk"));
	connect(_saveAction, &QAction::triggered, this, &MainWindow::save);

	auto saveAsIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Save), iconColor);
	_saveAsAction = new QAction(saveAsIcon, tr("Save &As..."), this);
	_saveAsAction->setShortcuts(QKeySequence::SaveAs);
	_saveAsAction->setStatusTip(tr("Save the document under a new name"));
	connect(_saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);

	auto exitIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Exit), iconColor);
	auto exitAction = new QAction(exitIcon, tr("E&xit"), this);
	connect(exitAction, &QAction::triggered, QCoreApplication::instance(), &QApplication::closeAllWindows);
	exitAction->setShortcuts(QKeySequence::Quit);
	exitAction->setStatusTip(tr("Exit the application"));

	auto cutIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Cut), iconColor);
	_cutAction = new QAction(cutIcon, tr("Cu&t"), this);
	_cutAction->setShortcuts(QKeySequence::Cut);
	_cutAction->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
	connect(_cutAction, &QAction::triggered, this, &MainWindow::cut);

	auto copyIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Copy),
		iconColor);
	_copyAction = new QAction(copyIcon, tr("&Copy"), this);
	_copyAction->setShortcuts(QKeySequence::Copy);
	_copyAction->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
	connect(_copyAction, &QAction::triggered, this, &MainWindow::copy);

	auto pasteIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Paste), iconColor);
	_pasteAction = new QAction(pasteIcon, tr("&Paste"), this);
	_pasteAction->setShortcuts(QKeySequence::Paste);
	_pasteAction->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
	connect(_pasteAction, &QAction::triggered, this, &MainWindow::paste);

	auto undoIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Undo), iconColor);
	_undoAction = new QAction(undoIcon, tr("&Undo"), this);
	_undoAction->setShortcuts(QKeySequence::Undo);
	_undoAction->setStatusTip(tr("Undo last made changes"));
	connect(_undoAction, &QAction::triggered, this, &MainWindow::undo);

	auto redoIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Redo), iconColor);
	_redoAction = new QAction(redoIcon, tr("&Redo"), this);
	_redoAction->setShortcuts(QKeySequence::Redo);
	_redoAction->setStatusTip(tr("Redo last made changes"));
	connect(_redoAction, &QAction::triggered, this, &MainWindow::redo);

	auto closeWindowIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::CloseWindow), iconColor);
	_closeWindowAction = new QAction(closeWindowIcon, tr("Cl&ose"), this);
	_closeWindowAction->setStatusTip(tr("Close the active window"));
	_closeWindowAction->setShortcuts(QKeySequence::Close);
	connect(_closeWindowAction, &QAction::triggered, _mdiArea, &QMdiArea::closeActiveSubWindow);

	auto closeWindowsIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::CloseWindows), iconColor);
	_closeWindowsAction = new QAction(closeWindowsIcon, tr("Close &All"), this);
	_closeWindowsAction->setStatusTip(tr("Close all the windows"));
	connect(_closeWindowsAction, &QAction::triggered, _mdiArea, &QMdiArea::closeAllSubWindows);

	_tileAction = new QAction(tr("&Tile"), this);
	_tileAction->setStatusTip(tr("Tile the windows"));
	connect(_tileAction, &QAction::triggered, _mdiArea, &QMdiArea::tileSubWindows);

	_cascadeAction = new QAction(tr("&Cascade"), this);
	_cascadeAction->setStatusTip(tr("Cascade the windows"));
	connect(_cascadeAction, &QAction::triggered, _mdiArea, &QMdiArea::cascadeSubWindows);

	_nextAction = new QAction(tr("Ne&xt"), this);
	_nextAction->setShortcuts(QKeySequence::NextChild);
	_nextAction->setStatusTip(tr("Move the focus to the next window"));
	connect(_nextAction, &QAction::triggered, _mdiArea, &QMdiArea::activateNextSubWindow);

	_previousAction = new QAction(tr("Pre&vious"), this);
	_previousAction->setShortcuts(QKeySequence::PreviousChild);
	_previousAction->setStatusTip(tr("Move the focus to the previous window"));
	connect(_previousAction, &QAction::triggered, _mdiArea, &QMdiArea::activatePreviousSubWindow);

	_windowMenuSeparatorAction = new QAction(this);
	_windowMenuSeparatorAction->setSeparator(true);

	auto moduleConfigIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Config), iconColor);
	_moduleConfigAction = new QAction(moduleConfigIcon, tr("&Modules"), this);
	connect(_moduleConfigAction, &QAction::triggered, this, &MainWindow::configModules);
	_moduleConfigAction->setStatusTip(tr("Configure application modules to load"));

	fileMenu->addAction(_newAction);
	fileMenu->addAction(_openAction);
	fileMenu->addAction(_saveAction);
	fileMenu->addAction(_saveAsAction);
	fileMenu->addSeparator();

	QMenu* recentMenu = fileMenu->addMenu(tr("Recent..."));
	connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
	_recentFileSubMenuAction = recentMenu->menuAction();
	_recentFileSeparator = fileMenu->addSeparator();
	setRecentFilesVisible(MainWindow::hasRecentFiles());

	for (auto& recentFileAct : _recentFileActions)
	{
		recentFileAct = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileAct->setVisible(false);
	}
	fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	editMenu->addAction(_cutAction);
	editMenu->addAction(_copyAction);
	editMenu->addAction(_pasteAction);
	editMenu->addAction(_undoAction);
	editMenu->addAction(_redoAction);

	configMenu->addAction(_moduleConfigAction);

	QAction* aboutAction = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
	aboutAction->setStatusTip(tr("Show the application's About box"));

	QAction* aboutQtAction = helpMenu->addAction(tr("About &Qt"), QCoreApplication::instance(), &QApplication::aboutQt);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));

	updateWindowMenu();

	QToolBar* fileToolBar = addToolBar(tr("File"));
	QToolBar* editToolBar = addToolBar(tr("Edit"));
	QToolBar* configToolBar = addToolBar(tr("Config"));

	fileToolBar->addAction(_newAction);
	fileToolBar->addAction(_openAction);
	fileToolBar->addAction(_saveAction);
	fileToolBar->addAction(_closeWindowAction);

	editToolBar->addAction(_cutAction);
	editToolBar->addAction(_copyAction);
	editToolBar->addAction(_pasteAction);
	editToolBar->addAction(_undoAction);
	editToolBar->addAction(_redoAction);

	configToolBar->addAction(_moduleConfigAction);

}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
	if (_moduleConfiguration)
	{
		// Load the missing modules from the configuration.
		_moduleConfiguration->load();
		// Create the interface implementations (that are missing).
		AppModuleInterface::instantiate(this);
	}
}

void MainWindow::writeSettings()
{
}

MultiDocInterface* MainWindow::createMdiChild(const QString& filename)
{
	AppModuleInterface* entry;
	// When empty a new file is expected.
	if (filename.isEmpty())
	{
		entry = AppModuleInterface::selectDialog(tr("New File"), this);
	}
	else
	{
		// Get the instance that can handle it.
		entry = AppModuleInterface::findByFile(filename);
		if (!entry)
		{
			entry = AppModuleInterface::selectDialog(tr("Open file with"), this);
		}
	}
	// When an entry was found for the file create a MDI child.
	if (entry)
	{
		auto child = entry->createChild(this);
		if (child)
		{
			auto sw = _mdiArea->addSubWindow(dynamic_cast<QWidget*>(child));
			sw->setWindowIcon(Resource::getSvgIcon(entry->getSvgIconResource(),
				QApplication::palette().color(
					_mdiArea->viewMode() == QMdiArea::ViewMode::TabbedView ?
						QPalette::ColorRole::ButtonText : QPalette::ColorRole::WindowText)));
			// Connect the MDI actions to the
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::modificationChanged, _saveAction, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::copyAvailable, _cutAction, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::copyAvailable, _copyAction, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::undoAvailable, _undoAction, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::redoAvailable, _redoAction, &QAction::setEnabled);
			return child;
		}
	}
	return nullptr;
}

MultiDocInterface* MainWindow::activeMdiChild() const
{
	if (QMdiSubWindow* activeSubWindow = _mdiArea->activeSubWindow())
	{
		return dynamic_cast<MultiDocInterface*>(activeSubWindow->widget());
	}
	return nullptr;
}

QMdiSubWindow* MainWindow::findMdiChild(const QString& fileName) const
{
	QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
	const QList<QMdiSubWindow*> subWindows = _mdiArea->subWindowList();
	for (QMdiSubWindow* window: subWindows)
	{
		auto child = dynamic_cast<MultiDocInterface*>(window->widget());
		if (child->currentFile() == canonicalFilePath)
		{
			return window;
		}
	}
	return nullptr;
}

void MainWindow::closeDocument()
{
	if (activeMdiChild())
	{
		if (activeMdiChild()->canClose())
		{
			_mdiArea->closeActiveSubWindow();
		}
	}
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

}
