#include <QAction>
#include <QMessageBox>
#include <misc/qt/Resource.h>
#include <misc/qt/PropertySheetDialog.h>
#include <misc/qt/qt_utils.h>
#include "ScriptManagerListModel.h"
#include "ScriptManagerPropertyPage.h"
#include "ui_ScriptManagerPropertyPage.h"

namespace sf
{

ScriptManagerPropertyPage::ScriptManagerPropertyPage(ScriptManager* manager, PropertySheetDialog* parent)
	:PropertyPage(parent)
	 , ui(new Ui::ScriptManagerPropertyPage)
	 , _manager(manager)
	 , _listModel(new ScriptManagerListModel(manager, this))
{
	ui->setupUi(this);
	ui->tvScripts->header()->setStretchLastSection(true);
	// Attach the list model to the view.
	ui->tvScripts->setModel(_listModel);
	// Set the delegates of the model onto the view for editing.
	_listModel->setDelegates(ui->tvScripts);
	// Create the actions for the buttons.
	for (auto& t: std::vector<std::tuple<QToolButton*, QAction*&, Resource::Icon, QString>>
		{
			{ui->tbStartAll, _actionStartAll, Resource::Run, tr("Compiles and initializes all scripts.")},
			{ui->tbStart, _actionStart, Resource::Start, tr("Compiles and initializes the selected script.")},
			{ui->tbStop, _actionStop, Resource::Stop, tr("Stops the selected script.")},
			{ui->tbEdit, _actionEdit, Resource::Edit, tr("Edit the selected script.")},
			{ui->tbAdd, _actionAdd, Resource::Add, tr("Add a script after the selected script.")},
			{ui->tbRemove, _actionRemove, Resource::Remove, tr("Remove the selected script.")},
		})
	{
		std::get<1>(t) = new QAction(
			Resource::getSvgIcon(Resource::getSvgIconResource(std::get<2>(t)), QPalette::ButtonText),
			std::get<3>(t),
			this
		);
		std::get<1>(t)->setToolTip(std::get<3>(t));
		std::get<0>(t)->setDefaultAction(std::get<1>(t));
		//std::get<0>(t)->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
	}
	// Function to get the selected item in an item view widget.
	auto getIndex = [&](QAbstractItemView* tv) -> QModelIndex
	{
		auto list = tv->selectionModel()->selectedIndexes();
		return list.isEmpty() ? QModelIndex() : list.first();
	};
	//
	connect(_actionStartAll, &QAction::triggered, _listModel, &ScriptManagerListModel::startAll);
	connect(_actionStart, &QAction::triggered, [&] {_listModel->start(getIndex(ui->tvScripts));});
	connect(_actionStop, &QAction::triggered, [&] {_listModel->stop(getIndex(ui->tvScripts));});
	connect(_actionAdd, &QAction::triggered, [&]
	{
		auto index = getIndex(ui->tvScripts);
		// When the index is valid and row zero is selected ask for position.
		if (index.isValid() && !index.row() && (QMessageBox::Yes == QMessageBox::question(this, tr("New Entry Location"),
			tr("Add the new entry at the to top?"), QMessageBox::Yes | QMessageBox::No)))
		{
			index = QModelIndex();
		}
		_listModel->add(index);
	});
	connect(_actionRemove, &QAction::triggered, [&] {_listModel->remove(getIndex(ui->tvScripts));});
	connect(_actionEdit, &QAction::triggered, [&]
	{
		auto idx = getIndex(ui->tvScripts);
		if (idx.isValid())
		{
			// Open the editor from index position.
			Q_EMIT openEditor(idx.row());
			// Apply modifications.
			_sheet->applySheet();
			// Close the sheet.
			_sheet->close();
		}
	});
	// Resize the columns.
	resizeColumnsToContents(ui->tvScripts);
}

ScriptManagerPropertyPage::~ScriptManagerPropertyPage()
{
	delete ui;
}

QString ScriptManagerPropertyPage::getPageName() const
{
	return QStringLiteral("Script Manager");
}

QString ScriptManagerPropertyPage::getPageDescription() const
{
	return QStringLiteral("Manage the scripts running in the background.");
}

QIcon ScriptManagerPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/scripting", QPalette::ColorRole::Text);
}

void ScriptManagerPropertyPage::updatePage()
{
}

bool ScriptManagerPropertyPage::isPageModified() const
{
	return _manager->isModified();
}

void ScriptManagerPropertyPage::applyPage()
{
	_manager->settingsReadWrite(true);
}

}
