#include <QToolBar>
#include <QDirIterator>
#include <misc/gen/DynamicLibraryInfo.h>
#include <misc/qt/Resource.h>
#include <misc/gen/gen_utils.h>
#include "ModuleSelectionDialog.h"
#include "ui_ModuleSelectionDialog.h"

namespace sf
{

class AppModuleList :public QAbstractListModel
{
	public:
		explicit AppModuleList(QObject* parent = nullptr);

		~AppModuleList() override;

		void refresh();

		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		[[nodiscard]] int rowCount(const QModelIndex& parent) const override;

		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

		[[nodiscard]] int columnCount(const QModelIndex& parent) const override;

		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

		void doubleClicked(const QModelIndex& index);

		void clicked(const QModelIndex& index);

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
			Qt::CheckState state{Qt::CheckState::PartiallyChecked};
		};
		// Holds the list library information.
		QList<ListItem> _libraryInfoList;
};

AppModuleList::AppModuleList(QObject* parent)
	:QAbstractListModel(parent)
{
}

AppModuleList::~AppModuleList()
{
	_libraryInfoList.clear();
}

void AppModuleList::refresh()
{
	_libraryInfoList.clear();
	beginRemoveRows(QModelIndex(), 0, (int) _libraryInfoList.length() - 1);
	removeRows(0, (int) _libraryInfoList.length());
	endRemoveRows();
	//
	QDirIterator it(QCoreApplication::applicationDirPath(), {"*.so", "*.dll"}, QDir::Files);
	while (it.hasNext())
	{
		qInfo() << "Checking: " << it.next();
		ListItem dld;
		dld.read(it.fileInfo().absoluteDir().absolutePath().toStdString(), it.fileName().toStdString());
		if (dld.filename.length())
		{
			_libraryInfoList.append(dld);
			qDebug() << "Filename:" << dld.filename;
			qDebug() << "Name: " << dld.name;
			qDebug() << "Description: " << dld.description;
			//QDir(QCoreApplication::applicationDirPath()).relativeFilePath(it.fileName());
		}
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
				return QString("Load");
			case cName:
				return QString("Name");
			case cDescription:
				return QString("Description");
			case cFilename:
				return QString("Filename");
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
	}
}

ModuleSelectionDialog* ModuleSelectionDialog::_singleton = nullptr;

ModuleSelectionDialog* ModuleSelectionDialog::getDialog(QSettings* settings, QWidget* parent)
{
	return _singleton ? _singleton : new ModuleSelectionDialog(settings, parent);
}

ModuleSelectionDialog::ModuleSelectionDialog(QSettings* settings, QWidget* parent)
	:QDialog(parent)
	 , ui(new Ui::ModuleSelectionDialog)
	 , _settings(settings)
	 , _moduleList(new AppModuleList(this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	_singleton = this;
	ui->setupUi(this);
	connect(ui->btnClose, &QPushButton::clicked, this, &ModuleSelectionDialog::close);
	connect(ui->btnOkay, &QPushButton::clicked, this, &ModuleSelectionDialog::applyClose);
	connect(ui->listAvailable, &QAbstractItemView::clicked, _moduleList, &AppModuleList::clicked);
	connect(ui->listAvailable, &QAbstractItemView::doubleClicked, _moduleList, &AppModuleList::doubleClicked);
	ui->listAvailable->setModel(_moduleList);
	//ui->listAvailable->setColumnHidden(0, true);
	auto toolBar = new QToolBar("Mine", this);
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
	_moduleList->refresh();
	//
	ui->listAvailable->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	for (int i = 0; i < _moduleList->columnCount(QModelIndex()); i++)
	{
		ui->listAvailable->resizeColumnToContents(i);
	}
	for (auto& i: QList<QPair<QAbstractButton*, Resource::Icon>>
	{
		{ui->btnOkay, Resource::Icon::Okay},
		{ui->btnClose, Resource::Icon::Close},
		{ui->btnApply, Resource::Icon::Check}
	})
	{
		i.first->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(i.second), QPalette::ColorRole::ButtonText));
	}
}

ModuleSelectionDialog::~ModuleSelectionDialog()
{
	delete ui;
	delete _moduleList;
	_singleton = nullptr;
}

void ModuleSelectionDialog::applyClose()
{
	_moduleList->submit();
	close();
}

}
