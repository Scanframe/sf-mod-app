#include "CodeEditor.h"

#include <QtWidgets>
#include <QPainter>
#include <QTextBlock>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QGuiApplication>
#include <QFileDialog>
#include <QSaveFile>
#include "Highlighter.h"

CodeEditor::CodeEditor(QWidget* parent)
	:QPlainTextEdit(parent)
	,isUntitled(true)
	,spacingNumber(1)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowIcon(QIcon(":logo/ico/scanframe"));
	// Assign highlighter to the underlying document.
	new Highlighter(document());

	lineNumberArea = new LineNumberArea(this);

	connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
	connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
	connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

	updateLineNumberAreaWidth(0);
	//
	highlightCurrentLine();
	//
	// Change the default editor colors.
	QPalette pal(palette());
	pal.setColor(QPalette::Base, Qt::white);
	pal.setColor(QPalette::Text, Qt::black);
	setPalette(pal);
	// Set monospaced font.
	QFont font("Monospace");
	font.setStyleHint(QFont::StyleHint::Monospace);
	// Make the new font the same size as the current one.
	font.setPointSizeF(document()->defaultFont().pointSizeF());
	//qDebug("%-15s %-15s %-20s %s", "QFont(\"\")", "setFixedPitch", qPrintable(QFontInfo(font).family()), QFontInfo(font).fixedPitch() ? "fixed-width" : "variable");
	document()->setDefaultFont(font);
	// Set tab stop the width of 2 space characters.
	QFontMetrics fm(document()->defaultFont());
	// Use the width of a space for tab indentation width.
	setTabStopDistance(fm.horizontalAdvance(' ') * 2);
	// Use the average width of a character.
	//setTabStopDistance(fm.averageCharWidth() * 2);

}

int CodeEditor::lineNumberAreaWidth()
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
	spacingNumber = space / 2 + 1;
	// Add on digit for spacing and an extra 2 pixels for a border.
	space *= (digits + 1) + 2;

	//
	return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
	if (dy)
	{
		lineNumberArea->scroll(0, dy);
	}
	else
	{
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
	}
	if (rect.contains(viewport()->rect()))
	{
		updateLineNumberAreaWidth(0);
	}
}

void CodeEditor::resizeEvent(QResizeEvent* e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;

		//QColor lineColor = QColor(Qt::yellow).lighter(170);
		QColor lineColor = QColor(255, 250, 227);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), QColor(240, 240, 240));
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
			painter.setPen(Qt::lightGray);
			painter.drawText(-spacingNumber, top, lineNumberArea->width(), fontMetrics().height(),Qt::AlignRight, number);
		}
		block = block.next();
		top = bottom;
		bottom = top + qRound(blockBoundingRect(block).height());
		++blockNumber;
	}
}

void CodeEditor::cut()
{
	QPlainTextEdit::cut();
}

void CodeEditor::copy()
{
	QPlainTextEdit::copy();
}

void CodeEditor::paste()
{
	QPlainTextEdit::paste();
}

bool CodeEditor::hasSelection() const
{
	return textCursor().hasSelection();
}

QString CodeEditor::currentFile() const
{
	return curFile;
}

void CodeEditor::setCurrentFile(const QString& fileName)
{
	curFile = QFileInfo(fileName).canonicalFilePath();
	isUntitled = false;
	document()->setModified(false);
	setWindowModified(false);
	setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString CodeEditor::strippedName(const QString& fullFileName) const
{
	return QFileInfo(fullFileName).fileName();
}

QString CodeEditor::userFriendlyCurrentFile() const
{
	return strippedName(curFile);
}

void CodeEditor::newFile()
{
	static int sequenceNumber = 1;
	isUntitled = true;
	curFile = tr("document%1.txt").arg(sequenceNumber++);
	setWindowTitle(curFile + "[*]");
	connect(document(), &QTextDocument::contentsChanged, this, &CodeEditor::documentWasModified);
}

bool CodeEditor::loadFile(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("MDI"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
		return false;
	}
	QTextStream in(&file);
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	setPlainText(in.readAll());
	QGuiApplication::restoreOverrideCursor();
	setCurrentFile(fileName);
	connect(document(), &QTextDocument::contentsChanged, this, &CodeEditor::documentWasModified);
	return true;
}

void CodeEditor::documentWasModified()
{
	setWindowModified(document()->isModified());
}

bool CodeEditor::save()
{
	return isUntitled ? saveAs() : saveFile(curFile);
}

bool CodeEditor::saveAs()
{
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
	return !fileName.isEmpty() && saveFile(fileName);
}

bool CodeEditor::saveFile(const QString& fileName)
{
	QString errorMessage;

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	QSaveFile file(fileName);
	if (file.open(QFile::WriteOnly | QFile::Text))
	{
		QTextStream out(&file);
		out << toPlainText();
		if (!file.commit())
		{
			errorMessage = tr("Cannot write file %1:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString());
		}
	}
	else
	{
		errorMessage = tr("Cannot open file %1 for writing:\n%2.")
			.arg(QDir::toNativeSeparators(fileName), file.errorString());
	}
	QGuiApplication::restoreOverrideCursor();

	if (!errorMessage.isEmpty())
	{
		QMessageBox::warning(this, tr("MDI"), errorMessage);
		return false;
	}

	setCurrentFile(fileName);
	return true;
}
