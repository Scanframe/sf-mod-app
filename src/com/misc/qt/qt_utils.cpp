#include "qt_utils.h"
#include "../gen/dbgutils.h"
#include "gen/math.h"
#include "gen/string.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QFormLayout>
#include <QHeaderView>
#include <QLayout>
#include <QMetaEnum>
#include <QSettings>
#include <QTimer>
#include <QWidget>
#if IS_WIN
	#include "../win/win_utils.h"
#endif

namespace sf
{

std::string toString(const QRect& rect)
{
	return std::string() + '(' + toString(rect.left()) + ',' + toString(rect.bottom()) + ',' + toString(rect.right()) + ',' + toString(rect.top()) + ')';
}

QRect moveRectWithinRect(const QRect& outer, const QRect& inner)
{
	//auto sz = inner.size();
	auto pos = inner.topLeft();
	// When beyond the left of outer.
	if (pos.y() < outer.y())
	{
		pos.setY(outer.y());
	}
	// When beyond the top of outer.
	if (pos.x() < outer.x())
	{
		pos.setX(outer.x());
	}
	// When beyond the right of outer.
	if (inner.right() > outer.right())
	{
		pos.setX(pos.x() + outer.right() - inner.right());
	}
	// When beyond the bottom of outer.
	if (inner.bottom() > outer.bottom())
	{
		pos.setY(pos.y() + outer.bottom() - inner.bottom());
	}
	return {pos, inner.size()};
}

PaletteColors::PaletteColors(const QPalette& palette)
{
	setColors(palette);
}

QPalette PaletteColors::getPalette() const
{
	QPalette palette;
	for (const auto& [role, color]: _colors)
	{
		palette.setColor(role, color);
	}
	return palette;
}

void PaletteColors::setColors(const QPalette& palette)
{
	_colors.clear();
	for (int i = 0; i < QPalette::ColorRole::NColorRoles; i++)
	{
		auto role = static_cast<QPalette::ColorRole>(i);
		_colors.append(Pair(role, palette.color(role)));
	}
}

bool PaletteColors::isEmpty() const
{
	return _colors.isEmpty();
}

void PaletteColors::styleFileDialog(QFileDialog& fd) const
{
	const auto pal = getPalette();
	//pal.setColor(QPalette::ColorRole::Button, QColorConstants::DarkCyan);
	const auto fmt = QString(R"(
QLabel
{
color: %1;
}
QToolButton:open
{
	background-color: %2;
}
QPushButton, QComboBox, QHeaderView
{
	background-color: %2;
	color: %3;
}
QScrollBar
{
	background-color: %4;
}
QScrollBar::handle
{
	background-color: %2;
}
QLineEdit
{
	color: %1;
	background-color: %4;
}
QListView, QTreeView
{
	background-color: %4;
	color: %1;
}
)");
	fd.setStyleSheet(fmt.arg(pal.color(QPalette::ColorRole::WindowText).name(QColor::HexArgb))
										 .arg(pal.color(QPalette::ColorRole::Button).name(QColor::HexArgb))
										 .arg(pal.color(QPalette::ColorRole::ButtonText).name(QColor::HexArgb))
										 .arg(pal.color(QPalette::ColorRole::Window).darker(130).name(QColor::HexArgb)));
	fd.setPalette(pal);
}

QStringList getObjectNamePath(const QObject* object)
{
	QStringList sl;
	while (object)
	{
		auto s = object->objectName();
		// Cannot be used in destructor of class concerned. sl.prepend(s.isEmpty() ? object->metaObject()->className() : s);
		sl.prepend(s.isEmpty() ? "_" : s);
		object = object->parent();
	}
	return sl;
}

QLayout* getWidgetLayout(const QWidget* widget)
{
	if (widget)
	{
		if (const auto pw = widget->parentWidget())
		{
			for (const auto layout: pw->findChildren<QLayout*>(QString(), Qt::FindChildrenRecursively))
			{
				if (layout->indexOf(widget) != -1)
				{
					return layout;
				}
			}
		}
	}
	return nullptr;
}

auto indexFromComboBox(const QComboBox* comboBox, const QVariant& value, int default_index) -> int
{
	for (int i = 0; i < comboBox->count(); i++)
	{
		if (comboBox->itemData(i) == value)
		{
			return i;
		}
	}
	return default_index;
}

QPair<int, QFormLayout::ItemRole> getLayoutPosition(const QFormLayout* layout, QObject* target)
{
	int row = -1;
	QFormLayout::ItemRole role(QFormLayout::SpanningRole);
	if (const auto wgt = dynamic_cast<QWidget*>(target))
	{
		layout->getWidgetPosition(wgt, &row, &role);
	}
	else if (const auto lo = dynamic_cast<QLayout*>(target))
	{
		layout->getLayoutPosition(lo, &row, &role);
	}
	return {row, role};
}

int getLayoutIndex(const QBoxLayout* layout, QObject* target)
{
	if (const auto wgt = dynamic_cast<QWidget*>(target))
	{
		return layout->indexOf(wgt);
	}
	if (const auto lo = dynamic_cast<QLayout*>(target))
	{
		return layout->indexOf(lo);
	}
	return -1;
}

void dumpObjectProperties(const QObject* obj)
{
	auto mo = obj->metaObject();
	qDebug() << "## Properties of" << obj;
	do
	{
		qDebug() << "### Class" << mo->className();
		std::vector<std::pair<QString, QVariant>> v;
		v.reserve(mo->propertyCount() - mo->propertyOffset());
		for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i)
		{
			v.emplace_back(mo->property(i).name(), mo->property(i).read(obj));
		}
		//std::sort(v.begin(), v.end());
		for (auto& [fst, snd]: v)
		{
			qDebug() << fst << "=>" << snd.toString();
		}
	} while ((mo = mo->superClass()));
}

QModelIndex getSourceModelIndex(const QModelIndex& index)
{
	// Check if a proxy is used.
	if (const auto apm = dynamic_cast<const QAbstractProxyModel*>(index.model()))
	{
		return apm->mapToSource(index);
	}
	// When not pass the index.
	return index;
}

void doExpandTreeView(QTreeView* tv, bool expand, const QModelIndex& index)// NOLINT(misc-no-recursion)
{
	if (!index.isValid())
	{
		return;
	}
	tv->setExpanded(index, expand);
	const int childCount = index.model()->rowCount(index);
	for (int i = 0; i < childCount; i++)
	{
		doExpandTreeView(tv, expand, tv->model()->index(i, 0, index));
	}
}

void expandTreeView(QTreeView* tv, bool expand, const QModelIndex& index)
{
	if (index.isValid())
	{
		doExpandTreeView(tv, expand, index);
	}
	else
	{
		const int count = tv->model()->rowCount();
		for (int i = 0; i < count; i++)
		{
			doExpandTreeView(tv, expand, tv->model()->index(i, 0));
		}
	}
}

bool isGuiApplication()
{
	return dynamic_cast<QGuiApplication*>(QCoreApplication::instance()) != nullptr;
}

}// namespace sf
