#pragma once

#include <QList>
#include <QObject>

#include "AppModuleAction.h"
#include "global.h"

namespace sf
{

class _AMI_CLASS AppModuleActions :public QObject
{
	Q_OBJECT

	public:
		explicit AppModuleActions(QObject* parent);

};

}
