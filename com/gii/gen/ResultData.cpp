#include <misc/gen/TVector.h>
#include <misc/gen/dbgutils.h>
#include <misc/gen/RangeManager.h>
#include <misc/gen/csf.h>

#include "ResultData.h"
#include "ResultDataHandler.h"
// Not part of the public interface.
#include "ResultDataStatic.h"
#include "ResultDataReference.h"

namespace sf
{

void ResultData::setDebug(bool debug)
{
	if (_reference)
	{
		_reference->_debug = debug;
	}
}

void ResultData::initialize()
{
	ResultDataStatic::initialize(true);
}

void ResultData::deinitialize()
{
	ResultDataStatic::initialize(false);
}

ResultData::ResultData()
{
	attachRef(ResultDataStatic::zero()._reference);
}

ResultData::ResultData(void*, void*)
{
	if (!ResultDataStatic::_zero)
	{
		ResultDataStatic::_zero = this;
		setup(std::string("0x0,n/a,H,n/a,INT8,2,4,8,0x7F"), 0);
	}
}

void ResultData::operator delete(void* p) // NOLINT(misc-new-delete-overloads)
{
	if (ResultDataStatic::_globalActive)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "Warning: Deleting instance during an event!")
		// Do not free allocated data before end of global event remove
		ResultDataStatic::_deleteWaitCache.add(p);
	}
	else
	{
		// Call real destructor.
		::operator delete(p);
		// If deletion is allowed again delete the wait cache too.
		TVector<void*>::size_type count = ResultDataStatic::_deleteWaitCache.size();
		if (count)
		{
			while (count--)
			{
				delete (char*) ResultDataStatic::_deleteWaitCache[count];
			}
			// Flush the entries from the list.
			ResultDataStatic::_deleteWaitCache.flush();
		}
	}
}

ResultData::~ResultData()
{
/*
	// Remove this clients current unfulfilled requests from the range manager.
	if (!isOwner())
	{
		clearRequests();
	}
*/
	// If a handler exist notify removal of this instance.
	emitEvent(reRemove, *this, Range());
	// Remove the link if there was one and doing so generating an event.
	setHandler(nullptr);
	// Remove this instance from the list by attach reference NULL.
	attachRef(nullptr);
	// When this is the zero variable being deleted null it because it is also used as a sentry.
	if (ResultDataStatic::_zero == this)
	{
		ResultDataStatic::_zero = nullptr;
	}
}

bool ResultData::recycleEnable(bool recycle)
{
	// Only allow when data is available.
	if (!_reference->_data->getSegmentCount())
	{
		// Should recycling be enabled.
		if (recycle && !_reference->_data->getRecycleCount())
		{
			if (_reference->_data->setRecycleCount(ResultDataStatic::_recycleSize))
			{
				// Make the function return success.
				return true;
			}
		}
		else // Disable recycling.
		{
			// Should recycling be disabled.
			if (!recycle && _reference->_data->getRecycleCount())
			{
				// Disable by setting zero.
				if (_reference->_data->setRecycleCount(0))
				{
					// Make the function return success.
					return true;
				}
			}
		}
	}
	// Report an error.
	SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Method failed!")
	// Signal error to the caller.
	return false;
}

ResultData::size_type ResultData::getCount()
{
	// Return the actual count -1 because of the Zero reference.
	return ResultDataStatic::_references.count() - 1;
}

ResultData::size_type ResultData::getTotalReservedSize()
{
	size_type rv = 0;
	for (auto i: ResultDataStatic::_references)
	{
		rv += i->_data->getSize();
	}
	return rv;
}

ResultData::size_type ResultData::getInstanceCount()
{
	unsigned count = 0;
	// Iterate through all result references and count the usage count
	for (auto i: ResultDataStatic::_references)
	{
		count += i->_list.count();
	}
	// Subtract 1 for zero result before returning
	return --count;
}

ResultData::Vector ResultData::getList()
{
	// Return value.
	ResultData::Vector rv;
	// Set the vector to reserve the maximum expected size.
	rv.reserve(ResultDataStatic::_references.size());
	// Iterate through the references.
	for (auto ref: ResultDataStatic::_references)
	{
		// Only instance references which has id of zero.
		if (ref->_id)
		{
			rv.insert(rv.begin(), ref->_list.at(0));
		}
	}
	return rv;
}

ResultDataReference* ResultData::getReferenceById(ResultDataTypes::id_type id)
{
	if (id)
	{
		for (auto ref: ResultDataStatic::_references)
		{
			if (ref->_id == id && ref != ResultDataStatic::zero()._reference)
			{
				return ref;
			}
		}
	}
	return ResultDataStatic::zero()._reference;
}

const ResultData& ResultData::getInstanceById(ResultDataTypes::id_type id, const ResultData::Vector& list)
{
	for (auto rd: list)
	{
		if (rd->getId() == id)
		{
			return *rd;
		}
	}
	return ResultDataStatic::zero();
}

const ResultData& ResultData::getInstanceBySequenceId(ResultData::id_type seq_id, const ResultData::Vector& list)
{
	for (auto rd: list)
	{
		if (rd->getSeqId() == seq_id)
		{
			return *rd;
		}
	}
	return ResultDataStatic::zero();
}

void ResultData::makeOwner()
{
	// Do not change ownership if it is ZeroRef.
	if (_reference == ResultDataStatic::zero()._reference)
	{
		return;
	}
	// Get the current owner.
	ResultData* prev_owner = _reference->_list[0];
	// Do not change ownership if current instance is already owner.
	if (prev_owner == this)
	{
		return;
	}
	// Locate this pointer at the beginning of the list which makes it the owner.
	_reference->_list.detach(this);
	_reference->_list.addAt(this, 0);
	// Notify previous owner of losing ownership.
	prev_owner->emitEvent(reLostOwner, *this, _reference->_rangeManager->getManaged());
	// Notify new owner of getting ownership.
	emitEvent(reGetOwner, *this, _reference->_rangeManager->getManaged());
}

bool ResultData::attachRef(ResultDataReference* ref)
{
	// If the new ref is the same as the current do nothing.
	if (_reference == ref)
	{
		return true;
	}
	//
	bool rv = true;
	if (_reference)
	{
		// If this was the last one or the owner the referring FRef will be deleted
		// List[0] is the owner default the one who created the FRef instance
		if (_reference->_list.count() == 0 || this == _reference->_list[0])
		{
			// Notify all results that this reference is to become invalid.
			emitLocalEvent(reInvalid, _reference->_rangeManager->getManaged(), false);
			// Make pointer to this result data NULL so it isn't attached again in
			// the reference destructor and detach it when the ref is deleted
			_reference->_list[0] = nullptr;
			// the destructor reattaches all results that are left
			// in the list to the Zero result data reference except for the first
			// that is a NULL pointer and prevents this way the entry of this part
			// of this function because the first in the list is the owner.
			delete _reference;
		}
		else
		{ // Just remove result from list if it is not the owner.
			if (!_reference->_list.detach(this))
			{
				SF_COND_RTTI_NOTIFY(isDebug(), DO_MSGBOX | DO_CERR, "Could not remove instance from list!")
				rv = false;
			}
		}
		// member FRef is assigned nullptr to indicate no attachment.
		_reference = nullptr;
	}
	//
	if (ref)
	{
		_reference = ref;
		// Add this instance to the the reference list.
		_reference->_list.add(this);
		// Notify the user of this instance by telling it was attached.
		emitEvent(reIdChanged, _reference->_rangeManager->getManaged(), false);
	}
	return rv;
}

bool ResultData::createDataStore(ResultDataReference* ref,
	ResultData::size_type segment_size, ResultData::size_type block_size)
{
	// Limit the size of the segment.
	if (segment_size > (10L * 1024L * 1024L) / block_size)
	{
		segment_size = (10L * 1024L * 1024L) / block_size;
	}
	// Change the segment size for debugging purposes.
	if (ResultDataStatic::_debugSegmentSize)
	{
		segment_size = ResultDataStatic::_debugSegmentSize;
	}
	// When the recycle flag is enabled recycling is used on the data store.
	size_type recycle = (ref->_flags & flgRecycle) ? ResultDataStatic::_recycleSize : 0;
	// Setup the 'Data' member of the reference.
	// Create a new TDataStore instance for this reference
	// The block size is the size of the type times the given type instances per block.
	ref->_data = new FileMappedStorage(segment_size,
		block_size * ResultDataStatic::_typeInfoArray[ref->_type].Size, recycle);
	// Attach ref to this result instance.
	if (attachRef(ref))
	{
		// Notify all global linked users of instances of new ID.
		emitGlobalEvent(reNewId, Range(), false);
		attachDesired();
		return true;
	}
	// Delete and destruct the reference if it cannot be attached.
	delete_null(ref);
	return false;
}

bool ResultData::setup(const ResultData::Definition& definition, ResultData::id_type id_ofs)
{
	// Detach existing reference first by Attaching to ZeroRef
	if (!attachRef(ResultDataStatic::zero()._reference))
	{
		return false;
	}
	// The default return value is true.
	bool rv = true;
	// Check if instances other then the zero result have an ID of zero.
	if (_reference != ResultDataStatic::zero()._reference && definition._id == 0)
	{
		rv = false;
	}
	// Don' bother to go on if an error occurred so far.
	if (rv)
	{ // check if the id already exist
		// GetResultRefById returns ResultDataStatic::zero() if not found
		ResultDataReference* ref = getReferenceById(definition._id);
		if (ref && ref != ResultDataStatic::zero()._reference)
		{
			SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT | DO_MSGBOX, "Tried to create duplicate ID: !\n"
				<< stringf("0x%lX,%s", definition._id, definition._name.c_str()) << "\nover!\n"
				<< ref->_name << '!')
			// If ref already exist return false.
			return false;
		}
		else
		{
			// create new reference for this id
			ref = new ResultDataReference();
		}
		// Set the reference valid flag default to true.
		ref->_valid = true;
		// Read all std::string field values into the reference fields
		ref->_id = definition._id + id_ofs;
		// First one to get id zero is the ResultDataStatic::zero() result.
		ref->_sequenceId = ResultDataStatic::_sequenceIdCounter++;
		ref->_name = definition._name;
		ref->_flags = definition._flags;
		// copy of original flags
		ref->_curFlags = ref->_flags;
		ref->_description = definition._description;
		ref->_type = definition._type;
		ref->_significantBits = sf::clip<size_type>(definition._significantBits, 1, getTypeSize(definition._type) * 8);
		ref->_offset = definition._offset;
		//
		auto block_size = definition._blockSize;
		// Correct impossible block size.
		if (block_size <= 0)
		{
			block_size = 1;
		}
		auto segment_size = definition._segmentSize;
		// Do not allow segment size of zero when not ID is zero.
		if (ref->_id && !segment_size)
		{
			rv = false;
		}
		// Create the data store with the passed information.
		if (rv)
		{
			rv = createDataStore(ref, segment_size, block_size);
		}
	}
	// If an error occurred On error report to standard out
	SF_COND_RTTI_NOTIFY(!rv && isDebug(), DO_DEFAULT, "Setup definition struct fault: " << definition._name)
	return rv;
}

bool ResultData::emitEvent(EEvent event, const ResultData& caller, const Range& rng)
{
	// Check if a handler was linked and if so call it.
	if (_handler)
	{
		_handler->handleResultDataEvent(event, caller, *this, rng, &caller == this);
		return true;
	}
	return false;
}

ResultData::size_type ResultData::emitEvent(EEvent event, const Range& rng, bool skip_self)
{
	// Check if the event must be broad casted or not
	if (event < reFirstLocal)
	{
		return emitGlobalEvent(event, rng, skip_self);
	}
	else
	{
		if (event > reFirstPrivate)
		{
			emitEvent(event, *this, rng);
			return 1;
		}
		else
		{
			return emitLocalEvent(event, rng, skip_self);
		}
	}
}

ResultData::size_type ResultData::emitLocalEvent(EEvent event, const Range& rng, bool skip_self)
{
	// Disable deletion of local instances.
	ResultDataStatic::_globalActive++;
	Vector ev_list;
	// Iterate through list and generate the results from which the event handler needs to be called.
	for (auto res: _reference->_list)
	{
		if (res && (!skip_self || res != this))
		{
			ev_list.add(res);
		}
	}
	// Iterate through the generated list and call the event handler.
	// Any changes made by event handlers that could affect the list is avoided.
	for (auto j: ev_list)
	{
		// Assign the temporary pointer.
		// Call the event handler.
		j->emitEvent(event, *this, rng);
	}
	// Enable deletion of local instances.
	ResultDataStatic::_globalActive--;
	// Return the amount of results that were effected by the call to this function.
	return ev_list.count();
}

ResultData::size_type ResultData::emitGlobalEvent(EEvent event, const Range& rng, bool skip_self)
{
	// Disable deletion of instances.
	ResultDataStatic::_globalActive++;
	// Declare event list for instance pointers.
	Vector ev_list;
	// Iterate through all references.
	for (auto ref: ResultDataStatic::_references)
	{
		// Iterate through all instances attached of the reference.
		for (auto rd: ref->_list)
		{
			if (rd && (!skip_self || rd != this))
			{
				// Add instance to list.
				ev_list.add(rd);
			}
		}
	}
	// Iterate through the generated instance list and call the handler of them.
	// Any changes made by event handlers that could affect the list is avoided.
	for (auto k: ev_list)
	{
		k->emitEvent(event, *this, rng);
	}
	// Enable deletion of instances.
	ResultDataStatic::_globalActive--;
	// Return  the lists size as the count of the events.
	return ev_list.count();
}

ResultData::size_type ResultData::attachDesired()
{
	// Disable deletion of instances.
	ResultDataStatic::_globalActive++;
	// Signal other event handler functions are called using a global predefined list.
	Vector ev_list;
	// Iterate through all references generating pointers to instances having a desired ID that must be attached.
	for (auto ref: ResultDataStatic::_references)
	{
		// Iterate through all instances of the reference.
		for (auto res: ref->_list)
		{
			// Check if the desired ID is set to a value greater then zero
			// which means it is enabled.
			// Also compare the desired and this ID.
			if (res && res->_desiredId && res->_desiredId == _reference->_id)
			{
				// Add instance pointer the to list.
				ev_list.add(res);
			}
		}
	}
	// Iterate through the generated list and attach them.
	// Any changes made by event handlers that could affect the list is avoided.
	for (auto res: ev_list)
	{
		// Compare them again to be sure after Attaching the others.
		if (res->_desiredId && res->_desiredId == _reference->_id)
		{
			// Attach the one having the correct desired ID.
			res->attachRef(_reference);
		}
	}
	// Enable deletion of instances.
	ResultDataStatic::_globalActive--;
	// Return  the lists size as the count of the events.
	return ev_list.count();
}

void ResultData::setHandler(ResultDataHandler* handler)
{
	// Only generate events if there is a change of handler.
	if (_handler != handler)
	{
		if (handler)
		{
			// Notify instance getting event link
			_handler = handler;
			emitEvent(reLinked, *this, _reference->_rangeManager->getManaged());
			_transId = (Range::id_type) this;
		}
		else
		{ // Notify instance losing event link in the was already a link
			if (_handler)
			{
				emitEvent(reUnlinked, *this, _reference->_rangeManager->getManaged());
				_handler = nullptr;
				_transId = 0;
			}
		}
	}
}

void ResultData::removeHandler(ResultDataHandler* handler)
{
	// Iterate through all result references.
	for (size_t i = 0; i < ResultDataStatic::_references.count(); i++)
	{
		// Get the current result pointer.
		ResultDataReference* ref = ResultDataStatic::_references[i];
		// Get the total amount of results attached to this reference.
		size_t vrc = ref->_list.count();
		// Iterate through the results.
		for (size_t j = 0; j < vrc; j++)
		{
			// Check if the links are the same.
			if (ref->_list[j]->_handler == handler)
			{
				// Set the link to NULL so the event function isn't called.
				ref->_list[j]->_handler = nullptr;
			}
		}
	}
}

bool ResultData::getRequests(Range::Vector& requests) const
{
	// Clear the list first.
	requests.flush();
	// Can only get when it is the owner.
	if (isOwner())
	{ // Check if there is anything to get.
		if (_reference->_rangeManager->getActualRequests().count())
		{ // Just make a copy of the real request list of the manager.
			requests = _reference->_rangeManager->getActualRequests();
			//
			return true;
		}
	}
	return false;
}

bool ResultData::getSplitRequests(Range::Vector& req_list) const
{
	// Clear the list first.
	req_list.flush();
	// Can only get when it is the owner.
	if (isOwner())
	{
		// Check if there is anything to get.
		return Range::split
			(
				_reference->_data->getSegmentSize(),
				_reference->_rangeManager->getRequests(),
				req_list
			) > 0;
	}
	return false;
}

const Range& ResultData::getAccessRange() const
{
	return const_cast<Range&> (_reference->_rangeManager->getManaged()).setId((Range::id_type) _reference->_id);
}

bool ResultData::setAccessRange(const Range& rng, bool skip_self)
{
	if (isOwner())
	{
		// Add clipped new range to existing range.
		Range nr(
			_reference->_rangeManager->getManaged() + rng & Range(0, std::numeric_limits<Range::size_type>::max()));
		// Compare the new formed range with the existing one.
		if (nr != _reference->_rangeManager->getManaged())
		{
			// Check if there are enough reserved blocks to access.
			if (nr.getStop() > _reference->_data->getBlockCount())
			{
				// Increment the reserved block count to satisfy the access range.
				if (!_reference->_data->reserve(nr.getStop()))
				{
					SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Failed to set reserve blocks!")
					return false;
				}
				// Generate an event to notify users of the change in reserved blocks.
				// The new value is passed in the 'Stop' Value of the passed 'Range' argument.
				emitLocalEvent(reReserve, Range(0, _reference->_data->getBlockCount(), (Range::id_type) _reference->_id),
					skip_self);
			}
			// Set the new managed range.
			_reference->_rangeManager->setManaged(nr);
			// Notify the users of this event
			emitLocalEvent(reAccessChange, _reference->_rangeManager->getManaged(), skip_self);
			// Return true to notify the caller of a change that was made.
			return true;
		}
	}
	// Return false to notify there was no change made to the range.
	return false;
}

ResultData::EType ResultData::getType(const char* type)
{
	return ResultDataStatic::getType(type);
}

const char* ResultData::getType(ResultData::EType type)
{
	return ResultDataStatic::_typeInfoArray[(type >= rtLastEntry || type < 0) ? rtInvalid : type].Name;
}

ResultData::size_type ResultData::getTypeSize(ResultData::EType type)
{
	return ResultDataStatic::_typeInfoArray[(type >= rtLastEntry || type < 0) ? rtInvalid : type].Size;
}

void ResultData::setDesiredId(id_type id)
{
	if (_desiredId != id)
	{
		// Make the change.
		_desiredId = id;
		// Signal this event to this single instance.
		emitEvent(reDesiredId, *this, _reference ? _reference->_rangeManager->getManaged() : Range());
	}
}

std::string ResultData::getName(int levels) const
{
	// Return full name path at levels equals zero.
	std::string rv = _reference->_name;
	auto len = rv.length();
	// Check if levels is non zero.
	if (levels)
	{
		if (levels > 0)
		{
			unsigned i = len;
			while (i--)
			{
				// Is the character a separator character.
				if (rv[i] == '|')
				{
					rv[i] = ' ';
					if (!--levels)
					{
						break;
					}
				}
			}
			if (i && i < len - 1)
			{
				return rv.substr(i + 1);
			}
		}
		else
		{
			unsigned p = 0;
			for (unsigned i = 0; i < rv.length(); i++)
			{
				// Is the character a separator character.
				if (rv[i] == '|')
				{
					// Clear the separator character.
					rv[i] = ' ';
					//
					if (!++levels)
					{
						p = i + 1;
					}
				}
			}
			if (p < (unsigned) len)
			{
				return rv.substr(p, rv.length() - p);
			}
		}
	}
	return rv;
}

ResultData::size_type ResultData::getNameLevelCount() const
{
	int levels = 0;
	for (auto ch: _reference->_name)
	{
		// Is the character a separator character.
		if (ch == '|')
		{
			levels++;
		}
	}
	return levels;
}

ResultData::flags_type ResultData::toFlags(const std::string& flags)
{
	flags_type rv{0};
	for (auto f: flags)
	{
		for (auto fl: ResultDataStatic::_flagLetters)
		{
			if (f == fl._letter)
			{
				rv |= fl._flag;
			}
		}
	}
	return rv;
}

std::string ResultData::getFlagsString(ResultData::flags_type flag)
{
	std::string rv;
	for (auto fl: ResultDataStatic::_flagLetters)
	{
		if (flag & fl._flag)
		{
			rv.append(1, fl._letter);
		}
	}
	return rv;
}

bool ResultData::setFlag(int flag, bool skip_self)
{
	// Only the owner is allowed to set the flags.
	if (isOwner())
	{
		// Test for the recycle flag and try to enable.
		if (flag == flgRecycle)
		{
			// Abort here when it failed to set it.
			if (!recycleEnable(true))
			{
				// Signal failure.
				return false;
			}
		}
		// Save the current flags for comparison.
		auto flags = _reference->_curFlags;
		// Set the flag bit.
		_reference->_curFlags |= flag;
		// Check for a change in flags.
		if (_reference->_curFlags != flags)
		{
			// Generate an event telling the flags have changed.
			emitLocalEvent(reFlagsChange, _reference->_rangeManager->getManaged(), skip_self);
		}
		// Signal success.
		return true;
	}
	// Signal failure.
	return false;
}

bool ResultData::unsetFlag(int flag, bool skip_self)
{ // Only the owner is allowed to set the flags.
	if (isOwner())
	{
		// Test for the recycle flag and try to disable.
		if (flag == flgRecycle)
		{
			// Abort here when it failed to set it.
			if (!recycleEnable(false))
			{
				// Signal failure.
				return false;
			}
		}
		// Save the current flags for comparison.
		auto flags = _reference->_curFlags;
		// Unset the flag bit.
		_reference->_curFlags &= ~flag;
		// Check for a change in flags.
		if (_reference->_curFlags != flags)
		{
			// Generate an event telling the flags have changed.
			emitLocalEvent(reFlagsChange, _reference->_rangeManager->getManaged(), skip_self);
		}
		// Signal success.
		return true;
	}
	// Signal failure.
	return false;
}

bool ResultData::updateFlags(int flags, bool skip_self)
{
	bool rv = false;
	// Only the owner is allowed to set the flags.
	if (isOwner())
	{ // Initially signal success.
		rv = true;
		// Test for a change in the recycle flag.
		if ((_reference->_curFlags ^ flags) & flgRecycle)
		{ // Establish for enable or disable of mode.
			bool enable = (_reference->_curFlags & flgRecycle) != 0;
			// Enable or disable the recycle mode of the data store.
			if (!recycleEnable(enable))
			{ // On failure skip change of the flag change.
				if (enable)
				{
					flags &= ~flgRecycle;
				}
				else
				{
					flags |= flgRecycle;
				}
				// Signal failure.
				rv = true;
			}
		}
		// Check for a change in flags.
		if (_reference->_curFlags != flags)
		{ // Set the flags member.
			_reference->_curFlags = flags;
			// Signal attached instances of the fact.
			emitLocalEvent(reFlagsChange, _reference->_rangeManager->getManaged(), skip_self);
		}
	}
	// Signal failure.
	return rv;
}

std::ostream& ResultData::reportStatus(std::ostream& os) const
{
	os << "Block Size   : " << getBlockSize() << '\n'
		<< "Segment Size : " << getSegmentSize() << '\n'
		<< "Block Count  : " << getBlockCount() << '\n'
		<< "Segment Count: " << getSegmentCount() << '\n'
		<< "Res. Blocks  : " << getReservedBlockCount() << '\n'
		<< "Total Res.   : " << getReservedSize() << '\n'
		<< "Access List  : " << _reference->_rangeManager->getAccessibles() << '\n'
		<< "Request.List : " << _reference->_rangeManager->getRequests() << '\n'
		<< "Real Req.List: " << _reference->_rangeManager->getActualRequests() << '\n';
	return os;
}

bool ResultData::setReservedBlockCount(Range::size_type sz, bool skip_self)
{
	// Only an owner is allowed to set reserve blocks.
	if (!isOwner())
	{
		SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Only owner is allowed to set reserve blocks!")
		return false;
	}
	// Check if this call really reserves some extra blocks or not. Otherwise skip it from here.
	if (sz > _reference->_data->getBlockCount())
	{
		// If reserved failed notify.
		if (!_reference->_data->reserve(sz))
		{
			SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Failed to set reserve blocks(" << sz << ")!")
			return false;
		}
		// Generate an event to notify users of the change in reserved blocks.
		// The new value is put in the Stop Value of the passed Range value.
		emitLocalEvent(reReserve, {0, (Range::size_type) _reference->_data->getBlockCount()}, skip_self);
	}
	return true;
}

bool ResultData::blockWrite(Range::size_type ofs, Range::size_type sz, const void* src, bool auto_reserve)
{
	// Only the owner of the instance may write to it.
	if (!isOwner())
	{
		SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Only owner is allowed write blocks!")
		// Bail out
		return false;
	}
	// When the passed offset is max the data must be appended.
	if (ofs == std::numeric_limits<Range::size_type>::max())
	{
		ofs = _reference->_rangeManager->getManaged().getStop();
	}
	// Check there are enough blocks reserved to write the data.
	if (_reference->_data->getBlockCount() < (ofs + sz))
	{
		// Check the auto_reserve flag.
		if (!auto_reserve)
		{
			SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Write blocks failed due to lak of reserved blocks!")
			// Bail out
			return false;
		}
		else
		{
			// Try to reserve the required blocks.
			if (!setReservedBlockCount(ofs + sz))
			{
				SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Reserving blocks failed!")
				// Bail out
				return false;
			}
		}
	}
	// If the function succeeds update the accessible range list.
	if (!_reference->_data->blockWrite(ofs, sz, src))
	{
		SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Writing blocks failed!")
		// Bail out
		return false;
	}

	// To intercept unwanted calls to this function for debugging.
	//LocalEvent(reUserLocal, Range(ofs, ofs + sz, FRef->Id), false);
	//LocalEvent(reUserLocal, Range(ofs, ofs + sz, FRef->Id), false);

	// Add this range to the validate cache that will be processed when commitValidations() is called for.
	_reference->_validatedCache.add(Range(ofs, ofs + sz, (Range::id_type) _reference->_id));
	return true;
}

void ResultData::validateRange(Range rng)
{
	// Only the owner can validate ranges.
	if (isOwner())
	{
		_reference->_validatedCache.add(rng.setId((Range::id_type) _reference->_id));
	}
}

void ResultData::validateRange(const Range::Vector& rl)
{
	// Only the owner can validate ranges.
	if (isOwner())
	{
		for (auto& r: rl)
		{
			Range rng(r.getStart(), r.getStop(), (Range::id_type) _reference->_id);
			_reference->_validatedCache.add(rng);
		}
	}
}

ResultData::size_type ResultData::commitValidations(bool skip_self)
{
	ResultData::size_type rv{0};
	// Only the owner can call this function.
	if (isOwner() && _reference->_validatedCache.count())
	{
		SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Committing full vector: " << _reference->_validatedCache)
		// Remove all ranges or part of ranges that were already committed/validated.
		_reference->_validatedCache.rearrange();
		_reference->_validatedCache.exclude(_reference->_rangeManager->getAccessibles());
		SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Committing reduced vector: " << _reference->_validatedCache)
		// Retrieve the current extends and compare them later.
		Range aer = _reference->_rangeManager->getManaged();
		// Create a temporary vector to let the function to return the resolved.
		// requests in.
		Range::Vector rrl;
		// Validate by telling the range manager that the ranges are accessible.
		// When the function returns true requests are fulfilled an returned in 'rrl'.
		if (_reference->_rangeManager->setAccessible(_reference->_validatedCache, rrl))
		{
			SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Range requests resolved:" << rrl)
			// Iterate through the vector of resolved requested ranges.
			for (auto& rng: rrl)
			{
				// Iterate through the result list and find the one with the
				// the same reference as this one
				for (auto rd: _reference->_list)
				{
					// Check if the pointer is valid Check if the transaction id is
					// non-zero and equal to the current in the list.
					if (rd && rd->_transId && rd->_transId == rng.getId())
					{
						// Generate an event for one of the result that requested it.
						rv += rd->emitEvent(reGotRange, *this, rng);
					}
				}
				// Generate an event for all of the results having the same ID as this instance.
				//LocalEvent(reGotRange, i.Current(), false);
			}
		}
		// Check if the extends have changed and generate an event for it.
		if (aer != _reference->_rangeManager->getManaged())
		{
			SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT,
				"Access range has changed: " << _reference->_rangeManager->getManaged())
			// Generate an event for all of the results having the same ID as this instance.
			rv += emitLocalEvent(reAccessChange, _reference->_rangeManager->getManaged(), skip_self);
		}
		// Notify clients of the committed vector.
		rv += emitLocalEvent(reCommitted, _reference->_rangeManager->getManaged(), skip_self);
		// Flush the validate cache.
		_reference->_validatedCache.flush();
	}
	return rv;
}

bool ResultData::clearValidations(bool skip_self)
{
	// Only the owner can clear validated ranges.
	if (isOwner())
	{
		// Flush the pending to be committed ranges.
		_reference->_validatedCache.flush();
		// Check if flushing has any effect of calling it
		if (_reference->_rangeManager->isFlushable())
		{ // Notify clients before it is actual cleared.
			emitLocalEvent(reClear, _reference->_rangeManager->getManaged(), skip_self);
			// Flush all ranges within the range manager.
			_reference->_rangeManager->flush();
			// Flush al allocated segments too.
			// This seems to improve performance for Windows.
			_reference->_data->flush();
			// Notify clients of change of accessible range
			emitLocalEvent(reAccessChange, _reference->_rangeManager->getManaged(), skip_self);
			// Signal flush performed.
			return true;
		}
	}
	return false;
}

const Range::Vector& ResultData::getCommitList() const
{
	return _reference->_validatedCache;
}

ResultData::size_type ResultData::getSegmentLocks() const
{
	return _reference->_data->getSegmentLocks();
}

const Range::Vector& ResultData::getValidatedList() const
{
	return _reference->_rangeManager->getAccessibles();
}

bool ResultData::blockRead(Range::size_type ofs, Range::size_type sz, void* src, bool force) const
{
	// If force is true do not check validity of the range.
	if (!force && !isRangeValid(ofs, sz))
	{
		SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "(" << ofs << "," << sz << ") invalid range!")
		// Return false because the wanted range is not completely validated.
		return false;
	}
	// BlockRead returns false if the block count is not available.
	bool ret_val = _reference->_data->blockRead(ofs, sz, src);
	if (!ret_val)
	{
		SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "(" << ofs << "," << sz << ") failed!")
	}
	return ret_val;
}

bool ResultData::readIndexRange(Range::size_type ofs, Range& range)
{
	// TODO: This needs a solution for an index based on 16, 32 and 64 in stead of only 64 bits use getValueU().
	// Only when the block size is at least that of the range start and stop values.
	if (_reference->_data->getBlockSize() <= sizeof(Range::size_type))
	{
		bool ok;
		Range::size_type idx[2]{0, 0};
		if (ofs != 0)
		{
			ok = blockRead(ofs - 1, 2, idx);
		}
		else
		{
			ok = blockRead(ofs, 1, &idx[1]);
		}
		// Assign the range on success.
		if (ok)
		{
			range.assign(getValue(&idx[0]), getValue(&idx[1]));
		}
		//
		return ok;
	}
	// Failed to read index range.
	return false;
}

bool ResultData::readIndexRange(const Range& index, Range& range)
{
	// Only when the block size is at least that of the range start and stop values.
	if (_reference->_data->getBlockSize() <= sizeof(Range::size_type) && index.getSize())
	{
		bool ok = true;
		Range::size_type idx[2]{0, 0};
		if (index.getStart() != 0)
		{
			ok &= blockRead(index.getStart() - 1, 1, &idx[0]);
		}
		ok &= blockRead(index.getStop() - 1, 1, &idx[1]);
		// Assign the range on success.
		if (ok)
		{
			range.assign(getValue(&idx[0]), getValue(&idx[1]));
		}
		return ok;
	}
	// Failed to read index range.
	return false;
}

ResultData::sdata_type ResultData::getValue(ResultData::size_type idx, const void* data) const
{
	char* p = const_cast<char*>((const char*) data);
	// Calculate the new data pointer from the type and passed index.
	p += getTypeSize(_reference->_type) * idx;
	// Call the index less version.
	return getValue(p);
}

ResultData::sdata_type ResultData::getValue(const void* data) const
{
	data_type mask;
	// Shift of 32 bits is the same as 0 which means it is actually a ROL
	// operation so special treatment for significant bits value of 32 and larger.
	if (_reference->_significantBits < sizeof(mask) * 8)
	{
		// Only set those bits in the mask that count.
		mask = (data_type(1) << _reference->_significantBits) - 1;
	}
	else
	{
		// Set all the bits in the mask.
		mask = (data_type) - 1;
	}
	//
	return (sdata_type)((*(data_type*) data & mask) - _reference->_offset);
}

ResultData::data_type ResultData::getValueU(ResultData::size_type idx, const void* data) const
{
	char* p = (char*) data;
	// Calculate the new data pointer from the type and passed index.
	p += getTypeSize(_reference->_type) * idx;
	// Call the index less version.
	return getValueU(p);
}

ResultData::data_type ResultData::getValueU(const void* data) const
{
	data_type mask = (data_type(1) << _reference->_significantBits) - 1;
	if (mask == 0)
	{
		return *(ResultData::data_type*) data;
	}
	else
	{
		return *(ResultData::data_type*) data & mask;
	}
}

void ResultData::clearRequests()
{
	// Remove all requests of this client.
	_reference->_rangeManager->flushRequests(_transId);
}

bool ResultData::requestRange(const Range& rng)
{
	// Reject requests made on ResultDataStatic::zero() result.
	if (!_reference->_id || rng.isEmpty())
	{
		return false;
	}
	// Copy the range to modify.
	Range r(rng);
	// Set the transaction id.
	r.setId(getTransId());
	// Create a range vector for the function to return the real requests in.
	Range::Vector rrl;
	// Call the request function of the range manager
	auto result = _reference->_rangeManager->request(r, rrl);
	switch (result)
	{
		case RangeManager::rmAccessible: SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Request " << rng << " Is accessible!")
			break;

		case RangeManager::rmOutOfRange: SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT,
			"Request " << rng << " Is out of range!")
			break;

		case RangeManager::rmInaccessible:
		{
			// Check if the owner of the result reference has an event link.
			ResultData* rd = _reference->_list[0];
			if (rd->_handler)
			{
				bool ok = true;
				// Generate an event for each real request for the owner of the result reference.
				for (auto i: rrl)
				{
					ok &= rd->emitEvent(reGetRange, *this, i.setId((Range::id_type) _reference->_id));
				}
				// for debugging purposes.
				if (!ok)
				{
					SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Request event is useless, owner has no handler assigned!")
				}
				// Return true to notify caller success.
				return true;
			}
			else
			{
				SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, "Request impossible, unlinked owner!")
			}
			break;
		}
	}
	// Return false to notify caller failure.
	return false;
}

bool ResultData::isIndexRangeValid(const Range& r) const
{
	RANGE rng{r.getStart(), r.getStop()};
	// Apply an offset to the passed index range start because it is an index index
	if (rng._start > 0)
	{
		rng._start -= 1;
	}
	return _reference->_rangeManager->isAccessible(Range(rng));
}

bool ResultData::requestIndexRange(const Range& rng)
{
	// Apply an offset to the passed index range because it is an index index
	RANGE range{rng.getStart() - 1, rng.getStop()};
	// Negative values are not allowed.
	if (range._start < 0)
	{
		range._start = 0;
	}
	return requestRange(Range(rng));
}

const char* ResultData::getEventName(EEvent event)
{
	const char* rv = "Unknown";
	switch (event)
	{
		case reNewId:
			rv = "NewId";
			break;

		case reDesiredId:
			rv = "DesiredId";
			break;

		case reFlagsChange:
			rv = "FlagsChange";
			break;

		case reAccessChange:
			rv = "AccessChange";
			break;

		case reReserve:
			rv = "Reserve";
			break;

		case reInvalid:
			rv = "Invalid";
			break;

		case reIdChanged:
			rv = "IdChanged";
			break;

		case reRemove:
			rv = "Remove";
			break;

		case reGetOwner:
			rv = "GetOwner";
			break;

		case reLostOwner:
			rv = "LostOwner";
			break;

		case reLinked:
			rv = "Linked";
			break;

		case reUnlinked:
			rv = "Unlinked";
			break;

		case reGotRange:
			rv = "GotRange";
			break;

		case reGetRange:
			rv = "GetRange";
			break;

		case reCommitted:
			rv = "Committed";
			break;

		case reClear:
			rv = "Clear";
			break;

		default:
			// Check for global event.
			if (event < 0)
			{
				rv = "UserGlobal";
				break;
			}
			else
			{
				if (event >= 0 && event < reFirstPrivate)
				{
					rv = "UserLocal + ?";
				}
				else
				{
					if (event >= reUserPrivate)
					{
						rv = "UserPrivate + ?";
					}
				}
			}
			break;
	}
	return rv;
}

bool ResultData::writeUpdate(std::ostream& os) const
{
	if (!_reference)
	{
		SF_COND_RTTI_NOTIFY(isDebug(), DO_MSGBOX | DO_CERR | DO_COUT, "Encountered nullptr\n")
		return false;
	}
	os << '(' << getAccessRange() << ',' << getFlagsString(_reference->_curFlags) << ')';
	// check if out stream is valid
	return !os.fail() && !os.bad();
}

bool ResultData::readUpdate(std::istream& is, bool skip_self, Vector& list)
{
	std::string flags;
	Range rng;
	char c = 0;
	// Read all fields
	is >> c >> rng >> c;
	getline(is, flags, ')');
	// Check if the reading of the stream failed.
	if (is.good())
	{
		// GetResultDataById get reference to owner to be able to update.
		// Check if the reference pointer is non-zero and look only into that list for the id.
		auto& res = const_cast<ResultData&>(not_ref_null(list) ?
			getInstanceById(rng.getId(), list) : getInstanceById(rng.getId()));
		// Check if new passed 'Stop' value is smaller then current one.
		// If so this result should be cleared. first
		if (rng.getStop() < res._reference->_rangeManager->getManaged().getStop())
		{
			res.clearValidations();
		}
		// Change the managed range
		res.setAccessRange(rng, skip_self);
		// Update the flags.
		res.updateFlags(toFlags(flags), skip_self);
		// Notify caller success of this operation.
		return true;
	}
	// Notify the caller failure of this operation.
	return false;
}

ResultData::Definition ResultData::getDefinition(const std::string& str)
{
	Definition def;
	//
	strings fields;
	GetCsfFields(str.c_str(), fields);
	// Set the reference valid flag default to true.
	def._valid = false;
	// Only when field exist.
	if (fields.size() > rfeSegmentSize)
	{
		def._valid = true;
		// Id.
		def._id = std::stoull(fields[rfeId], nullptr, 0);
		// Name.
		def._name = unescape(fields[rfeName]);
		// Flags.
		def._flags = toFlags(fields[rfeFlags]);
		// Description.
		def._description = unescape(fields[rfeDescription]);
		// Type.
		def._type = getType(fields[rfeType].c_str());
		// Block size.
		def._blockSize = std::stoull(fields[rfeBlockSize], nullptr, 0);
		// Segment size.
		def._segmentSize = std::stoull(fields[rfeSegmentSize], nullptr, 0);
		//
		def._valid &= (def._blockSize > 0);
	}
	// SignificantBits clipped to min and max value.
	if (fields.size() > rfeSigBits)
	{
		// Significant bits
		def._significantBits = std::stoull(fields[rfeSigBits], nullptr, 0);
	}
	// Offset
	if (fields.size() > rfeOffset)
	{
		def._offset = std::stoull(fields[rfeOffset], nullptr, 0);
	}
	//
	return def;
}

bool ResultData::create(std::istream& is, Vector& list, int& err_line)
{
	// TODO: Use Definition structure as intermediate list and for checking stream content.
	bool ret_val = true;
	// result keeps track of the lines read
	if (not_ref_null(err_line))
	{
		err_line = 1;
	}
	while (is.good() && ret_val)
	{

		if (is.fail())
		{
			SF_NORM_NOTIFY(DO_DEFAULT, "Input stream failure!")
			return false;
		}
		if (is.bad())
		{
			SF_NORM_NOTIFY(DO_DEFAULT, "Bad input stream!")
			return false;
		}

		std::string st;
		// get line with max length of 1024
		getline(is, st);

		// test ref pointer before increasing line count
		if (not_ref_null(err_line))
		{
			err_line++;
		}

		// check length and ignore lines starting with character ';'
		if (st.length() > 10 && st[0] != ';')
		{
			auto rd = new ResultData(st);
			if (rd->isValid())
			{
				if (not_ref_null(list))
				{
					list.add(rd);
				}
				else
				{
					delete rd;
				}
			}
			else
			{
				delete rd;
				ret_val = false;
			}
		}
	} // while
	return ret_val;
}

std::string ResultData::getSetupString() const
{
	const char sep = ',';
	char buf[32];
	//  vfeID,
	std::string ret_val = "0x";
	ret_val += itoa(getId(), buf, 16);
	ret_val += sep;
	//  vfeName,
	ret_val += getName() + sep;
	//  vfeFLAGS,
	ret_val += getFlagsString() + sep;
	//  vfeDescription,
	ret_val += unescape(getDescription()) + sep;
	//  vfeType,
	ret_val += getType(getType());
	ret_val += sep;
	// rfeBlockSize,
	ret_val += itoa(getBlockSize(), buf, 10);
	ret_val += sep;
	// rfeSegmentSize,
	ret_val += itoa(getSegmentSize(), buf, 10);
	ret_val += sep;
	ret_val += itoa(getSignificantBits(), buf, 10);
	ret_val += sep;
	ret_val += itoa(getValueOffset(), buf, 10);
	return ret_val;
}

ResultData& ResultData::getOwner()
{
	return *_reference->_list[0];
}

bool ResultData::isOwner() const
{
	return this == _reference->_list[0] && _reference->_id;
}

bool ResultData::isValid() const
{
	return _reference->_valid;
}

ResultData::data_type ResultData::getValueOffset() const
{
	return _reference->_offset;
}

ResultData::data_type ResultData::getValueRange() const
{
	data_type max_val = (data_type(1) << _reference->_significantBits) - 1;
	return max_val ? max_val : std::numeric_limits<data_type>::max();
}

ResultDataTypes::id_type ResultData::getId() const
{
	return _reference->_id;
}

ResultDataTypes::id_type ResultData::getSeqId() const
{
	return _reference->_sequenceId;
}

std::string ResultData::getDescription() const
{
	return _reference->_description;
}

ResultDataTypes::size_type ResultData::getBlockSize() const
{
	return _reference->_data->getBlockSize() / getTypeSize();
}

Range::size_type ResultData::getBlockCount() const
{
	return _reference->_rangeManager->getManaged().getStop();
}

FileMappedStorage::size_type ResultData::getSegmentSize() const
{
	return _reference->_data->getSegmentSize();
}

FileMappedStorage::size_type ResultData::getSegmentCount() const
{
	return _reference->_data->getSegmentCount();
}

FileMappedStorage::size_type ResultData::getReservedSize() const
{
	return _reference->_data->getSize();
}

FileMappedStorage::size_type ResultData::getReservedBlockCount() const
{
	return _reference->_data->getBlockCount();
}

bool ResultData::isRangeValid(Range::size_type ofs, Range::size_type sz) const
{
	return _reference->_rangeManager->isAccessible(Range(ofs, ofs + sz));
}

bool ResultData::isRangeValid(const Range& rng) const
{
	return _reference->_rangeManager->isAccessible(rng);
}

bool ResultData::isFlag(int flag) const
{
	return (_reference->_curFlags & flag) == flag;
}

std::string ResultData::getCurFlagsString() const
{
	return ResultData::getFlagsString(_reference->_curFlags);
}

ResultData::flags_type ResultData::getFlags() const
{
	return _reference->_flags;
}

ResultData::flags_type ResultData::getCurFlags() const
{
	return _reference->_curFlags;
}

std::string ResultData::getFlagsString() const
{
	return getFlagsString(_reference->_flags);
}

ResultData::EType ResultData::getType() const
{
	return _reference->_type;
}

unsigned ResultData::getSignificantBits() const
{
	return _reference->_significantBits;
}

const ResultData& ResultData::getInstanceById(ResultDataTypes::id_type id)
{
	return *getReferenceById(id)->_list[0];
}

ResultData::size_type ResultData::getUsageCount() const
{
	return _reference->_list.count();
}

const FileMappedStorage& ResultData::getDataStore() const
{
	return *_reference->_data;
}

void ResultData::setDesiredId()
{
	_desiredId = _reference->_id;
}

ResultDataTypes::size_type ResultData::getBufferSize(ResultDataTypes::size_type blocks) const
{
	return blocks * _reference->_data->getBlockSize();
}

std::ostream& operator<<(std::ostream& os, const ResultData& rd)
{
	return os << rd.getSetupString() << '\n';
}

std::istream& operator>>(std::istream& is, ResultData& rd)
{
	std::string st;
	// Get one line.
	getline(is, st);
	// Check stream on errors.
	int rv = (!is.fail() && !is.bad());
	// Check stream state length and ignore lines starting with character ';'.
	if (!rv && st.length() > 10 && st[0] != ';')
	{
		// Setup this result with the read line from the input stream
		rd.setup(st);
	}
	// Returns the input stream.
	return is;
}

}
