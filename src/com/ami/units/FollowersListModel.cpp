#include <QMetaEnum>
#include <QInputDialog>
#include <QMessageBox>
#include "FollowersListModel.h"
#include "FollowersDialog.h"

namespace sf
{

namespace
{

enum EColumn
{
	cMaster = 0,
	cScript,
	vcUnit,
	cFollowers,
	cMaxColumns
};

}

FollowersListModel::FollowersListModel(UnitConversionServerEx* ucs, QObject* parent)
	:QAbstractListModel(parent)
	 , _ucs(ucs)
{
	update();
}

void FollowersListModel::update()
{
	// Empty the list before filling.
	_list.clear();
	// Select the section to get the key value map.
	if (_ucs->getProfile().selectSection(_ucs->getFollowersSectionName()))
	{
		for (auto& i: _ucs->getProfile().getMap())
		{
			Entry entry;
			entry._masterId = std::stoull(i.first, nullptr, 0);
			strings sl;
			sl.split(i.second, ',', '\'');
			for (strings::size_type idx = 0; idx < sl.size(); idx++)
			{
				if (idx == 0)
				{
					entry._formula = QString::fromStdString(sl[0]);
				}
				else if (idx == 1)
				{
					entry._unit = QString::fromStdString(sl[1]);
				}
				else
				{
					entry._followerIds.append(std::stoull(sl[idx], nullptr, 0));
				}
			}
			_list.append(entry);
		}
	}
}

int FollowersListModel::rowCount(const QModelIndex& parent) const
{
	auto sz = static_cast<int>(_list.size());
	return sz;
}

int FollowersListModel::columnCount(const QModelIndex& parent) const
{
	return static_cast<int>(cMaxColumns);
}

void FollowersListModel::refresh()
{
//	beginRemoveRows(QModelIndex(), 0, std::numeric_limits<int>::max());
//	endRemoveRows();
	beginResetModel();
	endResetModel();
	beginInsertRows(QModelIndex(), 0, -1);
	update();
	insertRows(0, static_cast<int>(_list.size()));
	endInsertRows();
	// Reset the dirty flag.
	_dirty = false;
}

QVariant FollowersListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case cMaster:
				return QString(tr("Master"));
			case cScript:
				return QString(tr("Script"));
			case vcUnit:
				return QString(tr("Unit"));
			case cFollowers:
				return QString(tr("Followers"));
			default:
				return QString("Field %1").arg(section);
		}
	}
	return {};
}

Qt::ItemFlags FollowersListModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::ItemFlag::NoItemFlags;
	}
	//
	return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemNeverHasChildren;
}

QVariant FollowersListModel::data(const QModelIndex& index, int role) const
{
	// Display the value.
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		if (!index.isValid() || index.row() >= _list.size())
		{
			return {};
		}
		auto& entry(_list[index.row()]);
		switch (index.column())
		{
			case cMaster:
				return QString("0x%1").arg(entry._masterId, 0, 16);

			case cScript:
				return entry._formula;

			case vcUnit:
				return entry._unit;

			case cFollowers:
			{
				QStringList sl;
				for (auto id: entry._followerIds)
				{
					sl.append(QString("0x%1").arg(id, 0, 16));
				}
				return sl.join(',');
			}
		}
		return "??";
	}
	else
	{
		return {};
	}
}

bool FollowersListModel::add()
{
	return edit({});
}

bool FollowersListModel::edit(QModelIndex index)
{
	// Check edit or add mode on validity of index.
	if (!index.isValid())
	{
		Entry entry;
		if (FollowersDialog(qobject_cast<QWidget*>(parent())).execute(entry))
		{
			// Set the dirty flag before an event is going to be sent from the list modification.
			_dirty = true;
			// Always append.
			auto pos = static_cast<int>(_list.size());
			beginInsertRows(QModelIndex(), pos, pos);
			_list.append(entry);
			insertRow(pos);
			endInsertRows();
			return true;
		}
	}
	else
	{
		if (FollowersDialog(qobject_cast<QWidget*>(parent())).execute(_list[index.row()]))
		{
			// Set the dirty flag before an event is going to be sent from the list modification.
			_dirty = true;
			// Notify the list of a change.
			dataChanged(index, index);
			return true;
		}
	}
	return false;
}

bool FollowersListModel::remove(QModelIndex index)
{
	if (index.isValid())
	{
		// Set the dirty flag before an event is going to be sent from the list modification.
		_dirty = true;
		auto pos = index.row();
		beginRemoveRows(QModelIndex(), pos, pos);
		removeRow(pos);
		_list.erase(std::next(_list.begin(), index.row()));
		endRemoveRows();
		return true;
	}
	return false;
}

void FollowersListModel::sync()
{
	// Only when dirty sync the info.
	if (_dirty)
	{
		// Remove current section keys.
		if (_ucs->getProfile().selectSection(_ucs->getFollowersSectionName()))
		{
			_ucs->getProfile().removeKeys();
		}
		// Enter the fresh ones.
		for (auto& e: _list)
		{
			auto key = "0x" + itostr(e._masterId, 16);
			strings value;
			value.append("'" + e._formula.toStdString() + "'");
			value.append(e._unit.toStdString());
			for(auto id: e._followerIds)
			{
				value.append("0x" + itostr(id, 16));
			}
			_ucs->getProfile().setString(key, value.join(","));
		}
	}
	_dirty = false;
}

}
