#include "SfAnalogClock.h"
#include "SfAnalogClockPlugin.h"

#include <QtPlugin>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>

namespace sf
{

AnalogClockPlugin::AnalogClockPlugin(QObject* parent)
	:QObject(parent)
{
}

void AnalogClockPlugin::initialize(QDesignerFormEditorInterface* core)
{
	Q_UNUSED(core)
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool AnalogClockPlugin::isInitialized() const
{
	return initialized;
}

QWidget* AnalogClockPlugin::createWidget(QWidget* parent)
{
	return new AnalogClock(parent);
}

QString AnalogClockPlugin::name() const
{
	return QStringLiteral("sf::AnalogClock");
}

QString AnalogClockPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon AnalogClockPlugin::icon() const
{
	return {};
}

QString AnalogClockPlugin::toolTip() const
{
	return {};
}

QString AnalogClockPlugin::whatsThis() const
{
	return {};
}

bool AnalogClockPlugin::isContainer() const
{
	return false;
}

QString AnalogClockPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::AnalogClock" name="analogClock">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>100</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>The current time</string>
  </property>
  <property name="whatsThis">
   <string>The analog clock widget displays the current time.</string>
  </property>
 </widget>
</ui>
)";
}

QString AnalogClockPlugin::includeFile() const
{
	return QStringLiteral("custom-ui-plugin/src/SfAnalogClock.h");
}

} // namespace