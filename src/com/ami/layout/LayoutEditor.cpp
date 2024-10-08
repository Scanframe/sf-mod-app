#include "LayoutEditor.h"
#include "pages/WidgetPropertyPage.h"
#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QGuiApplication>
#include <QLayout>
#include <QMessageBox>
#include <QSaveFile>
#include <ami/layout/pages/ContainerPropertyPage.h>
#include <ami/layout/pages/MiscellaneousPropertyPage.h>
#include <ami/layout/pages/PositionPropertyPage.h>
#include <gii/qt/LayoutData.h>
#include <gii/qt/VariableWidgetBase.h>
#include <misc/qt/FormBuilder.h>
#include <misc/qt/Globals.h>
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

struct LayoutEditor::Data : LayoutData
{
		explicit Data(LayoutEditor* parent)
			: LayoutData(parent)
			, _layoutEditor(parent)
		{
		}

		void popupContextMenu(QObject* target, const QPoint& pos) override
		{
			_layoutEditor->popupContextMenu(target, pos);
		}

		void openPropertyEditor(QObject* target) override
		{
			if (!getReadOnly())
			{
				_layoutEditor->openPropertyEditor(target);
			}
		}

		[[nodiscard]] QDir getDirectory() const override
		{
			return _layoutEditor->getDirectory();
		}
		//
		LayoutEditor* _layoutEditor;
};

LayoutEditor::LayoutEditor(QSettings* settings, QWidget* parent)
	: QWidget(parent)
	, MultiDocInterface()
	, _settings(settings)
	, _isUntitled(true)
	, _rootName("FormRoot")
	// Add the layout data object for widgets to find.
	, _layoutEditorData(new LayoutEditor::Data(this))
{

	/*
	_scrollArea = new QScrollArea(this);
	_scrollArea->setFrameShape(QFrame::Shape::Box);
	_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
*/
	//scrollArea->setSizeAdjustPolicy(QScrollArea::SizeAdjustPolicy::AdjustToContentsOnFirstShow);
	adjustSize();
	//
	_targetContextMenu = new QMenu(this);
	// Sets the editor widget only so message boxes appear centered on the layout editor.
	setConnections(this);
	//
	_actionEdit = new QAction(tr("&Edit"), _targetContextMenu);
	_actionEdit->setToolTip(tr("Edit the properties of this widget."));
	_actionEdit->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Edit), QPalette::ColorRole::ButtonText));
	_targetContextMenu->addAction(_actionEdit);
	connect(_actionEdit, &QAction::triggered, [&]() {
		if (_currentTarget)
		{
			openPropertyEditor(_currentTarget);
		}
	});
	auto actionLookup = new QAction(tr("&Look up"), _targetContextMenu);
	actionLookup->setToolTip(tr("Look widget up in the tree."));
	actionLookup->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Search), QPalette::ColorRole::ButtonText));
	_targetContextMenu->addAction(actionLookup);
	connect(actionLookup, &QAction::triggered, [&]() {
		if (_currentTarget)
		{
			Q_EMIT objectSelected(_currentTarget);
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
		// On closing send deactivation signal.
		activate(false);
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
	_widget = new QWidget(this);
	// Set the default form root object name.
	_widget->setObjectName(_rootName);
	documentWasModified();
	_isUntitled = true;
	_curFile = tr("document-%1.ui").arg(sequenceNumber++);
	setWindowTitle(_curFile + "[*]");
	_modified = true;
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
	// file nam eif used by the _layoutEditorData instance.
	setCurrentFile(fileName);
	// TODO: Work-around using a dummy widget for https://bugreports.qt.io/browse/QTBUG-96693
	QScopedPointer dummy(new QWidget);
	dummy->setObjectName("LoadDummy");
	// Assign the layout-data object.
	_layoutEditorData->setParent(dummy.get());
	// Do not set parent when loading because it will mess up the QLabel buddy resolving by name.
	_widget = FormBuilderLoad(&file, dummy.get());
	// Restore the parent after loading.
	_layoutEditorData->setParent(this);
	// When loading is successful.
	if (_widget)
	{
		// Now set the parent after successful loading and buddy resolving.
		_widget->setParent(_scrollArea ? (QWidget*) _scrollArea : this);
		//
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
	// TODO: install event filter for all children up the tree.
	if (_widget)
	{
		for (auto child: _widget->children())
		{
			child->installEventFilter(this);
		}
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
		if (_widget)
		{
			FormBuilderSave(&file, _widget);
		}
		if (!file.commit())
		{
			errorMessage = tr("Cannot write file %1:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString());
		}
	}
	else
	{
		errorMessage = tr("Cannot open file %1 for writing:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString());
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
	Q_EMIT mdiSignals.modificationChanged(flag);
}

void LayoutEditor::documentModified()
{
	_modified = true;
	documentWasModified();
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

bool LayoutEditor::eventFilter(QObject* watched, QEvent* event)
{
	// Intercept mouse button pressed when layout is editable.
	if (event->type() == QEvent::MouseButtonPress)
	{
		// Get the mouse event.
		if (auto mouseEvent = dynamic_cast<QMouseEvent*>(event))
		{
			// Check if it was the right mouse button.
			if (mouseEvent->button() == Qt::MouseButton::RightButton && LayoutData::hasMenuModifiers())
			{
				// When in widget resides in a splitter, events of the widgets do not pass here.
				// So get the widget under the global position.
				auto widget = QApplication::widgetAt(mouseEvent->globalPosition().toPoint());
				// When the object/widget is contained by object also derived from sf::ObjectExtension use that parent for the popup menu.
				if (auto obj = ObjectExtension::getExtensionParent(widget, false))
				{
					popupContextMenu(obj, mouseEvent->globalPosition().toPoint());
				}
				else
				{
					popupContextMenu(widget, mouseEvent->globalPosition().toPoint());
				}
			}
		}
	}
	return QObject::eventFilter(watched, event);
}

void LayoutEditor::setReadOnly(bool readOnly)
{
	_layoutEditorData->setReadOnly(readOnly);
}

QDir LayoutEditor::getDirectory() const
{
	return QFileInfo(_curFile).absoluteDir();
}

void LayoutEditor::popupContextMenu(QObject* target, const QPoint& pos)
{
	_currentTarget = target;
	auto p = _currentTarget->property(VariableWidgetBase::propertyNameEditorObject());
	if (p.isValid())
	{
		_currentTarget = p.value<QObject*>();
	}
	// Set the text of the menu's action using the class name of the target.
	_actionEdit->setText(QString("&Edit (%1)").arg(QString(_currentTarget->metaObject()->className()).split("::").last()));
	//
	_targetContextMenu->exec(pos);
	_currentTarget = nullptr;
}

void LayoutEditor::openPropertyEditor(QObject* target)
{
	auto dlg = new PropertySheetDialog("LayoutObjectEditor", _settings, this);
	// Make the dialog close
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	// Set the window title using the objects name.
	dlg->setWindowTitle(tr("Item Properties (%1) ").arg(target->objectName()));
	// Add the miscellaneous property page.
	dlg->addPage(new MiscellaneousPropertyPage(target, dlg));
	//
	bool flagContainer{false};
	// When based of a widget.
	if (auto w = dynamic_cast<QWidget*>(target))
	{
		// Widgets can be containers.
		flagContainer = true;
		dlg->addPage(new WidgetPropertyPage(w, dlg));
		// When derived from an ObjectExtension class.
		if (auto oe = dynamic_cast<ObjectExtension*>(target))
		{
			flagContainer &= oe->getSaveChildren();
			oe->addPropertyPages(dlg);
		}
	}
	// When it is a container.
	if (flagContainer)
	{
		auto w = dynamic_cast<QWidget*>(target);
		//if (ContainerPropertyPage::isContainer(QWidget*))
		dlg->addPage(new ContainerPropertyPage(w, dlg));
	}
	// Add the position property page to change position in the layout.
	dlg->addPage(new PositionPropertyPage(target, dlg));
	// When the dialog was applied set the modified flag.
	connect(dlg, &PropertySheetDialog::modified, this, &LayoutEditor::documentModified);
	//
	dlg->setModal(true);
	// Signal that an object is selected.
	Q_EMIT objectSelected(target);
	//
	dlg->exec();
}

ObjectHierarchyModel* LayoutEditor::getHierarchyModel()
{
	// When the model does not exist create it.
	if (!_model)
	{
		_model = new ObjectHierarchyModel(false, this);
		_model->updateList(_widget);
	}
	return _model;
}

}// namespace sf
