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

#include "qt_utils.h"

namespace sf
{

ApplicationSettings::ApplicationSettings(QObject* parent)
	:QObject(parent)
	 , _watcher(new QFileSystemWatcher(this))
{
	connect(_watcher, &QFileSystemWatcher::fileChanged, this, &ApplicationSettings::onFileChance);
}

void ApplicationSettings::onFileChance(const QString& file)
{
	// Do not Set file watches.
	doStyleApplication(false);
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
		doStyleApplication(watch);
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
			qDebug() << "Watching:" << file;
		}
	}
}

const QFileInfo& ApplicationSettings::fileInfo() const
{
	return _fileInfo;
}

void ApplicationSettings::doStyleApplication(bool watch)
{
	// Form the ini's directory to relate too.
	QString dir = _fileInfo.absoluteDir().absolutePath() + QDir::separator();
	// To use the same ini file in Linux and Windows a different prefix is used.
#if IS_WIN
	QString prefix = "Windows";
#else
	QString prefix = "Linux";
#endif
	// Create the settings instance.
	QSettings settings(_fileInfo.absoluteFilePath(), QSettings::IniFormat);
	// String identifying the current key.
	QString key;
	// Get the current font.
	QFont font = QApplication::font();
	// Start the *-Style ini section.
	settings.beginGroup(prefix + "-Style");
	// Get the keys in the section to check existence in the ini-section.
	auto keys = settings.childKeys();
	// Check if settings can be written and the key does not exist.
	key = "App-Style";
	if (settings.isWritable() && !keys.contains(key))
	{
		settings.setValue(key, QApplication::style()->objectName());
	}
	QApplication::setStyle(settings.value(key, QApplication::style()->objectName()).toString());
	// Same as above.
	if (settings.isWritable() && !keys.contains(key = "Font-Family"))
	{
		settings.setValue(key, font.family());
	}
	font.setFamily(settings.value(key, font.family()).toString());
	// Same as above.
	if (settings.isWritable() && !keys.contains(key = "Font-PointSize"))
	{
		settings.setValue(key, font.pointSize());
	}
	font.setPointSize(settings.value(key, font.pointSize()).toInt());
	// Same as above.
	if (settings.isWritable() && !keys.contains(key = "StyleSheet"))
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
	settings.beginGroup("Palette");
	// Keys to see which ones are missing.
	keys = settings.childKeys();
	QPalette palette = QApplication::palette();
	QMetaEnum metaEnum = QMetaEnum::fromType<QPalette::ColorRole>();
	// List all the color enums to write to file.
	for (auto role:
		{
			QPalette::WindowText,
			QPalette::Button,
			QPalette::Light,
			QPalette::Midlight,
			QPalette::Dark,
			QPalette::Mid,
			QPalette::Text,
			QPalette::BrightText,
			QPalette::ButtonText,
			QPalette::Base,
			QPalette::Window,
			QPalette::Shadow,
			QPalette::Highlight,
			QPalette::HighlightedText,
			QPalette::Link,
			QPalette::LinkVisited,
			QPalette::AlternateBase,
			QPalette::NoRole,
			QPalette::ToolTipBase,
			QPalette::ToolTipText,
			QPalette::PlaceholderText,
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
			QPalette::WindowText,
			QPalette::Window
#else
			QPalette::Foreground,
			QPalette::Background
#endif
		})
	{
		// When the key does not exist write it with the current value.
		if (settings.isWritable() && !keys.contains(key = metaEnum.valueToKey(role)))
		{
			settings.setValue(key, palette.color(role).name(QColor::HexArgb));
		}
		palette.setColor(role, QColor(settings.value(key, palette.color(role).name(QColor::HexArgb)).toString()));
	}
	settings.endGroup();
	// SEt the font for the application.
	QApplication::setFont(font);
	// Set the color pallet of the application.
	QApplication::setPalette(palette);
	// Write changes to disk.
	settings.sync();
}

void ApplicationSettings::restoreWindowRect(const QString& win_name, QWidget* window)
{
	windowState(win_name, window, false);
}

void ApplicationSettings::saveWindowRect(const QString& win_name, QWidget* window)
{
	windowState(win_name, window, true);
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
	return QMetaObject::Connection();
}

}
