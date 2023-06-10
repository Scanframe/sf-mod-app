#include "FormBuilder.h"
#include "../gen/Exception.h"
#include "ModuleConfiguration.h"
#include "Globals.h"
#include <QWidget>
#include <QCoreApplication>

namespace sf
{

// Anonymous namespace.
namespace
{
	QSettings* GlobalSettings{nullptr};
	QWidget* GlobalParent{nullptr};
	QUiLoader* GlobalUiLoader{nullptr};
	QFormBuilder* GlobalFormBuilder{nullptr};
	QString PluginDir;

	__attribute__((constructor)) void Cleanup()
	{
		delete GlobalFormBuilder;
	}
}

QSettings* setGlobalSettings(QSettings* settings)
{
	std::swap(settings, GlobalSettings);
	return settings;
}

QSettings* getGlobalSettings()
{
	return GlobalSettings;
}

QWidget* setGlobalParent(QWidget* parent)
{
	std::swap(parent, GlobalParent);
	return parent;
}

QWidget* getGlobalParent()
{
	return GlobalParent;
}

QUiLoader* setGlobalUiLoader(QUiLoader* uiLoader)
{
	std::swap(uiLoader, GlobalUiLoader);
	return uiLoader;
}

QUiLoader* getGlobalUiLoader()
{
	if (!GlobalUiLoader)
	{
		throw Exception("Needed global QUiLoader instance not set using setGlobalUiLoader(...).");
	}
	return GlobalUiLoader;
}

QFormBuilder* getGlobalFormBuilder()
{
	// When it does not exist yet, create it.
	if (!GlobalFormBuilder)
	{
		// Create out form builder to load a layout.
		GlobalFormBuilder = new FormBuilder();
		// Add the application directory as the plugin directory to find custom plugins.
		GlobalFormBuilder->addPluginPath(getPluginDir());
	}
	return GlobalFormBuilder;
}

QWidget* FormBuilderLoad(QIODevice* io, QWidget* parent)
{
	// Create widget from the loaded ui-file.
	return getGlobalFormBuilder()->load(io, parent);
}

void FormBuilderSave(QIODevice* io, QWidget* widget)
{
	// Save widget to the ui-file.
	getGlobalFormBuilder()->save(io, widget);
}

void setPluginDir(QString dir)
{
	PluginDir = dir;
}

QString getPluginDir()
{
	return PluginDir.length() ? PluginDir : QCoreApplication::applicationDirPath();
}

}
