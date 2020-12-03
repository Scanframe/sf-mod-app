#include <QtWidgets/QListView>
#include "stringlistmodel.h"

StringListModel::StringListModel(QObject* parent)
	: QAbstractListModel(parent)
	, stringList()
{}

int StringListModel::rowCount(const QModelIndex& parent) const
{
	return stringList.count();
}

QVariant StringListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	if (index.row() >= stringList.size())
	{
		return QVariant();
	}
	if (role == Qt::DisplayRole)
	{
		return stringList.at(index.row());
	}
	else
	{
		return QVariant();
	}
}

QVariant StringListModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	if (orientation == Qt::Horizontal)
	{
		return QString("Column %1").arg(section);
	}
	else
	{
		return QString("Row %1").arg(section);
	}
}

void StringListModel::append(const QString& str)
{
	beginInsertRows(QModelIndex(), stringList.size(), stringList.size() + 1);
	stringList.append(str);
	endInsertRows();
	// Automatically scroll to bottom.
	if (auto lv = dynamic_cast<QListView*>(parent()))
	{
		lv->scrollToBottom();
	}
}
