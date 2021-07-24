#include <QtWidgets>
#include <QPlainTextEdit>

#include "MainWindow.h"
#include "ApplicationPropertyPage.h"
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/ModuleConfiguration.h>
#include <misc/qt/PropertySheetDialog.h>
#include <ami/iface/AppModuleInterface.h>
#include <misc/gen/gen_utils.h>

namespace sf
{

MainWindow::MainWindow(QSettings* settings)
	:_mdiArea(new QMdiArea)
	 , _settings(settings)
	 , _moduleConfiguration(new ModuleConfiguration(settings, this))
{
	// Set the object name for the getObjectNamePath() function so property sheet settings get a correct section assigned.
	setObjectName("MainWindow");
	// Set a property to be used as parent for shortcuts since an application shortcut needs a widget and not an object.
	QApplication::instance()->setProperty("ShortcutParent", QVariant::fromValue(this));
	// Title is the same as the application.
	setWindowTitle(QApplication::applicationDisplayName());
	//
	connect(qApp, &QApplication::applicationDisplayNameChanged, [&]() // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	{
		setWindowTitle(QApplication::applicationDisplayName());
	});
	// When libraries are loaded create the module instances.
	connect(_moduleConfiguration, &ModuleConfiguration::libraryLoaded, [&]()
	{
		// Create the interface implementations (that are missing).
		AppModuleInterface::instantiate(_settings, this);
	});
	//
	_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_mdiArea->setTabsMovable(true);
	//_mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
	_mdiArea->setViewMode(QMdiArea::ViewMode::TabbedView);

	setCentralWidget(_mdiArea);
	connect(_mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);

	createActions();
	createStatusBar();
	updateMenus();
	//
	setUnifiedTitleAndToolBarOnMac(true);
	//
	setAcceptDrops(true);
	//
	settingsReadWrite(false);
	//
	recentFilesReadWrite(false);
	//
	stateSaveRestore(false);
	//
	//AppModuleInterface::OpenFileClosure;
	AppModuleInterface::callbackOpenFile = [&](const QString& filename, AppModuleInterface* ami) -> MultiDocInterface*
	{
		return openFile(filename);
	};
}

MainWindow::~MainWindow()
{
	stateSaveRestore(true);
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
	_mdiArea->closeAllSubWindows();
	if (_mdiArea->currentSubWindow())
	{
		event->ignore();
	}
	else
	{
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

MultiDocInterface* MainWindow::openFile(const QString& fileName)
{
	if (auto existing = findMdiChild(fileName))
	{
		_mdiArea->setActiveSubWindow(existing);
		return dynamic_cast<MultiDocInterface*>(existing->widget());
	}
	return loadFile(fileName);
}

MultiDocInterface* MainWindow::loadFile(const QString& fileName)
{
	auto child = createMdiChild(fileName);
	if (child)
	{
		auto widget = dynamic_cast<QWidget*>(child);
		if (child->loadFile(fileName))
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
		MainWindow::prependToRecentFiles(fileName);
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
	_recentFileSubMenuAction->setVisible(visible);
	_recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
	const qsizetype count = qMin<qsizetype>(MaxRecentFiles, _recentFiles.size());
	int i = 0;
	for (; i < count; ++i)
	{
		const QString fileName = QFileInfo(_recentFiles.at(i)).fileName();
		_recentFileActions[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
		_recentFileActions[i]->setData(_recentFiles.at(i));
		_recentFileActions[i]->setStatusTip(_recentFiles.at(i));
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

void MainWindow::settingsPropertySheet()
{
	auto sheet = new PropertySheetDialog("ApplicationSettings", _settings, this);
	sheet->setWindowTitle(tr("Application Settings"));
	sheet->setWindowIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Settings), Qt::gray));
	//
	sheet->addPage(new ApplicationPropertyPage(this, sheet));
	// Adds all the property pages to the passed property sheet.
	AppModuleInterface::addAllPropertyPages(sheet);
	//
	sheet->exec();
	delete sheet;
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

	auto newIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::New), iconColor);
	_newAction = new QAction(newIcon, tr("&New"), this);
	_newAction->setShortcuts(QKeySequence::New);
	_newAction->setStatusTip(tr("Create a new file"));
	connect(_newAction, &QAction::triggered, this, &MainWindow::newFile);

	auto openIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Open), iconColor);
	_openAction = new QAction(openIcon, tr("&Open..."), this);
	_openAction->setShortcuts(QKeySequence::Open);
	_openAction->setStatusTip(tr("Open an existing file"));
	connect(_openAction, &QAction::triggered, this, &MainWindow::open);

	auto saveIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Save), iconColor);
	_saveAction = new QAction(saveIcon, tr("&Save"), this);
	_saveAction->setShortcuts(QKeySequence::Save);
	_saveAction->setStatusTip(tr("Save the document to disk"));
	connect(_saveAction, &QAction::triggered, this, &MainWindow::save);

	auto saveAsIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Save), iconColor);
	_saveAsAction = new QAction(saveAsIcon, tr("Save &As..."), this);
	_saveAsAction->setShortcuts(QKeySequence::SaveAs);
	_saveAsAction->setStatusTip(tr("Save the document under a new name"));
	connect(_saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);

	auto exitIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Exit), iconColor);
	auto exitAction = new QAction(exitIcon, tr("E&xit"), this);
	connect(exitAction, &QAction::triggered, QCoreApplication::instance(), &QApplication::closeAllWindows);
	exitAction->setShortcuts(QKeySequence::Quit);
	exitAction->setStatusTip(tr("Exit the application"));

	auto cutIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Cut), iconColor);
	_cutAction = new QAction(cutIcon, tr("Cu&t"), this);
	_cutAction->setShortcuts(QKeySequence::Cut);
	_cutAction->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
	connect(_cutAction, &QAction::triggered, this, &MainWindow::cut);

	auto copyIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Copy),
		iconColor);
	_copyAction = new QAction(copyIcon, tr("&Copy"), this);
	_copyAction->setShortcuts(QKeySequence::Copy);
	_copyAction->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
	connect(_copyAction, &QAction::triggered, this, &MainWindow::copy);

	auto pasteIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Paste), iconColor);
	_pasteAction = new QAction(pasteIcon, tr("&Paste"), this);
	_pasteAction->setShortcuts(QKeySequence::Paste);
	_pasteAction->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
	connect(_pasteAction, &QAction::triggered, this, &MainWindow::paste);

	auto undoIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Undo), iconColor);
	_undoAction = new QAction(undoIcon, tr("&Undo"), this);
	_undoAction->setShortcuts(QKeySequence::Undo);
	_undoAction->setStatusTip(tr("Undo last made changes"));
	connect(_undoAction, &QAction::triggered, this, &MainWindow::undo);

	auto redoIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Redo), iconColor);
	_redoAction = new QAction(redoIcon, tr("&Redo"), this);
	_redoAction->setShortcuts(QKeySequence::Redo);
	_redoAction->setStatusTip(tr("Redo last made changes"));
	connect(_redoAction, &QAction::triggered, this, &MainWindow::redo);

	auto closeWindowIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::CloseWindow), iconColor);
	_closeWindowAction = new QAction(closeWindowIcon, tr("Cl&ose"), this);
	_closeWindowAction->setStatusTip(tr("Close the active window"));
	_closeWindowAction->setShortcuts(QKeySequence::Close);
	connect(_closeWindowAction, &QAction::triggered, _mdiArea, &QMdiArea::closeActiveSubWindow);

	auto closeWindowsIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::CloseWindows), iconColor);
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

	auto moduleConfigIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Configuration), iconColor);
	_moduleConfigAction = new QAction(moduleConfigIcon, tr("&Modules"), this);
	connect(_moduleConfigAction, &QAction::triggered, this, &MainWindow::configModules);
	_moduleConfigAction->setStatusTip(tr("Configure application modules to load"));

	auto settingsIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Settings), iconColor);
	_settingsAction = new QAction(settingsIcon, tr("&Settings"), this);
	connect(_settingsAction, &QAction::triggered, this, &MainWindow::settingsPropertySheet);
	_settingsAction->setStatusTip(tr("Application and modules settings"));

	auto developIcon = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Development), iconColor);
	_developAction = new QAction(developIcon, tr("&Develop"), this);
	connect(_developAction, &QAction::triggered, [&](bool) -> void
	{
		if (auto mdi = activeMdiChild())
		{
			mdi->develop();
		}
	});
	_developAction->setStatusTip(tr("Development action as a shortcut for testing stuff."));

	fileMenu->addAction(_newAction);
	fileMenu->addAction(_openAction);
	fileMenu->addAction(_saveAction);
	fileMenu->addAction(_saveAsAction);
	fileMenu->addSeparator();

	QMenu* recentMenu = fileMenu->addMenu(tr("&Recent..."));
	connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
	_recentFileSubMenuAction = recentMenu->menuAction();
	_recentFileSeparator = fileMenu->addSeparator();
	//
	setRecentFilesVisible(!_recentFiles.isEmpty());

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
	configMenu->addAction(_settingsAction);

	QAction* aboutAction = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
	aboutAction->setStatusTip(tr("Show the application's About box"));

	QAction* aboutQtAction = helpMenu->addAction(tr("About &Qt"), QCoreApplication::instance(), &QApplication::aboutQt);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));

	updateWindowMenu();

	auto fileToolBar = addToolBar(tr("File"));
	fileToolBar->setObjectName("File");
	auto editToolBar = addToolBar(tr("Edit"));
	editToolBar->setObjectName("Edit");
	auto configToolBar = addToolBar(tr("Config"));
	configToolBar->setObjectName("Config");

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
	configToolBar->addAction(_settingsAction);
	configToolBar->addAction(_developAction);
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::settingsReadWrite(bool save)
{
	// Load the missing modules from the configuration.
	_moduleConfiguration->load();
	//
	auto settings = _moduleConfiguration->getSettings();
	//
	auto keyDisplayName = QString("DisplayName");
	auto keyViewMode = QString("ViewMode");
	//
	settings->beginGroup("Application");
	if (save)
	{
		settings->setValue(keyDisplayName, QApplication::applicationDisplayName());
		settings->setValue(keyViewMode, _mdiArea->viewMode());
	}
	else
	{
		QApplication::setApplicationDisplayName(
			settings->value(keyDisplayName, QApplication::applicationDisplayName()).toString());
		_mdiArea->setViewMode(
			clip(qvariant_cast<QMdiArea::ViewMode>(settings->value(keyViewMode)), QMdiArea::ViewMode::SubWindowView,
				QMdiArea::ViewMode::TabbedView));
	}
	settings->endGroup();
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
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::modificationChanged, _saveAction, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::copyAvailable, _cutAction, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::copyAvailable, _copyAction, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::undoAvailable, _undoAction, &QAction::setEnabled);
			connect(&child->mdiSignals, &MultiDocInterfaceSignals::redoAvailable, _redoAction, &QAction::setEnabled);
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

}
