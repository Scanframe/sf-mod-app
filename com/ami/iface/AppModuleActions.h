#pragma once

#include <QList>
#include <QObject>

#include "global.h"
#include "AppModuleAction.h"

namespace sf
{

class _AMI_CLASS AppModuleActions :public QObject
{
	Q_OBJECT

	public:
		explicit AppModuleActions(QObject* parent);

};

}
