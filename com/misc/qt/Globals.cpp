#include <optional>
#include <cassert>
#include <gen/Exception.h>
#include "Globals.h"

namespace sf
{

// Anonymous namespace.
namespace
{
	QSettings* GlobalSettings{nullptr};
	QWidget* GlobalParent{nullptr};
	QUiLoader* GlobalUiLoader{nullptr};
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



}
