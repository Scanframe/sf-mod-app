#include <QApplication>
#include <QFont>
#include <QStyle>
#include <QSettings>
#include <QMetaEnum>
#include <QDir>
#include <QFileInfo>
#include <QWidget>
#include <QFileSystemWatcher>
#include <gen/dbgutils.h>
#include <QLayout>
#include <QFormLayout>
#include <QFontDatabase>
#include <QTimer>

#include "qt_utils.h"

namespace sf
{

PaletteColors::PaletteColors(const QPalette& palette)
{
	setColors(palette);
}

QPalette PaletteColors::getPalette() const
{
	QPalette palette;
	for (auto& i: _colors)
	{
		palette.setColor(i.first, i.second);
	}
	return palette;
}

void PaletteColors::setColors(const QPalette& palette)
{
	_colors.clear();
	for (int i = 0; i < QPalette::ColorRole::NColorRoles; i++)
	{
		auto role = (QPalette::ColorRole) i;
		_colors.append(Pair(role, palette.color(role)));
	}
}

bool PaletteColors::isEmpty() const
{
	return _colors.isEmpty();
}

void PaletteColors::styleFileDialog(QFileDialog& fd) const
{
	auto pal = getPalette();
	//pal.setColor(QPalette::ColorRole::Button, QColorConstants::DarkCyan);
	fd.setStyleSheet(QString(R"(
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
)")
		.arg(pal.color(QPalette::ColorRole::WindowText).name(QColor::HexArgb))
		.arg(pal.color(QPalette::ColorRole::Button).name(QColor::HexArgb))
		.arg(pal.color(QPalette::ColorRole::ButtonText).name(QColor::HexArgb))
		.arg(pal.color(QPalette::ColorRole::Window).darker(130).name(QColor::HexArgb))
	);
	fd.setPalette(pal);
}

ApplicationSettings::ApplicationSettings(QObject* parent)
	:QObject(parent)
	 , _watcher(new QFileSystemWatcher(this))
{
	connect(_watcher, &QFileSystemWatcher::fileChanged, this, &ApplicationSettings::onFileChance);
}

ApplicationSettings::~ApplicationSettings()
{
	(void) _fileInfo;
}

void ApplicationSettings::onFileChance(const QString& file)
{
	QFileInfo fi(file);
	if (_lastModified < fi.lastModified())
	{
		_lastModified = fi.lastModified();
		// Do not Set file watches.
		doStyleApplication(true, false);
	}
}

void ApplicationSettings::setFilepath(const QString& filepath, bool watch)
{
	_fileInfo.setFile(filepath);
	if (_fileInfo.exists())
	{
		// Remove existing watches.
		if (_watcher->files().length())
		{
			_watcher->removePaths(_watcher->files());
		}
		// Set the styling on the application.
		doStyleApplication(false, watch);
		// When watch
		if (watch)
		{
			// Add the file to watch.
			_watcher->addPath(_fileInfo.absoluteFilePath());
		}
	}
	if (IsDebug())
	{
		for (auto& file: _watcher->files())
		{
			qDebug() << "Watching:" << QString("file://%1").arg(file);
		}
	}
}

const QFileInfo& ApplicationSettings::fileInfo() const
{
	return _fileInfo;
}

void ApplicationSettings::doStyleApplication(bool readOnly, bool watch)
{
	// Form the ini's directory to relate to.
	QString dir = _fileInfo.absoluteDir().absolutePath() + QDir::separator();
	// To use the same ini file in Linux and Windows a different prefix is used.
#if IS_WIN
	QString suffix = "Windows";
#else
	QString suffix = "Linux";
#endif
	// Create the settings instance.
	QSettings settings(_fileInfo.absoluteFilePath(), QSettings::IniFormat);
	// String identifying the current key.
	QString key;
	// Get the current font.
	QFont font = QApplication::font();
	// Start the 'Style-???' ini section.
	settings.beginGroup("Style-" + suffix);
	// Get the keys in the section to check existence in the ini-section.
	auto keys = settings.childKeys();
	// Check if settings can be written and the key does not exist.
	key = "App-Style";
	if (!readOnly && settings.isWritable() && !keys.contains(key))
	{
		settings.setValue(key, QApplication::style()->name());
	}
	auto app_style = settings.value(key, QApplication::style()->name()).toString();
	// Sentry and also the instance.
	if (_systemColors.isEmpty())
	{
		_systemColors.setColors(QApplication::palette());
		QApplication::instance()->setProperty("systemColors", QVariant::fromValue<PaletteColors*>(&_systemColors));
	}
	// Same as above.
	key = "Font-Family";
	if (!readOnly && settings.isWritable() && !keys.contains(key))
	{
		settings.setValue(key, font.family());
	}
	font.setFamily(settings.value(key, font.family()).toString());
//	for (auto fnm: QFontDatabase::families(QFontDatabase::Latin))
//	{
//		qInfo() << fnm;
//	}
	// Same as above.
	key = "Font-PointSize";
	if (!readOnly && settings.isWritable() && !keys.contains(key))
	{
		settings.setValue(key, font.pointSize());
	}
	auto font_size = settings.value(key, font.pointSize()).toInt();
	font.setPointSize(font_size);
	// Same as above.
	key = "StyleSheet";
	if (!readOnly && settings.isWritable() && !keys.contains(key))
	{
		settings.setValue(key, "");
	}
	QString rel_file(settings.value(key, "").toString());
	if (rel_file.length())
	{
		QFile qss(dir + rel_file);
		if (qss.exists())
		{
			if (qss.open(QFile::ReadOnly | QIODevice::Text))
			{
				QByteArray ba = qss.readAll();
				qApp->setStyleSheet(ba); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
			}
		}
		if (watch)
		{
			_watcher->addPath(qss.fileName());
		}
	}
	// End the section.
	settings.endGroup();
	// Start the Palette ini section.
	settings.beginGroup("Palette-" + app_style);
	// Keys to see which ones are missing.
	keys = settings.childKeys();
	auto palette = QApplication::palette();
	auto metaEnum = QMetaEnum::fromType<QPalette::ColorRole>();
	for (int i = 0; i < QPalette::ColorRole::NColorRoles; i++)
	{
		auto role = (QPalette::ColorRole) i;
		// When the key does not exist write it with the current value.
		key = metaEnum.valueToKey(role);
		if (!readOnly && settings.isWritable() && !keys.contains(key))
		{
			settings.setValue(key, palette.color(role).name(QColor::HexArgb));
		}
		palette.setColor(role, QColor(settings.value(key, palette.color(role).name(QColor::HexArgb)).toString()));
	}
	settings.endGroup();
	// Set the application style before the font is set otherwise menu's and some controls got their font sizes reset.
	QApplication::setStyle(app_style);
	// Set the font for the application.
	QApplication::setFont(font);
	// Set the color pallet of the application.
	QApplication::setPalette(palette);
	// Write changes to disk.
	if (!readOnly)
	{
		settings.sync();
	}
}

void ApplicationSettings::restoreWindowRect(const QString& win_name, QWidget* window)
{
	if (window)
	{
		windowState(win_name, window, false);
	}
}

void ApplicationSettings::saveWindowRect(const QString& win_name, QWidget* window)
{
	if (window)
	{
		windowState(win_name, window, true);
	}
}

void ApplicationSettings::windowState(const QString& name, QWidget* widget, bool save)
{
	// Form the ini's directory to relate too.
	QString dir = _fileInfo.absoluteDir().absolutePath() + QDir::separator();
	// Create the settings instance.
	QSettings settings(_fileInfo.absoluteFilePath(), QSettings::IniFormat);
	// Start the ini section.
	settings.beginGroup("WindowState");
	// Form the key.
	QString key = name + "-rectangle";
	//
	if (save)
	{
		settings.setValue(key, widget->geometry());
	}
	else
	{
		// Get the keys in the section to check existence in the ini-section.
		auto keys = settings.childKeys();
		if (keys.contains(key))
		{
			widget->setGeometry(settings.value(key).toRect());
		}
	}
	// End the section.
	settings.endGroup();
}

QMetaObject::Connection connectByName
	(
		const QWidget* widget,
		const QString& sender_name,
		const char* signal_name,
		const QObject* receiver,
		const char* method_name,
		Qt::ConnectionType ct
	)
{
	auto sender = widget->findChild<QObject*>(sender_name);
	if (sender)
	{
		std::string signal(SIGNAL(__s__()));
		std::string slot(SLOT(__m__()));
		//
		std::string::size_type pos = signal.find("__s__");
		//
		if (pos != std::string::npos)
		{
			signal.replace(pos, 5, signal_name);
		}
		pos = slot.find("__m__");
		if (pos != std::string::npos)
		{
			slot.replace(pos, 5, method_name);
		}
		//
		return QObject::connect(sender, signal.c_str(), receiver, slot.c_str(), ct);
	}
	return {};
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

QLayout* getWidgetLayout(QWidget* widget)
{
	if (widget)
	{
		if (auto pw = widget->parentWidget())
		{
			for (auto layout: pw->findChildren<QLayout*>(QString(), Qt::FindChildrenRecursively))
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

int indexFromComboBox(QComboBox* comboBox, const QVariant& value, int default_index)
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

QPair<int, QFormLayout::ItemRole> getLayoutPosition(QFormLayout* layout, QObject* target)
{
	int row = -1;
	QFormLayout::ItemRole role(QFormLayout::SpanningRole);
	if (auto wgt = dynamic_cast<QWidget*>(target))
	{
		layout->getWidgetPosition(wgt, &row, &role);
	}
	else if (auto lo = dynamic_cast<QLayout*>(target))
	{
		layout->getLayoutPosition(lo, &row, &role);
	}
	return {row, role};
}

int getLayoutIndex(QBoxLayout* layout, QObject* target)
{
	if (auto wgt = dynamic_cast<QWidget*>(target))
	{
		return layout->indexOf(wgt);
	}
	else if (auto lo = dynamic_cast<QLayout*>(target))
	{
		return layout->indexOf(lo);
	}
	return -1;
}

void childrenExpandCollapse(QTreeView* treeView, bool expand, const QModelIndex& index) // NOLINT(misc-no-recursion)
{
	if (!index.isValid())
	{
		//childrenExpandCollapse(expand, ui->treeView->rootIndex());
		int count = treeView->model()->rowCount();
		for (int i = 0; i < count; i++)
		{
			childrenExpandCollapse(treeView, expand, treeView->model()->index(i, 0));
		}
		return;
	}
	int childCount = index.model()->rowCount(index);
	for (int i = 0; i < childCount; i++)
	{
		childrenExpandCollapse(treeView, expand, treeView->model()->index(i, 0, index));
	}
	expand ? treeView->expand(index) : treeView->collapse(index);
}

void dumpObjectProperties(QObject* obj)
{
	auto mo = obj->metaObject();
	qDebug() << "## Properties of" << obj;
	do
	{
		qDebug() << "### Class" << mo->className();
		std::vector<std::pair<QString, QVariant> > v;
		v.reserve(mo->propertyCount() - mo->propertyOffset());
		for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i)
		{
			v.emplace_back(mo->property(i).name(), mo->property(i).read(obj));
		}
		//std::sort(v.begin(), v.end());
		for (auto& i: v)
		{
			qDebug() << i.first << "=>" << i.second;
		}
	}
	while ((mo = mo->superClass()));
}

}
