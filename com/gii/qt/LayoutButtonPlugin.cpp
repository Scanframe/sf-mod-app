#include "LayoutButton.h"
#include "LayoutButtonPlugin.h"

#include <QtPlugin>

namespace sf
{

LayoutButtonPlugin::LayoutButtonPlugin(QObject* parent)
	:QObject(parent)
{
}

void LayoutButtonPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool LayoutButtonPlugin::isInitialized() const
{
	return initialized;
}

QWidget* LayoutButtonPlugin::createWidget(QWidget* parent)
{
	return new LayoutButton(parent);
}

QString LayoutButtonPlugin::name() const
{
	return QStringLiteral("sf::LayoutButton");
}

QString LayoutButtonPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon LayoutButtonPlugin::icon() const
{
	return {};
}

QString LayoutButtonPlugin::toolTip() const
{
	return {};
}

QString LayoutButtonPlugin::whatsThis() const
{
	return {};
}

bool LayoutButtonPlugin::isContainer() const
{
	return false;
}

QString LayoutButtonPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::LayoutButton" name="LayoutButton">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>40</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>Custom push button.</string>
  </property>
  <property name="whatsThis">
   <string>Custom push button connected to a action.</string>
  </property>
 </widget>
 <!-- Property specification part -->
 <customwidgets>
  <customwidget>
   <class>sf::LayoutButton</class>
   <propertyspecifications>
    <stringpropertyspecification name="layoutFile" notr="true" type="singleline" />
   </propertyspecifications>
  </customwidget>
 </customwidgets>
</ui>
)";
}

QString LayoutButtonPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/LayoutButton.h");
}

}
