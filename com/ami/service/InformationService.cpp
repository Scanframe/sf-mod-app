#include "InformationService.h"
#include "SocketServer.h"
#include "SocketClient.h"
#include <misc/gen/Sustain.h>
#include <misc/gen/ElapseTimer.h>

namespace sf
{

struct InformationService::Private :QObject
{
	InformationService& _s;
	// Holds the type of the service connection.
	EServiceType _type{None};
	// Holds the id-offset for the information instances imported by the connected client.
	Gii::IdType _idOffset{0};
	// Set to true when service properties changed and reinitialization is needed.
	bool _reinitialize{false};
	//
	SocketServer* _server{nullptr};
	//
	SocketClient* _client{nullptr};
	// Hook for the sustain interface.
	TSustain<InformationService::Private> _sustain;
	//
	ElapseTimer _timer{};
	// Constructor.
	explicit Private(InformationService* service);

	// Sustain function for handling property changes.
	bool sustain(const timespec& t);
};

InformationService::Private::Private(InformationService* service)
	:QObject(service)
	 , _s(*service)
	 , _sustain(this, &InformationService::Private::sustain)
{
}

bool InformationService::Private::sustain(const timespec& t)
{
	if (_reinitialize)
	{
		_reinitialize = false;
		if (_type == Server || _type == Both)
		{
			if (!_server)
			{
				_server = new SocketServer(this);
			}
		}
		else
		{
			delete_null(_server);
		}
		//
		if (_type == Client || _type == Both)
		{
			if (!_client)
			{
				_client = new SocketClient(this);
				_timer.set(TimeSpec(1.0));
			}
		}
		else
		{
			delete_null(_client);
		}
	}
	//
	if (_timer(t))
	{
		_timer.disable();
		_client->connectHost("localhost", 8001);
	}
	return true;
}

InformationService::InformationService(QObject* parent)
	:QObject(parent)
	 , _p(new Private(this))
{
}

void InformationService::setType(InformationService::EServiceType type)
{
	if (_p->_type != type)
	{
		_p->_type = type;
		_p->_reinitialize = true;
	}
}

InformationService::EServiceType InformationService::getType() const
{
	return _p->_type;
}

void InformationService::setIdOffset(Gii::IdType idOffset)
{
	if (_p->_idOffset != idOffset)
	{
		_p->_idOffset = idOffset;
		_p->_reinitialize = true;
	}
}

Gii::IdType InformationService::getIdOffset() const
{
	return _p->_idOffset;
}

}

