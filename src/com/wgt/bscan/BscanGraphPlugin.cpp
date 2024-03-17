#include "BscanGraph.h"
#include "BscanGraphPlugin.h"

#include <QtPlugin>

namespace sf
{

BscanGraphPlugin::BscanGraphPlugin(QObject* parent)
	:QObject(parent)
{
}

void BscanGraphPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool BscanGraphPlugin::isInitialized() const
{
	return initialized;
}

QWidget* BscanGraphPlugin::createWidget(QWidget* parent)
{
	return new BscanGraph(parent);
}

QString BscanGraphPlugin::name() const
{
	return QStringLiteral("sf::BscanGraph");
}

QString BscanGraphPlugin::group() const
{
	return QStringLiteral("ScanFrame GII Graphs");
}

QIcon BscanGraphPlugin::icon() const
{
	return {};
}

QString BscanGraphPlugin::toolTip() const
{
	return {};
}

QString BscanGraphPlugin::whatsThis() const
{
	return {};
}

bool BscanGraphPlugin::isContainer() const
{
	return false;
}

QString BscanGraphPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::BscanGraph" name="bscanGraph">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>400</width>
    <height>250</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>B-scan graph</string>
  </property>
  <property name="whatsThis">
   <string>B-scan graph using GII interface.</string>
  </property>
 </widget>
</ui>
)";
}

QString BscanGraphPlugin::includeFile() const
{
	return QStringLiteral("wgt/bscan/BscanGraph.h");
}

}