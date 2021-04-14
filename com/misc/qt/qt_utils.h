#pragma once

#include <QString>
#include <QFileInfo>
#include <QDataStream>
#include <QPoint>
#include <QSize>
#include <QFileSystemWatcher>

// Import of shared library export defines.
#include "../global.h"

inline
std::ostream& operator<<(std::ostream& os, const QString& qs)
{
	return os << qs.toStdString();
}

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
 * @brief Keeps the application up-to-date with changes in the settings file.
 * Sets the styling and the color from an ini file.
 * Used to quickly create test applications.
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
		[[nodiscard]] const QFileInfo& fileInfo() const;

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
 * @brief Connects signals by name of sender and name of signal.
 *
 * @param widget Widget having the child with the passed sender_name
 * @param sender_name Name of sender widget.
 * @param signal_name Name of signal.
 * @param receiver Receiver object
 * @param method_name Name of receiver method.
 * @param ct Connection Type.
 * @return Connection object.
 */
_MISC_FUNC  QMetaObject::Connection connectByName
	(
		const QWidget* widget,
		const QString& sender_name,
		const char* signal_name,
		const QObject* receiver,
		const char* method_name,
		Qt::ConnectionType ct = Qt::AutoConnection
	);

inline
void inflate(QRect& r, int sz)
{
	r.adjust(-sz, -sz, sz, sz);
}

inline
QRect inflated(const QRect& r, int sz)
{
	return r.adjusted(-sz, -sz, sz, sz);
}

} // namespace
