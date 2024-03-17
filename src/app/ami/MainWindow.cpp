#include <QtWidgets>
#include <QPlainTextEdit>
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/ModuleConfiguration.h>
#include <misc/qt/PropertySheetDialog.h>
#include <ami/iface/AppModuleInterface.h>
#include <misc/gen/gen_utils.h>
#include <misc/qt/Globals.h>
#include "ApplicationPropertyPage.h"
#include "MainWindowPropertyPage.h"

namespace sf
{

MainWindow::MainWindow(QSettings* settings, Application* application)
	:_application(application)
	 , _mdiArea(new QMdiArea)
	 , _settings(settings)
{
	// Set the object name for the getObjectNamePath() function so property sheet settings get a correct section assigned.
	setObjectName("MainWindow");
	// Title is the same as the application.
	setWindowTitle(QApplication::applicationDisplayName());
	// Set the parent that can be used for floating windows created from script functions.
	setGlobalParent(this);
	//
	connect(qApp, &QApplication::applicationDisplayNameChanged, [&]() // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	{
		setWindowTitle(QApplication::applicationDisplayName());
	});
	//
	_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_mdiArea->setTabsMovable(true);
	//_mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
	_mdiArea->setViewMode(QMdiArea::ViewMode::TabbedView);
	//
	setCentralWidget(_mdiArea);
	connect(_mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::mdiSubActivated);
	//
	AppModuleInterface::callbackOpenFile = [&](const QString& filename, AppModuleInterface* ami) -> MultiDocInterface*
	{
		return openFile(filename);
	};
	//
	createActions();
	createStatusBar();
	createDockWindows();
	updateMenus();
	//
	setUnifiedTitleAndToolBarOnMac(true);
	//
	setAcceptDrops(true);
	// Show the dock widgets tabs at the top for all areas.
	setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
	//
	settingsReadWrite(false);
	//
	recentFilesReadWrite(false);
	// Restore the saved state it any.
	stateSaveRestore(false);
}

MainWindow::~MainWindow()
{
	// Reset the global parent.
	setGlobalParent(nullptr);
}

void MainWindow::stateSaveRestore(bool save)
{
	_settings->beginGroup(getObjectNamePath(this).join('.').prepend("State."));
	QString keyState("State");
	QString keyWidgetRect("WidgetRect");
	if (save)
	{
		_settings->setValue(keyWidgetRect, geometry());
		_settings->setValue(keyState, saveState());
	}
	else
	{
		// Get the keys in the section to check existence in the ini-section.
		if (_settings->value(keyWidgetRect).toRect().isValid())
		{
			setGeometry(_settings->value(keyWidgetRect).toRect());
		}
		restoreState(_settings->value(keyState).toByteArray());
	}
	_settings->endGroup();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	// Try closing all sub windows.
	_mdiArea->closeAllSubWindows();
	// If one ignored the closing abort the closing of the application.
	if (_mdiArea->currentSubWindow())
	{
		// Abort by ignoring.
		event->ignore();
	}
	else
	{
		// Save now before (dock)widgets change.
		stateSaveRestore(true);
		//
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
			// TODO: Show-mode should depend on other MDI children's status.
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
		if (auto pc = qvariant_cast<PaletteColors*>(QApplication::instance()->property("systemColors")))
		{
			pc->styleFileDialog(fd);
		}
	}
	if (fd.exec())
	{
		openFile(fd.selectedFiles().first());
	}
}

MultiDocInterface* MainWindow::openFile(const QString& filename)
{
	if (auto existing = findMdiChild(filename))
	{
		_mdiArea->setActiveSubWindow(existing);
		return dynamic_cast<MultiDocInterface*>(existing->widget());
	}
	return loadFile(filename);
}

MultiDocInterface* MainWindow::loadFile(const QString& filename)
{
	if (auto child = createMdiChild(filename))
	{
		auto widget = dynamic_cast<QWidget*>(child);
		if (child->loadFile(filename))
		{
			statusBar()->showMessage(tr("File loaded"), 2000);
			if (widget)
			{
				//widget->show();
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
		MainWindow::prependToRecentFiles(filename);
		return child;
	}
	return nullptr;
}

void MainWindow::recentFilesReadWrite(bool save)
{
	auto keyRecentFiles = QStringLiteral("RecentFiles");
	auto keyFile = QStringLiteral("File");
	if (save)
	{
		const auto count = _recentFiles.size();
		_settings->beginWriteArray(keyRecentFiles);
		for (qsizetype i = 0; i < count; ++i)
		{
			_settings->setArrayIndex(static_cast<int>(i));
			_settings->setValue(keyFile, _recentFiles.at(i));
		}
		_settings->endArray();
	}
	else
	{
		const auto count = _settings->beginReadArray(keyRecentFiles);
		for (int i = 0; i < count; ++i)
		{
			_settings->setArrayIndex(i);
			_recentFiles.append(_settings->value(keyFile).toString());
		}
		_settings->endArray();
		setRecentFilesVisible(!_recentFiles.isEmpty());
	}
}

void MainWindow::prependToRecentFiles(const QString& filename)
{
	// Check if a change is needed.
	if (_recentFiles.isEmpty() || _recentFiles.at(0) != filename)
	{
		_recentFiles.removeAll(filename);
		_recentFiles.prepend(filename);
		// Save the list to settings.
		recentFilesReadWrite(true);
	}
	setRecentFilesVisible(!_recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
	_actionRecentFileSubMenu->setVisible(visible);
	_actionRecentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
	const qsizetype count = qMin<qsizetype>(MaxRecentFiles, _recentFiles.size());
	int i = 0;
	for (; i < count; ++i)
	{
		const QString fileName = QFileInfo(_recentFiles.at(i)).fileName();
		_actionRecentFile[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
		_actionRecentFile[i]->setData(_recentFiles.at(i));
		_actionRecentFile[i]->setStatusTip(_recentFiles.at(i));
		_actionRecentFile[i]->setVisible(true);
	}
	for (; i < MaxRecentFiles; ++i)
	{
		_actionRecentFile[i]->setVisible(false);
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
	const char* msg = R"(
<h3>Scanframe Modular Application</h3>
Application to build faster.
)";
	QMessageBox::about(this, tr("About"), tr(msg));
}

void MainWindow::configModules()
{
	_application->getModuleConfiguration().openDialog(this);
}

void MainWindow::settingsPropertySheet()
{
	auto sheet = new PropertySheetDialog("ApplicationSettings", _settings, this);
	sheet->setWindowTitle(tr("Application Settings"));
	sheet->setWindowIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Settings), Qt::gray));
	//
	sheet->addPage(new ApplicationPropertyPage(_application, sheet));
	sheet->addPage(new MainWindowPropertyPage(this, sheet));
	// Adds all the property pages to the passed property sheet.
	AppModuleInterface::addAllPropertyPages(sheet);
	//
	sheet->exec();
	delete sheet;
}

void MainWindow::updateWindowMenu()
{
	_menuWindow->clear();
	_menuWindow->addAction(_actionCloseWindow);
	_menuWindow->addAction(_actionCloseWindows);
	_menuWindow->addSeparator();
	_menuWindow->addAction(_actionTileWindows);
	_menuWindow->addAction(_actionCascadeWindows);
	_menuWindow->addSeparator();
	_menuWindow->addAction(_actionNextWindow);
	_menuWindow->addAction(_actionPreviousWindow);
	_menuWindow->addAction(_actionWindowMenuSeparator);

	QList<QMdiSubWindow*> windows = _mdiArea->subWindowList();
	_actionWindowMenuSeparator->setVisible(!windows.isEmpty());

	for (int i = 0; i < windows.size(); ++i)
	{
		QMdiSubWindow* mdiSubWindow = windows.at(i);
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
		QAction* action = _menuWindow->addAction(text, mdiSubWindow, [this, mdiSubWindow]()
		{
			_mdiArea->setActiveSubWindow(mdiSubWindow);
		});
		action->setCheckable(true);
		action->setChecked(child == activeMdiChild());
	}
}

void MainWindow::mdiSubActivated(QMdiSubWindow* subWindow)
{
	// Deactivate all MDI documents first.
	for (auto window: _mdiArea->subWindowList())
	{
		if (auto iface = subWindow ? dynamic_cast<MultiDocInterface*>(window) : nullptr)
		{
			// Signal the interface it is deactivated.
			iface->activate(false);
		}
	}
	// Activate the single MDI documents.
	if (auto iface = subWindow ? dynamic_cast<MultiDocInterface*>(subWindow->widget()) : nullptr)
	{
		// Signal the interface it is activated.
		iface->activate(true);
	}
	updateMenus();
}

void MainWindow::updateMenus()
{
	bool hasMdiChild = (activeMdiChild() != nullptr);
	_actionSave->setEnabled(hasMdiChild && activeMdiChild()->isModified());
	_actionSaveAs->setEnabled(hasMdiChild);
	_actionPaste->setEnabled(hasMdiChild);
	_actionCloseWindow->setEnabled(hasMdiChild);
	_actionCloseWindows->setEnabled(hasMdiChild);
	_actionTileWindows->setEnabled(hasMdiChild);
	_actionCascadeWindows->setEnabled(hasMdiChild);
	_actionNextWindow->setEnabled(hasMdiChild);
	_actionPreviousWindow->setEnabled(hasMdiChild);
	_actionWindowMenuSeparator->setVisible(hasMdiChild);
	bool hasSelection = (hasMdiChild && activeMdiChild()->hasSelection());
	_actionCut->setEnabled(hasSelection);
	_actionCopy->setEnabled(hasSelection);
	bool isUndoRedo = (hasMdiChild && activeMdiChild()->isUndoRedoEnabled());
	_actionUndo->setEnabled(isUndoRedo && activeMdiChild()->isUndoAvailable());
	_actionRedo->setEnabled(isUndoRedo && activeMdiChild()->isUndoAvailable());
}

void MainWindow::createActions()
{
	QMenu* menuFile = menuBar()->addMenu(tr("&File"));
	QMenu* menuEdit = menuBar()->addMenu(tr("&Edit"));
	_menuSettings = menuBar()->addMenu(tr("&Settings"));
	_menuView = menuBar()->addMenu(tr("&View"));
	_menuTools = menuBar()->addMenu(tr("&Tools"));
	_menuWindow = menuBar()->addMenu(tr("&Window"));
	QMenu* configMenu = menuBar()->addMenu(tr("&Config"));
	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	//
	connect(_menuWindow, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);
	// Color for the icons.
	auto colorIcon = QApplication::palette().color(QPalette::ColorRole::ButtonText);

	auto newIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::New), colorIcon);
	_actionNew = new QAction(newIcon, tr("&New"), this);
	_actionNew->setShortcuts(QKeySequence::New);
	_actionNew->setStatusTip(tr("Create a new file"));
	connect(_actionNew, &QAction::triggered, this, &MainWindow::newFile);

	auto openIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::OpenFolder), colorIcon);
	_actionOpen = new QAction(openIcon, tr("&Open..."), this);
	_actionOpen->setShortcuts(QKeySequence::Open);
	_actionOpen->setStatusTip(tr("Open an existing file"));
	connect(_actionOpen, &QAction::triggered, this, &MainWindow::open);

	auto saveIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Save), colorIcon);
	_actionSave = new QAction(saveIcon, tr("&Save"), this);
	_actionSave->setShortcuts(QKeySequence::Save);
	_actionSave->setStatusTip(tr("Save the document to disk"));
	connect(_actionSave, &QAction::triggered, this, &MainWindow::save);

	auto saveAsIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Save), colorIcon);
	_actionSaveAs = new QAction(saveAsIcon, tr("Save &As..."), this);
	_actionSaveAs->setShortcuts(QKeySequence::SaveAs);
	_actionSaveAs->setStatusTip(tr("Save the document under a new name"));
	connect(_actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);

	auto exitIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Exit), colorIcon);
	auto _actionExit = new QAction(exitIcon, tr("E&xit"), this);
	connect(_actionExit, &QAction::triggered, QCoreApplication::instance(), &QApplication::closeAllWindows);
	_actionExit->setShortcuts(QKeySequence::Quit);
	_actionExit->setStatusTip(tr("Exit the application"));

	auto cutIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Cut), colorIcon);
	_actionCut = new QAction(cutIcon, tr("Cu&t"), this);
	_actionCut->setShortcuts(QKeySequence::Cut);
	_actionCut->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
	connect(_actionCut, &QAction::triggered, this, &MainWindow::cut);

	auto copyIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Copy), colorIcon);
	_actionCopy = new QAction(copyIcon, tr("&Copy"), this);
	_actionCopy->setShortcuts(QKeySequence::Copy);
	_actionCopy->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
	connect(_actionCopy, &QAction::triggered, this, &MainWindow::copy);

	auto pasteIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Paste), colorIcon);
	_actionPaste = new QAction(pasteIcon, tr("&Paste"), this);
	_actionPaste->setShortcuts(QKeySequence::Paste);
	_actionPaste->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
	connect(_actionPaste, &QAction::triggered, this, &MainWindow::paste);

	auto undoIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Undo), colorIcon);
	_actionUndo = new QAction(undoIcon, tr("&Undo"), this);
	_actionUndo->setShortcuts(QKeySequence::Undo);
	_actionUndo->setStatusTip(tr("Undo last made changes"));
	connect(_actionUndo, &QAction::triggered, this, &MainWindow::undo);

	auto redoIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Redo), colorIcon);
	_actionRedo = new QAction(redoIcon, tr("&Redo"), this);
	_actionRedo->setShortcuts(QKeySequence::Redo);
	_actionRedo->setStatusTip(tr("Redo last made changes"));
	connect(_actionRedo, &QAction::triggered, this, &MainWindow::redo);

	auto closeWindowIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::CloseWindow), colorIcon);
	_actionCloseWindow = new QAction(closeWindowIcon, tr("Cl&ose"), this);
	_actionCloseWindow->setStatusTip(tr("Close the active window"));
	_actionCloseWindow->setShortcuts(QKeySequence::Close);
	connect(_actionCloseWindow, &QAction::triggered, _mdiArea, &QMdiArea::closeActiveSubWindow);

	auto closeWindowsIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::CloseWindows), colorIcon);
	_actionCloseWindows = new QAction(closeWindowsIcon, tr("Close &All"), this);
	_actionCloseWindows->setStatusTip(tr("Close all the windows"));
	connect(_actionCloseWindows, &QAction::triggered, _mdiArea, &QMdiArea::closeAllSubWindows);

	_actionTileWindows = new QAction(tr("&Tile"), this);
	_actionTileWindows->setStatusTip(tr("Tile the windows"));
	connect(_actionTileWindows, &QAction::triggered, _mdiArea, &QMdiArea::tileSubWindows);

	_actionCascadeWindows = new QAction(tr("&Cascade"), this);
	_actionCascadeWindows->setStatusTip(tr("Cascade the windows"));
	connect(_actionCascadeWindows, &QAction::triggered, _mdiArea, &QMdiArea::cascadeSubWindows);

	_actionNextWindow = new QAction(tr("Ne&xt"), this);
	_actionNextWindow->setShortcuts(QKeySequence::NextChild);
	_actionNextWindow->setStatusTip(tr("Move the focus to the next window"));
	connect(_actionNextWindow, &QAction::triggered, _mdiArea, &QMdiArea::activateNextSubWindow);

	_actionPreviousWindow = new QAction(tr("Pre&vious"), this);
	_actionPreviousWindow->setShortcuts(QKeySequence::PreviousChild);
	_actionPreviousWindow->setStatusTip(tr("Move the focus to the previous window"));
	connect(_actionPreviousWindow, &QAction::triggered, _mdiArea, &QMdiArea::activatePreviousSubWindow);

	_actionWindowMenuSeparator = new QAction(this);
	_actionWindowMenuSeparator->setSeparator(true);

	auto moduleConfigIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Configuration), colorIcon);
	_actionModuleConfig = new QAction(moduleConfigIcon, tr("&Modules"), this);
	connect(_actionModuleConfig, &QAction::triggered, this, &MainWindow::configModules);
	_actionModuleConfig->setStatusTip(tr("Configure application modules to load"));

	auto settingsIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Settings), colorIcon);
	_actionSettings = new QAction(settingsIcon, tr("&Settings"), this);
	connect(_actionSettings, &QAction::triggered, this, &MainWindow::settingsPropertySheet);
	_actionSettings->setStatusTip(tr("Application and modules settings"));

	auto developIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Development), colorIcon);
	_actionDevelop = new QAction(developIcon, tr("&Develop"), this);
	connect(_actionDevelop, &QAction::triggered, [&](bool) -> void
	{
		if (auto mdi = activeMdiChild())
		{
			mdi->develop();
		}
	});
	_actionDevelop->setStatusTip(tr("Development action as a shortcut for testing stuff."));

	menuFile->addAction(_actionNew);
	menuFile->addAction(_actionOpen);
	menuFile->addAction(_actionSave);
	menuFile->addAction(_actionSaveAs);
	menuFile->addSeparator();

	QMenu* menuRecentFiles = menuFile->addMenu(tr("&Recent..."));
	connect(menuRecentFiles, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
	_actionRecentFileSubMenu = menuRecentFiles->menuAction();
	_actionRecentFileSeparator = menuFile->addSeparator();
	//
	setRecentFilesVisible(!_recentFiles.isEmpty());

	for (auto& recentFileAct: _actionRecentFile)
	{
		recentFileAct = menuRecentFiles->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileAct->setVisible(false);
	}
	menuFile->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);
	menuFile->addSeparator();
	menuFile->addAction(_actionExit);

	menuEdit->addAction(_actionCut);
	menuEdit->addAction(_actionCopy);
	menuEdit->addAction(_actionPaste);
	menuEdit->addAction(_actionUndo);
	menuEdit->addAction(_actionRedo);

	configMenu->addAction(_actionModuleConfig);
	configMenu->addAction(_actionSettings);

	QAction* actionAbout = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
	actionAbout->setStatusTip(tr("Show the application's About box"));

	QAction* aboutQtAction = helpMenu->addAction(tr("About &Qt"), QCoreApplication::instance(), &QApplication::aboutQt);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));

	createAppModuleMenus();

	auto toolBarFile = addToolBar(tr("File"));
	toolBarFile->setObjectName("File");
	auto toolBarEdit = addToolBar(tr("Edit"));
	toolBarEdit->setObjectName("Edit");
	auto toolBarConfig = addToolBar(tr("Config"));
	toolBarConfig->setObjectName("Config");

	toolBarFile->addAction(_actionNew);
	toolBarFile->addAction(_actionOpen);
	toolBarFile->addAction(_actionSave);
	toolBarFile->addAction(_actionCloseWindow);

	toolBarEdit->addAction(_actionCut);
	toolBarEdit->addAction(_actionCopy);
	toolBarEdit->addAction(_actionPaste);
	toolBarEdit->addAction(_actionUndo);
	toolBarEdit->addAction(_actionRedo);

	toolBarConfig->addAction(_actionModuleConfig);
	toolBarConfig->addAction(_actionSettings);
	toolBarConfig->addAction(_actionDevelop);

	createAppModuleToolBars();

	updateWindowMenu();
}

void MainWindow::createAppModuleMenus()
{

	AppModuleInterface::addAllMenuItems(AppModuleInterface::Settings, _menuSettings);
	AppModuleInterface::addAllMenuItems(AppModuleInterface::View, _menuView);
	AppModuleInterface::addAllMenuItems(AppModuleInterface::Tools, _menuTools);
	for (auto m: {&_menuView, &_menuTools, &_menuSettings})
	{
		if ((*m) && (*m)->isEmpty())
		{
			delete_null((*m));
		}
	}
}

void MainWindow::createAppModuleToolBars()
{
	AppModuleInterface::addAllToolBars(this);
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::settingsReadWrite(bool save)
{
	const auto keyDisplayName = QString("DisplayName");
	const auto keyViewMode = QString("ViewMode");
	//
	_settings->beginGroup("MainWindow");
	if (save)
	{
		_settings->setValue(keyDisplayName, QApplication::applicationDisplayName());
		_settings->setValue(keyViewMode, _mdiArea->viewMode());
	}
	else
	{
		QApplication::setApplicationDisplayName(_settings->value(keyDisplayName, QApplication::applicationDisplayName()).toString());
		_mdiArea->setViewMode(clip(qvariant_cast<QMdiArea::ViewMode>(_settings->value(keyViewMode)), QMdiArea::ViewMode::SubWindowView,
			QMdiArea::ViewMode::TabbedView));
	}
	_settings->endGroup();
}

MultiDocInterface* MainWindow::createMdiChild(const QString& filename)
{
	QFileInfo fi(filename);
	AppModuleInterface* entry;
	// When empty a new file is expected.
	if (filename.isEmpty())
	{
		entry = AppModuleInterface::selectDialog(tr("New File"), _settings, this);
	}
	else
	{
		// When the file does not exist.
		if (!fi.exists())
		{
			QMessageBox::information(this, tr("File does not exist!"),
				tr("Missing: '%1'").arg(fi.filePath()));
			return nullptr;
		}
		// Get the instance that can handle it.
		entry = AppModuleInterface::findByFile(filename);
		if (!entry)
		{
			entry = AppModuleInterface::selectDialog(tr("Open file with"), _settings, this);
		}
	}
	// When an entry was found for the file create an MDI child.
	if (entry)
	{
		auto child = entry->createChild(this);
		if (child)
		{
			auto sw = _mdiArea->addSubWindow(dynamic_cast<QWidget*>(child));
			// Restore the state of the child after it got its actual parent.
			child->stateSaveRestore(false);
			//
			sw->setWindowIcon(Resource::getSvgIcon(entry->getSvgIconResource(),
				QApplication::palette().color(
					_mdiArea->viewMode() == QMdiArea::ViewMode::TabbedView ?
						QPalette::ColorRole::ButtonText : QPalette::ColorRole::WindowText)));
			// Connect the MDI actions to the
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::modificationChanged, _actionSave, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::copyAvailable, _actionCut, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::copyAvailable, _actionCopy, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::undoAvailable, _actionUndo, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::redoAvailable, _actionRedo, &QAction::setEnabled);
			// Fix the system menu shortcut key for QKeySequence::Close (Ctrl + W) to be ambiguous
			// with an item in the main menu by replacing it.
			for (auto action: sw->systemMenu()->actions())
			{
				if (action->shortcut() == QKeySequence::Close)
				{
					action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F4));
				}
			}
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

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent* event)
{
	for (const auto& url: event->mimeData()->urls())
	{
		openFile(url.toLocalFile());
	}
}

void MainWindow::createDockWindows()
{
	dockWidgets = AppModuleInterface::createAllDockingWidgets(this);
	for (auto dock: dockWidgets)
	{
		addDockWidget(Qt::RightDockWidgetArea, dock);
	}
}

}
