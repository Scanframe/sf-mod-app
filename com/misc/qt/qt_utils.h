#pragma once

#include <QString>
#include <QFileInfo>
#include <QDataStream>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QFileSystemWatcher>
#include <QPalette>
#include <QFileDialog>

#include "../global.h"

/**
 * @brief Operator stream a QString instance to ans std ostream.
 */
inline
std::ostream& operator<<(std::ostream& os, const QString& qs)
{
	return os << qs.toStdString();
}

/**
 * @brief Allows moving a QPoint using a QSize.
 */
inline
QPoint operator-(const QPoint& pt, const QSize& sz)
{
	return {pt.x() - sz.width(), pt.y() - sz.height()};
}

/**
 * @brief Allows moving a QPoint using a QSize.
 */
inline
QPoint operator+(const QPoint& pt, const QSize& sz)
{
	return {pt.x() + sz.width(), pt.y() + sz.height()};
}

/**
 * @brief Allows adjusting the QRect size using a QSize.
 */
inline
QRect operator+(const QRect& rc, const QSize& sz)
{
	return {rc.topLeft(), rc.size() + sz};
}

/**
 * @brief Allows adjusting the QRect size using a QSize.
 */
inline
QRect operator-(const QRect& rc, const QSize& sz)
{
	return {rc.topLeft(), rc.size() - sz};
}

/**
 * @brief Allows adjusting the QRect position using a QPoint.
 */
inline
QRect operator+(const QRect& rc, const QPoint& pt)
{
	return {rc.topLeft() + pt, rc.size()};
}

/**
 * @brief Allows adjusting the QRect position using a QPoint.
 */
inline
QRect operator+=(QRect& rc, const QPoint& pt)
{
	rc = {rc.topLeft() + pt, rc.size()};
	return rc;
}

/**
 * @brief Allows adjusting the QRect position using a QPoint.
 */
inline
QRect operator-(const QRect& rc, const QPoint& pt)
{
	return {rc.topLeft() - pt, rc.size()};
}

/**
 * @brief Allows adjusting the QRect position using a QPoint.
 */
inline
QRect operator-=(QRect& rc, const QPoint& pt)
{
	rc = {rc.topLeft() - pt, rc.size()};
	return rc;
}


/**
 * @brief Inflates the passed rect on all sides using an integer.
 */
inline
void inflate(QRect& r, int sz)
{
	r.adjust(-sz, -sz, sz, sz);
}

/**
 * @brief Inflates a copy the rectangle an integer and returns it.
 */
inline
QRect inflated(const QRect& r, int sz)
{
	return r.adjusted(-sz, -sz, sz, sz);
}


namespace sf
{

/**
 * @brief Type to hold
 */
class _MISC_CLASS PaletteColors
{
	public:

		explicit PaletteColors() = default;

		explicit PaletteColors(const QPalette& palette);

		[[nodiscard]] QPalette getPalette() const;

		void setColors(const QPalette& palette);

		[[nodiscard]] bool isEmpty() const;

		void styleFileDialog(QFileDialog& fd) const;

	private:
		typedef QPair<QPalette::ColorRole, QColor> Pair;
		QList<Pair> _colors;
};


/**
 * @brief Keeps the application up-to-date with changes in the settings file.
 *
 * Sets the styling and the color from an ini file.
 * Used to quickly create test applications.
 */
class _MISC_CLASS ApplicationSettings :public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit ApplicationSettings(QObject* parent = nullptr);
		/**
		 * @brief Destructor.
		 */
		~ApplicationSettings() override;
		/**
		 * @brief Sets the fileInfo
		 */
		void setFilepath(const QString& filepath, bool watch = false);
		/**
		 * @brief Gets the fileInfo.
		 */
		[[nodiscard]] const QFileInfo& fileInfo() const;

		/**
		 * @brief Sets the window position and size from the settings file onto the passed widget.
		 * @param win_name Name of the window.
		 * @param window Window widget.
		 */
		void restoreWindowRect(const QString& win_name, QWidget* window);

		/**
		 * @brief Sets the window position and size from the settings file onto the passed widget.
		 * @param win_name Name of the window.
		 * @param window Window widget.
		 */
		void saveWindowRect(const QString& win_name, QWidget* window);

	private slots:

		/**
		 * @brief Triggered when a watched files changes.
		 */
		void onFileChance(const QString& file);

	private:
		/**
		 * @brief Called from setFilepath and the event handler.
		 * @param watch
		 */
		void doStyleApplication(bool watch);

		/**
		 * @brief File watcher instance.
		 */
		QFileSystemWatcher* _watcher;
		/**
		 * @brief File info structure of the ini-file.
		 */
		QFileInfo _fileInfo;

		/**
		 * @brief Holds system colors from startup.
		 *
		 * When having a different palette then the default one the icons in the file open dialog
		 * do not have the right color.
		 * This class provides a solution to store the initial palette for example.
		 */
		PaletteColors _systemColors;

		/**
		 * Save and restores the window state of the passed widget.
		 * @param name Name of the window widget.
		 * @param widget The window widget.
		 * @param save True for saving and false for restoring.
		 */
		void windowState(const QString& name, QWidget* widget, bool save);
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

} // namespace
