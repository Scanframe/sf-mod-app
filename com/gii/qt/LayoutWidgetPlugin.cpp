#include "LayoutWidget.h"
#include "LayoutWidgetPlugin.h"

#include <QtPlugin>

namespace sf
{

LayoutWidgetPlugin::LayoutWidgetPlugin(QObject* parent)
	:QObject(parent)
{
}

void LayoutWidgetPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool LayoutWidgetPlugin::isInitialized() const
{
	return initialized;
}

QWidget* LayoutWidgetPlugin::createWidget(QWidget* parent)
{
	return new LayoutWidget(parent);
}

QString LayoutWidgetPlugin::name() const
{
	return QStringLiteral("sf::LayoutWidget");
}

QString LayoutWidgetPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon LayoutWidgetPlugin::icon() const
{
	return {};
}

QString LayoutWidgetPlugin::toolTip() const
{
	return {};
}

QString LayoutWidgetPlugin::whatsThis() const
{
	return {};
}

bool LayoutWidgetPlugin::isContainer() const
{
	return true;
}

QString LayoutWidgetPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::LayoutWidget" name="LayoutWidget">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>200</width>
    <height>100</height>
   </rect>
  </property>
 </widget>
</ui>
)";
}

QString LayoutWidgetPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/LayoutWidget.h");
}

}
