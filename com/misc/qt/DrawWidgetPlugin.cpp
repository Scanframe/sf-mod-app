#include "DrawWidget.h"
#include "DrawWidgetPlugin.h"

#include <QtPlugin>

namespace sf
{

DrawWidgetPlugin::DrawWidgetPlugin(QObject* parent)
	:QObject(parent)
{
}

void DrawWidgetPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool DrawWidgetPlugin::isInitialized() const
{
	return initialized;
}

QWidget* DrawWidgetPlugin::createWidget(QWidget* parent)
{
	return new DrawWidget(parent);
}

QString DrawWidgetPlugin::name() const
{
	return QStringLiteral("sf::DrawWidget");
}

QString DrawWidgetPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon DrawWidgetPlugin::icon() const
{
	return QIcon();
}

QString DrawWidgetPlugin::toolTip() const
{
	return QString();
}

QString DrawWidgetPlugin::whatsThis() const
{
	return QString();
}

bool DrawWidgetPlugin::isContainer() const
{
	return false;
}

QString DrawWidgetPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::DrawWidget" name="drawWidget">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>100</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>Widget for drawing</string>
  </property>
  <property name="whatsThis">
   <string>Widget for drawing stuff into.</string>
  </property>
 </widget>
</ui>
)";
}

QString DrawWidgetPlugin::includeFile() const
{
	return QStringLiteral("misc/qt/DrawWidget.h");
}

} // namespace