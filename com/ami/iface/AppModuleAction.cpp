#include "AppModuleAction.h"
#include "AppModuleActions.h"

namespace sf
{

AppModuleAction::AppModuleAction(AppModuleActions* parent)
	:QObject(parent)
{

}

QAction AppModuleAction::getAction(QObject* parent) const
{
	return QAction(parent);
}

}
