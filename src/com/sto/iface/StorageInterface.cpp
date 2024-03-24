#include <misc/gen/dbgutils.h>
#include <misc/gen/Exception.h>
#include <misc/gen/Value.h>
#include <misc/gen/TVector.h>
#include <misc/gen/ConfigLocation.h>
#include <misc/gen/IniProfile.h>

#include "StorageInterface.h"

namespace sf
{

// Implementations of static functions and data members to be able to create registered stars implementations.
SF_IMPL_IFACE(StorageInterface, StorageInterface::Parameters, Interface)

StorageInterface::StorageInterface(const StorageInterface::Parameters&)
: _initialized(false)
{
}

StorageInterface::~StorageInterface()
{
	// Should actually be done in the derived class's destructor.
	uinitialize();
}

void StorageInterface::addPropertyPages(PropertySheetDialog* sheet)
{
	// Deliberate not implemented.
}

bool StorageInterface::uinitialize()
{
	if (_initialized)
	{
		_initialized = !doInitialize(false);
	}
	return !_initialized;
}

bool StorageInterface::initialize()
{
	if (!_initialized)
	{
		_initialized = doInitialize(true);
	}
	return _initialized;
}

}
