#include "ClientConnection.h"
#include "InformationPacket.h"
#include "misc/qt/qt_utils.h"
#include <QThread>
#include <QDateTime>
#include <QAbstractSocket>

namespace sf
{

ClientConnection::ClientConnection(QIODevice* ioDevice, QObject* parent)
	:QObject(parent)
	 , _ioDevice(ioDevice)
{
}

void ClientConnection::setState(ClientConnection::EState state)
{
	_prevState = _curState;
	_curState = state;
	if (state == sError)
	{
		if (auto socket = dynamic_cast<QAbstractSocket*>(_ioDevice))
		{
			qWarning() << "Socket State:" << enumToKey(socket->state()) << "Error: " << socket->errorString();
		}
	}
}

bool ClientConnection::process()
{
	switch (_curState)
	{
		case sError:
			// Returning false means disconnecting.
			return false;

		case sNone:
			qDebug() << SF_RTTI_NAME(*this).c_str() << "Initiate Ping Pong...";
			setState(sendPingPong(3) ? sBufferWrite : sError);
			break;

		case sIdle:
			qDebug() << SF_RTTI_NAME(*this).c_str() << "Idle state...";
			setState(sHeaderRead);
			break;

		case sWaitForRead:
			// Wait for a certain time for data to read.
			if (!_ioDevice->waitForReadyRead(100))
			{
				break;
			}
			qDebug() << SF_RTTI_NAME(*this).c_str() << "Bytes available: " << _ioDevice->bytesAvailable();
			// Return to th previous state.
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
				auto pp = _bufferRead.ptr<InformationPacket::PayloadPingPong>();
				qInfo() << SF_RTTI_NAME(*this).c_str() << "PingPong(" << pp->counter << "): " << pp->str;
				if (pp->counter <= 0)
				{
					//setState(sIdle);
					setState(sDisconnect);
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
		{
			// Check if all is written.
			if (bufChopper.isDone())
			{
				setState(sIdle);
				break;
			}
			// Write a chuck of data.
			auto sz = _ioDevice->write(bufChopper.getChunkData(), (qint64) bufChopper.getChunkSize());
			//qInfo() << SF_RTTI_NAME(*this).c_str() << "Writing: " << bufChopper.getChunkSize(); // << " Hex: " << hexString(buf, bufChopper.getChopSize());
			if (sz < 0)
			{
				setState(sError);
			}
			else
			{
				// Wait until the data is written.
				if (_ioDevice->waitForBytesWritten(0))
				{
					QThread::msleep(300);
				}
				// Move position to next chunk.
				if (!bufChopper.moveNext())
				{
					// When done set idle.
					setState(sIdle);
				}
			}
			break;
		}

		case sDisconnect:
		{
			return false;
		}
	}
	return true;
}

bool ClientConnection::sendPingPong(int counter)
{
	InformationPacket::Header hdr;
	hdr.type = InformationPacket::tPingPong;
	hdr.sequence = ++_headerRead.sequence;
	hdr.size = sizeof(InformationPacket::PayloadPingPong);
	//
	_bufferWrite.resize(hdr.size);
	auto plw = _bufferWrite.ptr<InformationPacket::PayloadPingPong>();
	plw->counter = counter;
	auto strTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz").toLocal8Bit();
	strncpy(plw->str, strTime.constData(), std::min<size_t>(sizeof(plw->str), strTime.length() + 1));
	// Assign the buffer to the chopper.
	bufChopper.assign(_bufferWrite.c_str(), _bufferWrite.size());
	// Write the header.
	auto sz = _ioDevice->write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
	return  sz >= 0;
}

}
