#include "messagelist.h"

#include <QByteArray>


Message::Message()
//	:QObject(nullptr)
{

}

Message::Message(const QString& identifier, const QString& title, const QString& message)
//	:QObject(nullptr)
	:_identifier(identifier)
	,_title(title)
	,_message(message)
{
}

const QString& Message::identifier() const
{
	return _identifier;
}

const QString& Message::title() const
{
	return _title;
}

const QString& Message::message() const
{
	return _message;
}


MessageListModal::MessageListModal() :QAbstractListModel(nullptr)
{
}

int MessageListModal::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return list.size();
}

QHash<int, QByteArray> MessageListModal::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[IdentifierRole] = "identifier";
	roles[TitleRole] = "title";
	roles[MessageRole] = "message";
	return roles;
}

QVariant MessageListModal::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= list.size() || index.row() < 0)
	{
		return QVariant();
	}
	switch(role)
	{
		case IdentifierRole:
			return list.at(index.row())->identifier();
			break;
		case TitleRole:
			return list.at(index.row())->title();
			break;
		case MessageRole:
			return list.at(index.row())->message();
			break;
		default:
			return QVariant();
	}
}

bool MessageListModal::insert(const QList<Message>& messages, int position)
{
	beginInsertRows(QModelIndex(), position, position + messages.size() - 1);
	for(int row = 0; row < messages.size(); ++row)
	{
		IndexMap::const_iterator indexMapItr(indexMap.constFind(messages.at(row).identifier()));
		if(indexMapItr == indexMap.constEnd())
		{
			MessagePointer newMessage(new Message(messages.at(row)));
			list.insert(position, newMessage);
			indexMap.insert(messages.at(row).identifier(), newMessage);
		}
	}
	endInsertRows();
	return true;
}

bool MessageListModal::reset()
{
	beginResetModel();
	list.clear();
	indexMap.clear();
	endResetModel();
	return true;
}

QVariantMap MessageListModal::get(const QString& identifier) const
{
	QVariantMap result;
	IndexMap::const_iterator indexMapItr(indexMap.constFind(identifier));
	if(indexMapItr != indexMap.constEnd()) {
		result["identifier"] = QVariant(indexMapItr->data()->identifier());
		result["title"] = QVariant(indexMapItr->data()->title());
		result["message"] = QVariant(indexMapItr->data()->message());
	}
	return result;
}

const Message& MessageListModal::at(int index) const
{
	return *(list.at(index).data());
}

