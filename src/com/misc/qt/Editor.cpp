#include "Editor.h"
#include "../qt/Globals.h"
#include "FindReplaceDialog.h"
#include "gen/pointer.h"
#include "qt_utils.h"
#include <QMessageBox>
#include <QPainter>
#include <QSaveFile>
#include <QTextBlock>
#include <QTextStream>
#include <QtWidgets>

namespace sf
{

struct Editor::Private
{
		explicit Private(Editor* editor)
			: _editor(editor)
			, _actionFind(editor)
			, _settings(getGlobalSettings())
		{
			_editor->addAction(&_actionFind);
			_actionFind.setShortcut(QKeySequence(tr("Ctrl+F", "Find|Replace")));
			connect(&_actionFind, &QAction::triggered, [&]() {
				if (!_dlgFindReplace)
				{
					_dlgFindReplace = new FindReplaceDialog(_editor);
					_dlgFindReplace->setEditor(_editor);
					connect(_dlgFindReplace, &QDialog::finished, [&]() {
						stateSaveRestore(true);
					});
				}
				if (_dlgFindReplace->isVisible())
				{
					_dlgFindReplace->activateWindow();
				}
				else
				{
					stateSaveRestore(false);
					_dlgFindReplace->show();
				}
			});
		}

		void stateSaveRestore(bool save) const
		{
			if (!_settings || !_dlgFindReplace)
			{
				return;
			}
			_settings->beginGroup(getObjectNamePath(_dlgFindReplace).join('.').prepend("State."));
			QString key("State");
			if (save)
			{
				_settings->setValue(key, _dlgFindReplace->saveGeometry());
			}
			else
			{
				_dlgFindReplace->restoreGeometry(_settings->value(key).toByteArray());
			}
			_settings->endGroup();
		}

		Editor* _editor;
		QString curFile;
		bool isUntitled{true};
		QWidget* lineNumberArea{nullptr};
		int spacingNumber{1};
		// Color for background of current line.
		QColor curLineColor;
		QAction _actionFind;
		FindReplaceDialog* _dlgFindReplace{nullptr};
		QSettings* _settings;
};

Editor::Configuration::Configuration(QSettings* settings)
	: _settings(settings)
{
}

void Editor::Configuration::settingsReadWrite(bool save)
{
	if (!_settings)
	{
		return;
	}
	_settings->beginGroup("AppModule.Editor.Configuration");
	QString keyDarkMode("DarkMode");
	QString keyHighLight("HighLight");
	QString keyGutter("Gutter");
	QString keyFontType("FontType");
	QString keyFontSize("FontSize");
	if (!save)
	{
		showGutter = _settings->value(keyGutter, showGutter).toBool();
		darkMode = _settings->value(keyDarkMode, darkMode).toBool();
		fontType = _settings->value(keyFontType, fontType).toString();
		fontSize = _settings->value(keyFontSize, fontSize).toInt();
		useHighLighting = _settings->value(keyHighLight, fontSize).toBool();
	}
	else
	{
		_settings->setValue(keyGutter, showGutter);
		_settings->setValue(keyDarkMode, darkMode);
		_settings->setValue(keyFontType, fontType);
		_settings->setValue(keyFontSize, fontSize);
		_settings->setValue(keyHighLight, useHighLighting);
	}
	_settings->endGroup();
}

// Internal class.
class Editor::LineNumberArea : public QWidget
{
	public:
		explicit LineNumberArea(Editor* editor)
			: QWidget(editor)
			, codeEditor(editor)
		{}

		[[nodiscard]] QSize sizeHint() const override
		{
			return {codeEditor->lineNumberAreaWidth(), 0};
		}

	protected:
		void paintEvent(QPaintEvent* event) override
		{
			codeEditor->lineNumberAreaPaintEvent(event);
		}

	private:
		Editor* codeEditor;
};

Editor::Editor(QWidget* parent)
	: QPlainTextEdit(parent)
	, _p(*new Private(this))
	, ObjectExtension(this)
{
	setWindowIcon(QIcon(":icon/svg/code-editor"));
	setAttribute(Qt::WA_DeleteOnClose);
	// Apply the
	setConfiguration(Configuration());
	connect(this, &Editor::cursorPositionChanged, this, &Editor::highlightCurrentLine);
	highlightCurrentLine();
	// Set tab stop the width of 2 space characters.
	QFontMetrics fm(document()->defaultFont());
	// Use the width of a space for tab indentation width.
	setTabStopDistance(fm.horizontalAdvance(' ') * 2);
	//
	setUndoRedoEnabled(true);
}

Editor::~Editor()
{
	delete &_p;
}

void Editor::setConfiguration(const Configuration& cfg)
{
	_p.curLineColor = cfg.darkMode ? QColorConstants::DarkYellow.darker() : QColor(255, 250, 227);
	if (cfg.showGutter)
	{
		_p.lineNumberArea = new LineNumberArea(this);
		connect(this, &Editor::blockCountChanged, this, &Editor::updateLineNumberAreaWidth);
		connect(this, &Editor::updateRequest, this, &Editor::updateLineNumberArea);
		updateLineNumberAreaWidth(0);
	}
	else
	{
		delete_null(_p.lineNumberArea);
	}
	// Change the default editor colors.
	QPalette pal(palette());
	pal.setColor(QPalette::Base, cfg.darkMode ? Qt::black : Qt::white);
	pal.setColor(QPalette::Text, cfg.darkMode ? Qt::white : Qt::black);
	setPalette(pal);
	// Set monospaced font.
	QFont font(cfg.fontType);
	font.setStyleHint(QFont::StyleHint::Monospace);
	// Make the new font the same size as the current one.
	font.setPointSizeF(cfg.fontSize ? cfg.fontSize : document()->defaultFont().pointSizeF());
	document()->setDefaultFont(font);
}

int Editor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10)
	{
		max /= 10;
		++digits;
	}
	int space = fontMetrics().horizontalAdvance(QChar('9'));
	// Spacing is half a single character advance and a pixel for the border.
	_p.spacingNumber = space / 2 + 1;
	// Add on digit for spacing and an extra 2 pixels for a border.
	space *= (digits + 1) + 2;
	//
	return space;
}

void Editor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void Editor::updateLineNumberArea(const QRect& rect, int dy)
{
	if (_p.lineNumberArea)
	{
		if (dy)
		{
			_p.lineNumberArea->scroll(0, dy);
		}
		else
		{
			_p.lineNumberArea->update(0, rect.y(), _p.lineNumberArea->width(), rect.height());
		}
	}
	if (rect.contains(viewport()->rect()))
	{
		updateLineNumberAreaWidth(0);
	}
}

void Editor::resizeEvent(QResizeEvent* e)
{
	QPlainTextEdit::resizeEvent(e);
	QRect cr = contentsRect();
	if (_p.lineNumberArea)
	{
		_p.lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
	}
}

void Editor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;
		selection.format.setBackground(_p.curLineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}
	setExtraSelections(extraSelections);
}

void Editor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
	QPainter painter(_p.lineNumberArea);
	painter.fillRect(event->rect(), palette().alternateBase());
	// Draw the border.
	painter.setPen(Qt::lightGray);
	painter.drawLine(event->rect().topRight(), event->rect().bottomRight());
	//
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + qRound(blockBoundingRect(block).height());
	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen(palette().color(QPalette::ColorRole::WindowText));
			painter.drawText(-_p.spacingNumber, top, _p.lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
		}
		block = block.next();
		top = bottom;
		bottom = top + qRound(blockBoundingRect(block).height());
		++blockNumber;
	}
}

bool Editor::isRequiredProperty(const QString& name)
{
	return true;
}

bool Editor::canClose()
{
	return true;
}

void Editor::closeEvent(QCloseEvent* event)
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

}// namespace sf
