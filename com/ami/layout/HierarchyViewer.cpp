#include <QAction>
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>
#include "HierarchyViewer.h"
#include "ui_HierarchyViewer.h"
#include "LayoutEditor.h"

namespace sf
{

void childrenExpandCollapse(QTreeView* treeView, bool expand, const QModelIndex& index = {}) // NOLINT(misc-no-recursion)
{
	if (!index.isValid())
	{
		//childrenExpandCollapse(expand, ui->treeView->rootIndex());
		int count = treeView->model()->rowCount();
		for (int i = 0; i < count; i++)
		{
			childrenExpandCollapse(treeView, expand, treeView->model()->index(i, 0));
		}
		return;
	}
	int childCount = index.model()->rowCount(index);
	for (int i = 0; i < childCount; i++)
	{
		childrenExpandCollapse(treeView, expand, treeView->model()->index(i, 0, index));
	}
	expand ? treeView->expand(index) : treeView->collapse(index);
}

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
			{ui->tbEdit, _actionEdit, Resource::Edit, tr("Edit"), tr("Edit the selected item.")}
		})
	{
		std::get<1>(t) = new QAction(Resource::getSvgIcon(Resource::getSvgIconResource(std::get<2>(t)), QPalette::ButtonText), std::get<3>(t), this);
		std::get<1>(t)->setToolTip(std::get<3>(t));
		std::get<0>(t)->setDefaultAction(std::get<1>(t));
		ui->treeView->addAction(std::get<1>(t));
	}
	// Create  context menu for tree view.
	ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
	for (auto a: {_actionEdit, _actionExpandAll, _actionCollapseAll})
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
	connect(_actionEdit, &QAction::triggered, this, &HierarchyViewer::editObject);
	connect(ui->treeView, &QTreeView::doubleClicked, this, &HierarchyViewer::editObject);
}

HierarchyViewer::~HierarchyViewer()
{
	delete ui;
}

void HierarchyViewer::editObject()
{
	if (!ui->treeView->selectionModel()->selectedIndexes().empty())
	{
		auto index = ui->treeView->selectionModel()->selectedIndexes().first();
		if (auto m = dynamic_cast<ObjectHierarchyModel*>(ui->treeView->model()))
		{
			if (auto o = m->getObject(index))
			{
				auto dlg = _layoutEditor->openPropertyEditor(o);
				dlg->exec();
				m->dataChanged(index, index);
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
		}
		else
		{
			ui->treeView->setModel(nullptr);
		}
	}
}

}
