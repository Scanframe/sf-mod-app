#pragma once
#include <QList>
#include <QObject>
#include <ami/iface/AppModuleAction.h>
#include <ami/iface/global.h>

namespace sf
{

class _AMI_CLASS AppModuleActions
	: public QObject
{
		Q_OBJECT

	public:
		explicit AppModuleActions(QObject* parent);
};

}// namespace sf
