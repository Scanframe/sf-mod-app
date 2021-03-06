#include <QtWidgets>
#include <misc/qt/resource.h>

#include "TextEditor.h"
#include "Highlighter.h"

TextEditor::TextEditor()
	:isUntitled(true)
{
	setWindowIcon(QIcon(":/logo/scanframe"));
	// Make the widget delete on close.
	setAttribute(Qt::WA_DeleteOnClose);
	// Assign highlighter to the underlying document.
	new Highlighter(document());
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

void TextEditor::newFile()
{
	static int sequenceNumber = 1;
	isUntitled = true;
	curFile = tr("document%1.txt").arg(sequenceNumber++);
	setWindowTitle(curFile + "[*]");
	connect(document(), &QTextDocument::contentsChanged, this, &TextEditor::documentWasModified);
}

bool TextEditor::loadFile(const QString& fileName)
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
	connect(document(), &QTextDocument::contentsChanged, this, &TextEditor::documentWasModified);
	return true;
}

bool TextEditor::save()
{
	return isUntitled ? saveAs() : saveFile(curFile);
}

bool TextEditor::saveAs()
{
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
	return !fileName.isEmpty() && saveFile(fileName);
}

bool TextEditor::saveFile(const QString& fileName)
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

void TextEditor::closeEvent(QCloseEvent* event)
{
	if (maybeSave())
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void TextEditor::documentWasModified()
{
	setWindowModified(document()->isModified());
}

bool TextEditor::maybeSave()
{
	if (!document()->isModified())
	{
		return true;
	}
	const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("MDI"),
		tr("'%1' has been modified.\nDo you want to save your changes?").arg(userFriendlyCurrentFile()),
		QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	switch (ret)
	{
		case QMessageBox::Save:
			return save();
		case QMessageBox::Cancel:
			return false;
		default:
			break;
	}
	return true;
}

QString TextEditor::currentFile() const
{
	return curFile;
}

void TextEditor::setCurrentFile(const QString& fileName)
{
	curFile = QFileInfo(fileName).canonicalFilePath();
	isUntitled = false;
	document()->setModified(false);
	setWindowModified(false);
	setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString TextEditor::strippedName(const QString& fullFileName) const
{
	return QFileInfo(fullFileName).fileName();
}

QString TextEditor::userFriendlyCurrentFile() const
{
	return strippedName(curFile);
}

bool TextEditor::hasSelection() const
{
	return textCursor().hasSelection();
}

void TextEditor::cut()
{
	QTextEdit::cut();
}

void TextEditor::copy()
{
	QTextEdit::copy();
}

void TextEditor::paste()
{
	QTextEdit::paste();
}





