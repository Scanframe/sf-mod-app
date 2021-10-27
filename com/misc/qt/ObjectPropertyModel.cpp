#include <bitset>
#include <QMetaEnum>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QFormLayout>
#include "CommonItemDelegate.h"
#include "ObjectPropertyModel.h"
#include "ObjectExtension.h"

namespace sf
{

namespace
{

// QWidget properties to ignore.
const char* ignoreList[] = {
	"palette",
	"geometry",
	"sizePolicy",
	"font",
	"focusPolicy",
	"mouseTracking",
	"tabletTracking",
	"contextMenuPolicy",
	"acceptDrops",
	"windowModality",
	"windowModified",
	"windowTitle",
	"windowIcon",
	"windowIconText",
	"windowOpacity",
	"windowFilePath",
	"toolTipDuration",
	"whatsThis",
	"accessibleName",
	"accessibleDescription",
	"layoutDirection",
	"styleSheet",
	"locale",
	"inputMethodHints",
	"cursor",
	// QLabel
	"pixmap"
};

bool ignored(const char* name)
{
	for (auto n: ignoreList) // NOLINT(readability-use-anyofallof)
	{
		if (std::strcmp(name, n) == 0)
		{
			return true;
		}
	}
	return false;
}

CommonItemDelegate::EEditorType getEditorType(QObject* obj, int index, bool dynamic)
{
	// Negative index indicates a dynamic property.
	if (dynamic)
	{
		return CommonItemDelegate::etDefault;
	}
	auto meta = obj->metaObject();
	QMetaType mt(meta->property(index).typeId());
	if (meta->property(index).enumerator().isValid())
	{
		if (meta->property(index).enumerator().isFlag())
		{
			return CommonItemDelegate::etDropDownFlags;
		}
		return CommonItemDelegate::etDropDownIndex;
	}
	else if (mt == QMetaType::fromType<QMargins>() || mt == QMetaType::fromType<QRect>() || mt == QMetaType::fromType<QSize>())
	{
		return CommonItemDelegate::etEdit;
	}
	else if (mt == QMetaType::fromType<qulonglong>())
	{
		return CommonItemDelegate::etULongLong;
	}
	else if (mt == QMetaType::fromType<QColor>())
	{
		return CommonItemDelegate::etColorEdit;
	}
	return CommonItemDelegate::etDefault;
}

enum EColumn
{
	vcName = 0,
	cValue,
	cMaxColumns
};

enum EColumnIgnore
{
	cIgnored = cMaxColumns,
	vcType
};

CommonItemDelegate::OptionsType getEnumOptions(const QMetaEnum& me)
{
	CommonItemDelegate::OptionsType rv;
	for (int i = 0; i < me.keyCount(); i++)
	{
		auto flag = me.value(i);
		rv.append({flag, me.key(i)});
	}
	return rv;
}

QStringList getEnumFlagsOptions(const QMetaEnum& me, int flags)
{
	QStringList rv;
	int done = 0;
	for (int i = 0; i < me.keyCount(); i++)
	{
		auto flag = me.value(i);
		if (std::bitset<sizeof(flag) * 8>(flag).count() == 1)
		{
			// Remove aliases.
			if (done & flag)
			{
				continue;
			}
			done |= flag;
			if (flags & flag)
			{
				rv.append(me.key(i));
			}
		}
	}
	return rv;
}

CommonItemDelegate::OptionsType getEnumFlagsOptions(const QMetaEnum& me)
{
	CommonItemDelegate::OptionsType rv;
	int done = 0;
	for (int i = 0; i < me.keyCount(); i++)
	{
		auto flag = me.value(i);
		if (std::bitset<sizeof(flag) * 8>(flag).count() == 1)
		{
			// Remove aliases.
			if (done & flag)
			{
				continue;
			}
			done |= flag;
			rv.append({flag, me.key(i)});
		}
	}
	return rv;
}

}

ObjectPropertyModel::ObjectPropertyModel(QObject* parent)
	:QAbstractListModel(parent)
{
}

int ObjectPropertyModel::columnCount(const QModelIndex& parent) const
{
	return static_cast<int>(cMaxColumns);
}

void ObjectPropertyModel::setTarget(QObject* target)
{
	_target = target;
	_indices.clear();
	//
	auto addProps = [this](QObject* obj)
	{
		auto mo = obj->metaObject();
		do
		{
			for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i)
			{
				const auto& prop = mo->property(i);
				if (prop.isWritable() && prop.isDesignable())
				{
					if (!ignored(prop.name()))
					{
						_indices.append({obj, i, false});
					};
				}
			}
		}
		while ((mo = mo->superClass()));
		// Add the dynamic indices.
		auto names = obj->dynamicPropertyNames();
		for (int ni = 0; ni < names.count(); ni++)
		{
			// Do not add Qt's own dynamic added properties.
			if (!names.at(ni).startsWith("_q_"))
			{
				_indices.append({obj, ni, true});
			}
		}
	};
	//
	if (target)
	{
		addProps(target);
		// When a widget.
		if (auto w = qobject_cast<QWidget*>(target))
		{
			// No layout properties when derived from Object extension.
			if (!dynamic_cast<ObjectExtension*>(w))
			{
				// Having a layout
				if (w->layout())
				{
					addProps(w->layout());
				}
			}
		}
	}
	refresh();
}

void ObjectPropertyModel::setDelegates(QAbstractItemView* view)
{
	auto cid = new CommonItemDelegate(view);
	// Propagate the signal.
	connect(cid, &CommonItemDelegate::addLineEditActions, [&](QLineEdit* lineEdit, const QModelIndex& index)
	{
		auto propIdx = _indices.at(index.row());
		Q_EMIT addLineEditActions(lineEdit, propIdx._obj, propIdx._index, propIdx._dynamic);
	});
	view->setItemDelegate(cid);
}

void ObjectPropertyModel::refresh()
{
	beginRemoveRows(QModelIndex(), 0, std::numeric_limits<int>::max());
	endRemoveRows();
	beginInsertRows(QModelIndex(), 0, -1);
	insertRows(0, static_cast<int>(_indices.size()));
	endInsertRows();
}

QVariant ObjectPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case vcName:
				return QString(tr("Name"));
			case vcType:
				return QString(tr("Type"));
			case cValue:
				return QString(tr("Value"));
			default:
				return QString("Field %1").arg(section);
		}
	}
	return {};
}

Qt::ItemFlags ObjectPropertyModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::ItemFlag::NoItemFlags;
	}
	//
	Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemNeverHasChildren;
	//
	if (index.column() == EColumn::cValue)
	{
		flags |= Qt::ItemFlag::ItemIsEditable;
	}
	return flags;
}

int ObjectPropertyModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(_indices.size());
}

QVariant ObjectPropertyModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return {};
	}
	if (index.row() >= _indices.size())
	{
		return {};
	}
	auto propIdx = _indices.at(index.row());
	auto meta = propIdx._obj->metaObject();
	// Display the value.
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		switch (index.column())
		{
			case vcName:
				// Negative index indicates a dynamic property.
				if (propIdx._dynamic)
				{
					return propIdx._obj->dynamicPropertyNames().at(propIdx._index);
				}
				return QString("%1::%2").arg(meta->property(propIdx._index).enclosingMetaObject()->className()).arg(meta->property(propIdx._index).name());

			case vcType:
				// Negative index indicates a dynamic property.
				if (propIdx._dynamic)
				{
					return propIdx._obj->property(propIdx._obj->dynamicPropertyNames().at(propIdx._index)).typeName();
				}
				return meta->property(propIdx._index).typeName();

			case cValue:
			{
				// Negative index indicates a dynamic property.
				if (propIdx._dynamic)
				{
					return propIdx._obj->property(propIdx._obj->dynamicPropertyNames().at(propIdx._index));
				}
				const auto& mp = meta->property(propIdx._index);
				QMetaType mt(meta->property(propIdx._index).typeId());
				if (mp.enumerator().isValid())
				{
					if (mp.enumerator().isFlag())
					{
						return getEnumFlagsOptions(mp.enumerator(), propIdx._obj->property(mp.name()).toInt()).join(',');
					}
				}
				else if (mt == QMetaType::fromType<QMargins>())
				{
					auto margins = propIdx._obj->property(mp.name()).value<QMargins>();
					return QString("%1,%2,%3,%4").arg(margins.left()).arg(margins.top()).arg(margins.right()).arg(margins.bottom());
				}
				else if (mt == QMetaType::fromType<QRect>())
				{
					auto rect = propIdx._obj->property(mp.name()).value<QRect>();
					return QString("%1,%2,%3,%4").arg(rect.left()).arg(rect.top()).arg(rect.right()).arg(rect.bottom());
				}
				else if (mt == QMetaType::fromType<QSize>())
				{
					auto size = propIdx._obj->property(mp.name()).value<QSize>();
					return QString("%1,%2").arg(size.width()).arg(size.height());
				}
				else if (mt == QMetaType::fromType<qulonglong>())
				{
					// Show hexadecimal value.
					return QString("0x%1").arg(propIdx._obj->property(mp.name()).value<qulonglong>(), 0, 16);
				}
				// Otherwise, just default.
				return propIdx._obj->property(mp.name());
			}
		}
	}
	// Used to initialize the delegate editor.
	else if (role == Qt::ItemDataRole::EditRole)
	{
		if (index.column() == cValue)
		{
			if (propIdx._dynamic)
			{
				return propIdx._obj->property(propIdx._obj->dynamicPropertyNames().at(propIdx._index));
			}
			const auto& mp = meta->property(propIdx._index);
			QMetaType mt(meta->property(propIdx._index).typeId());
			if (mp.enumerator().isValid())
			{
				// When an enumerator convert it to an integer otherwise comparison for current index fails.
				return propIdx._obj->property(mp.name()).toInt();
			}
			if (mt == QMetaType::fromType<QMargins>())
			{
				auto margins = propIdx._obj->property(mp.name()).value<QMargins>();
				return QString("%1,%2,%3,%4").arg(margins.left()).arg(margins.top()).arg(margins.right()).arg(margins.bottom());
			}
			else if (mt == QMetaType::fromType<QRect>())
			{
				auto rect = propIdx._obj->property(mp.name()).value<QRect>();
				return QString("%1,%2,%3,%4").arg(rect.left()).arg(rect.top()).arg(rect.right()).arg(rect.bottom());
			}
			else if (mt == QMetaType::fromType<QSize>())
			{
				auto size = propIdx._obj->property(mp.name()).value<QSize>();
				return QString("%1,%2").arg(size.width()).arg(size.height());
			}
			return propIdx._obj->property(mp.name());
		}
	}
	else if (role == Qt::ItemDataRole::ToolTipRole)
	{
		if (propIdx._dynamic)
		{
			return propIdx._obj->property(propIdx._obj->dynamicPropertyNames().at(propIdx._index)).typeName();
		}
		return QString(tr("%1 Type: %2")).arg(meta->property(propIdx._index).enclosingMetaObject()->className()).arg(meta->property(propIdx._index).typeName());
	}
	// Used for selection of editor type.
	else if (role == CommonItemDelegate::TypeRole)
	{
		if (index.column() == cValue)
		{
			return getEditorType(propIdx._obj, propIdx._index, propIdx._dynamic);
		}
	}
	// Used for editor's selectable options.
	if (role == CommonItemDelegate::OptionsRole)
	{
		if (index.column() == cValue)
		{
			const auto& mp = meta->property(propIdx._index);
			QMetaType mt(mp.typeId());
			if (!mt.isValid())
			{
				return {};
			}
			if (mp.enumerator().isValid())
			{
				if (mp.enumerator().isFlag())
				{
					return QVariant::fromValue(getEnumFlagsOptions(mp.enumerator()));
				}
				else
				{
					return QVariant::fromValue(getEnumOptions(mp.enumerator()));
				}
			}
			return {};
		}
	}
	return {};
}

bool ObjectPropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole)
	{
		auto propIdx = _indices.at(index.row());
		auto meta = propIdx._obj->metaObject();
		if (index.column() == cValue)
		{
			QByteArray propName;
			int typeId;
			if (propIdx._dynamic)
			{
				propName = propIdx._obj->dynamicPropertyNames().at(propIdx._index);
				typeId = propIdx._obj->property(propName).typeId();
			}
			else
			{
				propName = meta->property(propIdx._index).name();
				typeId = meta->property(propIdx._index).typeId();
			}
			// Get the current value for comparison to emit an event.
			auto curVal = propIdx._obj->property(propName);
			//
			QMetaType mt(typeId);
			if (mt == QMetaType::fromType<QMargins>() || mt == QMetaType::fromType<QRect>())
			{
				auto sl = value.toString().split(',');
				if (sl.size() == 4)
				{
					int v[4];
					for (int i = 0; i < 4; i++)
					{
						v[i] = sl[i].toInt();
					}
					if (mt == QMetaType::fromType<QMargins>())
					{
						propIdx._obj->setProperty(propName, QVariant::fromValue<QMargins>({v[0], v[1], v[2], v[3]}));
					}
					else
					{
						propIdx._obj->setProperty(propName, QVariant::fromValue<QRect>({v[0], v[1], v[2], v[3]}));
					}
				}
			}
			else if (mt == QMetaType::fromType<QSize>())
			{
				auto sl = value.toString().split(',');
				if (sl.size() == 2)
				{
					int v[2];
					for (int i = 0; i < 2; i++)
					{
						v[i] = sl[i].toInt();
					}
					propIdx._obj->setProperty(propName, QVariant::fromValue<QSize>({v[0],v[1]}));
				}
			}
			else
			{
				propIdx._obj->setProperty(propName, value);
			}
			// Signal that a property has changed when it did.
			if (curVal != propIdx._obj->property(propName))
			{
				Q_EMIT changed(propIdx._obj, propIdx._index, propIdx._dynamic);
			}
			dataChanged(index, index);
		}
		return true;
	}
	return false;
}

}
