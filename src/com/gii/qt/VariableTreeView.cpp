#include "VariableTreeView.h"
#include "VariableListModel.h"

#include <QKeyEvent>
#include <QTreeView>

namespace sf
{

VariableTreeView::VariableTreeView(QWidget* parent)
	: ::QTreeView(parent)
{
	setSelectionMode(SingleSelection);
	setSelectionBehavior(SelectRows);
	setEditTriggers(EditKeyPressed | DoubleClicked | SelectedClicked);
	setIndentation(0);
	setAlternatingRowColors(true);
	// Create a variable list model.
	const auto lm = new sf::VariableListModel(this);
	base_type::setModel(lm);
	// Set the delegates of the model onto the view for editing.
	lm->setDelegates(this);
}

VariableListModel* VariableTreeView::variableListModel() const
{
	return static_cast<VariableListModel*>(model());
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
		if (const auto var = variableListModel()->getByIndex(rows.first()))
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
	if (event->key() == Qt::Key_Space && toggleRow())
	{
		// Skip default handling of the event.
		return;
	}
	// Call the base class implementation for other keys.
	QTreeView::keyPressEvent(event);
}

}// namespace sf
