#pragma once
#include <QAbstractProxyModel>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QMetaEnum>
#include <QPalette>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QTreeView>
#include <misc/global.h>

/**
 * @brief Operator stream a QString instance to ans std ostream.
 */
inline std::ostream& operator<<(std::ostream& os, const QString& qs)
{
	return os << qs.toStdString();
}

/**
 * @brief Allows moving a QPoint using a QSize.
 */
inline QPoint operator-(const QPoint& pt, const QSize& sz)
{
	return {pt.x() - sz.width(), pt.y() - sz.height()};
}

/**
 * @brief Allows moving a QPoint using a QSize.
 */
inline QPoint operator+(const QPoint& pt, const QSize& sz)
{
	return {pt.x() + sz.width(), pt.y() + sz.height()};
}

/**
 * @brief Allows adjusting the QRect size using a QSize.
 */
inline QRect operator+(const QRect& rc, const QSize& sz)
{
	return {rc.topLeft(), rc.size() + sz};
}

/**
 * @brief Allows adjusting the QRect size using a QSize.
 */
inline QRect operator-(const QRect& rc, const QSize& sz)
{
	return {rc.topLeft(), rc.size() - sz};
}

/**
 * @brief Allows adjusting the QRect position using a QPoint.
 */
inline QRect operator+(const QRect& rc, const QPoint& pt)
{
	return {rc.topLeft() + pt, rc.size()};
}

/**
 * @brief Allows adjusting the QRect position using a QPoint.
 */
inline QRect operator+=(QRect& rc, const QPoint& pt)
{
	rc = {rc.topLeft() + pt, rc.size()};
	return rc;
}

/**
 * @brief Allows adjusting the QRect position using a QPoint.
 */
inline QRect operator-(const QRect& rc, const QPoint& pt)
{
	return {rc.topLeft() - pt, rc.size()};
}

/**
 * @brief Allows adjusting the QRect position using a QPoint.
 */
inline QRect operator-=(QRect& rc, const QPoint& pt)
{
	rc = {rc.topLeft() - pt, rc.size()};
	return rc;
}

/**
 * @brief Inflates the passed rect on all sides using an integer.
 */
inline QRect& inflate(QRect& r, int sz)
{
	r.adjust(-sz, -sz, sz, sz);
	return r;
}

/**
 * @brief Inflates a copy the rectangle an integer and returns it.
 */
constexpr QRect inflated(const QRect& r, int sz)
{
	return r.adjusted(-sz, -sz, sz, sz);
}

/**
 * @brief Gets the passed Qt point as a Qt size class.
 * @param pt Qt point instance.
 * @return Qt Size instance.
 */
inline QSize asQSize(const QPoint& pt)
{
	return {pt.x(), pt.y()};
}

namespace sf
{

/**
 * @brief Converts the passed Qt rectangle into a std string.
 * @param rect Qt Rectangle.
 * @return String like '(left, bottom, right, top)'.
 */
_MISC_FUNC std::string toString(const QRect& rect);

/**
 * @brief Moves the inner in to the outer rectangle.
 * @param outer Bounding rectangle.
 * @param inner Rectangle to be moved.
 * @return Modified rectangle.
 */
_MISC_FUNC QRect moveRectWithinRect(const QRect& outer, const QRect& inner);

/**
 * @brief Type to hold palette colors.
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
 * @brief Connects signals by name of sender and name of signal.
 * @param widget Widget having the child with the passed sender_name
 * @param sender_name Name of sender widget.
 * @param signal_name Name of signal.
 * @param receiver Receiver object
 * @param method_name Name of receiver method.
 * @param ct Connection Type.
 * @return Connection object.
 */
_MISC_FUNC QMetaObject::Connection connectByName(
	const QWidget* widget, const QString& sender_name, const char* signal_name, const QObject* receiver, const char* method_name,
	Qt::ConnectionType ct = Qt::AutoConnection
);

/**
 * @brief Gets all the parent names from the object's parent in a string list.
 * @param object Top object.
 * @return List of object names.
 */
_MISC_FUNC QStringList getObjectNamePath(const QObject* object);

/**
 * @brief Gets the layout containing the passed widget.
 * @param widget .
 * @return On not found nullptr.
 */
_MISC_FUNC QLayout* getWidgetLayout(const QWidget* widget);

/**
 * @brief Turns a QT enumerate type into a named key.
 *
 * @tparam T Enumerate type.
 * @param value Enum value.
 * @return Key name of the enumerate value.
 */
template<typename T>
static const char* enumToKey(const T value)
{
	return QMetaEnum::fromType<T>().valueToKey(value);
}

/**
 * @brief Turns a QT enumerate typed key name into an enumerate value.
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
 * @param comboBox Combo box to query on for the given value..
 * @param value Value to look the index up from.
 * @param default_index DEfault index when the value was not found.
 * @return The found index or default given one.
 */
_MISC_FUNC int indexFromComboBox(const QComboBox* comboBox, const QVariant& value, int default_index = -1);

/**
 * @brief Gets the position (row, role) from the passed target object in te form-layout.
 * @param layout Form layout.
 * @param target QLayout or QWidget type of object.
 * @return Pair of values (row, role),. On failure row = -1.
 */
_MISC_FUNC QPair<int, QFormLayout::ItemRole> getLayoutPosition(const QFormLayout* layout, QObject* target);

/**
 * @brief Gets the index from the passed target object in te box-layout.
 * @param layout Box-layout.
 * @param target QLayout(Item or QWidget type of object.
 * @return Index position.
 */
_MISC_FUNC int getLayoutIndex(const QBoxLayout* layout, QObject* target);

/**
 * @brief Resizes all columns to content of a tree view except the last column.
 * @param treeView
 */
inline void resizeColumnsToContents(QTreeView* treeView)
{
	const auto count = treeView->model()->columnCount({}) - 1;
	for (int i = 0; i < count; i++)
	{
		treeView->resizeColumnToContents(i);
	}
}

/**
 * @brief Dumps all the object properties in qDebug().
 */
_MISC_FUNC void dumpObjectProperties(const QObject* obj);

/**
 * @brief Gets the model type pointer from the passed abstract model pointer.
 * @tparam T Model type derived from QAbstractItemModel
 * @param am Pointer to model.
 * @return Non-null when found.
 */
template<typename T>
T* getSourceModel(const QAbstractItemModel* am)
{
	// First check if the passed abstract model is the model we look for.
	if (auto m = dynamic_cast<const T*>(am))
	{
		return const_cast<T*>(m);
	}
	// Secondly, when a proxy is used.
	if (const auto apm = dynamic_cast<const QAbstractProxyModel*>(am))
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

/**
 * @brief Checks if the running QCoreApplication is a GUI application.
 * @return True when a GUI application.
 */
_MISC_FUNC bool isGuiApplication();

}// namespace sf

/**
 * @brief Output stream operator for a QRect class.
 */
inline std::ostream& operator<<(std::ostream& os, const QRect& rect)
{
	return os << sf::toString(rect);
}
