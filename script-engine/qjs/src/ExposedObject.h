#ifndef QT_CONCEPTS_EXPOSEDOBJECT_H
#define QT_CONCEPTS_EXPOSEDOBJECT_H

#include <QObject>
#include <QJSValue>
#include <QDebug>
#include <misc/dbgutils.h>

class ExposedObject :public QObject
{
	Q_OBJECT
	public:
		explicit ExposedObject(QObject* parent = nullptr);

		Q_INVOKABLE int stdClog(const QString& str);

		Q_INVOKABLE int passValue(const QJSValue& value);
};

#endif //QT_CONCEPTS_EXPOSEDOBJECT_H
