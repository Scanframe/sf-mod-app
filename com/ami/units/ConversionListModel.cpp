#include <QMetaEnum>
#include <QInputDialog>
#include <QMessageBox>
#include "ConversionListModel.h"
#include "ConversionDialog.h"

namespace sf
{

class ConversionListModel::Adapter
{
	public:
		Adapter();

		[[nodiscard]] UnitConversionEvent& getEvent(const Entry& entry);

		void setEntry(Entry& entry) const;

	private:
		UnitConversionEvent _ev;
		double _multiplier{1.0};
		double _offset{0.0};
		std::string _to_unit;
		int _to_precision{0};
};

ConversionListModel::Adapter::Adapter()
	 :_ev({}, {}, 0, _multiplier, _offset, _to_unit, _to_precision)
{
}

UnitConversionEvent& ConversionListModel::Adapter::getEvent(const Entry& entry)
{
	_ev._option = entry._option.toStdString();
	_ev._from_unit = entry._from_unit.toStdString();
	_ev._from_precision = entry._from_precision;
	_ev._offset = entry._offset;
	_ev._multiplier = entry._multiplier;
	_ev._to_unit = entry._to_unit.toStdString();
	_ev._to_precision = entry._to_precision;
	return _ev;
}

void ConversionListModel::Adapter::setEntry(Entry& entry) const
{
	entry._option = QString::fromStdString(_ev._option);
	entry._from_unit = QString::fromStdString(_ev._from_unit);
	entry._from_precision = _ev._from_precision;
	entry._offset = _ev._offset;
	entry._multiplier = _ev._multiplier;
	entry._to_unit = QString::fromStdString(_ev._to_unit);
	entry._to_precision = _ev._to_precision;
}

namespace
{

enum EColumn
{
	cFromUnit = 0,
	cFromPrecision,
	cMultiplier,
	cOffset,
	cToUnit,
	cToPrecision,
	cMaxColumns
};

}

ConversionListModel::ConversionListModel(UnitConversionServerEx* ucs, QObject* parent)
	:QAbstractListModel(parent)
	 , _ucs(ucs)
{
	update();
}

void ConversionListModel::update()
{
	// Empty the list before filling.
	_list.clear();
	// Select the section to get the key value map.
	if (_ucs->getProfile().selectSection(UnitConversionServerEx::getUnitSystemName(_ucs->getUnitSystem())))
	{
		for (auto& i: _ucs->getProfile().getMap())
		{
			Entry entry;
			strings sl;
			sl.split(i.first, ',');
			sl.split(i.second, ',');
			for (strings::size_type idx = 0; idx < sl.size(); idx++)
			{
				switch (idx)
				{
					default:
						break;
					case 0:
						entry._from_unit = QString::fromStdString(sl[idx]);
						break;
					case 1:
						entry._from_precision = std::stoi(sl[idx]);
						break;
					case 2:
						entry._to_unit = QString::fromStdString(sl[idx]);
						break;
					case 3:
						entry._multiplier = std::stod(sl[idx]);
						break;
					case 4:
						entry._offset = std::stod(sl[idx]);
						break;
					case 5:
						entry._to_precision = std::stoi(sl[idx]);
						break;
				}
			}
			_list.append(entry);
		}
	}
}

int ConversionListModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(_list.size());
}

int ConversionListModel::columnCount(const QModelIndex& parent) const
{
	return static_cast<int>(cMaxColumns);
}

void ConversionListModel::refresh()
{
	beginRemoveRows(QModelIndex(), 0, std::numeric_limits<int>::max());
	endRemoveRows();
	beginInsertRows(QModelIndex(), 0, -1);
	// Find current selected section.
	auto section = _ucs->getProfile().findSection(UnitConversionServerEx::getUnitSystemName(_ucs->getUnitSystem()));
	// When found fill the map.
	if (section != IniProfile::npos)
	{
		// Assign a new map.
		update();
	}
	else
	{
		// Clear the current map.
		_list.clear();
	}
	insertRows(0, static_cast<int>(_list.size()));
	endInsertRows();
	// Reset the dirty flag.
	_dirty = false;
}

QVariant ConversionListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case cFromUnit:
				return QString(tr("Unit"));
			case cFromPrecision:
				return QString(tr("Precision"));
			case cToUnit:
				return QString(tr("To Unit"));
			case cToPrecision:
				return QString(tr("To Precision"));
			case cMultiplier:
				return QString(tr("Multiplier"));
			case cOffset:
				return QString(tr("Offset"));
			default:
				return QString("Field %1").arg(section);
		}
	}
	return {};
}

Qt::ItemFlags ConversionListModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::ItemFlag::NoItemFlags;
	}
	//
	return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemNeverHasChildren;
}

QVariant ConversionListModel::data(const QModelIndex& index, int role) const
{
	// Display the value.
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		if (!index.isValid() || index.row() >= _list.size())
		{
			return {};
		}
		auto entry = *std::next(_list.begin(), index.row());
		switch (index.column())
		{
			case cFromUnit:
				return entry._from_unit;

			case cFromPrecision:
				return entry._from_precision;

			case cToUnit:
				return entry._to_unit;

			case cToPrecision:
				return entry._to_precision;

			case cMultiplier:
				return entry._multiplier;

			case cOffset:
				return entry._offset;
		}
		return "??";
	}
	else
	{
		return {};
	}
}

bool ConversionListModel::add()
{
	return edit({});
}

bool ConversionListModel::edit(QModelIndex index)
{
	// Check edit or add mode on validity of index.
	bool edit = index.isValid();
	Entry entry;
	if (edit)
	{
		entry = _list[index.row()];
	}
	Adapter a;
	while (ConversionDialog(qobject_cast<QWidget*>(parent())).execute(a.getEvent(entry), false))
	{
		// Apply the made change to the entry.
		a.setEntry(entry);
		// Find entry with the same from unit en precision.
		auto it = std::find_if(_list.begin(), _list.end(), [&](const Entry& e)->bool
		{
			return entry._from_unit == e._from_unit && entry._from_precision == e._from_precision;
		});
		// Check if the entry  already exists.
		bool flag = it != _list.end();
		if (flag && edit)
		{
			// When entry is itself.
			flag = it != std::next(_list.begin(), index.row());
		}
		if (flag)//(edit ? (it != std::next(_list.begin(), index.row())) : (it != _list.end()))
		{
			QMessageBox::information(dynamic_cast<QWidget*>(parent()), tr("Unit Conversion"),
				tr("Unit and precision combination (%1,%2) already exist!").arg(entry._from_unit).arg(entry._from_precision));
			continue;
		}
		// Set the dirty flag before an event is going to be send from the list modification.
		_dirty = true;
		if (edit)
		{
			// Update the row.
			_list[index.row()] = entry;
			// Notify the list of a change.
			dataChanged(index, index);
		}
		else
		{
			// Always append.
			auto pos = static_cast<int>(_list.size());
			beginInsertRows(QModelIndex(), pos, pos);
			_list.append(entry);
			insertRow(pos);
			endInsertRows();
		}
		return true;
	}
	return false;
}

bool ConversionListModel::remove(QModelIndex index)
{
	if (index.isValid())
	{
		// Set the dirty flag before an event is going to be send from the list modification.
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

void ConversionListModel::sync()
{
	if (_dirty)
	{
		// Remove existing section keys.
		if (_ucs->getProfile().selectSection(UnitConversionServerEx::getUnitSystemName(_ucs->getUnitSystem())))
		{
			_ucs->getProfile().removeKeys();
		}
		// Sort the list first for readability.
		std::sort(_list.begin(), _list.end(), [](const Entry& a, const Entry& b) -> bool
		{
			if (a._from_unit < b._from_unit)
			{
				return true;
			}
			if (a._from_unit == b._from_unit && a._from_precision < b._from_precision)
			{
				return true;
			}
			return false;
		});
		// Enter the fresh ones.
		for (auto& e: _list)
		{
			_ucs->setConversion(Adapter().getEvent(e));
		}
	}
	_dirty = false;
}

}
