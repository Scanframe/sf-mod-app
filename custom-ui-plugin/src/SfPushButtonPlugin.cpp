#include "SfPushButton.h"
#include "SfPushButtonPlugin.h"

#include <QtPlugin>

SfPushButtonPlugin::SfPushButtonPlugin(QObject* parent)
	:QObject(parent)
{
}

void SfPushButtonPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}

	initialized = true;
}

bool SfPushButtonPlugin::isInitialized() const
{
	return initialized;
}

QWidget* SfPushButtonPlugin::createWidget(QWidget* parent)
{
	return new SfPushButton(parent);
}

QString SfPushButtonPlugin::name() const
{
	return QStringLiteral("SfPushButton");
}

QString SfPushButtonPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon SfPushButtonPlugin::icon() const
{
	return QIcon();
}

QString SfPushButtonPlugin::toolTip() const
{
	return QString();
}

QString SfPushButtonPlugin::whatsThis() const
{
	return QString();
}

bool SfPushButtonPlugin::isContainer() const
{
	return false;
}

QString SfPushButtonPlugin::domXml() const
{
	// The XML part starting with 'propertyspecifications' prevents the 'iconSource' QString property from being translated or
	// having those options in the designer.
	return R"(<ui language="c++">
 <widget class="SfPushButton" name="sfPushButton">
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
   <string>SfPushButton</string>
  </property>
  <property name="iconSource">
   <string>:/logo/scanframe</string>
  </property>
 </widget>
 <!-- Propery specification part -->
 <customwidgets>
  <customwidget>
   <class>SfPushButton</class>
   <propertyspecifications>
    <stringpropertyspecification name="iconSource" notr="true" type="singleline" />
   </propertyspecifications>
  </customwidget>
 </customwidgets>
</ui>
)";
}
/*

<ui language="c++"> displayname="MyWidget">
    <widget class="mynamespace::MyWidget" name="mywidget"/>
    <customwidgets>
        <customwidget>
            <class>mynamespace::MyWidget</class>
            <propertyspecifications>
                <stringpropertyspecification name="exampleProperty" notr="true" type="singleline" />
            </propertyspecifications>
        </customwidget>
    </customwidgets>
</ui>

*/


QString SfPushButtonPlugin::includeFile() const
{
	return QStringLiteral("sfpushbutton.h");
}
