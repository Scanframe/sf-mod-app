#include "Globals.h"

namespace sf
{

// Anonymous namespace.
namespace
{
	QSettings* GlobalSettings{nullptr};
	QWidget* GlobalParent{nullptr};
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

}
