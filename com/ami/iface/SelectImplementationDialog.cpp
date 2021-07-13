#include "SelectImplementationDialog.h"
#include "ui_SelectImplementationDialog.h"

namespace sf
{

SelectImplementationDialog::SelectImplementationDialog(QWidget* parent) :
	QDialog(parent), ui(new Ui::SelectImplementationDialog)
{
	ui->setupUi(this);
	// Attach the model to9 the view.
	ui->listAvailable->setModel(AppModuleInterface::getListModel(this));
	// Resize the columns.
	for (int i = 0; i < ui->listAvailable->model()->columnCount(QModelIndex()); i++)
	{
		ui->listAvailable->resizeColumnToContents(i);
	}
	// Connect the needed signals
	connect(ui->listAvailable, &QTreeView::doubleClicked, this, &SelectImplementationDialog::doubleClicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &SelectImplementationDialog::close);
	connect(ui->btnOkay, &QPushButton::clicked, this, &SelectImplementationDialog::okay);
}

SelectImplementationDialog::~SelectImplementationDialog()
{
	delete ui;
}

AppModuleInterface* SelectImplementationDialog::getSelected() const
{
	return _selected;
}

void SelectImplementationDialog::okay()
{
	auto indexes = ui->listAvailable->selectionModel()->selectedIndexes();
	if (indexes.count())
	{
		_selected =
			qvariant_cast<AppModuleInterface*>(ui->listAvailable->model()->data(indexes.at(0), Qt::ItemDataRole::UserRole));
	}
	close();
}

void SelectImplementationDialog::doubleClicked(const QModelIndex &index)
{
	_selected = qvariant_cast<AppModuleInterface*>(ui->listAvailable->model()->data(index, Qt::ItemDataRole::UserRole));
	close();
}

}
