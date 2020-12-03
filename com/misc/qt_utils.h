#ifndef MISC_QT_UTILS_H
#define MISC_QT_UTILS_H

#include <QObject>

// Import of shared library export defines.
#include "global.h"

namespace sf
{

/**
 * Connects void signal(void) functions and handlers by their string
 * @param sender
 * @param signal_name
 * @param receiver
 * @param method_name
 * @return
 */
_MISC_FUNC QMetaObject::Connection QObject_connect
	(
		const QObject* sender,
		const char* signal_name,
		const QObject* receiver,
		const char* method_name,
		Qt::ConnectionType = Qt::AutoConnection
	);

} // namespace sf

#endif // MISC_QT_UTILS_H