#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <QObject>
#include <QAbstractListModel>
#include <QModelIndex>
#include <QList>
#include <QHash>
#include <QVariant>
#include <QMap>
#include <QSharedPointer>


//#ifndef MESSAGE_HPP
//#define MESSAGE_HPP

#include <QObject>
#include <QString>

class Message
//	:public QObject
{
//	Q_OBJECT
//	Q_PROPERTY(QString identifier READ identifier NOTIFY identifierChanged)
//	Q_PROPERTY(QString title READ title NOTIFY titleChanged)
//	Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
	Message();
	Message(const QString& identifier, const QString& title, const QString& message);

	const QString& identifier() const;
	const QString& title() const;
	const QString& message() const;

	protected:
		QString _identifier;
		QString _title;
		QString _message;

//signals:
//	void identifierChanged();
//	void titleChanged();
//	void messageChanged();
};

//Q_DECLARE_METATYPE(Message)

//#endif // MESSAGE_H

class MessageListModal :public QAbstractListModel
{
	Q_OBJECT

	public:
		typedef QSharedPointer<Message> MessagePointer;
		typedef QList<MessagePointer> MessagePointerList;
		typedef QMap<QString, MessagePointer> IndexMap;

		enum Roles
		{
			IdentifierRole,
			TitleRole,
			MessageRole,
		};

		MessageListModal();

		int rowCount(const QModelIndex& parent) const;

		QHash<int, QByteArray> roleNames() const;

		QVariant data(const QModelIndex& index, int role) const;

		bool insert(const QList<Message>& messages, int position = 0);

		bool reset();

		Q_INVOKABLE QVariantMap get(const QString& identifier) const;

		const Message& at(int index) const;

	protected:
		MessagePointerList list;
		IndexMap indexMap;
};

#endif // MESSAGELIST_H
