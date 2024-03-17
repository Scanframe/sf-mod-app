#include "StorageInterface.h"
#include "StorageServer.h"

namespace sf
{

StorageServer::StorageServer(id_type deviceNumber, const std::string& serverName)
: _deviceNumber(deviceNumber)
, _serverName(serverName.empty() ? "Storage" : serverName)
{

}

StorageServer::~StorageServer()
{
	// Delete all variables and results for this implementation.
	destroyInterface();
	// If an implementation exists it is deleted.
	delete_null(_implementation);
}

std::string StorageServer::getServerName() const
{
	return _serverName;
}

std::string StorageServer::getImplementationName() const
{
	// Get the current name of the server.
	if (_implementation)
	{
		return StorageInterface::Interface().getRegisterName(_implementation);
	}
	return {};
}

bool StorageServer::createImplementation(const std::string& name)
{
	// If another implementation exists delete it first.
	if (_implementation)
	{
		// Destroy all variables and result-data instances of this server.
		destroyInterface();
		_implementation->uinitialize();
		delete_null(_implementation);
	}
	if (!name.empty())
	{
		int mode = 0;
		// Create a new implementation of a registered class.
		_implementation = StorageInterface::Interface().create(name, StorageInterface::Parameters(mode));
		if (_implementation)
		{
			// Create or check all needed interface items.
			// Initialize the implementation.
			if (_implementation->initialize())
			{
				createInterface();
				return true;
			}
		}
		return false;
	}
	return true;
}

bool StorageServer::createImplementation(int index)
{
	// If another implementation exists delete it first.
	if (_implementation)
	{
		// Destroy all variables and result-data instances of this server.
		destroyInterface();
		// Uninitialize.
		_implementation->uinitialize();
		// Delete the implementation and null the pointer.
		delete_null(_implementation);
	}
	if (index >= 0)
	{
		// Get the number with which it was registered.
		int mode = 0;
		// Create a new implementation of a registered class indicated by index.
		_implementation = StorageInterface::Interface().create(index, StorageInterface::Parameters(mode));
		// Return true if the implementation was created successfully.
		if (_implementation)
		{
			// Initialize.
			_implementation->initialize();
			// Create or check all needed interface items.
			createInterface();
			return true;
		}
	}
	return false;
}


void StorageServer::createInterface()
{
}

void StorageServer::destroyInterface()
{
}

}