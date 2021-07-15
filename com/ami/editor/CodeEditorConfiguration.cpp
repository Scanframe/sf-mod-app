#include <QSettings>
#include "CodeEditorConfiguration.h"

sf::CodeEditorConfiguration::CodeEditorConfiguration(QSettings& settings)
	:_settings(settings)
{
}

void sf::CodeEditorConfiguration::settingsReadWrite(bool rw)
{
	_settings.beginGroup("AppModule.CodeEditor.Configuration");
	QString keyDarkMode("DarkMode");
	QString keyHighLight("HighLight");
	QString keyGutter("Gutter");
	QString keyFontType("FontType");
	QString keyFontSize("FontSize");
	if (rw)
	{
		showGutter = _settings.value(keyGutter, showGutter).toBool();
		darkMode = _settings.value(keyDarkMode, darkMode).toBool();
		fontType = _settings.value(keyFontType, fontType).toString();
		fontSize = _settings.value(keyFontSize, fontSize).toInt();
		useHighLighting = _settings.value(keyHighLight, fontSize).toBool();
	}
	else
	{
		_settings.setValue(keyGutter, showGutter);
		_settings.setValue(keyDarkMode, darkMode);
		_settings.setValue(keyFontType, fontType);
		_settings.setValue(keyFontSize, fontSize);
		_settings.setValue(keyHighLight, useHighLighting);
	}
	_settings.endGroup();
}

void sf::CodeEditorConfiguration::save()
{
	settingsReadWrite(false);
}

void sf::CodeEditorConfiguration::load()
{
	settingsReadWrite(true);
}
