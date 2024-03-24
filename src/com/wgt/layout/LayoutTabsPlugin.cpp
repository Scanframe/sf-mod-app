#include "LayoutTabs.h"
#include "LayoutTabsPlugin.h"

#include <QtPlugin>

namespace sf
{

LayoutTabsPlugin::LayoutTabsPlugin(QObject* parent)
	:QObject(parent)
{
}

void LayoutTabsPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool LayoutTabsPlugin::isInitialized() const
{
	return initialized;
}

QWidget* LayoutTabsPlugin::createWidget(QWidget* parent)
{
	// Design constructor.
	return new LayoutTabs(parent);
}

QString LayoutTabsPlugin::name() const
{
	// Must be the exact full class name including namespace.
	return QStringLiteral("sf::LayoutTabs");
}

QString LayoutTabsPlugin::group() const
{
	return QStringLiteral("ScanFrame Layout Widgets");
}

QIcon LayoutTabsPlugin::icon() const
{
	return {};
}

QString LayoutTabsPlugin::toolTip() const
{
	return {};
}

QString LayoutTabsPlugin::whatsThis() const
{
	return {};
}

bool LayoutTabsPlugin::isContainer() const
{
	return false;
}

QString LayoutTabsPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::LayoutTabs" name="layoutTabs">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>200</width>
    <height>300</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>A frame containing multiple layouts in tabs.</string>
  </property>
  <property name="whatsThis" >
   <string>Special widget containing multiple layouts.</string>
  </property>
  <property name="idVariable">
   <uLongLong>0</uLongLong>
  </property>
  <property name="converted">
   <bool>false</bool>
  </property>
  <property name="readOnly">
   <bool>false</bool>
  </property>
 </widget>
</ui>
)";
}

QString LayoutTabsPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/LayoutTabs.h");
}

}
