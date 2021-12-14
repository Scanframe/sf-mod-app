#include "ServerConnection.h"
#include "InformationPacket.h"
#include <QThread>
#include <QDateTime>

namespace sf
{

ServerConnection::ServerConnection(QIODevice* ioDevice, QObject* parent)
	:QObject(parent)
	 , _ioDevice(ioDevice)
	 , _curState(sNone)
	 , _prevState(sNone)
	 ,_varCollector(nullptr)
{
	_relay.makeOwner(Thread::getMainId());
}

ServerConnection::~ServerConnection()
{
	qDebug() << SF_Q_RTTI_TYPENAME << Thread::getCurrentId();
}

void ServerConnection::relayThread()
{
	_relay.checkForWork();
}

int ServerConnection::fromMain(int v1)
{
	int rv;
	// Create the relay instance and use the bool operator to check if the code needs execution.
	if (ThreadRelay::Relay1<ServerConnection, decltype(&ServerConnection::fromMain), int, int>(_relay, this, &ServerConnection::fromMain, rv, v1))
	{
		qDebug() << SF_Q_RTTI_TYPENAME << "==============>> " << v1 << Thread::getCurrentId();
		return Thread::getCurrentId();
	}
	return rv;
}

void ServerConnection::setState(ServerConnection::EState state)
{
	_prevState = _curState;
	_curState = state;
}

bool ServerConnection::process()
{
	//QThread::msleep(100);
	switch (_curState)
	{
		case sError:
			// Returning false means disconnecting.
			return false;

		case sNone:
			setState(sHeaderRead);
			break;

		case sWaitForRead:
			// Wait for a certain time for data to read.
			if (!_ioDevice->waitForReadyRead(300))
			{
				break;
			}
			qDebug() << SF_RTTI_NAME(*this).c_str() << "Bytes available: " << _ioDevice->bytesAvailable();
			// Return to the previous state.
			setState(_prevState);
			break;

		case sHeaderRead:
		{
			auto sz = _ioDevice->bytesAvailable();
			// Wait until there is enough data to read for the header.
			if (sz < sizeof(_headerRead))
			{
				setState(sWaitForRead);
				break;
			}
			// Read the header and check for an error.
			if (_ioDevice->read(reinterpret_cast<char*>(&_headerRead), sizeof(_headerRead)) < 0)
			{
				setState(sError);
			}
			else
			{
				setState(sPayloadRead);
			}
			break;
		}

		case sPayloadRead:
		{
			auto sz = _ioDevice->bytesAvailable();
			// Wait until there is enough data to read for the header.
			if (sz < _headerRead.size)
			{
				setState(sWaitForRead);
				break;
			}
			// Resize the buffer without shrinking to hold the payload.
			_bufferRead.resize(_headerRead.size);
			// Read the payload into the buffer.
			if (_ioDevice->read(_bufferRead.ptr<char>(), (qint64) _bufferRead.size()) < 0)
			{
				setState(sError);
			}
			else
			{
				setState(sPayloadProcess);
			}
			break;
		}

		case sPayloadProcess:
			if (_headerRead.type == InformationPacket::tPingPong)
			{
				auto* pp = _bufferRead.ptr<InformationPacket::PayloadPingPong>();
				qInfo() << SF_RTTI_NAME(*this).c_str() << "PingPong(" << pp->counter << "): " << pp->str;
				fromMain(sPayloadProcess);
				if (pp->counter <= 0)
				{
					setState(sNone);
				}
				else
				{
					setState(sendPingPong(pp->counter - 1) ? sBufferWrite : sError);
				}
			}
			else
			{
				setState(sError);
			}
			break;

		case sBufferWrite:
			if (_ioDevice->write(_bufferWrite.ptr<char>(), (qint64) _bufferWrite.size()) < 0)
			{
				setState(sError);
			}
			else
			{
				setState(sNone);
			}
			break;
	}
	return true;
}

bool ServerConnection::sendPingPong(int counter)
{
	InformationPacket::Header hdr;
	hdr.type = InformationPacket::tPingPong;
	hdr.sequence = ++_headerRead.sequence;
	hdr.size = sizeof(InformationPacket::PayloadPingPong);
	//
	_bufferWrite.resize(hdr.size);
	auto* plw = _bufferWrite.ptr<InformationPacket::PayloadPingPong>();
	plw->counter = counter;
	auto strTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz").toLocal8Bit();
	memcpy(plw->str, strTime.constData(), std::min<size_t>(sizeof(plw->str), strTime.length() + 1));
	// Write the header.
	auto sz = _ioDevice->write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
	return sz >= 0;
}

}
