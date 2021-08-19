#include <QtWidgets>
#include <QPainter>
#include <QTextBlock>
#include <QFileInfo>
#include <QTextStream>
#include <QLabel>
#include <QMessageBox>
#include <QGuiApplication>
#include <QFileDialog>
#include <QSaveFile>

#include "PlainTextEditMdi.h"

namespace sf
{

PlainTextEditMdi::PlainTextEditMdi()
	:MultiDocInterface()
{
}

PlainTextEditMdi::PlainTextEditMdi(QPlainTextEdit* editor)
	:MultiDocInterface()
{
	setConnections(editor);
	setEditor(editor, editor);
}

void PlainTextEditMdi::setEditor(QPlainTextEdit* editor, QWidget* container)
{
	_editor = editor;
	Q_ASSERT(_editor);
	_container = container ?: editor;
}

void PlainTextEditMdi::cut()
{
	Q_ASSERT(_editor);
	_editor->cut();
}

void PlainTextEditMdi::copy()
{
	Q_ASSERT(_editor);
	_editor->copy();
}

void PlainTextEditMdi::paste()
{
	Q_ASSERT(_editor);
	_editor->paste();
}

void PlainTextEditMdi::undo()
{
	Q_ASSERT(_editor);
	_editor->undo();
}

void PlainTextEditMdi::redo()
{
	Q_ASSERT(_editor);
	_editor->redo();
}

bool PlainTextEditMdi::hasSelection() const
{
	Q_ASSERT(_editor);
	return _editor->textCursor().hasSelection();
}

bool PlainTextEditMdi::isUndoRedoEnabled() const
{
	Q_ASSERT(_editor);
	return _editor->isUndoRedoEnabled();
}

bool PlainTextEditMdi::isUndoAvailable() const
{
	Q_ASSERT(_editor);
	return _editor->document()->isUndoAvailable();
}

bool PlainTextEditMdi::isRedoAvailable() const
{
	Q_ASSERT(_editor);
	return _editor->document()->isRedoAvailable();
}

bool PlainTextEditMdi::isModified() const
{
	Q_ASSERT(_editor);
	return _editor->document()->isModified();
}

QString PlainTextEditMdi::currentFile() const
{
	return curFile;
}

void PlainTextEditMdi::setCurrentFile(const QString& fileName)
{
	Q_ASSERT(_editor);
	curFile = QFileInfo(fileName).canonicalFilePath();
	isUntitled = false;
	_editor->document()->setModified(false);
	_container->setWindowModified(false);
	_container->setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString PlainTextEditMdi::strippedName(const QString& fullFileName) const
{
	return QFileInfo(fullFileName).fileName();
}

QString PlainTextEditMdi::userFriendlyCurrentFile() const
{
	return strippedName(curFile);
}

void PlainTextEditMdi::documentWasModified()
{
	Q_ASSERT(_editor);
	_container->setWindowModified(_editor->document()->isModified());
}

void PlainTextEditMdi::newFile()
{
	Q_ASSERT(_editor);
	static int sequenceNumber = 1;
	isUntitled = true;
	curFile = QObject::tr("document-%1.js").arg(sequenceNumber++);
	_container->setWindowTitle(curFile + "[*]");
	QObject::connect(_editor->document(), &QTextDocument::contentsChanged, [&]() {documentWasModified();});
}

bool PlainTextEditMdi::loadFile(const QString& filename)
{
	Q_ASSERT(_editor);
	QFile file(filename);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(_container, QObject::tr("MDI"),
			QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()));
		return false;
	}
	QTextStream in(&file);
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	_editor->setPlainText(in.readAll());
	QGuiApplication::restoreOverrideCursor();
	setCurrentFile(filename);
	QObject::connect(_editor->document(), &QTextDocument::contentsChanged, [&]() {documentWasModified();});
	return true;
}

bool PlainTextEditMdi::save()
{
	return isUntitled ? saveAs() : saveFile(curFile);
}

bool PlainTextEditMdi::saveAs()
{
	Q_ASSERT(_editor);
	auto fileName = QFileDialog::getSaveFileName(_editor, QObject::tr("Save As"), curFile, getFileTypeFilters());
	return !fileName.isEmpty() && saveFile(fileName);
}

bool PlainTextEditMdi::saveFile(const QString& fileName)
{
	Q_ASSERT(_editor);
	QString errorMessage;
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	QSaveFile file(fileName);
	if (file.open(QFile::WriteOnly | QFile::Text))
	{
		QTextStream out(&file);
		out << _editor->toPlainText();
		if (!file.commit())
		{
			errorMessage = QObject::tr("Cannot write file %1:\n%2.").arg(QDir::toNativeSeparators(fileName),
				file.errorString());
		}
	}
	else
	{
		errorMessage = QObject::tr("Cannot open file %1 for writing:\n%2.")
			.arg(QDir::toNativeSeparators(fileName), file.errorString());
	}
	QGuiApplication::restoreOverrideCursor();
	if (!errorMessage.isEmpty())
	{
		QMessageBox::warning(_container, QObject::tr("MDI"), errorMessage);
		return false;
	}
	setCurrentFile(fileName);
	return true;
}

}
