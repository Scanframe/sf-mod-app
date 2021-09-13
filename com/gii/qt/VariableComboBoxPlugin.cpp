#include "VariableComboBox.h"
#include "VariableComboBoxPlugin.h"

#include <QtPlugin>

namespace sf
{

VariableComboBoxPlugin::VariableComboBoxPlugin(QObject* parent)
	:QObject(parent)
{
}

void VariableComboBoxPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool VariableComboBoxPlugin::isInitialized() const
{
	return initialized;
}

QWidget* VariableComboBoxPlugin::createWidget(QWidget* parent)
{
	// Design constructor.
	return new VariableComboBox(parent);
}

QString VariableComboBoxPlugin::name() const
{
	// Must be the exact full class name including namespace.
	return QStringLiteral("sf::VariableComboBox");
}

QString VariableComboBoxPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon VariableComboBoxPlugin::icon() const
{
	return {};
}

QString VariableComboBoxPlugin::toolTip() const
{
	return {};
}

QString VariableComboBoxPlugin::whatsThis() const
{
	return {};
}

bool VariableComboBoxPlugin::isContainer() const
{
	return false;
}

QString VariableComboBoxPlugin::domXml() const
{
	// The XML part starting with 'propertyspecifications' prevents the 'variableId' QString property
	// from being translated or having those options in the designer.
	return R"(<ui language="c++">
 <widget class="sf::VariableComboBox" name="variableComboBox">
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

QString VariableComboBoxPlugin::includeFile() const
{
	return QStringLiteral("com/gii/qt/VariableComboBox.h");
}

} // namespace
