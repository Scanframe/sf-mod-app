#include <misc/qt/qt_utils.h>
#include <gen/Variable.h>

#include <utility>
#include <misc/qt/Resource.h>
#include <misc/qt/ObjectExtension.h>
#include <QSplitter>
#include "ObjectHierarchyModel.h"

namespace sf
{

ObjectHierarchyModel::TreeItem::TreeItem(ObjectHierarchyModel::TreeItem* parent, QObject* obj)
	:_parentItem(parent)
	 , _object(obj)
{
	if (parent)
	{
		parent->_childItems.append(this);
	}
}

ObjectHierarchyModel::TreeItem::~TreeItem()
{
	qDeleteAll(_childItems);
}

namespace
{
enum
{
	vcName,
	vcType,
	cLayout,
	vcColumnCount
};
}

ObjectHierarchyModel::ObjectHierarchyModel(bool multi, QObject* parent)
	:QAbstractItemModel(parent)
	 , _rootItem(new TreeItem(nullptr, nullptr))
	 , _multi(multi)
{
	_iconForm = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Form), QPalette::ColorRole::Mid);
	_iconContainer = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Container), QPalette::ColorRole::Mid);
	_iconWidget = Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Widget), QPalette::ColorRole::Mid);
}

ObjectHierarchyModel::~ObjectHierarchyModel()
{
	delete _rootItem;
}

QModelIndex ObjectHierarchyModel::index(int row, int column, const QModelIndex& parent) const
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

QModelIndex ObjectHierarchyModel::parent(const QModelIndex& child) const
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

QVariant ObjectHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
			default:
				return QString("Field %1").arg(section);
			case vcName:
				return QString(tr("Name"));
			case vcType:
				return QString(tr("Type"));
			case cLayout:
				return QString(tr("Layout"));
		}
	}
	return {};
}

int ObjectHierarchyModel::columnCount(const QModelIndex& parent) const
{
	return vcColumnCount;
}

int ObjectHierarchyModel::rowCount(const QModelIndex& parent) const
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

QVariant ObjectHierarchyModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return {};
	}
	if (auto item = static_cast<TreeItem*>(index.internalPointer()))
	{
		if (role == Qt::CheckStateRole)
		{
			if (_multi)
			{
				return item->_selected ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
			}
			return {};
		}
		else if (role == Qt::DisplayRole)
		{
			switch (index.column())
			{
				case vcName:
					if (!item->_object)
					{
						return {};
					}
					return item->_object->objectName();

				case vcType:
					if (!item->_object)
					{
						return {};
					}
					return item->_object->metaObject()->className();

				case cLayout:
					if (!item->_childItems.empty())
					{
						if (auto w = dynamic_cast<QWidget*>(item->_object))
						{
							if (w->layout())
							{
								return w->layout()->metaObject()->className();
							}
						}
					}
					return {};
			}
		}
		else if (role == Qt::DecorationRole && !index.column())
		{
			if (item->_parentItem == _rootItem)
			{
				return _iconForm;
			}
			if (!item->_childItems.empty())
			{
				return _iconContainer;
			}
			return _iconWidget;
		}
	}
	return {};
}

Qt::ItemFlags ObjectHierarchyModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return QAbstractItemModel::flags(index);
	}
	//
	Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemIsSelectable;
	//
	if (_multi)
	{
		flags |= Qt::ItemIsUserCheckable;
	}
	return flags;
}

void ObjectHierarchyModel::addChild(QObject* obj, TreeItem* parent) // NOLINT(misc-no-recursion)
{
	// TODO: Ignoring "QSplitterHandle" is not structural coded.
	// When not an extension derived class.
	if (dynamic_cast<QWidget*>(obj) && !dynamic_cast<QSplitterHandle*>(obj))
	{
		auto item = new TreeItem(parent, obj);
		if (!dynamic_cast<ObjectExtension*>(obj))
		{
			for (auto child: obj->children())
			{
				addChild(child, item);
			}
		}
	}
};

void ObjectHierarchyModel::updateList(QObject* root)
{
	addChild(root, _rootItem);
}

void ObjectHierarchyModel::toggleSelection(const QModelIndex& index)
{
	if (!_multi)
	{
		return;
	}
	auto item = static_cast<TreeItem*>(index.internalPointer());
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
	dataChanged(index.siblingAtColumn(0), index.siblingAtColumn(0), {Qt::CheckStateRole});
}

QObject* ObjectHierarchyModel::getObject(QModelIndex index) const
{
	if (index.isValid())
	{
		if (auto item = static_cast<TreeItem*>(index.internalPointer()))
		{
			return item->_object;
		}
	}
	return nullptr;
}

}
