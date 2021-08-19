#include "AscanGraph.h"
#include "AscanGraphPlugin.h"

#include <QtPlugin>

namespace sf
{

AscanGraphPlugin::AscanGraphPlugin(QObject* parent)
	:QObject(parent)
{
}

void AscanGraphPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool AscanGraphPlugin::isInitialized() const
{
	return initialized;
}

QWidget* AscanGraphPlugin::createWidget(QWidget* parent)
{
	return new AscanGraph(parent);
}

QString AscanGraphPlugin::name() const
{
	return QStringLiteral("sf::AscanGraph");
}

QString AscanGraphPlugin::group() const
{
	return QStringLiteral("ScanFrame GII Graphs");
}

QIcon AscanGraphPlugin::icon() const
{
	return {};
}

QString AscanGraphPlugin::toolTip() const
{
	return {};
}

QString AscanGraphPlugin::whatsThis() const
{
	return {};
}

bool AscanGraphPlugin::isContainer() const
{
	return false;
}

QString AscanGraphPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::AscanGraph" name="ascanGraph">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>400</width>
    <height>250</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>A-scan graph</string>
  </property>
  <property name="whatsThis">
   <string>A-scan graph using GII interface.</string>
  </property>
 </widget>
 <!-- Propery specification part -->
</ui>
)";
}


/*

 <customwidgets>
  <customwidget>
   <class>sf::AscanGraph</class>
   <propertyspecifications>
    <stringpropertyspecification name="variableId" notr="true" type="singleline" />
   </propertyspecifications>
  </customwidget>
 </customwidgets>

 */

QString AscanGraphPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/AscanGraph.h");
}

}