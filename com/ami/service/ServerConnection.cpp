#include "ServerConnection.h"
#include "InformationPacket.h"
#include <QIODevice>

namespace sf
{

ServerConnection::ServerConnection(QIODevice* ioDevice, QObject* parent)
:QObject(parent)
, _ioDevice(ioDevice)
{
}

void ServerConnection::process()
{

}

}
