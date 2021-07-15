#include "SfPushButton.h"
#include "SfPushButtonPlugin.h"

#include <QtPlugin>

namespace sf
{

PushButtonPlugin::PushButtonPlugin(QObject* parent)
	:QObject(parent)
{
}

void PushButtonPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}

	initialized = true;
}

bool PushButtonPlugin::isInitialized() const
{
	return initialized;
}

QWidget* PushButtonPlugin::createWidget(QWidget* parent)
{
	return new PushButton(parent);
}

QString PushButtonPlugin::name() const
{
	// Must be the exact full class name including namespace.
	return QStringLiteral("sf::PushButton");
}

QString PushButtonPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon PushButtonPlugin::icon() const
{
	return QIcon();
}

QString PushButtonPlugin::toolTip() const
{
	return QString();
}

QString PushButtonPlugin::whatsThis() const
{
	return QString();
}

bool PushButtonPlugin::isContainer() const
{
	return false;
}

QString PushButtonPlugin::domXml() const
{
	// The XML part starting with 'propertyspecifications' prevents the 'iconSource' QString property from being translated or
	// having those options in the designer.
	return R"(<ui language="c++">
 <widget class="sf::PushButton" name="SfPushButton">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>30</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>A inherited push button</string>
  </property>
  <property name="whatsThis" >
   <string>The a button for testing properties.</string>
  </property>
  <property name="text">
   <string>sfPushButton</string>
  </property>
  <property name="iconSource">
   <string>:logo/ico/scanframe</string>
  </property>
 </widget>
 <!-- Propery specification part -->
 <customwidgets>
  <customwidget>
   <class>sf::PushButton</class>
   <propertyspecifications>
    <stringpropertyspecification name="iconSource" notr="true" type="singleline" />
   </propertyspecifications>
  </customwidget>
 </customwidgets>
</ui>
)";
}

QString PushButtonPlugin::includeFile() const
{
	return QStringLiteral("SfPushButton.h");
}

} // namespace