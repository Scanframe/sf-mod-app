#pragma once
#include "InformationPacket.h"
#include "BufferChopper.h"
#include <gii/gen/VariableTypes.h>
#include <gii/gen/ResultDataTypes.h>
#include <QIODevice>

namespace sf
{

/**
 * @brief Class handling the connection exchanging data using a QIODevice.
 */
class ClientConnection :public QObject, protected InformationTypes
{
	public:

		enum EState
		{
			sError = -1,
			sNone = 0,
			sHeaderRead,
			sPayloadRead,
			sPayloadProcess,
			sBufferWrite,
			sIdle,
			sWaitForRead,
			sDisconnect
		};

		/**
		 * @brief Constructor for passing a socket descriptor
		 */
		explicit ClientConnection(QIODevice* ioDevice, QObject* parent = nullptr);

		/**
		 * @brief Non blocking method which processes data incoming and outgoing data from the IO device.
		 *
		 * @return True when it needs to be called again.
		 */
		bool process();

		void setState(EState state);

	private:
		bool sendPingPong(int counter);

		// Holds the IO device reference.
		QIODevice* _ioDevice;
		// Holds the variable client instances which are served.
		VariableTypes::PtrVector _listVariable;
		// Holds the result-data client instances which are served.
		ResultDataTypes::PtrVector _listResultData;
		// Holds the current state of the connection.
		EState _curState{sNone};
		// Holds the previous state of the connection.
		EState _prevState{sNone};
		//
		InformationPacket::Header _headerRead;
		DynamicBuffer _bufferRead;
		DynamicBuffer _bufferWrite;
		//
		BufferChopper bufChopper{16};

};

}
