#include "InformationSelectDialog.h"
#include "ui_InformationSelectDialog.h"
#include <QDialogButtonBox>
#include <QAction>
#include <QTimer>
#include <misc/qt/Resource.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/Globals.h>

namespace sf
{

InformationSelectDialog::InformationSelectDialog(QWidget* parent)
	:QDialog(parent)
	 , ui(new Ui::InformationSelectDialog)
	 , _proxyModel(new QSortFilterProxyModel(this))
{
	ui->setupUi(this);
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/ico/scanframe"));
	// Set some properties for the proxy to do its job properly.
	_proxyModel->setDynamicSortFilter(false);
	_proxyModel->setRecursiveFilteringEnabled(true);
	_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	// Assign the proxy for allowing filtering by name.
	ui->treeView->setModel(_proxyModel);
	//
	ui->treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
	// Create the actions for the buttons.
	for (auto& t: std::vector<std::tuple<QToolButton*, QAction*&, Resource::Icon, QString, QString>>
		{
			{ui->tbCollapseAll, _actionCollapseAll, Resource::Collapse, tr("Collapse"), tr("Collapse all rows.")},
			{ui->tbExpandAll, _actionExpandAll, Resource::Expand, tr("Expand"), tr("Expand all rows.")}
		})
	{
		std::get<1>(t) = new QAction(Resource::getSvgIcon(Resource::getSvgIconResource(std::get<2>(t)), QPalette::ButtonText), std::get<3>(t), this);
		std::get<1>(t)->setToolTip(std::get<3>(t));
		std::get<0>(t)->setDefaultAction(std::get<1>(t));
		ui->treeView->addAction(std::get<1>(t));
	}
	connect(ui->treeView, &QTreeView::expanded, [&]()
	{
		resizeColumnsToContents(ui->treeView);
	});
	connect(ui->treeView, &QTreeView::doubleClicked, [&](const QModelIndex& index)
	{
		if (auto m = getSourceModel<InformationItemModel>(ui->treeView->model()))
		{
			if (_mode == Gii::Multiple)
			{
				m->toggleSelection(getSourceModelIndex(index));
			}
			else
			{
				auto idx = getSourceModelIndex(index);
				if (!m->isFolder(idx))
				{
					accept();
				}
			}
		}
	});
	connect(_actionCollapseAll, &QAction::triggered, [&]()
	{
		childrenExpandCollapse(false);
	});
	connect(_actionExpandAll, &QAction::triggered, [&]()
	{
		childrenExpandCollapse(true);
	});
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(this, &QDialog::accepted, [&]()
	{
		if (!ui->treeView->selectionModel()->selectedIndexes().empty())
		{
			auto index = getSourceModelIndex(ui->treeView->selectionModel()->selectedIndexes().first());
			_selectedId = index.isValid() ? _itemModel->getId(index) : 0;
		}
	});
	connect(ui->leSearch, &QLineEdit::textChanged, this, &InformationSelectDialog::applyFilter, Qt::QueuedConnection);
}

void InformationSelectDialog::applyFilter(const QString& filter)
{
	_proxyModel->setFilterFixedString(filter);
	// Expand when searching otherwise collapse.
	expandTreeView(ui->treeView, true);
}

void InformationSelectDialog::childrenExpandCollapse(bool expand, const QModelIndex& index) // NOLINT(misc-no-recursion)
{
	if (!index.isValid())
	{
		int count = ui->treeView->model()->rowCount();
		for (int i = 0; i < count; i++)
		{
			childrenExpandCollapse(expand, ui->treeView->model()->index(i, 0));
		}
		return;
	}
	int childCount = index.model()->rowCount(index);
	for (int i = 0; i < childCount; i++)
	{
		childrenExpandCollapse(expand, ui->treeView->model()->index(i, 0, index));
	}
	expand ? ui->treeView->expand(index) : ui->treeView->collapse(index);
}

void InformationSelectDialog::stateSaveRestore(bool save)
{
	if (_settings)
	{
		_settings->beginGroup(getObjectNamePath(this).join('.').prepend("State."));
		QString keyState("State");
		QString keyWidgetRect("WidgetRect");
		if (save)
		{
			_settings->setValue(keyWidgetRect, geometry());
		}
		else
		{
			// Get the keys in the section to check existence in the ini-section.
			if (_settings->value(keyWidgetRect).toRect().isValid())
			{
				resize(_settings->value(keyWidgetRect).toRect().size());
			}
		}
		_settings->endGroup();
	}
}

InformationTypes::IdVector InformationSelectDialog::getSelectedIds() const
{
	if (_mode == Gii::Multiple)
	{
		return _itemModel->getSelectedIds();
	}
	if (_selectedId)
	{
		return InformationTypes::IdVector{_selectedId};
	}
	return {};
}

InformationTypes::IdVector InformationSelectDialog::execute(Gii::SelectionMode mode, Gii::TypeId idType, QSettings* settings)
{
	_settings = settings ? settings : getGlobalSettings();
	_mode = mode;
	// Delete previous model when it exists.
	delete_null(_itemModel);
	// Create a new model for the given type and selection mode.
	_itemModel = new InformationItemModel(mode, idType, this);
	// Fill the model.
	_itemModel->updateList();
	// Assign the model.
	_proxyModel->setSourceModel(_itemModel);
	// Resize the columns.
	resizeColumnsToContents(ui->treeView);
	// Restore the settings for this instance.
	stateSaveRestore(false);
	// When the dialog is accepted. (Ok button clicked.)
	if (exec() == QDialog::Accepted)
	{
		// Save the settings for this instance.
		stateSaveRestore(true);
		// Return the selected ids.
		return getSelectedIds();
	}
	// Save the settings for this instance.
	stateSaveRestore(true);
	// Return an empty list to signal.
	return {};
}

}
