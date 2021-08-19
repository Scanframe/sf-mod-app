#include <QGuiApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QSaveFile>
#include <QCloseEvent>
#include <misc/qt/Resource.h>
#include <misc/qt/FormBuilder.h>
#include <misc/qt/PropertySheetDialog.h>
#include <gii/qt/VariableWidgetBase.h>
#include "LayoutEditor.h"
#include "pages/VariableIdPropertyPage.h"
#include "pages/WidgetPropertyPage.h"

namespace sf
{

LayoutEditor::LayoutEditor(QSettings* settings, QWidget* parent)
	:LayoutWidget(parent)
	 , MultiDocInterface()
	 , _settings(settings)
	 , _isUntitled(true)
	 , _rootName("FormRoot")
{
/*
	_scrollArea = new QScrollArea(this);
	_scrollArea->setFrameShape(QFrame::Shape::Box);
	_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
*/
	//scrollArea->setSizeAdjustPolicy(QScrollArea::SizeAdjustPolicy::AdjustToContentsOnFirstShow);
	adjustSize();
	// TODO: Needs to be set from property page.
	setReadOnly(false);
	//
	_targetContextMenu = new QMenu(this);
	//
	auto actionEdit = new QAction(tr("&Edit"), _targetContextMenu);
	actionEdit->setToolTip(tr("Edit the properties of this widget."));
	actionEdit->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Edit), QPalette::ColorRole::ButtonText));
	_targetContextMenu->addAction(actionEdit);
	connect(actionEdit, &QAction::triggered, [&]()
	{
		if (_currentTarget)
		{
			openPropertyEditor(_currentTarget);
		}
	});
	auto actionRemove = new QAction(tr("&Remove"), _targetContextMenu);
	actionRemove->setToolTip(tr("Remove this widget."));
	actionRemove->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Remove), QPalette::ColorRole::ButtonText));
	_targetContextMenu->addAction(actionRemove);
	connect(actionRemove, &QAction::triggered, [&]()
	{
		if (_currentTarget)
		{
			_currentTarget->deleteLater();
		}
	});
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
	_isUntitled = true;
	_curFile = tr("document-%1.ui").arg(sequenceNumber++);
	setWindowTitle(_curFile + "[*]");
	_modified = true;
	documentWasModified();
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
	_widget = builder.load(&file, _scrollArea ? (QWidget*) _scrollArea : (QWidget*) this);
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
	for (auto child: _widget->children())
	{
		child->installEventFilter(this);
	}
	return true;
}

bool LayoutEditor::save()
{
	return _isUntitled ? saveAs() : saveFile(_curFile);
}

bool LayoutEditor::saveAs()
{
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save As"), _curFile, getFileTypeFilters());
	if (!fileName.isEmpty() && saveFile(fileName))
	{
		_modified = false;
		return true;
	}
	return false;
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
	// Reset the modified flag.
	_modified = false;
	// Trigger the signal.
	documentWasModified();
	// Signal success.
	return true;
}

void LayoutEditor::documentWasModified()
{
	auto flag = isModified();
	setWindowModified(flag);
	emit mdiSignals.modificationChanged(flag);
}

QString LayoutEditor::currentFile() const
{
	return _curFile;
}

void LayoutEditor::setCurrentFile(const QString& fileName)
{
	_curFile = QFileInfo(fileName).canonicalFilePath();
	_isUntitled = false;
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
	return strippedName(_curFile);
}

bool LayoutEditor::hasSelection() const
{
	return false;
}

bool LayoutEditor::isUndoRedoEnabled() const
{
	return false;
}

bool LayoutEditor::isUndoAvailable() const
{
	return false;
}

bool LayoutEditor::isRedoAvailable() const
{
	return false;
}

bool LayoutEditor::isModified() const
{
	return _modified;
}

void LayoutEditor::cut()
{
}

void LayoutEditor::copy()
{
}

void LayoutEditor::paste()
{
}

void LayoutEditor::undo()
{
}

void LayoutEditor::redo()
{
}

void LayoutEditor::develop()
{
	// Append the xml extension to the existing file name.
	auto fn = QString(_curFile).append(".xml");
	// Save the file but keep the name.
	saveFile(fn, true);
	// Make the log line, so it can be opened in CLion.
	qDebug() << "Written to " << fn.prepend("file://");
}

void LayoutEditor::popupContextMenu(QObject* target, const QPoint& pos)
{
	_currentTarget = target;
	_targetContextMenu->exec(pos);
	_currentTarget = nullptr;
}

bool LayoutEditor::eventFilter(QObject* watched, QEvent* event)
{
	// Intercept mouse button pressed when layout is editable.
	if (_editable && event->type() == QEvent::MouseButtonPress)
	{
		// Get the mouse event.
		if (auto mouseEvent = dynamic_cast<QMouseEvent*>(event))
		{
			// Check if it was the right mouse button.
			if (mouseEvent->button() == Qt::MouseButton::RightButton && QGuiApplication::keyboardModifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
			{
				popupContextMenu(watched, mouseEvent->globalPosition().toPoint());
			}
		}
	}
	return QObject::eventFilter(watched, event);
}

void LayoutEditor::openPropertyEditor(QObject* target)
{
	auto dlg = new PropertySheetDialog("LayoutObjectEditor", _settings, this);
	// Make the dialog close
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	//
	if (auto vwb = dynamic_cast<VariableWidgetBase*>(target))
	{
		dlg->addPage(new VariableIdPropertyPage(vwb, dlg));
	}
	if (auto w = dynamic_cast<QWidget*>(target))
	{
		dlg->addPage(new WidgetPropertyPage(w, dlg));
	}
	// When the dialog was applied set the modified flag.
	connect(dlg, &PropertySheetDialog::modified, [&]()
	{
		_modified = true;
		documentWasModified();
	});
	// FixMe: Prevent multiple properties sheet dialogs per target using a shared and week pointer.
	dlg->show();
}

}
