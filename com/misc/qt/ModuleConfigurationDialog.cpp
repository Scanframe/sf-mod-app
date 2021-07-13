#include <QToolBar>
#include <QDirIterator>
#include <QLibrary>
#include "Resource.h"
#include "../gen/DynamicLibraryInfo.h"
#include "../gen/gen_utils.h"

#include "ModuleConfiguration.h"
#include "ModuleConfigurationDialog.h"
#include "ui_ModuleConfigurationDialog.h"

namespace sf
{

class AppModuleList :public QAbstractListModel
{
	public:
		explicit AppModuleList(ModuleConfiguration* config, QObject* parent = nullptr);

		~AppModuleList() override;

		void refresh();

		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		[[nodiscard]] int rowCount(const QModelIndex& parent) const override;

		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

		[[nodiscard]] int columnCount(const QModelIndex& parent) const override;

		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

		void doubleClicked(const QModelIndex& index);

		void clicked(const QModelIndex& index);

		bool submit() override;

		enum EColumn
		{
			cCheckBox = 0,
			cName,
			cFilename,
			cDescription,
			cMaxColumns
		};

		void toggleSelection(const QModelIndex& index);

		struct ListItem :public DynamicLibraryInfo
		{
			Qt::CheckState state{Qt::CheckState::Unchecked};
		};

		// Holds the configuration structure.
		ModuleConfiguration* _config;
		// Holds the list library information.
		QList<ListItem> _libraryInfoList;
		// Holds the flag if a change occurred.
		bool _dirty{false};
};

AppModuleList::AppModuleList(ModuleConfiguration* config, QObject* parent)
	:QAbstractListModel(parent)
	,_config(config)
{
}

AppModuleList::~AppModuleList()
{
	_libraryInfoList.clear();
}

void AppModuleList::refresh()
{
	if (!_libraryInfoList.isEmpty())
	{
		beginRemoveRows(QModelIndex(), 0, (int) _libraryInfoList.length() - 1);
		removeRows(0, (int) _libraryInfoList.length());
		_libraryInfoList.clear();
		endRemoveRows();
	}
	// Get the current list of modules.
	auto list = _config->getList();
	// Iterate through the list module file in the module directory.
	QDirIterator it(_config->getModuleDir(), QDir::Filter::Files);
	while (it.hasNext())
	{
		// Check if the file is a library.
		if (QLibrary::isLibrary(it.next()))
		{
			qInfo() << "Checking: " << it.fileName();
			ListItem dld;
			dld.read(it.fileInfo().absoluteDir().absolutePath().toStdString(), it.fileName().toStdString());
			if (dld.filename.length())
			{
				// Check if the list is in the to be loaded modules.
				if (list.contains(it.fileName()))
				{
					// When it exists set the state (makes it be loaded)
					dld.state = Qt::CheckState::Checked;
					// Remove the filename from the list when found to keep only the ones that are not available right now.
					list.remove(it.fileName());
				}
				_libraryInfoList.append(dld);
			}
		}
	}
	// For all entries which were not present add an entry.
	for (auto i = list.begin(); i != list.constEnd(); ++i)
	{
		ListItem dld;
		// Assign the name of filename of the entry.
		dld.name = i.value().toStdString();
		// Assign the name of filename of the entry.
		dld.filename = i.key().toStdString();
		// Indicates the module should have been loaded but is not available at this moment.
		dld.state = Qt::CheckState::PartiallyChecked;
			_libraryInfoList.append(dld);
	}
	// Sort the list of libraries by name.
	std::sort(_libraryInfoList.begin(), _libraryInfoList.end(),
		[](const DynamicLibraryInfo& a, const DynamicLibraryInfo& b) {return a.name < b.name;});
	//
	beginInsertRows(QModelIndex(), 0, (int) _libraryInfoList.length() - 1);
	insertRows(0, (int) _libraryInfoList.length());
	endInsertRows();
}

QVariant AppModuleList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}
	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case cCheckBox:
				return QString(tr("Load"));
			case cName:
				return QString(tr("Name"));
			case cDescription:
				return QString(tr("Description"));
			case cFilename:
				return QString(tr("Filename"));
			default:
				return QString("Column %1").arg(section);
		}
	}
	else
	{
		return QString("Row %1").arg(section);
	}
}

Qt::ItemFlags AppModuleList::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::ItemFlag::NoItemFlags;
	}
	//
	Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable |
		Qt::ItemFlag::ItemNeverHasChildren;
	//
	if (index.column() == cName)
	{
		//flags |= Qt::ItemFlag::ItemIsEditable;
		flags |= Qt::ItemFlag::ItemIsUserCheckable;
		flags |= Qt::ItemFlag::ItemIsUserTristate;
	}
	return flags;
}

int AppModuleList::rowCount(const QModelIndex& parent) const
{
	return (int) _libraryInfoList.count();
}

QVariant AppModuleList::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	if (index.row() >= _libraryInfoList.count())
	{
		return QVariant();
	}
	if (role == Qt::CheckStateRole && index.column() == cCheckBox)
	{
		return _libraryInfoList.at(index.row()).state;
	}
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		switch (index.column())
		{
			case cName:
				return QString::fromStdString(_libraryInfoList.at(index.row()).name);
			case cDescription:
				return QString::fromStdString(_libraryInfoList.at(index.row()).description);
			case cFilename:
				return QString::fromStdString(_libraryInfoList.at(index.row()).filename);
		}
	}
	return QVariant();
}

int AppModuleList::columnCount(const QModelIndex& parent) const
{
	// Return the amount of columns to display.
	return cMaxColumns;
}

void AppModuleList::clicked(const QModelIndex& index)
{
	if (index.column() == AppModuleList::EColumn::cCheckBox)
	{
		toggleSelection(index);
	}
}

void AppModuleList::doubleClicked(const QModelIndex& index)
{
	if (index.column() != AppModuleList::EColumn::cCheckBox)
	{
		toggleSelection(index);
	}
}

void AppModuleList::toggleSelection(const QModelIndex& index)
{
	if (index.isValid())
	{
		auto& state = _libraryInfoList[index.row()].state;
		switch (state)
		{
			case Qt::CheckState::Unchecked:
				state = Qt::CheckState::Checked;
				break;
			case Qt::CheckState::PartiallyChecked:
			case Qt::CheckState::Checked:
				state = Qt::CheckState::Unchecked;
				break;
		}
		emit dataChanged(index, QModelIndex());
		_dirty = true;
	}
}

bool AppModuleList::submit()
{
	if (_dirty)
	{
		ModuleConfiguration::ModuleListType ml;
		// Fill the module list.
		for (auto& i: _libraryInfoList)
		{
			// Disregard unchecked entries.
			if (i.state == Qt::CheckState::PartiallyChecked || i.state == Qt::CheckState::Checked)
			{
				ml[QString::fromStdString(i.filename)] = QString::fromStdString(i.name);
			}
		}
		// Save the module list.
		_config->save(ml);
		// Reset the dirty flag.
		_dirty = false;
		// Load the modules if the haven't already.
		_config->load();
	}
	return true;
}

ModuleConfigurationDialog::ModuleConfigurationDialog(ModuleConfiguration* config, QWidget* parent)
	:QDialog(parent)
	 , ui(new Ui::ModuleConfigurationDialog)
	 , _moduleList(new AppModuleList(config, this))
{
	ui->setupUi(this);
	// Hook the needed signals.
	connect(ui->btnClose, &QPushButton::clicked, this, &ModuleConfigurationDialog::close);
	connect(ui->btnOkay, &QPushButton::clicked, this, &ModuleConfigurationDialog::applyClose);
	connect(ui->btnApply, &QPushButton::clicked, _moduleList, &AppModuleList::submit);
	connect(ui->btnRefresh, &QPushButton::clicked, _moduleList, &AppModuleList::refresh);
	connect(ui->listAvailable, &QAbstractItemView::clicked, _moduleList, &AppModuleList::clicked);
	connect(ui->listAvailable, &QAbstractItemView::doubleClicked, _moduleList, &AppModuleList::doubleClicked);
	// Assign the model to the view.
	ui->listAvailable->setModel(_moduleList);
/*
	//ui->listAvailable->setColumnHidden(0, true);
	auto toolBar = new QToolBar("Mine", this);
	// Add a toolbar for some buttons when a box layout is present.
	auto lo = qobject_cast<QBoxLayout*>(layout());
	if (lo)
	{
		lo->insertWidget(0, toolBar);
		auto act = new QAction("&Refesh", toolBar);
		act->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Reload),
			QPalette::ColorRole::ButtonText));
		connect(act, &QAction::triggered, _moduleList, &AppModuleList::refresh);
		toolBar->addAction(act);
	}
*/
	// Populate the list.
	_moduleList->refresh();
	// Resize the columns.
	for (int i = 0; i < _moduleList->columnCount(QModelIndex()); i++)
	{
		ui->listAvailable->resizeColumnToContents(i);
	}
	// Assign svg icons to the buttons.
	for (auto& i: QList<QPair<QAbstractButton*, Resource::Icon>>{
		{ui->btnOkay, Resource::Icon::Okay},
		{ui->btnClose, Resource::Icon::Close},
		{ui->btnApply, Resource::Icon::Check},
		{ui->btnRefresh, Resource::Icon::Reload}
	})
	{
		i.first->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(i.second), QPalette::ColorRole::ButtonText));
	}
}

ModuleConfigurationDialog::~ModuleConfigurationDialog()
{
	delete ui;
	delete _moduleList;
}

void ModuleConfigurationDialog::applyClose()
{
	_moduleList->submit();
	close();
}

}
