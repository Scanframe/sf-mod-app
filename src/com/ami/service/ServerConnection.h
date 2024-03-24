#pragma once
#include "InformationPacket.h"
#include "VariableCollector.h"
#include "misc/gen/ThreadRelay.h"
#include <gii/gen/VariableTypes.h>
#include <gii/gen/ResultDataTypes.h>
#include <QIODevice>

namespace sf
{

/**
 * @brief Class handling the connection exchanging data using a QIODevice.
 */
class ServerConnection :public QObject, protected InformationTypes
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
			sWaitForRead
		};

		/**
		 * @brief Constructor for passing a socket descriptor.
		 */
		explicit ServerConnection(QIODevice* ioDevice, QObject* parent = nullptr);

		~ServerConnection() override;

		/**
		 * @brief Non-blocking method which processes data incoming and outgoing data from the IO device.
		 *
		 * @return True when it needs to be called again.
		 */
		bool process();

		void setState(EState state);

		/**
		 * @brief Allows this instance to do main thread handling.
		 */
		void relayThread();

	private:

		ThreadRelay _relay;

		VariableCollector* _varCollector;

		int fromMain(int v1);

		bool sendPingPong(int counter = -1);

		// Holds the IO device reference.
		QIODevice* _ioDevice;
		// Holds the current state of the connection.
		EState _curState;
		// Holds the previous state of the connection.
		EState _prevState;
		//
		InformationPacket::Header _headerRead;
		DynamicBuffer _bufferRead;
		DynamicBuffer _bufferWrite;
};

}
