#include "VariableTreeViewPlugin.h"
#include "VariableTreeView.h"

#include <QtPlugin>

namespace sf
{

VariableTreeViewPlugin::VariableTreeViewPlugin(QObject* parent)
	: QObject(parent)
{}

void VariableTreeViewPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool VariableTreeViewPlugin::isInitialized() const
{
	return initialized;
}

QWidget* VariableTreeViewPlugin::createWidget(QWidget* parent)
{
	return new VariableTreeView(parent);
}

QString VariableTreeViewPlugin::name() const
{
	return QStringLiteral("sf::VariableTreeView");
}

QString VariableTreeViewPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon VariableTreeViewPlugin::icon() const
{
	return {};
}

QString VariableTreeViewPlugin::toolTip() const
{
	return {};
}

QString VariableTreeViewPlugin::whatsThis() const
{
	return {};
}

bool VariableTreeViewPlugin::isContainer() const
{
	return false;
}

QString VariableTreeViewPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::VariableTreeView" name="VariableTreeView">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>100</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>Variable table</string>
  </property>
  <property name="whatsThis">
   <string>Variable tree view for viewing and editing variables in a table.</string>
  </property>
 </widget>
</ui>
)";
}

QString VariableTreeViewPlugin::includeFile() const
{
	return QStringLiteral("gii/qt/VariableTree.h");
}

}// namespace sf