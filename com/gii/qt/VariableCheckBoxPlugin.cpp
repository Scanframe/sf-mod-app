#include "VariableCheckBox.h"
#include "VariableCheckBoxPlugin.h"

#include <QtPlugin>

namespace sf
{

VariableCheckBoxPlugin::VariableCheckBoxPlugin(QObject* parent)
	:QObject(parent)
{
}

void VariableCheckBoxPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool VariableCheckBoxPlugin::isInitialized() const
{
	return initialized;
}

QWidget* VariableCheckBoxPlugin::createWidget(QWidget* parent)
{
	// Design constructor.
	return new VariableCheckBox(parent);
}

QString VariableCheckBoxPlugin::name() const
{
	// Must be the exact full class name including namespace.
	return QStringLiteral("sf::VariableCheckBox");
}

QString VariableCheckBoxPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon VariableCheckBoxPlugin::icon() const
{
	return {};
}

QString VariableCheckBoxPlugin::toolTip() const
{
	return {};
}

QString VariableCheckBoxPlugin::whatsThis() const
{
	return {};
}

bool VariableCheckBoxPlugin::isContainer() const
{
	return false;
}

QString VariableCheckBoxPlugin::domXml() const
{
	// The XML part starting with 'propertyspecifications' prevents the 'variableId' QString property
	// from being translated or having those options in the designer.
	return R"(<ui language="c++">
 <widget class="sf::VariableCheckBox" name="variableCheckBox">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>30</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>A frame containing a check box.</string>
  </property>
  <property name="whatsThis" >
   <string>Special check box having name label.</string>
  </property>
  <property name="variableId">
   <string></string>
  </property>
  <property name="converted">
   <bool>false</bool>
  </property>
  <property name="readOnly">
   <bool>false</bool>
  </property>
 </widget>
 <!-- Propery specification part -->
 <customwidgets>
  <customwidget>
   <class>sf::VariableCheckBox</class>
   <propertyspecifications>
    <stringpropertyspecification name="variableId" notr="true" type="singleline" />
   </propertyspecifications>
  </customwidget>
 </customwidgets>
</ui>
)";
}

QString VariableCheckBoxPlugin::includeFile() const
{
	return QStringLiteral("com/gii/qt/VariableCheckBox.h");
}

} // namespace
