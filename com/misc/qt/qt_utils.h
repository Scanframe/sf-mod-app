#pragma once

#include <QFileInfo>
#include <QDataStream>
#include <QFileSystemWatcher>

// Import of shared library export defines.
#include "../global.h"

/**
 * Allows moving a QPoint using a QSize.
 */
inline
QPoint operator-(const QPoint& pt, const QSize& sz)
{
	return {pt.x() - sz.width(), pt.y() - sz.height()};
}

/**
 * Allows moving a QPoint using a QSize.
 */
inline
QPoint operator+(const QPoint& pt, const QSize& sz)
{
	return {pt.x() + sz.width(), pt.y() + sz.height()};
}

namespace sf
{

/**
 * Keeps the application up-to-date with changes in the settings file.
 * Sets the styling and the color from an ini file.
 */
class _MISC_CLASS ApplicationSettings :public QObject
{
	Q_OBJECT

	public:
		/**
		 * Constructor
		 */
		explicit ApplicationSettings(QObject* parent = nullptr);
		/**
		 * Sets the fileInfo
		 */
		void setFilepath(const QString& filepath, bool watch = false);
		/**
		 * Sets the fileInfo
		 */
		const QFileInfo& fileInfo() const;

	private slots:

		/**
		 * Triggered when a watched files changes.
		 */
		void onFileChance(const QString& file);

	private:
		// Called from setFilepath and the event handler.
		void doStyleApplication(bool watch);

		// File watcher instance.
		QFileSystemWatcher* _watcher;
		// File info structure of the ini-file.
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

} // namespace
