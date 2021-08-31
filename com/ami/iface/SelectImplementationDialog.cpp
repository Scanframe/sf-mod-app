#include <misc/qt/qt_utils.h>
#include <misc/qt/Resource.h>
#include "SelectImplementationDialog.h"
#include "ui_SelectImplementationDialog.h"

namespace sf
{

AppModuleInterface* SelectImplementationDialog::execute(QSettings* settings, bool file_only, QWidget* parent)
{
	SelectImplementationDialog dlg(settings, file_only, parent);
	if (dlg.exec() == QDialog::Accepted)
	{
		return dlg.getSelected();
	}
	return nullptr;
}

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
	resizeColumnsToContents(ui->listAvailable);
	// Connect the needed signals
	connect(ui->listAvailable, &QTreeView::doubleClicked, [&](const QModelIndex &index)
	{
		accept();
	});
	connect(this, &QDialog::accepted, [&]()
	{
		auto index = ui->listAvailable->selectionModel()->selectedIndexes().first();
		if (index.isValid())
		{
			_selected = qvariant_cast<AppModuleInterface*>(ui->listAvailable->model()->data(index, Qt::ItemDataRole::UserRole));
		}
	});
	// Connect the needed buttonbox signals.
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
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

}
