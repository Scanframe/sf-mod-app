#include "test-ScriptWindow.h"
#include "ui_test-ScriptWindow.h"

#include <iostream>
#include <QDebug>
#include <QAbstractButton>
#include <QPainter>
#include <misc/qt/CaptureListModel.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/ScriptModelList.h>
#include <misc/qt/ScriptHighlighter.h>
#include <misc/qt/Resource.h>

using namespace sf;

const char* InitialScript = R"(// Some comment 1.
// Some comment 2.
int my_int = 1;
float my_float = PI * 2e+2;
string my_string = "Just some text... ";
return;

// Some comment on main label.
:main;
my_int = my_int + 1;
if (my_int % 2)
	gosub subroutine1;
else
	gosub subroutine2;
return;

// Some comment on routine 1.
:subroutine1;
writelog("The sub-routine1\n");
return;

// Some comment on routine 2.
:subroutine2;
writelog("The sub-routine2 ", my_string, my_float / 2e1, my_int, "\n");
return;

)";

ScriptWindow::ScriptWindow(QSettings* settings, QWidget* parent)
	:QMainWindow(parent)
	 , ui(new Ui::ScriptWindow)
	 , _settings(settings)
{
	ui->setupUi(this);
	ui->tvInstructions->header()->setStretchLastSection(true);
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/ico/scanframe"));
	centralWidget()->setAttribute(Qt::WA_AlwaysShowToolTips, true);
	// Assign the model to the listView.
	auto clm = new CaptureListModel(ui->lvLogging);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog);
	// Assign the modal to the listview.
	ui->lvLogging->setModel(clm);
	// Method of connecting Qt5 style
	connect(ui->actionCompile, &QAction::triggered, this, &ScriptWindow::onActionCompile);
	connect(ui->actionInitialize, &QAction::triggered, this, &ScriptWindow::onActionInitialize);
	connect(ui->actionStep, &QAction::triggered, this, &ScriptWindow::onActionStep);
	connect(ui->actionRun, &QAction::triggered, this, &ScriptWindow::onActionRun);
	connect(ui->tvInstructions, &QTreeView::clicked, this, &ScriptWindow::onClickInstruction);
	ui->pteSource->setPlainText(InitialScript);
	_script.setScriptName("test-script");
	auto lm_instr = new ScriptModelList(this);
	lm_instr->setInterpreter(&_script, ScriptModelList::mInstructions);
	ui->tvInstructions->setModel(lm_instr);
	auto lm_var = new ScriptModelList(this);
	lm_var->setInterpreter(&_script, ScriptModelList::mVariables);
	ui->tvVariables->setModel(lm_var);
	// Highlight the keywords and so on.
	new ScriptHighlighter(&_script, ui->pteSource->document());
	// Set monospaced font.
	QFont font("Monospace");
	font.setStyleHint(QFont::StyleHint::Monospace);
	// Make the new font the same size as the current one.
	font.setPointSizeF(ui->pteSource->document()->defaultFont().pointSizeF());
	ui->pteSource->document()->setDefaultFont(font);
	// Set tab stop the width of 2 space characters.
	QFontMetrics fm(ui->pteSource->document()->defaultFont());
	// Use the width of a space for tab indentation width.
	ui->pteSource->setTabStopDistance(fm.horizontalAdvance(' ') * 2);
	ui->actionCompile->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Compile), QPalette::ColorRole::ButtonText));
	ui->actionInitialize->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Initialize), QPalette::ColorRole::ButtonText));
	ui->actionStep->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Step), QPalette::ColorRole::ButtonText));
	ui->actionRun->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Run), QPalette::ColorRole::ButtonText));
	//
	stateSaveRestore(false);
}

ScriptWindow::~ScriptWindow()
{
	stateSaveRestore(true);
	auto clm = reinterpret_cast<CaptureListModel*>(ui->lvLogging->model());
	clm->setSource(0);
}

void ScriptWindow::stateSaveRestore(bool save)
{
	_settings->beginGroup(getObjectNamePath(this).join('.').prepend("State."));
	QString keyWidgetRect("WidgetRect");
	QString keyState("State");
	QString keySplitter("SplitterHorizontal");
	QString keySplitter2("SplitterVertical");
	QString keyTabWidget("TabWidget");
	if (save)
	{
		_settings->setValue(keyWidgetRect, geometry());
		_settings->setValue(keyState, saveState());
		_settings->setValue(keySplitter, ui->splitterHorizontal->saveState());
		_settings->setValue(keySplitter2, ui->splitterVertical->saveState());
		_settings->setValue(keyTabWidget, ui->tabWidget->currentIndex());
	}
	else
	{
		// Get the keys in the section to check existence in the ini-section.
		if (_settings->value(keyWidgetRect).toRect().isValid())
		{
			setGeometry(_settings->value(keyWidgetRect).toRect());
		}
		restoreState(_settings->value(keyState).toByteArray());
		ui->splitterHorizontal->restoreState(_settings->value(keySplitter).toByteArray());
		ui->splitterVertical->restoreState(_settings->value(keySplitter2).toByteArray());
		ui->tabWidget->setCurrentIndex(_settings->value(keyTabWidget).toInt());
	}
	_settings->endGroup();
}

void ScriptWindow::updateStatus()
{
	statusBar()->showMessage(QString(_script.getStateName()) + ": " + _script.getErrorText() +
		": " + QString::fromStdString(_script.getErrorReason()));
	// Get the current instruction pointer.
	auto instr = (int) _script.getInstructionPtr();
	// Check if the pointer is valid.
	if (instr >= 0)
	{
		// Select the instruction list row and just the first column for no apparent reason.
		ui->tvInstructions->setCurrentIndex(ui->tvInstructions->model()->index(instr, 0));
		//
		moveCursorToInstruction(instr);
	}
}

void ScriptWindow::onActionCompile()
{
	auto src = ui->pteSource->toPlainText().toStdString();

	if (_script.compile(src.c_str()))
	{
		std::clog << "Compile succeeded." << std::endl;
	}
	else
	{
		std::clog << "Compile failed: (" << _script.getError() << ") " << _script.getErrorReason() << std::endl;
	}
	//
	dynamic_cast<ScriptModelList*>(ui->tvInstructions->model())->refresh();
	for (int i = 0; i < ui->tvInstructions->model()->columnCount(QModelIndex()) - 1; i++)
	{
		ui->tvInstructions->resizeColumnToContents(i);
	}
	//
	dynamic_cast<ScriptModelList*>(ui->tvVariables->model())->refresh();
	//
	updateStatus();
}

void ScriptWindow::onActionInitialize()
{
	_script.Execute(ScriptInterpreter::emInit);
	dynamic_cast<ScriptModelList*>(ui->tvVariables->model())->refresh();
	for (int i = 0; i < ui->tvVariables->model()->columnCount(QModelIndex()) - 1; i++)
	{
		ui->tvInstructions->resizeColumnToContents(i);
	}
	updateStatus();
}

void ScriptWindow::onActionStep()
{
	if (_script.getState() == ScriptInterpreter::esReady)
	{
		if (_script.Execute(ScriptInterpreter::emStart) != ScriptInterpreter::esError)
		{
			std::clog << "Start succeeded." << std::endl;
		}
		else
		{
			std::clog << "Start failed: (" << _script.getError() << ") " << _script.getErrorReason() << std::endl;
		}
	}
	else if (_script.getState() == ScriptInterpreter::esRunning)
	{
		_script.Execute(ScriptInterpreter::emStep);
	}
	dynamic_cast<ScriptModelList*>(ui->tvVariables->model())->refresh();
	updateStatus();
}

void ScriptWindow::onActionRun()
{
	if (_script.Execute(ScriptInterpreter::emRun) != ScriptInterpreter::esError)
	{
		std::clog << "Run succeeded." << std::endl;
	}
	else
	{
		std::clog << "Run failed: (" << _script.getError() << ") " << _script.getErrorReason() << std::endl;
	}
	dynamic_cast<ScriptModelList*>(ui->tvVariables->model())->refresh();
	updateStatus();
}

void ScriptWindow::onClickInstruction(const QModelIndex& index)
{
	moveCursorToInstruction(index.row());
	ui->pteSource->setFocus();
}

void ScriptWindow::moveCursorToInstruction(int row)
{
	// Get copy of the current text cursor.
	auto cur = ui->pteSource->textCursor();
/*
	// Move to start of document.
	cur.movePosition(QTextCursor::MoveOperation::Start);
	// Move down the amount of lines.
	cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, _script.getInstructions().at(row)._codePos._line);
*/
	// Set the absolute position in the document.
	cur.setPosition((int)_script.getInstructions().at(row)._codePos._offset);
	// Apply the new cursor.
	ui->pteSource->setTextCursor(cur);
}
