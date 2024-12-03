#include "VariableListModel.h"
#include "misc/gen/dbgutils.h"
#include "misc/gen/string.h"
#include <QAbstractItemView>
#include <QFormLayout>
#include <QLineEdit>
#include <QMetaEnum>
#include <bitset>
#include <misc/qt/CommonItemDelegate.h>
#include <misc/qt/ObjectExtension.h>

namespace sf
{

namespace
{

enum EColumn
{
	cId = 0,
	cName,
	cValue,
	cUnit,
	cFlags,
	cMaxColumns
};

QVariant toQVariant(const Value& v)
{
	QVariant rv;
	switch (v.getType())
	{
		case Value::vitInteger:
			return QVariant::fromValue(v.getInteger());

		case Value::vitFloat:
			return QVariant::fromValue(v.getFloat());

		case Value::vitString:
		default:
			return QVariant::fromValue(QString::fromStdString(v.getString()));
	}
}

Value fromQVariant(const QVariant& v, Value::EType type)
{
	switch (type)
	{
		case Value::vitInteger:
			return Value(v.value<Value::int_type>());

		case Value::vitFloat:
			return Value(v.value<Value::flt_type>());

		case Value::vitString:
		default:
			return Value(v.value<std::string>());
	}
}

CommonItemDelegate::OptionsType getStateOptions(const Variable& var)
{
	CommonItemDelegate::OptionsType rv;
	for (auto& s: var.getStates())
	{
		rv.append({toQVariant(s._value), QString::fromStdString(s._name)});
	}
	return rv;
}

}// namespace

VariableListModel::VariableListModel(QObject* parent)
	: QAbstractListModel(parent)
{
}

int VariableListModel::columnCount(const QModelIndex& parent) const
{
	return static_cast<int>(cMaxColumns);
}

void VariableListModel::setDelegates(QAbstractItemView* view)
{
	auto cid = new CommonItemDelegate(view);
	// Propagate the signal.
	QObject::connect(cid, &CommonItemDelegate::addLineEditActions, [&](QLineEdit* lineEdit, const QModelIndex& index) {
		Q_EMIT addLineEditActions(lineEdit, _vars.at(index.row()).get());
	});
	view->setItemDelegate(cid);
}

void VariableListModel::refresh()
{
	// Seems this removal causes a problem.
	// beginRemoveRows(QModelIndex(), 0, std::numeric_limits<int>::max());
	// endRemoveRows();
	// A reset of the model seems also not needed here somehow.
	beginResetModel();
	endResetModel();

	beginInsertRows(QModelIndex(), 0, -1);
	insertRows(0, static_cast<int>(_vars.size()));
	endInsertRows();
}

QVariant VariableListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal)
	{
		if (role == Qt::DisplayRole)
		{
			switch (section)
			{
				case cId:
					return QString(tr("Id"));
				case cName:
					return QString(tr("Name"));
				case cValue:
					return QString(tr("Value"));
				case cUnit:
					return QString(tr("Unit"));
				case cFlags:
					return QString(tr("Flags"));
				default:
					return QString("Field %1").arg(section);
			}
		}
	}
	return {};
}

Qt::ItemFlags VariableListModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::ItemFlag::NoItemFlags;
	}
	//
	Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemNeverHasChildren;
	//
	if (index.column() == EColumn::cValue)
	{
		flags |= Qt::ItemFlag::ItemIsSelectable;
		// Only editable when not read-only.
		if (!_vars.at(index.row()).get()->isReadOnly())
		{
			flags |= Qt::ItemFlag::ItemIsEditable;
		}
	}
	else if (index.column() == EColumn::cName)
	{
		flags |= Qt::ItemFlag::ItemIsUserCheckable;
	}
	return flags;
}

int VariableListModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(_vars.size());
}

QVariant VariableListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= _vars.size())
	{
		return {};
	}
	auto var = _vars.at(index.row()).get();
	//
	if (role == Qt::CheckStateRole && index.column() == cName)
	{
		return var->getData<bool>() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
	}
	// Display the value.
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		switch (index.column())
		{
			case cId:
				return QString("0x%1").arg(var->getId(), 0, 16);

			case cName:
				return QString::fromStdString(var->getName(_nameLevels));

			case cValue:
				return QString::fromStdString(var->getCurString());

			case cUnit:
				return QString::fromStdString(var->getUnit());

			case cFlags:
				return QString::fromStdString(var->getFlagsString());
		}
	}
	// Used to initialize the delegate editor.
	else if (role == Qt::ItemDataRole::EditRole)
	{
		if (index.column() == cValue)
		{
			return toQVariant(var->getCur());
		}
	}
	else if (role == Qt::ItemDataRole::ToolTipRole)
	{
		return QString::fromStdString(var->getDescription());
	}
	// Used for selection of editor type.
	else if (role == CommonItemDelegate::TypeRole)
	{
		if (index.column() == cValue)
		{
			if (!var->isReadOnly())
			{
				if (var->getStateCount())
				{
					return CommonItemDelegate::etDropDownIndex;
				}
				else if (var->getType() == Value::vitInteger)
				{
					return CommonItemDelegate::etSpinBox;
				}
				else if (var->getType() == Value::vitFloat)
				{
					return CommonItemDelegate::etDoubleSpinBox;
				}
				return CommonItemDelegate::etEdit;
			}
		}
	}
	// Used for editor's selectable options.
	if (role == CommonItemDelegate::OptionsRole)
	{
		if (index.column() == cValue)
		{
			if (var->getStateCount())
			{
				return QVariant::fromValue(getStateOptions(*var));
			}
		}
	}
	if (role == CommonItemDelegate::MinimumRole)
	{
		if (index.column() == cValue)
		{
			return toQVariant(var->getMin());
		}
	}
	if (role == CommonItemDelegate::MaximumRole)
	{
		if (index.column() == cValue)
		{
			return toQVariant(var->getMax());
		}
	}
	if (role == CommonItemDelegate::IncrementRole)
	{
		if (index.column() == cValue)
		{
			return toQVariant(var->getRnd());
		}
	}
	return {};
}

bool VariableListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole)
	{
		auto var = _vars.at(index.row()).get();
		if (index.column() == cValue)
		{
			if (var->getStateCount())
			{
				if (var->setCur(Value(value.toInt())))
				{
					Q_EMIT changed(var);
				}
			}
			else
			{
				if (var->setCur(Value(value.toString())))
				{
					Q_EMIT changed(var);
				}
			}
		}
		dataChanged(index, index);
		return true;
	}
	return false;
}

void VariableListModel::addVariable(const Variable* var)
{
	_vars.append(std::make_unique<Variable>(*var));
}

void VariableListModel::addVariables(const InformationTypes::Vector& list)
{
	for (auto ib: list)
	{
		auto var = dynamic_cast<const sf::Variable*>(ib);
		if (var)
		{
			addVariable(var);
		}
	}
}

}// namespace sf
