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
	SignificantBits = 0;
	Offset = 0;
	// Create reference list if it not exts yet.
	if (!ResultDataStatic::RefList)
	{
		ResultDataStatic::RefList = new ReferenceVector;
		ResultDataStatic::DeleteWaitCache = new TVector<void*>;
	}
	// Add to global reference list declared in TResultDataCommon
	ResultDataStatic::RefList->add(this);
	// Create the range manager for this instance.
	_rangeManager = new RangeManager();
	// Enable auto increment of the managed range.
	_rangeManager->setAutoManaged(true);
}

ResultDataReference::~ResultDataReference()
{
	// If TResultData instances are still referenced these must be removed first
	// except if it is ZeroResultData itself.
	if (_id != 0L)
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
		_COND_RTTI_NOTIFY(ResultData::getInstanceCount(), DO_MSGBOX | DO_CERR | DO_COUT,
			"Dangling Instances In The System!");
	}
	// Remove from global reference list declared in TResultDataCommon.
	ResultDataStatic::RefList->detach(this);
	// Remove the list itself if the count is zero.
	if (ResultDataStatic::RefList->count() == 0)
	{
		delete_null(ResultDataStatic::RefList);
		delete_null(ResultDataStatic::DeleteWaitCache);
	}
	// Delete the allocated range manager.
	delete_null(_rangeManager);
	// Delete the allocated data storage container.
	delete_null(_data);
}

}
