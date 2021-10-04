#include <QAction>
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>
#include "HierarchyViewer.h"
#include "LayoutEditor.h"
#include "AddItemDialog.h"
#include "ui_HierarchyViewer.h"

namespace sf
{

HierarchyViewer::HierarchyViewer(QWidget* parent)
	:QWidget(parent)
	 , ui(new Ui::HierarchyViewer)
{
	ui->setupUi(this);
	// Create the actions for the buttons.
	for (auto& t: std::vector<std::tuple<QToolButton*, QAction*&, Resource::Icon, QString, QString>>
		{
			{ui->tbCollapseAll, _actionCollapseAll, Resource::Collapse, tr("Collapse"), tr("Collapse all rows.")},
			{ui->tbExpandAll, _actionExpandAll, Resource::Expand, tr("Expand"), tr("Expand all rows.")},
			{ui->tbEdit, _actionEdit, Resource::Edit, tr("Edit"), tr("Edit the selected item.")},
			{ui->tbAdd, _actionAdd, Resource::Add, tr("Add"), tr("Add a new item.")},
			{ui->tbRemove, _actionRemove, Resource::Remove, tr("Remove"), tr("Remove an item.")},
		})
	{
		std::get<1>(t) = new QAction(Resource::getSvgIcon(Resource::getSvgIconResource(std::get<2>(t)), QPalette::ButtonText), std::get<3>(t), this);
		std::get<1>(t)->setToolTip(std::get<3>(t));
		std::get<0>(t)->setDefaultAction(std::get<1>(t));
	}
	// Create  context menu for tree view.
	ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
	for (auto a: {_actionEdit, _actionAdd, _actionRemove})
	{
		ui->treeView->addAction(a);
	}
	//
	connect(_actionCollapseAll, &QAction::triggered, [&]()
	{
		childrenExpandCollapse(ui->treeView, false);
	});
	connect(_actionExpandAll, &QAction::triggered, [&]()
	{
		childrenExpandCollapse(ui->treeView, true);
	});
	connect(ui->treeView, &QTreeView::doubleClicked, this, &HierarchyViewer::editObject);
	connect(_actionEdit, &QAction::triggered, this, &HierarchyViewer::editObject);
	connect(_actionAdd, &QAction::triggered, this, &HierarchyViewer::addObject);
	connect(_actionRemove, &QAction::triggered, this, &HierarchyViewer::removeObject);
	connect(ui->treeView, &QTreeView::clicked, this, &HierarchyViewer::objectSelected);
}

HierarchyViewer::~HierarchyViewer()
{
	delete ui;
}

void HierarchyViewer::documentModified()
{
	if (_layoutEditor)
	{
		_layoutEditor->documentModified();
	}
}

QObject* HierarchyViewer::objectSelected(const QModelIndex &index)
{
	auto idx = index;
	QObject* rv{nullptr};
	if (!idx.isValid())
	{
		if (!ui->treeView->selectionModel()->selectedIndexes().empty())
		{
			idx = ui->treeView->selectionModel()->selectedIndexes().first();
		}
	}
	if (idx.isValid())
	{
		if (auto m = dynamic_cast<const ObjectHierarchyModel*>(index.model()))
		{
			rv = m->getObject(idx);
		}
	}
	objectSelectChange(rv);
	return rv;
}

void HierarchyViewer::editObject()
{
	if (!ui->treeView->selectionModel()->selectedIndexes().empty())
	{
		auto index = ui->treeView->selectionModel()->selectedIndexes().first();
		if (auto model = dynamic_cast<ObjectHierarchyModel*>(ui->treeView->model()))
		{
			if (auto obj = model->getObject(index))
			{
				_layoutEditor->openPropertyEditor(obj);
				model->dataChanged(index, index);
			}
		}
	}
}

void HierarchyViewer::addObject()
{
	if (!ui->treeView->selectionModel()->selectedIndexes().empty())
	{
		auto index = ui->treeView->selectionModel()->selectedIndexes().first();
		if (auto model = dynamic_cast<ObjectHierarchyModel*>(ui->treeView->model()))
		{
			if (auto obj = model->getObject(index))
			{
				if (auto widget = qobject_cast<QWidget*>(obj))
				{
					if (widget->layout())
					{
						auto dialog = new AddItemDialog(this);
						auto children = dialog->execute(widget->layout());
						for (auto child: children)
						{
							model->insertItemAt(index, child);
						}
						delete dialog;
						// Signal the layout editor the document has modified.
						if (children.count())
						{
							documentModified();
						}
					}
				}
			}
		}
	}
}

void HierarchyViewer::removeObject()
{
	// Check if an item is selected.
	if (!ui->treeView->selectionModel()->selectedIndexes().empty())
	{
		// Get the first and also only selected index.
		auto index = ui->treeView->selectionModel()->selectedIndexes().first();
		// Get the correct pointer of the model.
		if (auto model = dynamic_cast<ObjectHierarchyModel*>(ui->treeView->model()))
		{
			// Prevent removing row the root.
			if (!model->isRoot(index))
			{
				// Get the object from the passed index.
				if (auto obj = model->getObject(index))
				{
					// Signal the object is removed.
					Q_EMIT objectSelected();
					// Remove the item also from the tree.
					model->removeItem(index);
					delete obj;
					// Signal the layout editor the document has modified.
					documentModified();
				}
			}
		}
	}
}

void HierarchyViewer::setEditor(sf::LayoutEditor* editor)
{
	if (_layoutEditor != editor)
	{
		_layoutEditor = editor;
		if (_layoutEditor)
		{
			ui->treeView->setModel(_layoutEditor->getHierarchyModel());
			childrenExpandCollapse(ui->treeView, true);
			resizeColumnsToContents(ui->treeView);
			// Clear selected object.
			objectSelected({});
			// Connect only once to the destruction signal of a layout editor.
			connect(_layoutEditor, &LayoutEditor::destroyed, this, &HierarchyViewer::editorDisconnect, Qt::UniqueConnection);
		}
		else
		{
			// Clear selected object.
			objectSelected({});
			// Clear the set modal.
			ui->treeView->setModel(nullptr);
		}
	}
}

void HierarchyViewer::editorDisconnect(QObject* obj)
{
	// Check if the current layout editor is destroyed.
	if (_layoutEditor == obj)
	{
		setEditor(nullptr);
	}
}

void HierarchyViewer::selectObject(QObject* obj)
{
	if (_layoutEditor)
	{
		auto index = _layoutEditor->getHierarchyModel()->getObjectIndex(obj);
		ui->treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
		Q_EMIT objectSelected(index);
		resizeColumnsToContents(ui->treeView);
	}
}

}
