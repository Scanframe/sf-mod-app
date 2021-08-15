#include <misc/gen/dbgutils.h>
#include "ResultDataReference.h"
#include "ResultDataStatic.h"

namespace sf
{

ResultDataReference::ResultDataReference()
{
	_description = "<INVALID>";
	_name = _description;
	// Add to global reference list declared in TResultDataCommon
	ResultDataStatic::_references->add(this);
	// Create the range manager for this instance.
	_rangeManager = new RangeManager();
	// Enable auto increment of the managed range.
	_rangeManager->setAutoManaged(true);
}

ResultDataReference::~ResultDataReference()
{
	// If instance are still referenced these must be removed first except if it is Zero instance itself.
	if (ResultDataStatic::zero()._reference == this)
	{
		// Qt Designer has something weird going on. Memory seems to be garbled.
		if (!ResultDataStatic::_references->empty() && ResultDataStatic::_references->at(0) != this)
		{
			// Notification of warning
			SF_RTTI_NOTIFY(DO_MSGBOX | DO_DEFAULT, "Unexpected Zero pointer not as first in list!");
		}
		else
		{
			// Check if there are still instances in the system when Zero instance is destructed.
			size_t total_count = 0;
			// Iterate through all references and count the usage count.
			for (auto ref: *ResultDataStatic::_references)
			{
				total_count += ref->_list.size();
				// Notification of warning
				// Check if current ref is the Zero instance reference.
				if (ref == ResultDataStatic::zero()._reference)
				{
					// Iterate through each entry in the list.
					size_type count = ref->_list.count();
					// Skip the first one because there is always Zero instance itself.
					for (size_type idx = 1; idx < count; idx++)
					{
						if (ref->_list[idx])
						{
							// Notification of warning
							SF_RTTI_NOTIFY(DO_CLOG, "Dangling instance [" << idx << "/" << count << "] with desired ID 0x"
							<< std::hex << ref->_list[idx]->_desiredId << " in this process!")
						}
						// Limit the amount shown to a maximum of 3.
						if (idx >= 3)
						{
							SF_RTTI_NOTIFY(DO_MSGBOX | DO_DEFAULT, ": Too many [" << count << "] dangling instances to be shown!")
							break;
						}
					}
				}
				else
				{
					SF_RTTI_NOTIFY(DO_MSGBOX | DO_DEFAULT,
						": Dangling reference owning instance with ID 0x" << std::hex << ref->_id << " in this process!")
				}
			}
			// Subtract 1 for zero instance itself. There should only be one left in the list.
			total_count--;
			// Notification of warning
			SF_COND_RTTI_NOTIFY(total_count, DO_MSGBOX | DO_DEFAULT, "Total of " << total_count << " dangling instances in this process!")
		}
	}
	else
	{
		size_type count = _list.size();
		while (count--)
		{
			// Attach instance to Zero which is the default and the invalid one.
			if (_list[count])
			{
				_list[count]->setup(0L, false);
			}
		}
	}
	// Remove from global reference list.
	ResultDataStatic::_references->detach(this);
	// Delete the allocated range manager.
	delete _rangeManager;
	// Delete the allocated data storage container.
	delete _data;
}

}
