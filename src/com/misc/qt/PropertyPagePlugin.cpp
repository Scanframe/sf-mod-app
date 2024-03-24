#include <QtPlugin>
#include <QSettings>
#include "PropertyPage.h"
#include "PropertyPagePlugin.h"

namespace sf
{

PropertyPagePlugin::PropertyPagePlugin(QObject* parent)
	:QObject(parent)
{
}

void PropertyPagePlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool PropertyPagePlugin::isInitialized() const
{
	return initialized;
}

QWidget* PropertyPagePlugin::createWidget(QWidget* parent)
{
	return new PropertyPage(parent);
}

QString PropertyPagePlugin::name() const
{
	return QStringLiteral("sf::PropertyPage");
}

QString PropertyPagePlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon PropertyPagePlugin::icon() const
{
	return {};
}

QString PropertyPagePlugin::toolTip() const
{
	return {};
}

QString PropertyPagePlugin::whatsThis() const
{
	return {};
}

bool PropertyPagePlugin::isContainer() const
{
	return true;
}

QString PropertyPagePlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::PropertyPage" name="propertyPage">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>100</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>Widget for editing code.</string>
  </property>
  <property name="whatsThis">
   <string>Widget for editing code.</string>
  </property>
 </widget>
</ui>
)";
}

QString PropertyPagePlugin::includeFile() const
{
	return QStringLiteral("misc/qt/PropertyPage.h");
}

}