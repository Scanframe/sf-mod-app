#include <misc/qt/qt_utils.h>
#include <misc/qt/Resource.h>
#include "SelectImplementationDialog.h"
#include "ui_SelectImplementationDialog.h"

namespace sf
{

SelectImplementationDialog::SelectImplementationDialog(QSettings* settings, bool file_only, QWidget* parent)
	:QDialog(parent)
	 , _settings(settings)
	 , ui(new Ui::SelectImplementationDialog)
{
	ui->setupUi(this);
	// Attach the model to9 the view.
	ui->listAvailable->setModel(AppModuleInterface::getListModel(file_only, this));
	ui->listAvailable->header()->setStretchLastSection(true);
	// Resize the columns.
	for (int i = 0; i < ui->listAvailable->model()->columnCount(QModelIndex()) - 1; i++)
	{
		ui->listAvailable->resizeColumnToContents(i);
	}
	// Connect the needed signals
	connect(ui->listAvailable, &QTreeView::doubleClicked, this, &SelectImplementationDialog::doubleClicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &SelectImplementationDialog::close);
	connect(ui->btnOkay, &QPushButton::clicked, this, &SelectImplementationDialog::okay);
	// Assign svg icons to the buttons.
	for (auto& i: QList<QPair<QAbstractButton*, Resource::Icon >>
		{
			{ui->btnOkay, Resource::Icon::Okay},
			{ui->btnCancel, Resource::Icon::Cancel},
		})
	{
		i.first->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(i.second), QPalette::ButtonText));
	}
	//
	stateSaveRestore(false);
}

SelectImplementationDialog::~SelectImplementationDialog()
{
	stateSaveRestore(true);
	delete ui;
}

void SelectImplementationDialog::stateSaveRestore(bool save)
{
	_settings->beginGroup(getObjectNamePath(this).join('.').prepend("State."));
	QString key("State");
	if (save)
	{
		_settings->setValue(key, saveGeometry());
	}
	else
	{
		restoreGeometry(_settings->value(key).toByteArray());
	}
	_settings->endGroup();
}

AppModuleInterface* SelectImplementationDialog::getSelected() const
{
	return _selected;
}

void SelectImplementationDialog::okay()
{
	auto index = ui->listAvailable->selectionModel()->selectedIndexes().first();
	if (index.isValid())
	{
		_selected = qvariant_cast<AppModuleInterface*>
			(ui->listAvailable->model()->data(index, Qt::ItemDataRole::UserRole));
	}
/*
	auto indexes = ui->listAvailable->selectionModel()->selectedIndexes();
	if (indexes.count())
	{
		_selected = qvariant_cast<AppModuleInterface*>
			(ui->listAvailable->model()->data(indexes.at(0), Qt::ItemDataRole::UserRole));
	}
*/
	close();
}

void SelectImplementationDialog::doubleClicked(const QModelIndex& index)
{
	_selected = qvariant_cast<AppModuleInterface*>(ui->listAvailable->model()->data(index, Qt::ItemDataRole::UserRole));
	close();
}

}
