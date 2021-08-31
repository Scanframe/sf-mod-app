#include <misc/qt/qt_utils.h>
#include <gen/Variable.h>

#include <utility>
#include <misc/qt/Resource.h>
#include <gen/ResultData.h>
#include "InformationItemModel.h"

namespace sf
{

InformationItemModel::TreeItem::TreeItem(InformationItemModel::TreeItem* parent, QString name)
	:_parentItem(parent)
	 , _name(std::move(name))
{
	if (parent)
	{
		parent->_childItems.append(this);
	}
}

InformationItemModel::TreeItem::~TreeItem()
{
	qDeleteAll(_childItems);
}

QStringList InformationItemModel::TreeItem::getNamePath() const
{
	QStringList sl;
	auto item = this;
	while (item)
	{
		sl.prepend(item->_name);
		item = item->_parentItem;
	}
	return sl;
}

namespace
{
enum
{
	vcName,
	vcId,
	vcType,
	vcUnit,
	vcFlags,
	vcValue,
	vcDescription,
	vcColumnCount
};

enum
{
	rcName,
	rcId,
	rcType,
	rcTypeSize,
	rcBlockSize,
	rcFlags,
	rcDescription,
	rcColumnCount
};
}

InformationItemModel::InformationItemModel(Gii::SelectionMode mode, Gii::TypeId idType, QObject* parent)
	:QAbstractItemModel(parent)
	 , _rootItem(new TreeItem(nullptr, "Root"))
	 , _mode(mode)
	 , _idType(idType)
{
	_icons[TreeItem::dtFolder] = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Folder), QPalette::ColorRole::Mid);
	_icons[TreeItem::dtVariable] = Resource::getSvgIcon(":icon/svg/variable", QPalette::ColorRole::Mid);
	_icons[TreeItem::dtResultData] = Resource::getSvgIcon(":icon/svg/resultdata", QPalette::ColorRole::Mid);
}

InformationItemModel::~InformationItemModel()
{
	delete _rootItem;
}

QModelIndex InformationItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
	{
		return {};
	}
	auto parentItem = parent.isValid() ? static_cast<TreeItem*>(parent.internalPointer()) : _rootItem;
	if (auto childItem = parentItem->_childItems.at(row))
	{
		return createIndex(row, column, childItem);
	}
	return {};
}

QModelIndex InformationItemModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return {};
	}

	auto* childItem = static_cast<TreeItem*>(child.internalPointer());
	auto* parentItem = childItem->_parentItem;

	if (parentItem == _rootItem)
	{
		return {};
	}

	auto row = childItem->_parentItem ? static_cast<int>(childItem->_parentItem->_childItems.indexOf(childItem)) : 0;
	return createIndex(row, 0, parentItem);
}

QVariant InformationItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		if (_idType == Gii::Variable)
		{
			switch (section)
			{
				default:
					return QString("Field %1").arg(section);
				case vcId:
					return QString(tr("Id"));
				case vcName:
					return QString(tr("Name"));
				case vcValue:
					return QString(tr("Value"));
				case vcDescription:
					return QString(tr("Description"));
				case vcUnit:
					return QString(tr("Unit"));
				case vcType:
					return QString(tr("Type"));
				case vcFlags:
					return QString(tr("Flags"));
			}
		}
		else if (_idType == Gii::ResultData)
		{
			switch (section)
			{
				default:
					return QString("Field %1").arg(section);
				case rcId:
					return QString(tr("Id"));
				case rcName:
					return QString(tr("Name"));
				case rcFlags:
					return QString(tr("Flags"));
				case rcType:
					return QString(tr("Type"));
				case rcTypeSize:
					return QString(tr("Type Size"));
				case rcBlockSize:
					return QString(tr("Block Size"));
				case rcDescription:
					return QString(tr("Description"));
			}
		}
	}
	return {};
}

int InformationItemModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		if (auto parentItem = static_cast<TreeItem*>(parent.internalPointer()))
		{
			if (parent.row() < parentItem->_childItems.count())
			{
				if (parentItem->_childItems.at(parent.row())->_type == TreeItem::dtFolder)
				{
					return IsDebug() ? 2 : 1;
				}
			}
		}
	}
	return (_idType == Gii::Variable) ? static_cast<int>(vcColumnCount) : static_cast<int>(rcColumnCount);
}

int InformationItemModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
	{
		return 0;
	}
	TreeItem* parentItem;
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

QVariant InformationItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return {};
	}
	if (auto item = static_cast<TreeItem*>(index.internalPointer()))
	{
		if (role == Qt::CheckStateRole)
		{
			if (_mode == Gii::Multiple && index.column() == vcName && item->_type != TreeItem::dtFolder)
			{
				return item->_selected ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
			}
			return {};
		}
		else if (role == Qt::DisplayRole)
		{
			if (_idType == Gii::Variable)
			{
				switch (index.column())
				{
					case vcId:
						if (item->_type == TreeItem::dtFolder)
						{
							return item->_childItems.size();
						}
						return QString("0x%1").arg(item->_id, 0, 16);

					case vcName:
						return item->_name;

					case vcValue:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return QString::fromStdString(Variable::getInstanceById(item->_id).getCurString(true));

					case vcDescription:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return QString::fromStdString(Variable::getInstanceById(item->_id).getDescription());

					case vcUnit:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return QString::fromStdString(Variable::getInstanceById(item->_id).getUnit());

					case vcType:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return Variable::getType(Variable::getInstanceById(item->_id).getType());

					case vcFlags:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return QString::fromStdString(Variable::getInstanceById(item->_id).getFlagsString());
				}
			}
			else if (_idType == Gii::ResultData)
			{
				switch (index.column())
				{
					case rcId:
						if (item->_type == TreeItem::dtFolder)
						{
							return item->_childItems.size();
						}
						return QString("0x%1").arg(item->_id, 0, 16);

					case rcName:
						return item->_name;

					case rcType:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return ResultData::getType(ResultData::getInstanceById(item->_id).getType());

					case rcTypeSize:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return QVariant::fromValue(ResultData::getInstanceById(item->_id).getTypeSize());

					case rcBlockSize:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return QVariant::fromValue(ResultData::getInstanceById(item->_id).getBlockSize());

					case rcDescription:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return QString::fromStdString(ResultData::getInstanceById(item->_id).getDescription());

					case rcFlags:
						if (item->_type == TreeItem::dtFolder)
						{
							return {};
						}
						return QString::fromStdString(ResultData::getInstanceById(item->_id).getFlagsString());
				}
			}
		}
		else if (role == Qt::DecorationRole && !index.column())
		{
			return _icons[item->_type];
		}
	}
	return {};
}

Qt::ItemFlags InformationItemModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return QAbstractItemModel::flags(index);
	}
	//
	Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled;
	//
	auto item = static_cast<TreeItem*>(index.internalPointer());
	//
	if (item->_type != TreeItem::dtFolder)
	{
		flags |= Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
		if (_mode == Gii::Multiple)
		{
			flags |= Qt::ItemIsUserCheckable;
		}
	}
	return flags;
}

/*
void InformationItemModel::refresh()
{
	beginRemoveRows(QModelIndex(), 0, std::numeric_limits<int>::max());
	endRemoveRows();
	beginInsertRows(QModelIndex(), 0, -1);
	// Clear the current map.
	_entries.clear();
	//
	updateList();
	//
	insertRows(0, static_cast<int>(_entries.size()));
	endInsertRows();
}
*/

InformationItemModel::TreeItem* InformationItemModel::createPath(const QStringList& namePath)
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
				return *it == i->_name && (it + 1) != namePath.end() && i->_type == TreeItem::dtFolder;
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

void InformationItemModel::updateList()
{
	if (_idType == Gii::Variable)
	{
		for (auto v: Variable::getList())
		{
			QStringList namePath;
			strings sl;
			for (auto& s: sl.split(v->getName(), '|'))
			{
				namePath.append(QString::fromStdString(s));
			}
			if (auto item = createPath(namePath))
			{
				item->_type = TreeItem::dtVariable;
				item->_id = v->getId();
			}
		}
	}
	else if (_idType == Gii::ResultData)
	{
		for (auto r: ResultData::getList())
		{
			QStringList namePath;
			strings sl;
			for (auto& s: sl.split(r->getName(), '|'))
			{
				namePath.append(QString::fromStdString(s));
			}
			if (auto item = createPath(namePath))
			{
				item->_type = TreeItem::dtResultData;
				item->_id = r->getId();
			}
		}
	}
}

void InformationItemModel::toggleSelection(const QModelIndex& index)
{
	if (_mode != Gii::Single)
	{
		return;
	}
	auto item = static_cast<TreeItem*>(index.internalPointer());
	// Folders can not be selected.
	if (item->_type != TreeItem::dtFolder)
	{
		if (_selected.contains(item))
		{
			item->_selected = false;
			_selected.removeAll(item);
		}
		else
		{
			item->_selected = true;
			_selected.append(item);
		}
	}
	dataChanged(index.siblingAtColumn(0), index.siblingAtColumn(0), {Qt::CheckStateRole});
}

InformationTypes::IdVector InformationItemModel::getSelectedIds() const
{
	if (_mode == Gii::Multiple)
	{
		sf::InformationTypes::IdVector list;
		for (auto i: _selected)
		{
			list.append(i->_id);
		}
		return list;
	}
	return {};
}

bool InformationItemModel::isFolder(const QModelIndex& index)
{
	return static_cast<TreeItem*>(index.internalPointer())->_type == TreeItem::dtFolder;
}

Gii::IdType InformationItemModel::getId(const QModelIndex& index)
{
	return static_cast<TreeItem*>(index.internalPointer())->_id;
}

}