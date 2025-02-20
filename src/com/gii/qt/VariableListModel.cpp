#include "VariableListModel.h"
#include <QAbstractItemView>
#include <QFileDialog>
#include <QLineEdit>
#include <QMetaEnum>
#include <misc/gen/dbgutils.h>
#include <misc/gen/string.h>
#include <misc/qt/CommonItemDelegate.h>
#include <misc/qt/ObjectExtension.h>
#include <misc/qt/Resource.h>

namespace sf
{
namespace
{

enum EColumn
{
	cId,
	cName,
	cValue,
	cUnit,
	cMaxColumns
};

// Columns not being shown.
enum EColumnDisabled
{
	//cId = -1000,
	cFlags = -1001
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

/*
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
*/

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
{}

int VariableListModel::columnCount(const QModelIndex& parent) const
{
	return cMaxColumns;
}

void VariableListModel::setDelegates(QAbstractItemView* view)
{
	const auto cid = new CommonItemDelegate(view);
	// Add actions to the few.
	connect(cid, &CommonItemDelegate::addLineEditActions, [&](QLineEdit* line_edit, const QModelIndex& index) {
		const auto var = _varList.at(index.row()).get();
		auto type = var->getStringType();
		QAction* action{nullptr};
		if (type == stPath)
		{
			action = line_edit->addAction(Resource::getSvgIcon(":icon/svg/file", line_edit->palette(), QPalette::Text), QLineEdit::TrailingPosition);
		}
		else if (type == stDirectory)
		{
			action = line_edit->addAction(Resource::getSvgIcon(":icon/svg/folder", line_edit->palette(), QPalette::Text), QLineEdit::TrailingPosition);
		}
		if (action != nullptr)
		{
			connect(action, &QAction::triggered, [action, type] {
				if (const auto le = qobject_cast<QLineEdit*>(action->parent()))
				{
					// Open a directory selection dialog.
					if (type == stDirectory)
					{
						QFileDialog dialog(le, "Select Directory");
						dialog.setDirectory(QDir::current());
						dialog.setFileMode(QFileDialog::Directory);
						dialog.setOption(QFileDialog::ShowDirsOnly, true);
						// The non-native dialog causes a segmentation fault.
						dialog.setOption(QFileDialog::DontUseNativeDialog, true);
						if (dialog.exec() == QDialog::Accepted)
						{
							le->setText(dialog.selectedFiles().first());
						}
					}
					else if (type == stPath)
					{
						QFileDialog dialog(le, "Select Filepath");
						dialog.setDirectory(QDir::current());
						dialog.setFileMode(QFileDialog::AnyFile);
						dialog.setOption(QFileDialog::HideNameFilterDetails, true);
						// The non-native dialog causes a segmentation fault.
						dialog.setOption(QFileDialog::DontUseNativeDialog, true);
						if (dialog.exec() == QDialog::Accepted)
						{
							le->setText(dialog.selectedFiles().first());
						}
					}
				}
			});
		}
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
	insertRows(0, static_cast<int>(_varList.size()));
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
	Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemNeverHasChildren | Qt::ItemFlag::ItemIsSelectable;
	//
	if (index.column() == cValue)
	{
		// Only editable when not read-only.
		if (!_varList.at(index.row()).get()->isReadOnly())
		{
			flags |= Qt::ItemFlag::ItemIsEditable;
		}
	}
	else if (index.column() == 0)
	{
		flags |= Qt::ItemFlag::ItemIsUserCheckable;
	}
	return flags;
}

int VariableListModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(_varList.size());
}

QVariant VariableListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= _varList.size())
	{
		return {};
	}
	const auto var = _varList.at(index.row()).get();
	// First row always get the selection check box.
	if (role == Qt::CheckStateRole && index.column() == 0)
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

			default:
				return QString("?%1?").arg(index.column());
		}
	}
	// Used to initialize the delegate editor.
	if (role == Qt::ItemDataRole::EditRole)
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
				// When states are available create a dropdown edit.
				if (var->getStateCount())
				{
					return CommonItemDelegate::etDropDownIndex;
				}
				if (var->getType() == Value::vitInteger)
				{
					return CommonItemDelegate::etSpinBox;
				}
				if (var->getType() == Value::vitFloat)
				{
					return CommonItemDelegate::etDoubleSpinBox;
				}
				if (var->getType() == Value::vitString)
				{
					// Multi line string with new-line control characters.
					if (var->getStringType() == stMulti)
					{
						return CommonItemDelegate::etStringList;
					}
					return CommonItemDelegate::etString;
				}
				return CommonItemDelegate::etDefault;
			}
		}
	}
	// Used for editor's selectable options.
	else if (role == CommonItemDelegate::OptionsRole)
	{
		if (index.column() == cValue)
		{
			if (var->getStateCount())
			{
				return QVariant::fromValue(getStateOptions(*var));
			}
		}
	}
	else if (role == CommonItemDelegate::MinimumRole)
	{
		if (index.column() == cValue)
		{
			return toQVariant(var->getMin());
		}
	}
	else if (role == CommonItemDelegate::MaximumRole)
	{
		if (index.column() == cValue)
		{
			return toQVariant(var->getMax());
		}
	}
	else if (role == CommonItemDelegate::IncrementRole)
	{
		if (index.column() == cValue)
		{
			return toQVariant(var->getRnd());
		}
	}
	else if (role == CommonItemDelegate::IncrementRole)
	{
		if (index.column() == cValue)
		{
			return toQVariant(var->getRnd());
		}
	}
	else if (role == CommonItemDelegate::TextColorRole && index.column() == cValue)
	{
		// For all columns the same text color.
		return var->isReadOnly() ? QPalette::ColorRole::Mid : QPalette::ColorRole::Text;
	}
	else if (role == CommonItemDelegate::AlignmentRole && index.column() == cValue)
	{
		// For all columns the same text color.
		return var->isNumber() && var->getStateCount() == 0 ? Qt::AlignmentFlag::AlignRight : Qt::AlignmentFlag::AlignLeft;
	}
	return {};
}

bool VariableListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole)
	{
		const auto var = _varList.at(index.row()).get();
		if (index.column() == cValue)
		{
			if (var->getStateCount())
			{
				// Do not trigger the event for itself sinds dataChanged() is called.
				if (var->setCur(Value(value.toInt()), true))
				{
					Q_EMIT changed(var);
				}
			}
			else
			{
				// Do not trigger the event for itself sinds dataChanged() is called.
				if (var->setCur(Value(value.toString()), true))
				{
					Q_EMIT changed(var);
				}
			}
		}
		// Notify the
		dataChanged(index, index, {Qt::DisplayRole});
		return true;
	}
	return false;
}

int VariableListModel::getRow(Variable& link) const
{
	// Partial match (case-insensitive name comparison):
	auto pred = [&link](const std::shared_ptr<Variable>& p) {
		// Compare the shared pointer content with the linked variable.
		return p.get() == &link;
	};
	const auto it = std::find_if(_varList.begin(), _varList.end(), pred);
	if (it != this->_varList.end())
	{
		return std::distance(_varList.begin(), it);
	}
	return -1;
}

void VariableListModel::variableEventHandler(EEvent event, const Variable& caller, Variable& link, bool same_inst)
{
	switch (event)
	{
		// Value changed on linked variable.
		case veValueChange:
		{
			const auto row = getRow(link);
			if (row >= 0)
			{
				const auto idx = index(row, cValue);
				dataChanged(idx, idx, {Qt::ItemDataRole::DisplayRole});
			}
			break;
		}

		case veConverted:
		{
			const auto row = getRow(link);
			if (row >= 0)
			{
				for (const auto column: {cValue, cUnit})
				{
					const auto idx = index(row, column);
					dataChanged(idx, idx, {Qt::ItemDataRole::DisplayRole});
				}
			}
			break;
		}

		case veUserPrivate:
			break;

		default:
			break;
	}
}

void VariableListModel::addVariable(const Variable* var)
{
	// Create owning pointer instance.
	const auto v = std::make_shared<Variable>(*var);
	// Attach the handler to the new variable instance.
	v->setHandler(this);
	// Make the variable convert units if possible.
	v->setConvert(true);
	// Move the owning pointer instance to the list.
	_varList.append(std::move(v));
}

void VariableListModel::addVariables(const InformationTypes::Vector& list)
{
	for (const auto ib: list)
	{
		if (const auto var = dynamic_cast<const Variable*>(ib))
		{
			addVariable(var);
			//#error Needs a handler.
		}
	}
}

Variable* VariableListModel::getByIndex(const QModelIndex& index) const
{
	return index.isValid() ? _varList.at(index.row()).get() : nullptr;
}

}// namespace sf
