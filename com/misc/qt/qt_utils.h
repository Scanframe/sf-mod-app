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
#include <QLayout>
#include <QTreeView>
#include <QMetaEnum>
#include <QMessageBox>
#include <QComboBox>
#include <QFormLayout>
#include <QAbstractProxyModel>

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
QRect& inflate(QRect& r, int sz)
{
	r.adjust(-sz, -sz, sz, sz);
	return r;
}

/**
 * @brief Inflates a copy the rectangle an integer and returns it.
 */
inline
constexpr QRect inflated(const QRect& r, int sz)
{
	return r.adjusted(-sz, -sz, sz, sz);
}

inline
QSize asQSize(const QPoint& pt)
{
	return {pt.x(), pt.y()};
}

namespace sf
{

_MISC_FUNC QRect moveRectWithinRect(const QRect& outer, const QRect& inner);


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

		//void styleMessageBox(QMessageBox& mb) const;
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

	private Q_SLOTS:

		/**
		 * @brief Triggered when a watched files changes.
		 */
		void onFileChance(const QString& file);

	private:
		/**
		 * @brief Called from setFilepath and the event handler.
		 * @param watch
		 */
		void doStyleApplication(bool readOnly, bool watch);
		/**
		 * Save and restores the window state of the passed widget.
		 * @param name Name of the window widget.
		 * @param widget The window widget.
		 * @param save True for saving and false for restoring.
		 */
		void windowState(const QString& name, QWidget* widget, bool save);

		/**
		 * @brief File watcher instance.
		 */
		QFileSystemWatcher* _watcher;
		/**
		 * @brief Holds the timestamp of the last processed and watched file.
		 */
		QDateTime _lastModified;
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
_MISC_FUNC QMetaObject::Connection connectByName
	(
		const QWidget* widget,
		const QString& sender_name,
		const char* signal_name,
		const QObject* receiver,
		const char* method_name,
		Qt::ConnectionType ct = Qt::AutoConnection
	);

/**
 * @brief Gets all the parent names from the object's parent in a string list.
 *
 * @param object Top object.
 * @return List of object names.
 */
_MISC_FUNC QStringList getObjectNamePath(const QObject* object);

/**
 * @brief Gets the layout containing the passed widget.
 *
 * @param widget .
 * @return On not found nullptr.
 */
_MISC_FUNC QLayout* getWidgetLayout(QWidget* widget);

/**
 * @brief Turns a QT enumerate type into a named key.
 *
 * @tparam T Enumerate type.
 * @param value Enum value.
 * @return Key name of the enumerate.
 */
template<typename T>
static const char* enumToKey(const T value)
{
	return QMetaEnum::fromType<T>().valueToKey(value);
}

/**
 * @brief Turns a QT enumerate typed key name into an enumerate value.
 *
 * @tparam T Enumerate type.
 * @param key Key name
 * @return Enumerate value.
 */
template<typename T>
static T keyToEnum(const char* key)
{
	return QMetaEnum::fromType<T>().keyToValue(key);
}

/**
 * @brief Turns a QT enumerate typed key name into an enumerate value.
 *
 * @tparam T Enumerate type.
 * @param key Key name
 * @return Enumerate value.
 */
template<typename T>
static T keyToEnum(QString key)
{
	return QMetaEnum::fromType<T>().keyToValue(key);
}

/**
 * @brief Gets the index from the passed data value of the passed combo box widget.
 *
 * @param comboBox Combo box to query on for the given value..
 * @param value Value to look the index up from.
 * @param default_index DEfault index when the value was not found.
 * @return The found index or default given one.
 */
_MISC_FUNC int indexFromComboBox(QComboBox* comboBox, const QVariant& value, int default_index = -1);

/**
 * @brief Gets the position (row, role) from the passed target object in te form-layout.
 *
 * @param layout Form layout.
 * @param target QLayout or QWidget type of object.
 * @return Pair of values (row, role),. On failure row = -1.
 */
_MISC_FUNC QPair<int, QFormLayout::ItemRole> getLayoutPosition(QFormLayout* layout, QObject* target);

/**
 * @brief Gets the index from the passed target object in te box-layout.
 *
 * @param layout Box-layout.
 * @param target QLayout(Item or QWidget type of object.
 * @return Index position.
 */
_MISC_FUNC int getLayoutIndex(QBoxLayout* layout, QObject* target);

/**
 * @brief Resizes all columns to content of a tree view except the last column.
 * @param treeView
 */
inline
void resizeColumnsToContents(QTreeView* treeView)
{
	auto count = treeView->model()->columnCount({}) - 1;
	for (int i = 0; i < count; i++)
	{
		treeView->resizeColumnToContents(i);
	}
}

/**
 * @brief Dumps the object properties in qDebug().
 */
_MISC_FUNC void dumpObjectProperties(QObject* obj);

/**
 * @brief Gets the model type pointer from the passed abstract model pointer.
 *
 * @tparam T Model type derived from QAbstractItemModel
 * @param am Pointer to model.
 * @return Non-null when found.
 */
template <typename T>
T* getSourceModel(const QAbstractItemModel* am)
{
	// First check if the passed abstract model is the model we look for.
	if (auto m = dynamic_cast<const T*>(am))
	{
		return const_cast<T*>(m);
	}
	// Secondly, when a proxy is used.
	if (auto apm = dynamic_cast<const QAbstractProxyModel*>(am))
	{
		if (auto m = dynamic_cast<T*>(apm->sourceModel()))
		{
			return const_cast<T*>(m);
		}
	}
	return nullptr;
}

/**
 * @brief Gets the source index in case a proxy model has been used.
 */
_MISC_FUNC QModelIndex getSourceModelIndex(const QModelIndex& index);

/**
 * @brief Expands or collapses a tree view's items.
 * @param tv TreeView
 * @param expand True to expand false to collapse.
 * @param index Optional child index. Defaults to the root item.
 */
_MISC_FUNC void expandTreeView(QTreeView* tv, bool expand = true, const QModelIndex& index = {});


}
