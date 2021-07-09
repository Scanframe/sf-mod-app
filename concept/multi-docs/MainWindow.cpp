
#include <QtWidgets>

#include "MainWindow.h"
#include "TextEditor.h"
#include "CodeEditor.h"
#include <misc/qt/Resource.h>

MainWindow::MainWindow()
	:mdiArea(new QMdiArea)
{
	mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mdiArea->setViewMode(QMdiArea::TabbedView);

	setCentralWidget(mdiArea);
	connect(mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);

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
	mdiArea->closeAllSubWindows();
	if (mdiArea->currentSubWindow())
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
	auto child = createMdiChild();
	child->newFile();
	if (auto widget = dynamic_cast<QWidget*>(child))
	{
		//child->show();
		widget->showMaximized();
	}
}

void MainWindow::open()
{
	const QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty())
	{
		openFile(fileName);
	}
}

bool MainWindow::openFile(const QString& fileName)
{
	if (QMdiSubWindow* existing = findMdiChild(fileName))
	{
		mdiArea->setActiveSubWindow(existing);
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
	auto child = createMdiChild();
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

static inline QString recentFilesKey() {return QStringLiteral("recentFileList");}

static inline QString fileKey() {return QStringLiteral("file");}

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

void MainWindow::prependToRecentFiles(const QString& fileName)
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

	const QStringList oldRecentFiles = readRecentFiles(settings);
	QStringList recentFiles = oldRecentFiles;
	recentFiles.removeAll(fileName);
	recentFiles.prepend(fileName);
	if (oldRecentFiles != recentFiles)
	{
		writeRecentFiles(recentFiles, settings);
	}

	setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
	recentFileSubMenuAct->setVisible(visible);
	recentFileSeparator->setVisible(visible);
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
		recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
		recentFileActs[i]->setData(recentFiles.at(i));
		recentFileActs[i]->setVisible(true);
	}
	for (; i < MaxRecentFiles; ++i)
	{
		recentFileActs[i]->setVisible(false);
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

#ifndef QT_NO_CLIPBOARD

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

#endif

void MainWindow::about()
{
	QMessageBox::about(this, tr("About MDI"),
		tr("The <b>MDI</b> example demonstrates how to write multiple "
			 "document interface applications using Qt."));
}

void MainWindow::updateMenus()
{
	bool hasMdiChild = (activeMdiChild() != nullptr);
	saveAct->setEnabled(hasMdiChild);
	saveAsAct->setEnabled(hasMdiChild);
#ifndef QT_NO_CLIPBOARD
	pasteAct->setEnabled(hasMdiChild);
#endif
	closeAct->setEnabled(hasMdiChild);
	closeAllAct->setEnabled(hasMdiChild);
	tileAct->setEnabled(hasMdiChild);
	cascadeAct->setEnabled(hasMdiChild);
	nextAct->setEnabled(hasMdiChild);
	previousAct->setEnabled(hasMdiChild);
	windowMenuSeparatorAct->setVisible(hasMdiChild);

#ifndef QT_NO_CLIPBOARD
	bool hasSelection = (activeMdiChild() && activeMdiChild()->hasSelection());
	cutAct->setEnabled(hasSelection);
	copyAct->setEnabled(hasSelection);
#endif
}

void MainWindow::updateWindowMenu()
{
	windowMenu->clear();
	windowMenu->addAction(closeAct);
	windowMenu->addAction(closeAllAct);
	windowMenu->addSeparator();
	windowMenu->addAction(tileAct);
	windowMenu->addAction(cascadeAct);
	windowMenu->addSeparator();
	windowMenu->addAction(nextAct);
	windowMenu->addAction(previousAct);
	windowMenu->addAction(windowMenuSeparatorAct);

	QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
	windowMenuSeparatorAct->setVisible(!windows.isEmpty());

	for (int i = 0; i < windows.size(); ++i)
	{
		QMdiSubWindow* mdiSubWindow = windows.at(i);
		//auto child = qobject_cast<TextEditor*>(mdiSubWindow->widget());
		auto child = dynamic_cast<MdiChild*>(mdiSubWindow->widget());
		QString text;
		if (i < 9)
		{
			text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
		}
		else
		{
			text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
		}
		QAction* action = windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]()
		{
			mdiArea->setActiveSubWindow(mdiSubWindow);
		});
		action->setCheckable(true);
		action->setChecked(child == activeMdiChild());
	}
}

MdiChild* MainWindow::createMdiChild()
{
	if (1)
	{
		auto child = new CodeEditor;
		mdiArea->addSubWindow(child);
#ifndef QT_NO_CLIPBOARD
		connect(child, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
		connect(child, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif
		return child;
	}
	else
	{
		auto child = new TextEditor;
		mdiArea->addSubWindow(child);
#ifndef QT_NO_CLIPBOARD
		connect(child, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
		connect(child, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif
		return child;
	}
}

void MainWindow::createActions()
{
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QToolBar* fileToolBar = addToolBar(tr("File"));
	// Color for the icons.
	const QColor iconColor = QApplication::palette().color(QApplication::palette().currentColorGroup(), QPalette::ColorRole::ButtonText);

	//const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
	const QIcon newIcon = 	sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::New), iconColor);

	newAct = new QAction(newIcon, tr("&New"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Create a new file"));
	connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
	fileMenu->addAction(newAct);
	fileToolBar->addAction(newAct);

	//const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
	const QIcon openIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Open), iconColor);

	auto openAct = new QAction(openIcon, tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	fileMenu->addAction(openAct);
	fileToolBar->addAction(openAct);

	//const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
	const QIcon saveIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Save), iconColor);

	saveAct = new QAction(saveIcon, tr("&Save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	fileToolBar->addAction(saveAct);

	//const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
	const QIcon saveAsIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Save), iconColor);

	saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
	saveAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
	fileMenu->addAction(saveAsAct);

	fileMenu->addSeparator();

	QMenu* recentMenu = fileMenu->addMenu(tr("Recent..."));
	connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
	recentFileSubMenuAct = recentMenu->menuAction();

	for (auto& recentFileAct : recentFileActs)
	{
		recentFileAct = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileAct->setVisible(false);
	}

	recentFileSeparator = fileMenu->addSeparator();

	setRecentFilesVisible(MainWindow::hasRecentFiles());

	fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);

	fileMenu->addSeparator();

	const QIcon exitIcon = QIcon::fromTheme("application-exit");
	QAction* exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), QCoreApplication::instance(),
		&QApplication::closeAllWindows);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Exit the application"));
	fileMenu->addAction(exitAct);

#ifndef QT_NO_CLIPBOARD
	QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
	QToolBar* editToolBar = addToolBar(tr("Edit"));

	//const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
	const QIcon cutIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Cut), iconColor);

	cutAct = new QAction(cutIcon, tr("Cu&t"), this);
	cutAct->setShortcuts(QKeySequence::Cut);
	cutAct->setStatusTip(tr("Cut the current selection's contents to the "
													"clipboard"));
	connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
	editMenu->addAction(cutAct);
	editToolBar->addAction(cutAct);

	//const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
	const QIcon copyIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Copy), iconColor);

	copyAct = new QAction(copyIcon, tr("&Copy"), this);
	copyAct->setShortcuts(QKeySequence::Copy);
	copyAct->setStatusTip(tr("Copy the current selection's contents to the "
													 "clipboard"));
	connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
	editMenu->addAction(copyAct);
	editToolBar->addAction(copyAct);

	//const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
	const QIcon pasteIcon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Paste), iconColor);

	pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
	pasteAct->setShortcuts(QKeySequence::Paste);
	pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
														"selection"));
	connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
	editMenu->addAction(pasteAct);
	editToolBar->addAction(pasteAct);
#endif

	windowMenu = menuBar()->addMenu(tr("&Window"));
	connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

	closeAct = new QAction(tr("Cl&ose"), this);
	closeAct->setStatusTip(tr("Close the active window"));
	connect(closeAct, &QAction::triggered, mdiArea, &QMdiArea::closeActiveSubWindow);

	closeAllAct = new QAction(tr("Close &All"), this);
	closeAllAct->setStatusTip(tr("Close all the windows"));
	connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

	tileAct = new QAction(tr("&Tile"), this);
	tileAct->setStatusTip(tr("Tile the windows"));
	connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

	cascadeAct = new QAction(tr("&Cascade"), this);
	cascadeAct->setStatusTip(tr("Cascade the windows"));
	connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

	nextAct = new QAction(tr("Ne&xt"), this);
	nextAct->setShortcuts(QKeySequence::NextChild);
	nextAct->setStatusTip(tr("Move the focus to the next window"));
	connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

	previousAct = new QAction(tr("Pre&vious"), this);
	previousAct->setShortcuts(QKeySequence::PreviousChild);
	previousAct->setStatusTip(tr("Move the focus to the previous window"));
	connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);

	windowMenuSeparatorAct = new QAction(this);
	windowMenuSeparatorAct->setSeparator(true);

	updateWindowMenu();

	menuBar()->addSeparator();

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
}

void MainWindow::writeSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("geometry", saveGeometry());
}

MdiChild* MainWindow::activeMdiChild() const
{
	if (QMdiSubWindow* activeSubWindow = mdiArea->activeSubWindow())
	{
		//return qobject_cast<TextEditor*>(activeSubWindow->widget());
		return dynamic_cast<MdiChild*>(activeSubWindow->widget());
	}
	return nullptr;
}

QMdiSubWindow* MainWindow::findMdiChild(const QString& fileName) const
{
	QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
	const QList<QMdiSubWindow*> subWindows = mdiArea->subWindowList();
	for (QMdiSubWindow* window: subWindows)
	{
		//auto child = qobject_cast<TextEditor*>(window->widget());
		auto child = dynamic_cast<MdiChild*>(window->widget());
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
