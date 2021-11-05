#pragma once

#include <gii/gen/VariableTypes.h>
#include <gii/gen/ResultDataTypes.h>
#include <QThread>

namespace sf
{

/**
 * @brief Class handling the connection exchanging data using a QIODevice.
 */
class ServerConnection :public QObject, protected InformationTypes
{
	public:
		/**
		 * @brief Constructor for passing a socket descriptor
		 */
		explicit ServerConnection(QIODevice* ioDevice, QObject* parent = nullptr);

	protected:
		/**
		 * @brief Processes data incoming and outgoing data.
		 */
		void process();

	private:
		// Holds the IO device reference.
		QIODevice* _ioDevice;
		// Holds the variable client instances which are served.
		VariableTypes::PtrVector _listVariable;
		// Holds the result-data client instances which are served.
		ResultDataTypes::PtrVector _listResultData;
};

}
