#include "VariableTreeView.h"
#include "VariableListModel.h"

#include <QKeyEvent>
#include <QTreeView>

namespace sf
{

struct VariableTreeView::Private final : QObject
{
		explicit Private(VariableTreeView* widget)
			: QObject(widget)
			, _widget(widget)
			, _listModel(new VariableListModel(this))
		{
			// Set the delegates of the model onto the view for editing.
			_listModel->setDelegates(_widget);
		}

		VariableTreeView* _widget;
		VariableListModel* _listModel;
};

VariableTreeView::VariableTreeView(QWidget* parent)
	: QTreeView(parent)
	, ObjectExtension(this)
	, _p(new Private(this))
{
	setSelectionMode(SingleSelection);
	setSelectionBehavior(SelectRows);
	setEditTriggers(EditKeyPressed | DoubleClicked | SelectedClicked);
	setIndentation(0);
	setAlternatingRowColors(true);
	QTreeView::setModel(_p->_listModel);
}

VariableTreeView::~VariableTreeView()
{
	delete _p;
}

bool VariableTreeView::isRequiredProperty(const QString& name)
{
	return true;
}

VariableListModel* VariableTreeView::variableListModel() const
{
	return _p->_listModel;
}

int VariableTreeView::nameLevel() const
{
	return _p->_listModel->nameLevel();
}

void VariableTreeView::setNameLevel(int level) const
{
	_p->_listModel->setNameLevel(level);
}

void VariableTreeView::setRowCheckBox(bool enabled) const
{
	_p->_listModel->setRowCheckBox(enabled);
}

bool VariableTreeView::rowCheckBox() const
{
	return _p->_listModel->rowCheckBox();
}

int VariableTreeView::editableColumn() const
{
	for (int column = 0; column < model()->columnCount(); ++column)
	{
		// Get an index for the first selected row and the given column.
		auto index = model()->index(selectedIndexes().first().row(), column);
		// Check if the ItemIsEditable flag is set.
		if (model()->flags(index) & Qt::ItemIsEditable)
		{
			// Return once you find the first editable column.
			return column;
		}
	}
	// Signal no editable column was found.
	return -1;
}

bool VariableTreeView::editRow() const
{
	// Get selected rows and check if validity (empty) before continuing.
	const auto selected = selectionModel()->selectedRows().first();
	if (selected.isValid())
	{
		// Get the first selected row's index.
		const auto index = model()->index(selected.row(), editableColumn());
		// Start editing the "age" column, even if another column is visually selected.
		// Can only edit when an editor is not open.
		if (index.isValid() && !isPersistentEditorOpen(index))
		{
			// Cast this since it is a const method which is calling a non-const edit signal trigger.
			const_cast<VariableTreeView*>(this)->edit(index);
			// Signal edit was called for this row.
			return true;
		}
	}
	// Signal edit is not available for this row.
	return false;
}

bool VariableTreeView::toggleRow()
{
	// Get selected rows and check if validity (empty) before continuing.
	auto rows = selectionModel()->selectedRows();
	if (!rows.empty())
	{
		if (const auto var = variableListModel()->getVariable(rows.first()))
		{
			// Toggle the data field.
			var->setData<bool>(!var->getData<bool>());
			dataChanged(rows.first(), rows.last(), {Qt::DisplayRole});
			var->emitEvent(Variable::EEvent::veUserPrivate, false);
			return true;
		}
	}
	// Signal edit is not available for this row.
	return false;
}

void VariableTreeView::initSelection() const
{
	// Select the first row.
	if (model()->rowCount() > 0)
	{
		selectionModel()->select(model()->index(0, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
	}
}

void VariableTreeView::keyPressEvent(QKeyEvent* event)
{
	// Override the edit key.
	if (event->key() == Qt::Key_F2 && editTriggers() | EditKeyPressed && editRow())
	{
		// Skip default handling of the event.
		return;
	}
	// When the space bar is pressed toggle the checkbox.
	if (event->key() == Qt::Key_Space && _p->_listModel->rowCheckBox() && toggleRow())
	{
		// Skip default handling of the event.
		return;
	}
	// Call the base class implementation for other keys.
	QTreeView::keyPressEvent(event);
}

}// namespace sf
