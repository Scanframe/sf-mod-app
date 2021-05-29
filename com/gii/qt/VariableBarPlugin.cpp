#include "VariableBar.h"
#include "VariableBarPlugin.h"

#include <QtPlugin>

namespace sf
{

VariableBarPlugin::VariableBarPlugin(QObject* parent)
	:QObject(parent)
{
}

void VariableBarPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool VariableBarPlugin::isInitialized() const
{
	return initialized;
}

QWidget* VariableBarPlugin::createWidget(QWidget* parent)
{
	return new VariableBar(parent);
}

QString VariableBarPlugin::name() const
{
	return QStringLiteral("sf::VariableBar");
}

QString VariableBarPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon VariableBarPlugin::icon() const
{
	return QIcon();
}

QString VariableBarPlugin::toolTip() const
{
	return QString();
}

QString VariableBarPlugin::whatsThis() const
{
	return QString();
}

bool VariableBarPlugin::isContainer() const
{
	return false;
}

QString VariableBarPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::VariableBar" name="variableBar">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>100</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>Progress bar</string>
  </property>
  <property name="whatsThis">
   <string>Variable bar used for progress or level indication.</string>
  </property>
 </widget>
 <!-- Propery specification part -->
 <customwidgets>
  <customwidget>
   <class>sf::VariableBar</class>
   <propertyspecifications>
    <stringpropertyspecification name="variableId" notr="true" type="singleline" />
   </propertyspecifications>
  </customwidget>
 </customwidgets>
</ui>
)";
}

QString VariableBarPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/VariableBar.h");
}

} // namespace