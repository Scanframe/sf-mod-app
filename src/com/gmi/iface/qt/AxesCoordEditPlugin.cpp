#include "AxesCoordEditPlugin.h"
#include "AxesCoordEdit.h"

#include <QtPlugin>

namespace sf
{

AxesCoordEditPlugin::AxesCoordEditPlugin(QObject* parent)
	: QObject(parent)
{
}

void AxesCoordEditPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool AxesCoordEditPlugin::isInitialized() const
{
	return initialized;
}

QWidget* AxesCoordEditPlugin::createWidget(QWidget* parent)
{
	// Design constructor.
	return new AxesCoordEdit(parent);
}

QString AxesCoordEditPlugin::name() const
{
	// Must be the exact full class name including namespace.
	return QStringLiteral("sf::AxesCoordEdit");
}

QString AxesCoordEditPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon AxesCoordEditPlugin::icon() const
{
	return {};
}

QString AxesCoordEditPlugin::toolTip() const
{
	return {};
}

QString AxesCoordEditPlugin::whatsThis() const
{
	return {};
}

bool AxesCoordEditPlugin::isContainer() const
{
	return false;
}

QString AxesCoordEditPlugin::domXml() const
{
	// The XML part starting with 'propertyspecifications' prevents the 'variableId' QString property
	// from being translated or having those options in the designer.
	return R"(<ui language="c++">
 <widget class="sf::AxesCoordEdit" name="AxesCoordEdit">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>30</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>A frame containing an edit.</string>
  </property>
  <property name="whatsThis" >
   <string>Special edit having name and unit label.</string>
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

QString AxesCoordEditPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/AxesCoordEdit.h");
}

}// namespace sf
