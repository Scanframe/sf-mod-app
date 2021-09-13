#pragma once

#include <gii/gen/InformationBase.h>
#include "global.h"

namespace sf
{

class StorageInterface;

/**
 * @brief Storage server
 */
class _STO_CLASS StorageServer :public InformationTypes
{
	public:
		/**
		 * @brief Constructor.
		 */
		explicit StorageServer(id_type deviceNumber, const std::string& serverName = {});

		/**
		 * Destructor.
		 */
		~StorageServer();

		/**
		 * Gets the name given at the constructor.
		 */
		[[nodiscard]] std::string getServerName() const;

		/**
		 * @brief Creates an attached implementation by name.
		 */
		bool createImplementation(const std::string& name);

		/**
		 * @brief Gets the name of the current implementation.
		 */
		[[nodiscard]] std::string getImplementationName() const;

		/**
		 * @brief Destroys the created implementation.
		 */
		void destroyImplementation()
		{
			createImplementation(-1);
		}

		/**
		 * @brief Gets a pointer to the current acquisition implementation.
		 */
		StorageInterface* getImplementation() {return _implementation;}

	protected:

		/**
		 * @brief Creates the implementation of index on the registered implementations.
		 *
		 * @return True on success.
		 */
		bool createImplementation(int index);

		/**
		 * @brief Create interface parameters at construction once.
		 */
		void createInterface();

		/**
		 * @brief Remove all parameters and results.
		 */
		void destroyInterface();

		/**
		 * @brief Holds the implementation.
		 */
		StorageInterface* _implementation{nullptr};

		/**
		 * @brief Holds the device number passed at the constructor.
		 */
		id_type _deviceNumber{0};

		/**
		 * @brief Holds the device name for creating the variable names.
		 */
		std::string _serverName;
};

}
