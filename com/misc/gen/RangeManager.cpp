#include <string>
#include "dbgutils.h"
#include "Range.h"
#include "RangeManager.h"

namespace sf
{

bool RangeManager::isAccessible(const Range& r) const
{
	// Check if the requested range is within the managed range.
	if (!_managedRange.isWithinOther(r))
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Request" << r << " Out of the managed range!" << _managedRange)
		return false;
	}
	// Check if the requested range is part of the accessible ranges.
	Range::Iterator i(_accessibles);
	while (i)
	{
		if (i++.isWithinOther(r))
		{
			// The passed range is accessible.
			_RTTI_NOTIFY(DO_DEFAULT, "Request" << r << " Is accessible.")
			return true;
		}
	}
	// Return false to signal that the range is not accessible.
	return false;
}

bool RangeManager::isFlushable()
{
	// Check if flush has any effect
	return
		!_managedRange.isEmpty()
			|| _accessibles.count()
			|| _requests.count()
			|| _actualRequests.count();
}

bool RangeManager::flush()
{
	if (isFlushable())
	{
		// Clear all ranges and range lists.
		_managedRange.clear();
		_accessibles.flush();
		_requests.flush();
		_actualRequests.flush();
		return true;
	}
	return false;
}

void RangeManager::flushRequests(Range::id_type id)
{
	auto count = _requests.count();
	while (count)
	{
		if (_requests[--count].getId() == id)
		{
			_requests.detach(count);
		}
	}
}

RangeManager::EResult RangeManager::request(const Range& r, Range::Vector& rrl)
{
	// Check if the requested range is within the managed range first.
	if (!_managedRange.isWithinSelf(r) || _managedRange.isEmpty())
	{
		_COND_RTTI_NOTIFY(_flagLog, DO_CLOG, r << " Out of the managed range " << _managedRange << "!")
		return rmOutOfRange;
	}
	// Check if the requested range is part of the accessible ranges secondly.
	for (auto& i:_accessibles)
	{
		if (i.isWithinSelf(r))
		{
			_COND_RTTI_NOTIFY(_flagLog, DO_CLOG, r << " Is already accessible.")
			// Request is accessible.
			return rmAccessible;
		}
	}
	// Create range list for the requested range. It could split up several ranges.
	Range::Vector rl{{r}};
	// Narrow the request down to what is missing from the accessible list.
	rl.exclude(_accessibles);
	// Check if the request resolved.
	if (rl.empty())
	{
		_COND_RTTI_NOTIFY(_flagLog, DO_CLOG, r << " Is accessible a second time!")
		// Request is accessible.
		return rmAccessible;
	}
	// If the requested range is not accessible yet add it to the request list.
	_requests.add(r);
	// Narrow the request down again to what is missing from the already requested list.
	rl.exclude(_actualRequests);
	// Check if the request was resolved.
	if (rl.empty())
	{
		_COND_RTTI_NOTIFY(_flagLog, DO_CLOG, r << " Is already requested in some way!")
		return rmInaccessible;
	}
	// Add the real requests to the passed range vector.
	_actualRequests.add(rl);
	// Also add the requested ranges to the return value.
	rrl.add(rl);
	// Log a notification.
	_COND_RTTI_NOTIFY(_flagLog, DO_CLOG, r << " Generated Requests " << rl)
	// Execution went well.
	return rmInaccessible;
}

bool RangeManager::setAccessible(const Range& r, Range::Vector& rl_req)
{
	// Put passed range in a vector and call the other method.
	return setAccessible(Range::Vector{{r}}, rl_req);
}

bool RangeManager::setAccessible(const Range::Vector& rl, Range::Vector& rl_req)
{
	// Increment the manged range if the FlagAutoManagedRange is true.
	if (_autoManaged)
	{
		for (auto& i: rl)
		{
			_managedRange += i;
		}
	}
	// Exclude the the range sections that have become accessible from the current actual requests.
	_actualRequests.exclude(rl);
	// Merge the new accessibles with the current ones.
	_accessibles.merge(rl);
	// Get the current size.
	auto sz = rl_req.size();
	// Extract the fulfilled requests.
	_requests = _requests.extract(_accessibles, rl_req);
	// Return true if requests were fulfilled.
	return rl_req.size() > sz;
}

void RangeManager::unitTest(Range::Vector* accessibles, Range::Vector* actual_requests, Range::Vector* requests)
{
	if (accessibles)
	{
		_accessibles = *accessibles;
	}
	if (requests)
	{
		_requests = *requests;
	}
	if (actual_requests)
	{
		_actualRequests = *actual_requests;
	}
}

}
