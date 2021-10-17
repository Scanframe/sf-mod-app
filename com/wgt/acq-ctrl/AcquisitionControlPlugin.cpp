#include "AcquisitionControl.h"
#include "AcquisitionControlPlugin.h"

#include <QtPlugin>

namespace sf
{

AcquisitionControlPlugin::AcquisitionControlPlugin(QObject* parent)
	:QObject(parent)
{
}

void AcquisitionControlPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool AcquisitionControlPlugin::isInitialized() const
{
	return initialized;
}

QWidget* AcquisitionControlPlugin::createWidget(QWidget* parent)
{
	return new AcquisitionControl(parent);
}

QString AcquisitionControlPlugin::name() const
{
	return QStringLiteral("sf::AcquisitionControl");
}

QString AcquisitionControlPlugin::group() const
{
	return QStringLiteral("ScanFrame GII Graphs");
}

QIcon AcquisitionControlPlugin::icon() const
{
	return {};
}

QString AcquisitionControlPlugin::toolTip() const
{
	return {};
}

QString AcquisitionControlPlugin::whatsThis() const
{
	return {};
}

bool AcquisitionControlPlugin::isContainer() const
{
	return false;
}

QString AcquisitionControlPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::AcquisitionControl" name="acquisitionControl">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>400</width>
    <height>250</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>Acquisition control</string>
  </property>
  <property name="whatsThis">
   <string>Acquisition control using GII interface.</string>
  </property>
 </widget>
</ui>
)";
}


/*
 <!-- Property specification part -->
 <customwidgets>
  <customwidget>
   <class>sf::AcquisitionControl</class>
   <propertyspecifications>
    <stringpropertyspecification name="variableId" notr="true" type="singleline" />
   </propertyspecifications>
  </customwidget>
 </customwidgets>

 */

QString AcquisitionControlPlugin::includeFile() const
{
	return QStringLiteral("wgt/acq-ctrl/AcquisitionControl.h");
}

}