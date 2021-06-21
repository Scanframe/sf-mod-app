#include "ActionButton.h"
#include "ActionButtonPlugin.h"

#include <QtPlugin>

namespace sf
{

ActionButtonPlugin::ActionButtonPlugin(QObject* parent)
	:QObject(parent)
{
}

void ActionButtonPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
	if (initialized)
	{
		return;
	}
	initialized = true;
}

bool ActionButtonPlugin::isInitialized() const
{
	return initialized;
}

QWidget* ActionButtonPlugin::createWidget(QWidget* parent)
{
	return new ActionButton(parent);
}

QString ActionButtonPlugin::name() const
{
	return QStringLiteral("sf::ActionButton");
}

QString ActionButtonPlugin::group() const
{
	return QStringLiteral("ScanFrame Widgets");
}

QIcon ActionButtonPlugin::icon() const
{
	return QIcon();
}

QString ActionButtonPlugin::toolTip() const
{
	return QString();
}

QString ActionButtonPlugin::whatsThis() const
{
	return QString();
}

bool ActionButtonPlugin::isContainer() const
{
	return false;
}

QString ActionButtonPlugin::domXml() const
{
	return R"(<ui language="c++">
 <widget class="sf::ActionButton" name="actionButton">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>100</width>
    <height>40</height>
   </rect>
  </property>
 </widget>
</ui>
)";
}

QString ActionButtonPlugin::includeFile() const
{
	return QStringLiteral("misc/qt/ActionButton.h");
}

}
