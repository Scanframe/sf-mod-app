#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QtPrintSupport>
#endif
#endif

#include "mainwindow.h"

MainWindow::MainWindow()
	:_textEdit(new QTextEdit)
{
	setCentralWidget(_textEdit);

	createActions();
	createStatusBar();
	createDockWindows();

	setWindowTitle(tr("Dock Widgets"));

	newLetter();
	setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::newLetter()
{
	_textEdit->clear();

	QTextCursor cursor(_textEdit->textCursor());
	cursor.movePosition(QTextCursor::Start);
	QTextFrame* topFrame = cursor.currentFrame();
	QTextFrameFormat topFrameFormat = topFrame->frameFormat();
	topFrameFormat.setPadding(16);
	topFrame->setFrameFormat(topFrameFormat);

	QTextCharFormat textFormat;
	QTextCharFormat boldFormat;
	boldFormat.setFontWeight(QFont::Bold);
	QTextCharFormat italicFormat;
	italicFormat.setFontItalic(true);

	QTextTableFormat tableFormat;
	tableFormat.setBorder(1);
	tableFormat.setCellPadding(16);
	tableFormat.setAlignment(Qt::AlignRight);
	cursor.insertTable(1, 1, tableFormat);
	cursor.insertText("The Firm", boldFormat);
	cursor.insertBlock();
	cursor.insertText("321 City Street", textFormat);
	cursor.insertBlock();
	cursor.insertText("Industry Park");
	cursor.insertBlock();
	cursor.insertText("Some Country");
	cursor.setPosition(topFrame->lastPosition());
	cursor.insertText(QDate::currentDate().toString("d MMMM yyyy"), textFormat);
	cursor.insertBlock();
	cursor.insertBlock();
	cursor.insertText("Dear ", textFormat);
	cursor.insertText("NAME", italicFormat);
	cursor.insertText(",", textFormat);
	for (int i = 0; i < 3; ++i)
	{
		cursor.insertBlock();
	}
	cursor.insertText(tr("Yours sincerely,"), textFormat);
	for (int i = 0; i < 3; ++i)
	{
		cursor.insertBlock();
	}
	cursor.insertText("The Boss", textFormat);
	cursor.insertBlock();
	cursor.insertText("ADDRESS", italicFormat);
}

void MainWindow::print()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printdialog)
	QTextDocument* document = _textEdit->document();
	QPrinter printer;

	QPrintDialog dlg(&printer, this);
	if (dlg.exec() != QDialog::Accepted)
	{
		return;
	}

	document->print(&printer);
	statusBar()->showMessage(tr("Ready"), 2000);
#endif
}

void MainWindow::save()
{
	QMimeDatabase mimeDatabase;
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Choose a file name"), ".",
		mimeDatabase.mimeTypeForName("text/html").filterString());
	if (fileName.isEmpty())
	{
		return;
	}
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("Dock Widgets"),
			tr("Cannot write file %1:\n%2.")
				.arg(QDir::toNativeSeparators(fileName), file.errorString()));
		return;
	}

	QTextStream out(&file);
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	out << _textEdit->toHtml();
	QGuiApplication::restoreOverrideCursor();

	statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}

void MainWindow::undo()
{
	QTextDocument* document = _textEdit->document();
	document->undo();
}

void MainWindow::insertCustomer(const QString& customer)
{
	if (customer.isEmpty())
	{
		return;
	}
	auto customerList = customer.split(", ");
	QTextDocument* document = _textEdit->document();
	QTextCursor cursor = document->find("NAME");
	if (!cursor.isNull())
	{
		cursor.beginEditBlock();
		cursor.insertText(customerList.at(0));
		QTextCursor oldcursor = cursor;
		cursor = document->find("ADDRESS");
		if (!cursor.isNull())
		{
			for (int i = 1; i < customerList.size(); ++i)
			{
				cursor.insertBlock();
				cursor.insertText(customerList.at(i));
			}
			cursor.endEditBlock();
		}
		else
		{
			oldcursor.endEditBlock();
		}
	}
}

void MainWindow::addParagraph(const QString& paragraph)
{
	if (paragraph.isEmpty())
	{
		return;
	}
	QTextDocument* document = _textEdit->document();
	QTextCursor cursor = document->find(tr("Yours sincerely,"));
	if (cursor.isNull())
	{
		return;
	}
	cursor.beginEditBlock();
	cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, 2);
	cursor.insertBlock();
	cursor.insertText(paragraph);
	cursor.insertBlock();
	cursor.endEditBlock();

}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Dock Widgets"),
		tr("The <b>Dock Widgets</b> example demonstrates how to "
			 "use Qt's dock widgets. You can enter your own text, "
			 "click a customer to add a customer name and "
			 "address, and click standard paragraphs to add them."));
}

void MainWindow::createActions()
{
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QToolBar* fileToolBar = addToolBar(tr("File"));

	const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
	auto newLetterAct = new QAction(newIcon, tr("&New Letter"), this);
	newLetterAct->setShortcuts(QKeySequence::New);
	newLetterAct->setStatusTip(tr("Create a new form letter"));
	connect(newLetterAct, &QAction::triggered, this, &MainWindow::newLetter);
	fileMenu->addAction(newLetterAct);
	fileToolBar->addAction(newLetterAct);

	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
	auto saveAct = new QAction(saveIcon, tr("&Save..."), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the current form letter"));
	connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	fileMenu->addAction(saveAct);
	fileToolBar->addAction(saveAct);

	const QIcon printIcon = QIcon::fromTheme("document-print", QIcon(":/images/print.png"));
	auto printAct = new QAction(printIcon, tr("&Print..."), this);
	printAct->setShortcuts(QKeySequence::Print);
	printAct->setStatusTip(tr("Print the current form letter"));
	connect(printAct, &QAction::triggered, this, &MainWindow::print);
	fileMenu->addAction(printAct);
	fileToolBar->addAction(printAct);

	fileMenu->addSeparator();

	QAction* quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
	quitAct->setShortcuts(QKeySequence::Quit);
	quitAct->setStatusTip(tr("Quit the application"));

	QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
	QToolBar* editToolBar = addToolBar(tr("Edit"));
	const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(":/images/undo.png"));
	auto undoAct = new QAction(undoIcon, tr("&Undo"), this);
	undoAct->setShortcuts(QKeySequence::Undo);
	undoAct->setStatusTip(tr("Undo the last editing action"));
	connect(undoAct, &QAction::triggered, this, &MainWindow::undo);
	editMenu->addAction(undoAct);
	editToolBar->addAction(undoAct);

	_viewMenu = menuBar()->addMenu(tr("&View"));

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

void MainWindow::createDockWindows()
{
	auto dock = new QDockWidget(tr("Customers"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	_customerList = new QListWidget(dock);
	_customerList->addItems(QStringList()
		<< "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
		<< "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
		<< "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
		<< "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
		<< "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
		<< "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
	dock->setWidget(_customerList);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	_viewMenu->addAction(dock->toggleViewAction());

	dock = new QDockWidget(tr("Paragraphs"), this);
	_paragraphsList = new QListWidget(dock);
	_paragraphsList->addItems(QStringList()
		<< "Thank you for your payment which we have received today."
		<< "Your order has been dispatched and should be with you "
			 "within 28 days."
		<< "We have dispatched those items that were in stock. The "
			 "rest of your order will be dispatched once all the "
			 "remaining items have arrived at our warehouse. No "
			 "additional shipping charges will be made."
		<< "You made a small overpayment (less than $5) which we "
			 "will keep on account for you, or return at your request."
		<< "You made a small underpayment (less than $1), but we have "
			 "sent your order anyway. We'll add this underpayment to "
			 "your next bill."
		<< "Unfortunately you did not send enough money. Please remit "
			 "an additional $. Your order will be dispatched as soon as "
			 "the complete amount has been received."
		<< "You made an overpayment (more than $5). Do you wish to "
			 "buy more items, or should we return the excess to you?");
	dock->setWidget(_paragraphsList);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	_viewMenu->addAction(dock->toggleViewAction());

	connect(_customerList, &QListWidget::currentTextChanged,
		this, &MainWindow::insertCustomer);
	connect(_paragraphsList, &QListWidget::currentTextChanged,
		this, &MainWindow::addParagraph);
}

