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
 </widget>
</ui>
)";
}

QString SfPushButtonPlugin::includeFile() const
{
	return QStringLiteral("sfpushbutton.h");
}
