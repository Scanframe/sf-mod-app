#include <QtPlugin>
#include <QSettings>
#include "Editor.h"
#include "EditorPlugin.h"

namespace sf
{

EditorPlugin::EditorPlugin(QObject* parent)
	:QObject(parent)
{
}

void EditorPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool EditorPlugin::isInitialized() const
{
	return initialized;
}

QWidget* EditorPlugin::createWidget(QWidget* parent)
{
	return new Editor(parent);
}

QString EditorPlugin::name() const
{
	return QStringLiteral("sf::Editor");
}

QString EditorPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon EditorPlugin::icon() const
{
	return {};
}

QString EditorPlugin::toolTip() const
{
	return {};
}

QString EditorPlugin::whatsThis() const
{
	return {};
}

bool EditorPlugin::isContainer() const
{
	return false;
}

QString EditorPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::Editor" name="editor">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>100</height>
   </rect>
  </property>
  <property name="toolTip" >
   <string>Widget for editing code.</string>
  </property>
  <property name="whatsThis">
   <string>Widget for editing code.</string>
  </property>
 </widget>
</ui>
)";
}

QString EditorPlugin::includeFile() const
{
	return QStringLiteral("misc/qt/Editor.h");
}

}