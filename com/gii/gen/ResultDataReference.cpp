#include <misc/gen/dbgutils.h>
#include "ResultDataReference.h"
#include "ResultData.h"
#include "ResultDataStatic.h"

namespace sf
{

ResultDataReference::ResultDataReference()
	:_data(nullptr)
	 , _rangeManager(nullptr)
{
	_valid = 0;
	_id = 0;
	_type = rtInvalid;
	_description = "<INVALID>";
	_name = _description;
	_flags = 0;
	_curFlags = 0;
	_significantBits = 0;
	_offset = 0;
	// Add to global reference list declared in TResultDataCommon
	ResultDataStatic::_references.add(this);
	// Create the range manager for this instance.
	_rangeManager = new RangeManager();
	// Enable auto increment of the managed range.
	_rangeManager->setAutoManaged(true);
}

ResultDataReference::~ResultDataReference()
{
	// If instances are still referenced these must be removed first except if it is ZeroResultData itself.
	if (_id != 0)
	{
		size_t count = _list.count();
		while (count--)
		{
			// Attach result to zero result data which is the default and the invalid one.
			if (_list[count])
			{
				_list[count]->setup(0, false);
			}
		}
	}
	else
	{ // Check if there are still results in the system when zero result is destructed.
		SF_COND_RTTI_NOTIFY(ResultData::getInstanceCount(), DO_CERR | DO_COUT,
			"Dangling Instances In The System!");
	}
	// Remove from global reference list declared in TResultDataCommon.
	ResultDataStatic::_references.detach(this);
	// Delete the allocated range manager.
	delete _rangeManager;
	// Delete the allocated data storage container.
	delete _data;
}

}
