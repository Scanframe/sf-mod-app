#include "InformationIdEdit.h"
#include "InformationIdEditPlugin.h"

#include <QtPlugin>

namespace sf
{

InformationIdEditPlugin::InformationIdEditPlugin(QObject* parent)
	:QObject(parent)
{
}

void InformationIdEditPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool InformationIdEditPlugin::isInitialized() const
{
	return initialized;
}

QWidget* InformationIdEditPlugin::createWidget(QWidget* parent)
{
	// Design constructor.
	return new InformationIdEdit(parent);
}

QString InformationIdEditPlugin::name() const
{
	// Must be the exact full class name including namespace.
	return QStringLiteral("sf::InformationIdEdit");
}

QString InformationIdEditPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon InformationIdEditPlugin::icon() const
{
	return {};
}

QString InformationIdEditPlugin::toolTip() const
{
	return {};
}

QString InformationIdEditPlugin::whatsThis() const
{
	return {};
}

bool InformationIdEditPlugin::isContainer() const
{
	return false;
}

QString InformationIdEditPlugin::domXml() const
{
	// The XML part starting with 'propertyspecifications' prevents the 'variableId' QString property
	// from being translated or having those options in the designer.
	return R"(<ui language="c++">
 <widget class="sf::InformationIdEdit" name="informationIdEdit">
  <property name="geometry">
   <rect>
    <width>100</width>
   </rect>
  </property>
  <property name="toolTip" >
   <string>Information ID select edit.</string>
  </property>
  <property name="whatsThis" >
   <string>Information ID select edit with ID select dialog.</string>
  </property>
 </widget>
 <!-- Property specification part -->
 <customwidgets>
  <customwidget>
   <class>sf::VariableEdit</class>
   <propertyspecifications>
    <stringpropertyspecification name="text" notr="true" type="singleline" />
   </propertyspecifications>
  </customwidget>
 </customwidgets>
</ui>
)";
}

QString InformationIdEditPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/InformationIdEdit.h");
}

} // namespace
