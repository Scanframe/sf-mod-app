#include "ApplicationSettings.h"
#include "misc/gen/dbgutils.h"
#include "qt_utils.h"
#include <QApplication>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QFont>
#include <QHeaderView>
#include <QStyle>
#if IS_WIN
	#include "win/win_utils.h"
#endif

namespace sf
{

ApplicationSettings::ApplicationSettings(QObject* parent)
	: QObject(parent)
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
	const QFileInfo fi(file);
	if (_lastModified < fi.lastModified())
	{
		_lastModified = fi.lastModified();
		// Do not Set file watches.
		doStyleApplication(true, false);
	}
}

QSettings ApplicationSettings::getSettings() const
{
	return QSettings(_fileInfo.absoluteFilePath(), QSettings::IniFormat);
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
	if (isDebug())
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
	// Bailout if this is not a GUI application.
	if (!isGuiApplication())
	{
		return;
	}
	// Form the ini-file's directory to relate to.
	const QString dir = _fileInfo.absoluteDir().absolutePath() + QDir::separator();
	// Create the settings instance.
	auto settings = getSettings();
	// String identifying the current key.
	QString key;
	// Get the current font.
	QFont font = QApplication::font();
// To use the same ini file in Linux and Windows a different prefix is used.
#if IS_WIN
	QString suffix = isRunningWine() ? "Wine" : "Windows";
#else
	QString suffix = "Linux";
#endif
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
	const auto app_style = settings.value(key, QApplication::style()->name()).toString();
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
	const auto font_size = settings.value(key, font.pointSize()).toInt();
	font.setPointSize(font_size);
	// Same as above.
	key = "StyleSheet";
	if (!readOnly && settings.isWritable() && !keys.contains(key))
	{
		settings.setValue(key, "");
	}
	const QString rel_file(settings.value(key, "").toString());
	if (rel_file.length())
	{
		QFile qss(dir + rel_file);
		if (qss.exists())
		{
			if (qss.open(QFile::ReadOnly | QIODevice::Text))
			{
				const QByteArray ba = qss.readAll();
				qApp->setStyleSheet(ba);// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
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
	const auto metaEnum = QMetaEnum::fromType<QPalette::ColorRole>();
	for (int i = 0; i < QPalette::ColorRole::NColorRoles; i++)
	{
		const auto role = static_cast<QPalette::ColorRole>(i);
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

void ApplicationSettings::restoreWindowRect(const QString& win_name, QWidget* window) const
{
	windowState(win_name, window, false);
}

void ApplicationSettings::saveWindowRect(const QString& win_name, QWidget* window) const
{
	windowState(win_name, window, true);
}

void ApplicationSettings::windowState(const QString& name, QWidget* widget, bool save) const
{
	// No widget set bailout.
	if (!widget) return;
	// Form the ini-file's directory to relate too.
	QString dir = _fileInfo.absoluteDir().absolutePath() + QDir::separator();
	// Create the settings instance.
	auto settings = getSettings();
	// Start the ini section.
	settings.beginGroup("WindowState");
	// Form the key.
	const QString key = name + "-rectangle";
	//
	if (save)
	{
		settings.setValue(key, widget->geometry());
	}
	else
	{
		// Get the keys in the section to check existence in the ini-section.
		const auto keys = settings.childKeys();
		if (keys.contains(key))
		{
			widget->setGeometry(settings.value(key).toRect());
		}
	}
	// End the section.
	settings.endGroup();
}

void ApplicationSettings::treeViewColumns(const QString& name, const QTreeView* tv, bool save) const
{
	// No tree view set bailout.
	if (!tv) return;
	// Create the settings instance.
	auto settings = getSettings();
	// Start the ini section.
	settings.beginGroup("WindowState");
	auto* header = tv->header();
	for (int i = 0; i < header->count(); ++i)
	{
		auto key = QString("%1-Col%2Width").arg(name).arg(i);
		if (save) settings.setValue(key, header->sectionSize(i));
		else
		{
			auto width = settings.value(key);
			if (width.isValid())
			{
				header->resizeSection(i, width.toInt());
			}
		}
	}
	settings.endGroup();
}

void ApplicationSettings::restoreTreeViewColumns(const QString& name, const QTreeView* tv) const
{
	treeViewColumns(name, tv, false);
}

void ApplicationSettings::saveTreeViewColumns(const QString& name, const QTreeView* tv) const
{
	treeViewColumns(name, tv, true);
}

QMetaObject::Connection connectByName(
	const QWidget* widget, const QString& sender_name, const char* signal_name, const QObject* receiver, const char* method_name, Qt::ConnectionType ct
)
{
	if (const auto sender = widget->findChild<QObject*>(sender_name))
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

}// namespace sf
