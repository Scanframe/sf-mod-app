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
		explicit InterfaceListModel(bool file_only, QObject* parent = nullptr);

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

		QList<AppModuleInterface*> _list;
};

InterfaceListModel::InterfaceListModel(bool file_only, QObject* parent)
	:QAbstractListModel(parent)
{
	if (file_only)
	{
		for (auto entry: AppModuleInterface::getMap())
		{
			if (entry->hasFileTypes())
			{
				_list.append(entry);
			}
		}
	}
	else
	{
		// No restrictions so copy the map.
		_list = AppModuleInterface::getMap().values();
	}
}

QVariant InterfaceListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return {};
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
	return static_cast<int>(_list.count());
}

QVariant InterfaceListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return {};
	}
	if (index.row() >= _list.size())
	{
		return {};
	}
	auto entry = _list.at(index.row());
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
	return {};
}

int InterfaceListModel::columnCount(const QModelIndex& parent) const
{
	// Return the amount of columns to display.
	return cMaxColumns;
}

SF_IMPL_IFACE(AppModuleInterface, AppModuleInterface::Parameters, Interface)

AppModuleInterface::Map AppModuleInterface::_map;

AppModuleInterface::OpenFileClosure AppModuleInterface::callbackOpenFile;

AppModuleInterface::AppModuleInterface(const AppModuleInterface::Parameters& parameters)
	:QObject(parameters._parent)
	 , _settings(parameters._settings)
	 , _initializeStage(InitializeStage::Uninitialize)
{
}

AppModuleInterface::~AppModuleInterface() = default;

void AppModuleInterface::instantiate(QSettings* settings, QObject* parent)
{
	// Iterate through the available registered implementation names.
	for (auto& nm: Interface().getNames())
	{
		auto name = QString::fromStdString(nm);
		// Check if not created yet.
		if (!_map.contains(name))
		{
			_map[name] = AppModuleInterface::Interface().create(nm, Parameters(settings, parent));
		}
	}
}

size_t AppModuleInterface::initializeInstances(InitializeStage stage)
{
	size_t rv = 0;
	for (auto i: _map.values())
	{
		// Check if (next) stage is allowed or not done yet.
		if (stage != i->_initializeStage && (stage > i->_initializeStage || stage == InitializeStage::Uninitialize))
		{
			// Set the initialized member to prevent a second pass of the same value.
			i->_initializeStage = stage;
			// Call possible overloaded method.
			i->initialize(stage);
			// Increment the return value.
			rv++;
		}
	}
	return rv;
}

const AppModuleInterface::Map& AppModuleInterface::getMap()
{
	return _map;
}

AppModuleInterface* AppModuleInterface::selectDialog(const QString& title, QSettings* settings, QWidget* parent)
{
	SelectImplementationDialog dlg(settings, true, parent);
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

QAbstractItemModel* AppModuleInterface::getListModel(bool file_only, QObject* parent)
{
	return new InterfaceListModel(file_only, parent);
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
		// Format the filetypes as open dialog filter.
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
		// Format the filetypes as open dialog filter.
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
	if (auto child = createWidget(parent))
	{
		// Assign this module for access to configuration.
		child->_module = this;
		return child;
	}
	return nullptr;
}

void AppModuleInterface::addAllPropertyPages(PropertySheetDialog* sheet)
{
	// Iterate through the modules and add
	for (auto m: _map)
	{
		m->addPropertyPages(sheet);
	}
}

QSettings* AppModuleInterface::getSettings() const
{
	return _settings;
}

MultiDocInterface* AppModuleInterface::createWidget(QWidget* parent) const
{
	return nullptr;
}

bool AppModuleInterface::hasFileTypes() const
{
	return !_fileTypes.isEmpty();
}

MultiDocInterface* AppModuleInterface::openFile(const QString& filename, AppModuleInterface* ami) const
{
	// Check if the closure is assigned.
	if (callbackOpenFile)
	{
		// Make the call and return the interface implementation.
		return callbackOpenFile(filename, ami);
	}
	else
	{
		qWarning() << "Callback to open a file has not been assigned.";
	}
	return nullptr;
}

AppModuleInterface::DockWidgetList AppModuleInterface::createAllDockingWidgets(QWidget* parent)
{
	AppModuleInterface::DockWidgetList rv;
	// Iterate through the modules and add
	for (auto m: _map)
	{
		rv.append(m->createDockingWidgets(parent));
	}
	return rv;
}

QList<QDockWidget*> AppModuleInterface::createDockingWidgets(QWidget* parent)
{
	return {};
}

void AppModuleInterface::documentActivated(MultiDocInterface* iface, bool yn) const
{
}

void AppModuleInterface::addMenuItems(AppModuleInterface::MenuType menuType, QMenu* menu)
{
}

void AppModuleInterface::addAllMenuItems(AppModuleInterface::MenuType menuType, QMenu* menu)
{
	// Sanity check.
	if (menu)
	{
		// Iterate through the modules and add the menus of each module.
		for (auto m: _map)
		{
			m->addMenuItems(menuType, menu);
		}
	}
}

void AppModuleInterface::addToolBars(QMainWindow* mainWindow)
{
}

void AppModuleInterface::addAllToolBars(QMainWindow* mainWindow)
{
	// Iterate through the modules and add
	for (auto m: _map)
	{
		m->addToolBars(mainWindow);
	}
}

}
