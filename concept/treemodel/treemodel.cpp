#include <QStringList>
#include "treemodel.h"

TreeModel::TreeItem::TreeItem(TreeItem* parent, const QString& name)
	:_parentItem(parent)
{
	if (_parentItem)
	{
		_parentItem->_childItems.append(this);
	}
	_name = name;
}

TreeModel::TreeItem::~TreeItem()
{
	qDeleteAll(_childItems);
}

TreeModel::TreeModel(QObject* parent)
	:QAbstractItemModel(parent)
	,_rootItem(new TreeItem(nullptr, "Root"))
{
}

TreeModel::~TreeModel()
{
	delete _rootItem;
}

int TreeModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant TreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || role != Qt::DisplayRole)
	{
		return {};
	}
	auto item = static_cast<TreeItem*>(index.internalPointer());
	return item->_name;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::NoItemFlags;
	}

	return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return QString("Column %1").arg(section);
	}

	return {};
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
	{
		return {};
	}
	auto parentItem = parent.isValid() ? static_cast<TreeItem*>(parent.internalPointer()) : _rootItem;
	auto childItem = parentItem->_childItems.at(row);
	if (childItem)
	{
		return createIndex(row, column, childItem);
	}
	return {};
}

QModelIndex TreeModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return {};
	}
	auto childItem = static_cast<TreeItem*>(child.internalPointer());
	auto parentItem = childItem->_parentItem;
	if (parentItem == _rootItem)
	{
		return {};
	}
	auto row = childItem->_parentItem->_parentItem->_childItems.indexOf(childItem->_parentItem);
	return createIndex((int)row, 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex& parent) const
{
	TreeItem* parentItem;
	if (parent.column() > 0)
	{
		return 0;
	}
	if (!parent.isValid())
	{
		parentItem = _rootItem;
	}
	else
	{
		parentItem = static_cast<TreeItem*>(parent.internalPointer());
	}
	return static_cast<int>(parentItem->_childItems.count());
}

TreeModel::TreeItem* TreeModel::createPath(const QStringList& namePath)
{
	// When not empty.
	if (!namePath.empty())
	{
		auto cur = _rootItem;
		// Iterate through the name path names.
		for (auto it = namePath.begin(); it != namePath.end(); it++)
		{
			// Find the name in the child list.
			auto found = std::find_if(cur->_childItems.begin(), cur->_childItems.end(), [&](const TreeItem* i) -> bool
			{
				return *it == i->_name;
			});
			// When found.
			if (found != cur->_childItems.end())
			{
				// Restart the loop using the found item when not at the end.
				if (it + 1 != namePath.end())
				{
					cur = *found;
					continue;
				}
				else
				{
					// When at the end of the list return the found item.
					return *found;
				}
			}
			else
			{
				// Create new items for the last part of the name list.
				for (auto itn = it; itn != namePath.end(); itn++)
				{
					cur = new TreeItem(cur, *itn);
				}
				return cur;
			}
		}
	}
	return nullptr;
}

void TreeModel::update()
{
	static const char* TreeData = R"(Storage|Filename
Storage|Sub Directory
Storage|Directory
Storage|File Number
Storage|Compression
Storage|Disk Space
Storage|Logfile Path
Eddy Current|Channels
Eddy Current|Amplitude|Unit
Project|Sound Velocity|Medium
Project|Sound Velocity|Material
Project|System|Time
Project|System|Date
Project|Ultrasonic|Threshold
Project|Eddy Current|Peak-to-Peak|Threshold
Project|Binder|Medium Velocity
Project|Project
Project|Inspection
Project|Medium
)";
	for (auto& line: QString(TreeData).split('\n'))
	{
		createPath(line.split('|'));
	}
}
