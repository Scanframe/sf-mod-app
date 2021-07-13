#include <QApplication>
#include <QMimeDatabase>
#include <QSettings>
#include <QAbstractListModel>
#include <misc/qt/Resource.h>
#include <QFileInfo>
#include "AppModuleInterface.h"
#include "SelectImplementationDialog.h"

namespace sf
{

class InterfaceListModel :public QAbstractListModel
{
	public:
		explicit InterfaceListModel(QObject* parent = nullptr);

		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		[[nodiscard]] int rowCount(const QModelIndex& parent) const override;

		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

		[[nodiscard]] int columnCount(const QModelIndex& parent) const override;

		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

		enum EColumn
		{
			cName,
			cDescription,
			cModule,
			cLibrary,
			cMaxColumns
		};
};

InterfaceListModel::InterfaceListModel(QObject* parent)
	:QAbstractListModel(parent)
{
}

QVariant InterfaceListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}
	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case cName:
				return QString(tr("Name"));
			case cDescription:
				return QString(tr("Description"));
			case cModule:
				return QString(tr("Module"));
			case cLibrary:
				return QString(tr("Library"));
			default:
				return QString("Column %1").arg(section);
		}
	}
	else
	{
		return QString("Row %1").arg(section);
	}
}

Qt::ItemFlags InterfaceListModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::ItemFlag::NoItemFlags;
	}
	return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemNeverHasChildren;
}

int InterfaceListModel::rowCount(const QModelIndex& parent) const
{
	return (int) AppModuleInterface::getMap().count();
}

QVariant InterfaceListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	auto& map = AppModuleInterface::getMap();
	if (index.row() >= map.count())
	{
		return QVariant();
	}
	auto entry = map[map.keys().at(index.row())];
	if (role == Qt::ItemDataRole::UserRole)
	{
		return QVariant::fromValue(entry);
	}
	else if (role == Qt::DecorationRole)
	{
		if (index.column() == cName)
		{
			return Resource::getSvgIcon(entry->getSvgIconResource(),
				QApplication::palette().color(QPalette::ColorRole::WindowText));
		}
	}
	else if (role == Qt::ItemDataRole::DisplayRole)
	{
		switch (index.column())
		{
			case cName:
				return entry->getName();
			case cDescription:
				return entry->getDescription();
			case cModule:
				return QString::fromStdString(AppModuleInterface::Interface().getRegisterName(entry));
			case cLibrary:
				return entry->getLibraryFilename();
		}
	}
	return QVariant();
}

int InterfaceListModel::columnCount(const QModelIndex& parent) const
{
	// Return the amount of columns to display.
	return cMaxColumns;
}

SF_IMPL_IFACE(AppModuleInterface, AppModuleInterface::Parameters, Interface)

AppModuleInterface::Map AppModuleInterface::_map;

AppModuleInterface::AppModuleInterface(const AppModuleInterface::Parameters& parameters)
	:QObject(parameters._parent)
{
}

AppModuleInterface::~AppModuleInterface() = default;

void AppModuleInterface::instantiate(QObject* parent)
{
	// Iterate through the available registered implementation names.
	for (auto& nm: Interface().getNames())
	{
		auto name = QString::fromStdString(nm);
		// Check if not created yet.
		if (!_map.contains(name))
		{
			_map[name] = AppModuleInterface::Interface().create(nm, Parameters(parent));
		}
	}
}

const AppModuleInterface::Map& AppModuleInterface::getMap()
{
	return _map;
}

AppModuleInterface* AppModuleInterface::selectDialog(const QString& title, QWidget* parent)
{
	SelectImplementationDialog dlg(parent);
	dlg.setWindowTitle(title);
	dlg.exec();
	return dlg.getSelected();
}

AppModuleInterface* AppModuleInterface::findByFile(const QString& filename)
{
	QFileInfo fi(filename);
	auto mime = QMimeDatabase().mimeTypeForFile(filename);
	//
	for (auto entry: getMap())
	{
		if (std::any_of(entry->_fileTypes.begin(), entry->_fileTypes.end(),
			[mime](const AppModuleFileType& ft) -> bool {return ft.isMime(mime);}))
		{
			return entry;
		}
		if (std::any_of(entry->_fileTypes.begin(), entry->_fileTypes.end(),
			[fi](const AppModuleFileType& ft) -> bool {return ft.isSuffix(fi.suffix());}))
		{
			return entry;
		}
	}
	return nullptr;
}

QAbstractItemModel* AppModuleInterface::getListModel(QObject* parent)
{
	return new InterfaceListModel(parent);
}

QString AppModuleInterface::getSvgIconResource() const
{
	return ":icon/svg/default-app-module";
}

void AppModuleInterface::addFileType(const QString& name, const QString& suffix)
{
	_fileTypes.append(AppModuleFileType(name, suffix));
}

void AppModuleInterface::addFileType(const QString& mime)
{
	_fileTypes.append(AppModuleFileType(mime));
}

void AppModuleInterface::addFileType(const QMimeType& mime)
{
	_fileTypes.append(AppModuleFileType(mime));
}

QString AppModuleInterface::getFileTypeFilters(bool all_files)
{
	QMap<QString, const AppModuleFileType*> ftm;
	// Iterate through the interface instances and create map where the key is the file suffix.
	for (auto entry: getMap())
	{
		for (auto& ft: entry->_fileTypes)
		{
			// Using the suffix as key makes it unique.
			auto suffix = ft.getSuffix();
			if (suffix.length())
			{
				ftm[suffix] = &ft;
			}
		}
	}
	QStringList sl;
	// Iterate over the map types.
	for (auto ft: ftm)
	{
		// Format the filetypes as a open dialog filter.
		sl.append(QString("%1 (*.%2)").arg(ft->getName()).arg(ft->getSuffix()));
	}
	// Sort the entries by name.
	std::sort(sl.begin(), sl.end());
	// Add the all files filter when requested.
	if (all_files)
	{
		sl.prepend("All Files (*)");
	}
	return sl.join(";;");
}

QString AppModuleInterface::getFileTypeFilters() const
{
	QMap<QString, const AppModuleFileType*> ftm;
	// Iterate through the interface instances and create map where the key is the file suffix.
	for (auto& ft: _fileTypes)
	{
		// Using the suffix as key makes it unique.
		auto suffix = ft.getSuffix();
		if (suffix.length())
		{
			ftm[suffix] = &ft;
		}
	}
	QStringList sl;
	// Iterate over the map types.
	for (auto ft: ftm)
	{
		// Format the filetypes as a open dialog filter.
		sl.append(QString("%1 (*.%2)").arg(ft->getName()).arg(ft->getSuffix()));
	}
	// Sort the entries by name.
	std::sort(sl.begin(), sl.end());
	// Add all files.
	sl.prepend(tr("All Files").append("(*)"));
	// Add separators between the list entries in the final string.
	return sl.join(";;");
}

MultiDocInterface* AppModuleInterface::createChild(QWidget* parent) const
{
	auto widget = createWidget(parent);
	if (widget)
	{
		widget->_module = this;
	}
	return widget;
}

}
