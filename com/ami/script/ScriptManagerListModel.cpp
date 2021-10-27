#include <QMetaEnum>
#include "ScriptManagerListModel.h"
#include <misc/qt/CommonItemDelegate.h>

namespace sf
{

namespace
{

enum EColumn
{
	vcName = 0,
	cScriptState,
	cBackground,
	cShortcut,
	cGlobal,
	cFilename,
	cMaxColumns
};

}

ScriptManagerListModel::ScriptManagerListModel(ScriptManager* manager, QObject* parent)
	:QAbstractListModel(parent)
	 , _manager(manager)
{
}

void ScriptManagerListModel::setDelegates(QAbstractItemView* view)
{
	view->setItemDelegate(new CommonItemDelegate(view));
}

void ScriptManagerListModel::refresh()
{
	beginRemoveRows(QModelIndex(), 0, std::numeric_limits<int>::max());
	endRemoveRows();
	beginInsertRows(QModelIndex(), 0, -1);
	insertRows(0, static_cast<int>(_manager->_list.size()));
	endInsertRows();
}

QVariant ScriptManagerListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case vcName:
				return QString(tr("Name"));
			case cScriptState:
				return QString(tr("State"));
			case cBackground:
				return QString(tr("BkGnd"));
			case cShortcut:
				return QString(tr("Shortcut"));
			case cGlobal:
				return QString(tr("Global"));
			case cFilename:
				return QString(tr("Filename"));
			default:
				return QString("Field %1").arg(section);
		}
	}
	return {};
}

Qt::ItemFlags ScriptManagerListModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::ItemFlag::NoItemFlags;
	}
	//
	Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable |
		Qt::ItemFlag::ItemNeverHasChildren;
	//
	if (index.column() != EColumn::cScriptState)
	{
		flags |= Qt::ItemFlag::ItemIsEditable;
	}
	return flags;
}

int ScriptManagerListModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(_manager->_list.size());
}

QVariant ScriptManagerListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return {};
	}
	if (index.row() >= _manager->_list.size())
	{
		return {};
	}
	auto entry = _manager->_list.at(index.row());
	// Display the value.
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		switch (index.column())
		{
			case vcName:
				return entry->getDisplayName();
			case cScriptState:
				return entry->getStateName();
			case cBackground:
				return QString(QMetaEnum::fromType<ScriptEntry::EBackgroundMode>().valueToKey(entry->getBackgroundMode())).remove(0, 2);
			case cShortcut:
				return entry->getKeySequence();
			case cGlobal:
				return entry->isGlobal() ? tr("Yes") : tr("No");
			case cFilename:
				return entry->getFilename();
		}
	}
	// Used to initialize the delegate editor.
	if (role == Qt::ItemDataRole::EditRole)
	{
		switch (index.column())
		{
			case vcName:
				return entry->getDisplayName();
			case cScriptState:
				return entry->getStateName();
			case cBackground:
				return entry->getBackgroundMode();
			case cShortcut:
				return entry->getKeySequence();
			case cGlobal:
				return entry->isGlobal();
			case cFilename:
				return entry->getFilename();
		}
	}
	// Used for selection of editor type.
	if (role == CommonItemDelegate::TypeRole)
	{
		switch (index.column())
		{
			case vcName:
				return CommonItemDelegate::etEdit;
			case cBackground:
				return CommonItemDelegate::etDropDownIndex;
			case cShortcut:
				return CommonItemDelegate::etShortcut;
			case cGlobal:
				return CommonItemDelegate::etDropDownIndex;
			case cFilename:
				return CommonItemDelegate::etDropDown;
			default:
				return CommonItemDelegate::etDefault;
		}
	}
	// Used for editor's selectable options.
	if (role == CommonItemDelegate::OptionsRole)
	{
		switch (index.column())
		{
			case vcName:
				return entry->getDisplayName();
			case cScriptState:
				return entry->getStateName();
			case cBackground:
			{
				QStringList sl;
				auto meta = QMetaEnum::fromType<ScriptEntry::EBackgroundMode>();
				for (auto bm: {ScriptEntry::bmNo, ScriptEntry::bmOnce, ScriptEntry::bmContinuous})
				{
					sl.append(QString(meta.valueToKey(bm)).remove(0, 2));
				}
				return sl;
			}
			case cShortcut:
				return entry->getKeySequence();
			case cGlobal:
				return QStringList({"No", "Yes"});
			case cFilename:
			{
				QStringList files = _manager->getFilenames();
				// Add the current file name when not in the list.
				if (!files.contains(entry->getFilename()))
				{
					files.prepend(entry->getFilename());
				}
				return files;
			}
		}
	}
	return {};
}

bool ScriptManagerListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole)
	{
		auto& entry = _manager->_list[index.row()];
		switch (index.column())
		{
			case vcName:
				entry->setDisplayName(value.toString());
				break;
			case cBackground:
				entry->setBackgroundMode(value.value<ScriptEntry::EBackgroundMode>());
				break;
			case cShortcut:
				entry->setKeySequence(value.value<QKeySequence>());
				break;
			case cGlobal:
				entry->setGlobal(value.toBool());
				break;
			case cFilename:
				entry->setFilename(value.toString());
		}
		this->dataChanged(index, index);
		return true;
	}
	return false;
}

int ScriptManagerListModel::columnCount(const QModelIndex& parent) const
{
	return static_cast<int>(cMaxColumns);
}

void ScriptManagerListModel::startAll()
{
	_manager->start(-1);
}

void ScriptManagerListModel::start(QModelIndex index)
{
	if (index.isValid())
	{
		_manager->start(index.row());
	}
}

void ScriptManagerListModel::stop(QModelIndex index)
{
	if (index.isValid())
	{
		_manager->stop(index.row());
	}
}

void ScriptManagerListModel::add(QModelIndex index)
{
	// When in valid prepend the new entry.
	auto pos = index.isValid() ? index.row() + 1 : 0;
	beginInsertRows(QModelIndex(), pos, pos);
	_manager->addAt(pos);
	insertRow(pos);
	endInsertRows();
}

void ScriptManagerListModel::remove(QModelIndex index)
{
	if (index.isValid())
	{
		auto pos = index.row();
		beginRemoveRows(QModelIndex(), pos, pos);
		removeRow(pos);
		_manager->remove(pos);
		endRemoveRows();
	}
}

}
