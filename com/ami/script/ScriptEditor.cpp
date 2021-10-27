#include "ScriptEditor.h"
#include "ui_ScriptEditor.h"
#include "ScriptManager.h"

#include <iostream>
#include <QDebug>
#include <QAbstractButton>
#include <QPainter>
#include <misc/qt/CaptureListModel.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/ScriptListModel.h>
#include <misc/qt/ScriptHighlighter.h>
#include <misc/qt/Resource.h>
#include <gii/gen/GiiScriptInterpreter.h>

namespace sf
{

ScriptEditor::FindInterpreterClosure ScriptEditor::callbackFindInterpreter{};

// Inherit from ListenerList to allow adding listeners for this class.
struct ScriptEditor::ScriptEditor::Private :ListenerList
{
	explicit Private(QSettings* settings, ScriptEditor* editor)
		:_settings(settings)
	{
		_slmInstructions = new ScriptListModel(editor);
		_slmVariables = new ScriptListModel(editor);
	}

	~Private()
	{
		// Prevent handling during destruction by deleting the listeners early.
		flushListeners();
	}

	QSharedPointer<ScriptInterpreter> _interpreter{};
	ScriptListModel* _slmInstructions{nullptr};
	ScriptListModel* _slmVariables{nullptr};
	QSettings* _settings{nullptr};
	QString _curFile;
	bool _isUntitled{true};
};

ScriptEditor::ScriptEditor(QSettings* settings, QWidget* parent)
	:QWidget(parent)
	 , _p(*new Private(settings, this))
	 , PlainTextEditMdi()
	 , ui(new Ui::ScriptEditor)
{
	ui->setupUi(this);
	// Make the widget delete on close.
	setAttribute(Qt::WA_DeleteOnClose);
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/ico/scanframe"));
	// Set the editor form the MDI.
	setEditor(ui->pteSource, this);
	// Create the model list for instructions.
	ui->tvInstructions->setModel(_p._slmInstructions);
	// Create the model list for variables.
	ui->tvVariables->setModel(_p._slmVariables);
	// Create connections for the editor to enable MDI actions.
	setConnections(ui->pteSource);
	ui->tvInstructions->header()->setStretchLastSection(true);
	// Method of connecting Qt5 style
	connect(ui->actionCompile, &QAction::triggered, this, &ScriptEditor::onActionCompile);
	connect(ui->actionInitialize, &QAction::triggered, this, &ScriptEditor::onActionInitialize);
	connect(ui->actionStep, &QAction::triggered, this, &ScriptEditor::onActionStep);
	connect(ui->actionRun, &QAction::triggered, this, &ScriptEditor::onActionRun);
	connect(ui->actionStop, &QAction::triggered, this, &ScriptEditor::onActionStop);
	//
	connect(ui->tvInstructions, &QTreeView::clicked, this, &ScriptEditor::onClickInstruction);
	//
	ui->tbCompile->setDefaultAction(ui->actionCompile);
	ui->tbInitialize->setDefaultAction(ui->actionInitialize);
	ui->tbStep->setDefaultAction(ui->actionStep);
	ui->tbRun->setDefaultAction(ui->actionRun);
	ui->tbStop->setDefaultAction(ui->actionStop);
	//
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
	QSize sz(24, 24);
	ui->actionCompile->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Compile), QPalette::ColorRole::ButtonText, sz));
	ui->actionInitialize->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Initialize), QPalette::ColorRole::ButtonText, sz));
	ui->actionStep->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Step), QPalette::ColorRole::ButtonText, sz));
	ui->actionRun->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Run), QPalette::ColorRole::ButtonText, sz));
	ui->actionStop->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Stop), QPalette::ColorRole::ButtonText, sz));
	//
	//setInterpreter(QSharedPointer<ScriptInterpreter>(new GiiScriptInterpreter()));
}

ScriptEditor::~ScriptEditor()
{
	stateSaveRestore(true);
	delete &_p;
}

void ScriptEditor::stateSaveRestore(bool save)
{
	_p._settings->beginGroup(getObjectNamePath(this).join('.').prepend("State."));
	QString keyWidgetRect("WidgetRect");
	QString keyState("State");
	QString keySplitter("SplitterHorizontal");
	QString keySplitter2("SplitterVertical");
	QString keyTabWidget("TabWidget");
	if (save)
	{
		_p._settings->setValue(keyWidgetRect, geometry());
		_p._settings->setValue(keySplitter, ui->splitter->saveState());
		_p._settings->setValue(keyTabWidget, ui->tabWidget->currentIndex());
	}
	else
	{
		// Get the keys in the section to check existence in the ini-section.
		if (_p._settings->value(keyWidgetRect).toRect().isValid())
		{
			setGeometry(_p._settings->value(keyWidgetRect).toRect());
		}
		ui->splitter->restoreState(_p._settings->value(keySplitter).toByteArray());
		ui->tabWidget->setCurrentIndex(_p._settings->value(keyTabWidget).toInt());
	}
	_p._settings->endGroup();
}

void ScriptEditor::updateStatus()
{
	ui->leState->setText(_p._interpreter->getStateName());
	ui->leError->setText(_p._interpreter->getErrorText());
	ui->teReason->setText(QString::fromStdString(_p._interpreter->getErrorReason()));
	// Get the current instruction pointer.
	auto instr = static_cast<int>(_p._interpreter->getInstructionPtr());
	// Check if the pointer is valid.
	if (instr >= 0)
	{
		// Select the instruction list row and just the first column for no apparent reason.
		ui->tvInstructions->setCurrentIndex(ui->tvInstructions->model()->index(instr, 0));
		//
		moveCursorToInstruction(instr);
	}
}

void ScriptEditor::adjustColumns()
{
	// Only resize columns when data is there.
	if (!_p._interpreter->getInstructions().empty())
	{
		dynamic_cast<ScriptListModel*>(ui->tvInstructions->model())->refresh();
		resizeColumnsToContents(ui->tvInstructions);
	}
	dynamic_cast<ScriptListModel*>(ui->tvVariables->model())->refresh();
	resizeColumnsToContents(ui->tvVariables);
}

void ScriptEditor::onActionCompile()
{
	if (!_p._interpreter)
	{
		return;
	}
	auto src = ui->pteSource->toPlainText().toStdString();
	//
	if (!_p._interpreter->compile(src.c_str()))
	{
		qWarning() << "Compile failed:" << _p._interpreter->getErrorText() << _p._interpreter->getErrorReason();
	}
	//
	dynamic_cast<ScriptListModel*>(ui->tvVariables->model())->refresh();
	//
	adjustColumns();
}

void ScriptEditor::onActionInitialize()
{
	if (_p._interpreter)
	{
		_p._interpreter->Execute(ScriptInterpreter::emInit);
		adjustColumns();
	}
}

void ScriptEditor::onActionStep()
{
	if (!_p._interpreter)
	{
		return;
	}
	if (_p._interpreter->getState() == ScriptInterpreter::esReady)
	{
		if (_p._interpreter->Execute(ScriptInterpreter::emStart) == ScriptInterpreter::esError)
		{
			qWarning() << "Step failed:" << _p._interpreter->getError() << _p._interpreter->getErrorReason();
		}
	}
	else if (_p._interpreter->getState() == ScriptInterpreter::esRunning)
	{
		_p._interpreter->Execute(ScriptInterpreter::emStep);
	}
	dynamic_cast<ScriptListModel*>(ui->tvVariables->model())->refresh();
}

void ScriptEditor::onActionRun()
{
	if (!_p._interpreter)
	{
		return;
	}
	if (_p._interpreter->Execute(ScriptInterpreter::emRun) == ScriptInterpreter::esError)
	{
		qWarning() << "Run failed:" << _p._interpreter->getError() << _p._interpreter->getErrorReason();
	}
	dynamic_cast<ScriptListModel*>(ui->tvVariables->model())->refresh();
}

void ScriptEditor::onActionStop()
{
	if (!_p._interpreter)
	{
		return;
	}
	if (_p._interpreter->Execute(ScriptInterpreter::emAbort) == ScriptInterpreter::esError)
	{
		qWarning() << "Stop failed:" << _p._interpreter->getError() << _p._interpreter->getErrorReason();
	}
	dynamic_cast<ScriptListModel*>(ui->tvVariables->model())->refresh();
}

void ScriptEditor::onClickInstruction(const QModelIndex& index)
{
	moveCursorToInstruction(index.row());
	ui->pteSource->setFocus();
}

void ScriptEditor::moveCursorToInstruction(int row)
{
	if (!_p._interpreter)
	{
		return;
	}
	// Get copy of the current text cursor.
	auto cur = ui->pteSource->textCursor();
	/*
		// Move to start of document.
		cur.movePosition(QTextCursor::MoveOperation::Start);
		// Move down the amount of lines.
		cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, _script.getInstructions().at(row)._codePos._line);
	*/
	// Set the absolute position in the document.
	if (!_p._interpreter->getInstructions().isEmpty())
	{
		cur.setPosition((int) _p._interpreter->getInstructions().at(row)._codePos._offset);
	}
	// Apply the new cursor.
	ui->pteSource->setTextCursor(cur);
}

Editor* ScriptEditor::getEditor() const
{
	return ui->pteSource;
}

ScriptInterpreter* ScriptEditor::getScriptInterpreter() const
{
	return _p._interpreter.get();
}

void ScriptEditor::closeEvent(QCloseEvent* event)
{
	if (canClose())
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void ScriptEditor::setInterpreter(const QSharedPointer<ScriptInterpreter>& interpreter)
{
	// Flush the listeners of the current interpreter.
	_p.flushListeners();
	// Assign the new shared pointer.
	_p._interpreter = interpreter;
	// Assign the pointer to the module lists.
	_p._slmInstructions->setInterpreter(_p._interpreter.get(), ScriptListModel::mInstructions);
	_p._slmVariables->setInterpreter(_p._interpreter.get(), ScriptListModel::mVariables);
	// Highlight the keywords and so on.
	new ScriptHighlighter(_p._interpreter.get(), ui->pteSource->document());
	//
	adjustColumns();
	// Link the interpreter change events.
	interpreter->getChangeEmitter().linkListener(&_p, [&](const ScriptInterpreter*)
	{
		updateStatus();
	});
	// Update the status right now.
	updateStatus();
}

bool ScriptEditor::loadFile(const QString& filename)
{
	// Converts relative file path into absolute.
	QFileInfo fi (filename);
	// Bailout when loading fails.
	if (!PlainTextEditMdi::loadFile(fi.absoluteFilePath()))
	{
		// Signal failure.
		return false;
	}
	// Check if closure is assigned.
	if (callbackFindInterpreter)
	{
		// When an interpreter has been found.
		if (auto interpreter = callbackFindInterpreter(fi.absoluteFilePath()))
		{
			// Assign the interpreter.
			setInterpreter(interpreter);
		}
	}
	// Signal success.
	return true;
}

void ScriptEditor::develop()
{
//	if (!_p._interpreter)
	{
		auto i = new GiiScriptInterpreter();
		i->setScriptName(userFriendlyCurrentFile().toStdString());
		setInterpreter(QSharedPointer<ScriptInterpreter>(i));
	}
}

QString ScriptEditor::newFileName() const
{
	static int sequenceNumber = 1;
	return QObject::tr("document-%1.%2").arg(sequenceNumber++).arg(ScriptManager::getFileSuffix());
}

}
