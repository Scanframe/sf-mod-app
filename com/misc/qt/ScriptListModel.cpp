#include <QObject>
#include "ScriptListModel.h"
#include "../gen/ScriptInterpreter_p.h"

namespace sf
{

namespace
{

enum EInstructionColumn
{
	icIp = 0,
	icMnemonic,
	icAbsIp,
	icLine,
	icPosition,
	icScript,
	icMaxColumns
};

enum EVariableColumn
{
	vcName = 0,
	vcType,
	vcValue,
	vcMaxColumns
};

}

ScriptListModel::ScriptListModel(QObject* parent)
	:QAbstractListModel(parent)
{
}

void ScriptListModel::setInterpreter(ScriptInterpreter* interpreter, ScriptListModel::EMode mode)
{
	_interpreter = interpreter;
	_mode = mode;
}

void ScriptListModel::refresh()
{
	beginRemoveRows(QModelIndex(), 0, std::numeric_limits<int>::max());
	endRemoveRows();
	int sz = 0;
	switch (_mode)
	{
		case mInstructions:
			sz = (int) _interpreter->getInstructions().size();
			break;

		case mVariables:
			sz = (int) _interpreter->getVariables().size();
			break;

		default:
			break;
	}
	beginInsertRows(QModelIndex(), 0, (int) -1);
	insertRows(0, sz);
	endInsertRows();
}

QVariant ScriptListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return {};
	}
	//
	if (_mode == mInstructions)
	{
		if (orientation == Qt::Horizontal)
		{
			switch (section)
			{
				case icIp:
					return QString(tr("Ip"));
				case icMnemonic:
					return QString(tr("Mnem"));
				case icAbsIp:
					return QString(tr("Abs Ip"));
				case icScript:
					return QString(tr("Script"));
				case icLine:
					return QString(tr("Line"));
				case icPosition:
					return QString(tr("Pos"));
				default:
					return QString("Field %1").arg(section);
			}
		}
	}
	else if (_mode == mVariables)
	{
		switch (section)
		{
			case vcName:
				return QString(tr("Name"));
			case vcType:
				return QString(tr("Type"));
			case vcValue:
				return QString(tr("Value"));
			default:
				return QString("Field %1").arg(section);
		}
	}
	return QString("Field %1").arg(section);
}

Qt::ItemFlags ScriptListModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::ItemFlag::NoItemFlags;
	}
	//
	Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable |
		Qt::ItemFlag::ItemNeverHasChildren;
	//
	switch (_mode)
	{
		case mInstructions:
			if (index.column() == icScript)
			{
				flags |= Qt::ItemFlag::ItemIsSelectable;
			}
			break;

		case mVariables:
			if (index.column() == vcValue)
			{
				flags |= Qt::ItemFlag::ItemIsEditable;
			}
			break;

		default:
			break;
	}
	return flags;
}

int ScriptListModel::rowCount(const QModelIndex& parent) const
{
	if (!_interpreter)
	{
		return 0;
	}
	switch (_mode)
	{
		case mInstructions:
			return (int) _interpreter->getInstructions().size();

		case mVariables:
			return (int) _interpreter->getVariables().size();

		default:
			return 0;
	}
}

QVariant ScriptListModel::data(const QModelIndex& index, int role) const
{
	// Sanity check.
	if (!index.isValid() || !_interpreter)
	{
		return {};
	}
	switch (_mode)
	{
		case mInstructions:
		{
			auto& iis = _interpreter->getInstructions();
			if (index.row() >= iis.size())
			{
				return {};
			}
			if (role == Qt::ItemDataRole::DisplayRole)
			{
				switch (index.column())
				{
					case icIp:
						return index.row();
					case icMnemonic:
						return QString::fromStdString(iis.at(index.row()).getMnemonic());
					case icAbsIp:
					{
						auto ip = iis.at(index.row()).getJumpIp();
						if (ip >= 0)
						{
							return (int) ip;
						}
						else
						{
							return "-";
						}
					}
					case icLine:
						return (int) iis.at(index.row())._codePos._line;
					case icPosition:
						return (int) iis.at(index.row())._codePos._offset;
					case icScript:
						return QString::fromStdString(iis.at(index.row())._script);
				}
			}
			break;
		}

		case mVariables:
		{
			auto& ids = _interpreter->getVariables();
			if (index.row() >= ids.size())
			{
				return {};
			}
			if (role == Qt::ItemDataRole::DisplayRole)
			{
				switch (index.column())
				{
					case vcName:
						return QString::fromStdString(ids.at(index.row())->_name);
					case vcType:
					{
						auto& value = ids.at(index.row())->_value;
						// Custom types are script objects.
						if (value.getType() == Value::EType::vitCustom)
						{
							if (auto obj = _interpreter->castToObject(value))
							{
								return QString::fromStdString(ScriptObject::Interface().getRegisterName(obj));
							}
						}
						return Value::getType(value.getType());
					}
					case vcValue:
						return QString::fromStdString(ids.at(index.row())->_value.getString());
				}
			}
			else if (role == Qt::EditRole)
			{
				return QString::fromStdString(_interpreter->getVariables().at(index.row())->_value.getString());
			}
			break;
		}
	}
	return {};
}

int ScriptListModel::columnCount(const QModelIndex& parent) const
{
	int rv = 0;
	// Return the amount of columns to display.
	switch (_mode)
	{
		case mInstructions:
			rv = icMaxColumns;
			break;

		case mVariables:
			rv = vcMaxColumns;
			break;
	}
	return rv;
}

bool ScriptListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (_interpreter && role == Qt::EditRole)
	{
		if (_interpreter->getVariables().at(index.row())->_value.getType() != Value::vitCustom)
		{
			_interpreter->getVariables().at(index.row())->_value.assign(value.toString().toStdString());
			this->dataChanged(index, index);
			return true;
		}
	}
	return false;
}

}
