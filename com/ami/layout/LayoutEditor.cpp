#include <QGuiApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QSaveFile>
#include <QCloseEvent>
#include <misc/qt/Resource.h>
#include <misc/qt/FormBuilder.h>
#include "LayoutEditor.h"

namespace sf
{

LayoutEditor::LayoutEditor(QWidget* parent)
	:QWidget(parent)
	 , MultiDocInterface()
	 , isUntitled(true)
	 , _rootName("FormRoot")
{
	//setConnections(this);
	// Make the widget delete on close.
	setAttribute(Qt::WA_DeleteOnClose);
	//
/*
	_scrollArea = new QScrollArea(this);
	_scrollArea->setFrameShape(QFrame::Shape::Box);
	_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
*/
	//scrollArea->setSizeAdjustPolicy(QScrollArea::SizeAdjustPolicy::AdjustToContentsOnFirstShow);
	adjustSize();
}

void LayoutEditor::resizeEvent(QResizeEvent* event)
{
	if (_scrollArea)
	{
		_scrollArea->setGeometry(QRect(QPoint(), event->size()));
	}
	if (_widget)
	{
		_widget->setGeometry(QRect(QPoint(), event->size()));
	}
	QWidget::resizeEvent(event);
}

void LayoutEditor::closeEvent(QCloseEvent* event)
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

void LayoutEditor::newFile()
{
	static int sequenceNumber = 1;
	isUntitled = true;
	curFile = tr("document-%1.ui").arg(sequenceNumber++);
	setWindowTitle(curFile + "[*]");
//	connect(document(), &QTextDocument::contentsChanged, this, &LayoutEditor::documentWasModified);
}

bool LayoutEditor::loadFile(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("MDI"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
		return false;
	}
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	// Create out form builder to load a layout.
	FormBuilder builder;
	// Add the application directory as the plugin directory to find custom plugins.
	builder.addPluginPath(QCoreApplication::applicationDirPath());
	// Create widget from the loaded ui-file.
	_widget = builder.load(&file, _scrollArea ? (QWidget*)_scrollArea : (QWidget*)this);
	// When loading is successful.
	if (_widget)
	{
		if (!_widget->objectName().length())
		{
			// Set the root name.
			_widget->setObjectName(_rootName);
		}
		else
		{
			_rootName = _widget->objectName();
		}
		// The widget min size is also the actual size.
		//_widget->setMinimumSize(_widget->size());
		// Copy the title from the widget.
		//setWindowTitle(widget->windowTitle());
		// Set the widget onto the layout.
		//_scrollArea->setWidget(widget);
	}

	QGuiApplication::restoreOverrideCursor();
	setCurrentFile(fileName);
	//connect(document(), &QTextDocument::contentsChanged, this, &LayoutEditor::documentWasModified);
	return true;
}

bool LayoutEditor::save()
{
	return isUntitled ? saveAs() : saveFile(curFile);
}

bool LayoutEditor::saveAs()
{
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile, getFileTypeFilters());
	return !fileName.isEmpty() && saveFile(fileName);
}

bool LayoutEditor::saveFile(const QString& fileName, bool keep_name)
{
	QString errorMessage;
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	QSaveFile file(fileName);
	if (file.open(QFile::WriteOnly | QFile::Text))
	{
		QTextStream out(&file);
		//
		FormBuilder builder;
		//
		auto wgt = findChild<QWidget*>(_rootName, Qt::FindChildrenRecursively);
		if (wgt)
		{
			builder.save(&file, wgt);
		}
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
	//
	if (!errorMessage.isEmpty())
	{
		QMessageBox::warning(this, tr("MDI"), errorMessage);
		return false;
	}
	if (!keep_name)
	{
		setCurrentFile(fileName);
	}
	return true;
}

void LayoutEditor::documentWasModified()
{
	setWindowModified(isModified());
}

QString LayoutEditor::currentFile() const
{
	return curFile;
}

void LayoutEditor::setCurrentFile(const QString& fileName)
{
	curFile = QFileInfo(fileName).canonicalFilePath();
	isUntitled = false;
	//document()->setModified(false);
	setWindowModified(false);
	setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString LayoutEditor::strippedName(const QString& fullFileName) const
{
	return QFileInfo(fullFileName).fileName();
}

QString LayoutEditor::userFriendlyCurrentFile() const
{
	return strippedName(curFile);
}

bool LayoutEditor::hasSelection() const
{
	return false;//textCursor().hasSelection();
}

bool LayoutEditor::isUndoRedoEnabled() const
{
	return false;//QWidget::isUndoRedoEnabled();
}

bool LayoutEditor::isUndoAvailable() const
{
	return false;//document()->isUndoAvailable();
}

bool LayoutEditor::isRedoAvailable() const
{
	return false;//document()->isUndoAvailable();
}

bool LayoutEditor::isModified() const
{
	return false;//document()->isModified();
}

void LayoutEditor::cut()
{
	//QTextEdit::cut();
}

void LayoutEditor::copy()
{
	//QTextEdit::copy();
}

void LayoutEditor::paste()
{
	//QTextEdit::paste();
}

void LayoutEditor::undo()
{
	//QTextEdit::undo();
}

void LayoutEditor::redo()
{
	//QTextEdit::redo();
}

void LayoutEditor::develop()
{
	// Append the xml extension to the existing file name.
	auto fn = QString(curFile).append(".xml");
	// Save the file but keep the name.
	saveFile(fn, true);
	// Make the log lne so it can be opened in CLion.
	qDebug() << "Written to " << fn.prepend("file://");
}

}
