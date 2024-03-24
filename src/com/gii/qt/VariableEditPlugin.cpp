#include "VariableEdit.h"
#include "VariableEditPlugin.h"

#include <QtPlugin>

namespace sf
{

VariableEditPlugin::VariableEditPlugin(QObject* parent)
	:QObject(parent)
{
}

void VariableEditPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool VariableEditPlugin::isInitialized() const
{
	return initialized;
}

QWidget* VariableEditPlugin::createWidget(QWidget* parent)
{
	// Design constructor.
	return new VariableEdit(parent);
}

QString VariableEditPlugin::name() const
{
	// Must be the exact full class name including namespace.
	return QStringLiteral("sf::VariableEdit");
}

QString VariableEditPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon VariableEditPlugin::icon() const
{
	return {};
}

QString VariableEditPlugin::toolTip() const
{
	return {};
}

QString VariableEditPlugin::whatsThis() const
{
	return {};
}

bool VariableEditPlugin::isContainer() const
{
	return false;
}

QString VariableEditPlugin::domXml() const
{
	// The XML part starting with 'propertyspecifications' prevents the 'variableId' QString property
	// from being translated or having those options in the designer.
	return R"(<ui language="c++">
 <widget class="sf::VariableEdit" name="variableEdit">
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

QString VariableEditPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/VariableEdit.h");
}

} // namespace
