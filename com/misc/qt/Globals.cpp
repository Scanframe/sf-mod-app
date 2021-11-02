#include "Globals.h"
#include "FormBuilder.h"
#include <gen/Exception.h>
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

QWidget* FormBuilderLoad(QIODevice* io, QWidget* parent)
{
	if (!GlobalFormBuilder)
	{
		// Create out form builder to load a layout.
		GlobalFormBuilder = new FormBuilder();
		// Add the application directory as the plugin directory to find custom plugins.
		GlobalFormBuilder->addPluginPath(QCoreApplication::applicationDirPath());
	}
	// Create widget from the loaded ui-file.
	return GlobalFormBuilder->load(io, parent);
}

void FormBuilderSave(QIODevice* io, QWidget* widget)
{
	if (!GlobalFormBuilder)
	{
		// Create out form builder to load a layout.
		GlobalFormBuilder = new FormBuilder();
		// Add the application directory as the plugin directory to find custom plugins.
		GlobalFormBuilder->addPluginPath(QCoreApplication::applicationDirPath());
	}
	// Save widget to the ui-file.
	GlobalFormBuilder->save(io, widget);
}

}
