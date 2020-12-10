#ifndef MISC_QT_UTILS_H
#define MISC_QT_UTILS_H

#include <QFileInfo>
#include <QFileSystemWatcher>

#include "qt_utils.h"
// Import of shared library export defines.
#include "global.h"

namespace sf
{

/**
 * Keeps the application up-to-date with changes in the settings file.
 * Sets the styling and the color from an ini file.
 */
class _MISC_CLASS QApplicationSettings : public QObject
{
	Q_OBJECT

	public:
		/**
		 * Constructor
		 */
		explicit QApplicationSettings(QObject* parent = nullptr);
		/**
		 * Sets the filepath
		 */
		void setFilepath(const QString& filepath, bool watch = false);

	private slots:
		void onFileChance(const QString& file);

	private:
		//
		void doStyleApplication(bool watch);
		//
		QFileSystemWatcher* _watcher;
		//
		QFileInfo _fileInfo;
};

/**
 * Connects void signal(void) functions and handlers by their string.
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