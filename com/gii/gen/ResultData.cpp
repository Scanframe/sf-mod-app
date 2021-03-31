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

ResultData::ResultData()
{
	_COND_RTTI_THROW(!ResultDataStatic::zero()._reference, "ZeroResultData is NOT created! Library initialisation error!")
	attachRef(ResultDataStatic::zero()._reference);
}

ResultData::ResultData(void*, void*)
{
	if (!ResultDataStatic::_zero)
	{
		// Assign the zero variable.
		ResultDataStatic::_zero = this;
		setup(std::string("0x0,n/a,H,n/a,,,,,,,"), 0);
	}
}

void ResultData::operator delete(void* p) // NOLINT(misc-new-delete-overloads)
{
	if (ResultDataStatic::GlobalActive)
	{
		_NORM_NOTIFY(DO_DEFAULT, "TResultData: Warning: Deleting Instance During An Event!")
		// Do not free allocated data before end of global event remove
		ResultDataStatic::DeleteWaitCache->add(p);
	}
	else
	{
		// Call real destructor.
		::operator delete(p);
		// If deletion is allowed again delete the wait cache too.
		unsigned count = ResultDataStatic::DeleteWaitCache ? ResultDataStatic::DeleteWaitCache->count() : 0;
		if (count)
		{
			while (count--)
			{
				delete (char*) ((*ResultDataStatic::DeleteWaitCache)[count]);
			}
			// Flush the entries from the list.
			ResultDataStatic::DeleteWaitCache->flush();
		}
	}
}

ResultData::~ResultData()
{
	// If a VarLink function exist notify removal of this instance.
	emitEvent(reRemove, *this, Range());
	// Remove the link if there was one and doing so generating an event.
	setHandler(nullptr);
	// Remove this instance from the list by attach reference NULL.
	attachRef(nullptr);
}

bool ResultData::recycleEnable(bool recycle)
{
	// Only allow ehrn Check if data is available.
	if (!_reference->_data->getSegmentCount())
	{ // Should recycling be enabled.
		if (recycle && !_reference->_data->getRecycleCount())
		{
			if (_reference->_data->setRecycleCount(ResultDataStatic::RecycleSize))
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
	_RTTI_NOTIFY(DO_DEFAULT, "RecycleEnable(...) Failed!")
	// Signal error to the caller.
	return false;
}

ResultData::size_type ResultData::getResultDataCount()
{
	// Return the actual count -1 because of ZeroResult
	return ResultDataStatic::RefList->count() - 1;
}

ResultData::size_type ResultData::getTotalReservedSize()
{
	size_type rv = 0;
	for (unsigned i = 0; i < ResultDataStatic::RefList->count(); i++)
	{
		rv += (*ResultDataStatic::RefList)[i]->_data->getSize();
	}
	return rv;
}

ResultData::size_type ResultData::getInstanceCount()
{
	unsigned count = 0;
	// Iterate through all result references and count the usage count
	for (auto ref: *ResultDataStatic::RefList)
	{
		count += ref->_list.count();
	}
	// Subtract 1 for zero result before returning
	return --count;
}

const ResultData* ResultData::getResultDataListItem(unsigned p)
{
	// return the first TResultData in the list because this
	// there is always one in there otherwise no Reference would exist
	// increase p by 1 to skip ZeroResultData which is the first in the list
	p++;
	ResultDataReference* ref = (p < ResultDataStatic::RefList->count()) ? (*ResultDataStatic::RefList)[p] : nullptr;
	return (ref) ? ref->_list[0] : &ResultDataStatic::zero();
}

ResultDataReference* ResultData::getReferenceById(ResultDataTypes::id_type id)
{
	// Do not search for the ZeroResultData reference
	if (id)
	{
		// Iterate through the list and return the reference having the passed id.
		for (auto ref: *ResultDataStatic::RefList)
		{
			if (ref->_id == id)
			{
				return ref;
			}
		}
	}
	// Return by default the ZeroResultData result.
	return ResultDataStatic::zero()._reference;
}

ResultData& ResultData::getResultDataById(ResultDataTypes::id_type id, ResultData::Vector& list)
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

ResultData& ResultData::getResultDataBySeqId(ResultDataTypes::id_type seq_id, ResultData::Vector& list)
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
			// Make pointer to this variable NULL so it isn't attached again in
			// the reference destructor and detach it when the ref is deleted
			_reference->_list[0] = nullptr;
			// the destructor reattaches all variables that are left
			// in the list to the ZeroVariable reference except for the first
			// that is a NULL pointer and prevents this way the entry of this part
			// of this function because the first in the list is the owner.
			delete _reference;
		}
		else
		{ // Just remove result from list if it is not the owner.
			if (!_reference->_list.detach(this))
			{
				_RTTI_NOTIFY(DO_MSGBOX | DO_CERR, "Could not remove instance from list!")
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
		// Add this instance to the the Variable Reference List
		_reference->_list.add(this);
		// Notify the user of this instance by telling it was attached.
		emitEvent(reIdChanged, _reference->_rangeManager->getManaged(), false);
	}
	return rv;
}

bool ResultData::createDataStore(ResultDataReference* ref, ResultData::size_type segment_size,
	ResultData::size_type block_size)
{
	bool rv = true;
	// Limit the size of the segment.
	if (segment_size > (10L * 1024L * 1024L) / block_size)
	{
		segment_size = (10L * 1024L * 1024L) / block_size;
	}
	// Change the segment size for debugging purposes.
	if (ResultDataStatic::DebugSegSize)
	{
		segment_size = ResultDataStatic::DebugSegSize;
	}
	// Check if no errors occurred so far before generating some more.
	if (rv)
	{
		// When the recycle flag is enabled recycling is used on the data store.
		size_type recycle = (ref->_flags & flgRecycle) ? ResultDataStatic::RecycleSize : 0;
		// Setup the 'Data' member of the reference.
		// Create a new TDataStore instance for this reference
		// The block size is the size of the type times the given type instances per block.
		ref->_data = new ResultDataStorage(segment_size, block_size * ResultDataStatic::TypeInfoArray[ref->_type].Size,
			recycle);
		// Set signal to no go if the construction did not go alright.
		rv = ref->_data->isValid();
	}
	// Check if no errors occurred so far before generating some more.
	if (rv)
	{ // Attach ref to this result instance.
		rv = attachRef(ref);
	}
	else
	{ // Delete and destruct the reference if it cannot be attached.
		delete_null(ref);
	}
	//
	if (rv)
	{ // Notify all global linked users of TVariable instances of new ID.
		emitGlobalEvent(reNewId, Range(), false);
		attachDesired();
	}
	//
	return rv;
}

bool ResultData::setup(const std::string& definition, long id_ofs)
{
	// Detach existing reference first by Attaching to ZeroRef
	if (!attachRef(ResultDataStatic::zero()._reference))
	{
		return false;
	}
	// Pointer that points to the place where the conversion of the ID went wrong.
	char* end_ptr = nullptr;
	// Get the result ID from the setup string.
	std::string tmp = GetCsfField(rfeId, definition);
	long id = strtol(tmp.c_str(), &end_ptr, 0);
	if (id)
	{
		id += id_ofs;
	}
	// The default return value is true.
	bool ret_val = true;
	// Return zero if an error occurred durring conversion of the ID.
	if (end_ptr && *end_ptr != '\0') {ret_val = false;}
	// Check if variables other then the zero variable have an ID of zero.
	if (_reference != ResultDataStatic::zero()._reference && id == 0) {ret_val = false;}
	// Don' bother to go on if an error occurred so far.
	if (ret_val)
	{
		// check if the id already exist
		// GetResultRefById returns ZeroResultData if not found
		ResultDataReference* ref = getReferenceById(id);
		if (ref && ref != ResultDataStatic::zero()._reference)
		{
			_RTTI_NOTIFY(DO_DEFAULT | DO_MSGBOX, "Tried to create duplicate ID: !\n"
				<< definition << "\nover!\n" << ref->_name << '!')
			// If ref already exist return false.
			return false;
		}
		else
		{
			// create new reference for this id
			ref = new ResultDataReference();
		}

		// Copy definition std::string in to std::string class buffer.
		std::string defin = definition;
		// Set the reference valid flag default to true.
		ref->_valid = true;
		// Read all std::string field values into the reference fields
		ref->_id = id;
		// First one to get id zero is the ZeroResultData result.
		ref->_sequenceId = ResultDataStatic::SeqIdCounter++;
		ref->_name = GetCsfField(rfeName, defin);
		ref->_flags = stringToFlags(GetCsfField(rfeFlags, defin).c_str());
		ref->_curFlags = ref->_flags;// copy of original flags
		ref->_description = escape(GetCsfField(rfeDescription, defin));
		ref->_type = getType(GetCsfField(rfeType, defin).c_str());
		auto max_bits = getTypeSize(ref->_type) * 8;
		ref->SignificantBits = (unsigned) strtoul(GetCsfField(rfeSigBits, defin).c_str(), &end_ptr, 0);
		ref->Offset = strtoul(GetCsfField(rfeOffset, defin).c_str(), &end_ptr, 0);
		// If the significant bits value is zero the maximum amount of bits
		// is taken for this instance. Also if the value is larger then the maximum
		// it is adjusted.
		if (ref->SignificantBits == 0 || ref->SignificantBits > max_bits)
		{
			ref->SignificantBits = max_bits;
		}
		//
		tmp = GetCsfField(rfeBlockSize, defin);
		size_type block_size = strtoul(tmp.c_str(), &end_ptr, 0);
		// Correct impossible block size.
		if (block_size <= 0)
		{
			block_size = 1;
		}
		ret_val &= (end_ptr && *end_ptr == '\0');
		tmp = GetCsfField(rfeSegmentSize, defin);
		size_type segment_size = strtoul(tmp.c_str(), &end_ptr, 0);
		//
		ret_val &= (end_ptr && *end_ptr == '\0');
		// Do not allow segment size of zero when not ID is zero.
		if (ref->_id && !segment_size)
		{
			ret_val = false;
		}
		// Create the data store with the passed information.
		if (ret_val)
		{
			ret_val = createDataStore(ref, segment_size, block_size);
		}
	}
	// If an error occurred On error report to standard out.
	_COND_RTTI_NOTIFY(!ret_val, DO_MSGBOX | DO_CERR | DO_COUT, "setup String Fault: \"" << definition << '"')
	return ret_val;
}

bool ResultData::setup(const ResultData::Definition* definition)
{
	// Detach existing reference first by Attaching to ZeroRef
	if (!attachRef(ResultDataStatic::zero()._reference))
	{
		return false;
	}
	// The default return value is true.
	bool retval = true;
	// Check if variables other then the zero variable have an ID of zero.
	if (_reference != ResultDataStatic::zero()._reference && definition->_id == 0)
	{
		retval = false;
	}
	// Don' bother to go on if an error occurred so far.
	if (retval)
	{ // check if the id already exist
		// GetResultRefById returns ResultDataStatic::zero() if not found
		ResultDataReference* ref = getReferenceById(definition->_id);
		if (ref && ref != ResultDataStatic::zero()._reference)
		{
			_RTTI_NOTIFY(DO_DEFAULT | DO_MSGBOX, "Tried to create duplicate ID: !\n"
				<< stringf("0x%lX,%s", definition->_id, definition->_name.c_str()) << "\nover!\n"
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
		ref->_id = definition->_id;
		// First one to get id zero is the ResultDataStatic::zero() result.
		ref->_sequenceId = ResultDataStatic::SeqIdCounter++;
		ref->_name = definition->_name;
		ref->_flags = definition->_flags;
		// copy of original flags
		ref->_curFlags = ref->_flags;
		ref->_description = definition->_description;
		ref->_type = definition->_type;
		size_t max_bits = getTypeSize(ref->_type) * 8;
		ref->SignificantBits = definition->_significantBits;
		ref->Offset = definition->_offset;
		// If the significant bits value is zero the maximum amount of bits
		// is taken for this instance. Also if the value is larger then the maximum
		// it is adjusted.
		if (ref->SignificantBits == 0 || ref->SignificantBits > max_bits)
		{
			ref->SignificantBits = max_bits;
		}
		//
		long block_size = definition->_blockSize;
		// Correct impossible block size.
		if (block_size <= 0)
		{
			block_size = 1;
		}
		long segment_size = definition->_segmentSize;
		// Do not allow segment size of zero when not ID is zero.
		if (ref->_id && !segment_size)
		{
			retval = false;
		}
		// Create the data store with the passed information.
		if (retval)
		{
			retval = createDataStore(ref, segment_size, block_size);
		}
	}
	// If an error occurred On error report to standard out
	_COND_RTTI_NOTIFY(!retval, DO_MSGBOX | DO_CERR | DO_COUT, "setup Definition Struct Fault: " << definition->_name)
	return retval;
}

void ResultData::emitEvent(EEvent ev, const ResultData& caller, const Range& rng)
{
	// Check if a handler was linked and if so call it.
	if (_handler)
	{
		_handler->ResultDataEventHandler(ev, caller, *this, rng, &caller == this);
	}
}

unsigned ResultData::emitEvent(EEvent event, const Range& rng, bool skip_self)
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
	ResultDataStatic::GlobalActive++;
	Vector ev_list;
	// Iterate through list and generate the variables from which
	// the event handler needs to be called.
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
	ResultDataStatic::GlobalActive--;
	// return the amount of variables that were effected by the call to this function.
	return ev_list.count();
}

ResultData::size_type ResultData::emitGlobalEvent(EEvent event, const Range& rng, bool skip_self)
{
	// Disable deletion of instances.
	ResultDataStatic::GlobalActive++;
	// Declare event list for instance pointers.
	Vector ev_list;
	// Iterate through all references.
	for (auto ref: *ResultDataStatic::RefList)
	{
		// Iterate through all variables of the reference.
		for (auto rd: ref->_list)
		{
			if (rd && (!skip_self || rd != this))
			{
				// Add variable to list.
				ev_list.add(rd);
			}
		}
	}
	// Iterate through the generated variable list and call the handler of them.
	// Any changes made by event handlers that could affect the list is avoided.
	for (auto k: ev_list)
	{
		k->emitEvent(event, *this, rng);
	}
	// Enable deletion of instances.
	ResultDataStatic::GlobalActive--;
	// Return  the lists size as the count of the events.
	return ev_list.count();
}

unsigned ResultData::attachDesired()
{
	// Disable deletion of instances.
	ResultDataStatic::GlobalActive++;
	// Signal other event handler functions are called using a global predefined list.
	Vector ev_list;
	// Iterate through all references generating pointers to instances having a desired ID that must be attached.
	for (auto ref: *ResultDataStatic::RefList)
	{
		// Iterate through all variables of the reference.
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
	// Iterate through the generated variable list and attach them.
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
	ResultDataStatic::GlobalActive--;
	// Return  the lists size as the count of the events.
	return ev_list.count();
}

void ResultData::setHandler(ResultDataHandler* handler)
{
	// Only generate events if there is a change of Varlink
	if (_handler != handler)
	{
		if (handler)
		{ // Notify instance getting event link
			_handler = handler;
			emitEvent(reLinked, *this, _reference->_rangeManager->getManaged());
			_transId = (int64_t) this;
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
	// Get the total count of results references.
	size_t vc = ResultDataStatic::RefList ? ResultDataStatic::RefList->count() : 0;
	// Iterate through result references
	for (size_t i = 0; i < vc; i++)
	{
		// Get the current result pointer.
		ResultDataReference* ref = (*ResultDataStatic::RefList)[i];
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

bool ResultData::getRequests(Range::Vector& reqlist) const
{
	// Clear the list first.
	reqlist.flush();
	// Can only get when it is the owner.
	if (isOwner())
	{ // Check if there is anything to get.
		if (_reference->_rangeManager->getActualRequests().count())
		{ // Just make a copy of the real request list of the manager.
			reqlist = _reference->_rangeManager->getActualRequests();
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
	return const_cast<Range&> (_reference->_rangeManager->getManaged()).setId(_reference->_id);
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
			{ // Increment the reserved block count to satisfy the access range.
				if (!_reference->_data->reserve(nr.getStop()))
				{
					_RTTI_NOTIFY(DO_DEFAULT, "Failed To Set Reserve Blocks")
					return false;
				}
				// Generate an event to notify users of the change in reserved blocks.
				// The new value is put in the Stop Value of the passed Range value.
				emitLocalEvent(reReserve, Range(0, _reference->_data->getBlockCount(), _reference->_id), skip_self);
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

ResultData::EType ResultData::getType(const char* type_str)
{
	for (int i = rtInvalid; i < rtLastEntry; i++)
	{
		if (!strcmp(type_str, ResultDataStatic::TypeInfoArray[i].Name))
		{
			return ResultData::EType(i);
		}
	}
	return rtInvalid;
}

const char* ResultData::getType(ResultData::EType type)
{
	return ResultDataStatic::TypeInfoArray[(type >= rtLastEntry || type < 0) ? rtInvalid : type].Name;
}

ResultData::size_type ResultData::getTypeSize(ResultData::EType type)
{
	return ResultDataStatic::TypeInfoArray[(type >= rtLastEntry || type < 0) ? rtInvalid : type].Size;
}

void ResultData::setDesiredId(long id)
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

int ResultData::getNameLevelCount() const
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

ResultData::flags_type ResultData::stringToFlags(const char* flags)
{
	if (!flags)
	{
		return 0;
	}
	struct
	{
		char ch;
		int flag;
	}
		a[] =
		{
			{'R', flgRecycle},
			{'A', flgArchive},
			{'S', flgShare},
			{'H', flgHidden},
			{0, 0}
		};

	int rv = 0;
	int i = 0;
	while (flags[i])
	{
		int t = 0;
		while (a[t].ch)
		{
			if (a[t].ch == flags[i])
			{
				rv |= 1 << t;
			}
			t++;
		}
		i++;
	}
	return rv;
}

std::string ResultData::getFlagsString(long flg)
{
	struct
	{
		char ch;
		int flag;
	}
		a[] =
		{
			{'R', flgRecycle},
			{'A', flgArchive},
			{'S', flgShare},
			{'H', flgHidden},
			{0, 0}
		};

	char flags[33];
	char* tmp = &flags[0];
	memset(flags, 0, sizeof(flags));
	for (int i = 0; a[i].ch; i++)
	{
		if (flg & a[i].flag)
		{
			*tmp++ = a[i].ch;
		}
	}
	return &flags[0];
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
		long tflgs = _reference->_curFlags;
		// Set the flag bit.
		_reference->_curFlags |= flag;
		// Check for a change in flags.
		if (_reference->_curFlags != tflgs)
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
		long tflgs = _reference->_curFlags;
		// Unset the flag bit.
		_reference->_curFlags &= ~flag;
		// Check for a change in flags.
		if (_reference->_curFlags != tflgs)
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
	bool retval = false;
	// Only the owner is allowed to set the flags.
	if (isOwner())
	{ // Initially signal success.
		retval = true;
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
				retval = true;
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
	return retval;
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

bool ResultData::setReservedBlockCount(long sz, bool skip_self)
{
	// Only an owner is allowed to set reserve blocks.
	if (!isOwner() || !isDataOwner())
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Only Owner Can Set Reserve Blocks")
		return false;
	}
	// Check if this call really reserves some extra blocks or not.
	// Otherwise skip it from here.
	if (sz > _reference->_data->getBlockCount())
	{
		// If reserved failed notify.
		if (!_reference->_data->reserve(sz))
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Failed To Set Reserve Blocks!")
			return false;
		}
		// Generate an event to notify users of the change in reserved blocks.
		// The new value is put in the Stop Value of the passed Range value.
		emitLocalEvent(reReserve, Range(0, _reference->_data->getBlockCount()), skip_self);
	}
	return true;
}

bool ResultData::blockWrite(long ofs, long sz, const void* src, bool auto_reserve)
{
	// Only the owner of the instance may write to it.
	if (!isOwner() || !isDataOwner())
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Only Owner May Write Blocks!")
		// Bail out
		return false;
	}
	// When the passed offset is -1 the data must be appended.
	if (ofs == -1)
	{

		ofs = _reference->_rangeManager->getManaged().getStop();
	}
	// Check there are enough blocks reserved to write the data.
	if (_reference->_data->getBlockCount() < (ofs + sz))
	{
		// Check the autoreserve flag
		if (!auto_reserve)
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Failed To Write Blocks Due To Lak Of Reserved Blocks!")
			// Bail out
			return false;
		}
		else
		{
			// Try to reserve the required blocks.
			if (!setReservedBlockCount(ofs + sz))
			{
				_RTTI_NOTIFY(DO_DEFAULT, "Failed To Reserve Blocks!")
				// Bail out
				return false;
			}
		}
	}
	// If the function succeeds update the accessible range list.
	if (!_reference->_data->blockWrite(ofs, sz, src))
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Failed Call On Write Blocks Function!")
		// Bail out
		return false;
	}

	// To intercept unwanted calls to this function for debugging.
	//LocalEvent(reUSERLOCAL, Range(ofs, ofs + sz, FRef->Id), false);

	// Add this range to the validate cache that will be processed when
	// 'CommitValidations' is call for.
	_reference->_validatedCache.add(Range(ofs, ofs + sz, _reference->_id));
	return true;
}

void ResultData::validateRange(Range r)
{
	// Only the owner can validate ranges.
	if (isOwner())
	{
		_reference->_validatedCache.add(r.setId(_reference->_id));
	}
}

void ResultData::validateRange(const Range::Vector& rl)
{
	// Only the owner can validate ranges.
	if (isOwner())
	{
		for (auto& r: rl)
		{
			Range rng(r.getStart(), r.getStop(), _reference->_id);
			_reference->_validatedCache.add(rng);
		}
	}
}

void ResultData::commitValidations(bool skip_self)
{
	// Only the owner can call this function.
	if (isOwner() && _reference->_validatedCache.count())
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Committing Full Vector: " << _reference->_validatedCache)
		// Remove all ranges or part of ranges that were already committed/validated.
		_reference->_validatedCache.rearrange();
		_reference->_validatedCache.exclude(_reference->_rangeManager->getAccessibles());
		_RTTI_NOTIFY(DO_DEFAULT, "Committing Reduced Vector: " << _reference->_validatedCache)
		// Retrieve the current extends and compare them later.
		Range aer = _reference->_rangeManager->getManaged();
		// Create a temporary vector to let the function to return the resolved.
		// requests in.
		Range::Vector rrl;
		// Validate by telling the range manager that the ranges are accessible.
		// When the function returns true requests are fulfilled an returned in 'rrl'.
		if (_reference->_rangeManager->setAccessible(_reference->_validatedCache, rrl))
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Range Requests Resolved:" << rrl)
			// Iterate through the vector of resolved requested ranges.
			for (auto& rng: rrl)
			{
				// Iterate through the result list and find the one with the
				// the same reference as this one
				for (auto rd: _reference->_list)
				{
					// Check if the pointer is valid Check if the transaction id is
					// non-zero and equal to the current in the list.
					if (rd && rd->_transId && rd->_transId == rng.getId() && rd->_handler)
					{
						// Generate an event for one of the result that requested it.
						rd->emitEvent(reGotRange, *this, rng);
					}
				}
				// Generate an event for all of the results having the same ID as this instance.
				//LocalEvent(reGOTRANGE, i.Current(), false);
			}
		}
		// Check if the extends have changed and generate an event for it.
		if (aer != _reference->_rangeManager->getManaged())
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Access Range Has Changed: " << _reference->_rangeManager->getManaged())
			// Generate an event for all of the results having the same ID as this instance.
			emitLocalEvent(reAccessChange, _reference->_rangeManager->getManaged(), skip_self);
		}
		// Notify clients of the committed vector.
		emitLocalEvent(reCommitted, _reference->_rangeManager->getManaged(), skip_self);
		// Flush the validate cache
		_reference->_validatedCache.flush();
	}
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
			// Flush all ranges within the rangemanager.
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

bool ResultData::blockRead(long ofs, long sz, void* src, bool force) const
{
	// If force is true do not check validity of the range.
	if (!force && !isRangeValid(ofs, sz))
	{
		_RTTI_NOTIFY(DO_DEFAULT, "blockRead(" << ofs << "," << sz << ") Invalid Range!")
		// Return false because the wanted range is not completely validated.
		return false;
	}
	// BlockRead returns false if the block count is not available.
	bool ret_val = _reference->_data->blockRead(ofs, sz, src);
	if (!ret_val)
	{
		_RTTI_NOTIFY(DO_DEFAULT, "TDataStore::blockRead(" << ofs << "," << sz << ") failed!")
	}
	return ret_val;
}

bool ResultData::readIndexRange(long ofs, Range& range)
{
	// Only when the block size is at least that of the range start and stop values.
	if (_reference->_data->getBlockSize() <= (long) sizeof(long))
	{
		bool ok;
		long idx[2] = {0, 0};
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
			range.assign(idx[0], idx[1]);
		}
		//
		return ok;
	}
	// Failed to read index range.
	return false;
}

bool ResultData::readIndexRange(const Range& ridx, Range& range)
{
	// Only when the block size is at least that of the range start and stop values.
	if (_reference->_data->getBlockSize() <= (int32_t) sizeof(long) && ridx.getSize())
	{
		bool ok = true;
		long idx[2] = {0, 0};
		if (ridx.getStart() != 0)
		{
			ok &= blockRead(ridx.getStart() - 1, 1, &idx[0]);
		}
		ok &= blockRead(ridx.getStop() - 1, 1, &idx[1]);
		// Assign the range on success.
		if (ok)
		{
			range.assign(idx[0], idx[1]);
		}
		//
		return ok;
	}
	// Failed to read index range.
	return false;
}

int32_t ResultData::getValue(ResultData::size_type idx, const void* data) const
{
	char* p = const_cast<char*>((const char*) data);
	// Calculate the new data pointer from the type and passed index.
	p += getTypeSize(_reference->_type) * idx;
	// Call the index less version.
	return getValue(p);
}

int32_t ResultData::getValue(const void* data) const
{
	unsigned long mask;
	// Shift of 32 bits is the same as 0 which means it is actually a ROL
	// operation so special treatment for significant bits value of 32 and larger.
	if (_reference->SignificantBits < sizeof(mask) * 8)
	{
		// Only set those bits in the mask that count.
		mask = (1UL << _reference->SignificantBits) - 1UL;
	}
	else
	{
		// Set all the bits in the mask.
		mask = (unsigned long) -1;
	}
	//
	return int32_t(*(uint32_t*) data & mask) - _reference->Offset;
}

uint32_t ResultData::getValueU(ResultData::size_type idx, const void* data) const
{
	char* p = (char*) data;
	// Calculate the new data pointer from the type and passed index.
	p += getTypeSize(_reference->_type) * idx;
	// Call the index less version.
	return getValueU(p);
}

uint32_t ResultData::getValueU(const void* data) const
{
	unsigned long mask = (1UL << _reference->SignificantBits) - 1UL;
	if (mask == 0)
	{
		return *(unsigned long*) data;
	}
	else
	{
		return *(unsigned long*) data & mask;
	}
}

void ResultData::clearRequests()
{
	// Remove all requests of this client.
	_reference->_rangeManager->flushRequests(_transId);
}

bool ResultData::requestRange(const Range& rr)
{
	// Reject requests made on ResultDataStatic::zero() result.
	if (!_reference->_id || rr.isEmpty())
	{
		return false;
	}
	// Copy the range to work with
	Range r(rr);
	// Set the transaction id
	r.setId(getTransId());
	// Create a range vector for the function to return the real requests in.
	Range::Vector rrl;
	// Call the request function of the range manager
	auto result = _reference->_rangeManager->request(r, rrl);
	switch (result)
	{
		case RangeManager::rmAccessible:
			_RTTI_NOTIFY(DO_DEFAULT, "Request " << rr << " Is Accessible!")
			break;

		case RangeManager::rmOutOfRange:
			_RTTI_NOTIFY(DO_DEFAULT, "Request " << rr << " Is Out Of Range!")
			break;

		case RangeManager::rmInaccessible:
		{
			// Check if the owner of the result reference has an event link.
			ResultData* rd = _reference->_list[0];
			if (rd->_handler)
			{
				// Generate an event for each real request for
				// the owner of the result reference.
				Range::Iterator i(rrl);
				while (i)
				{
					rd->emitEvent(reGetRange, *this, ((Range) i++).setId(_reference->_id));
				}
				// Return true to notify caller success.
				return true;
			}
			else
			{
				_RTTI_NOTIFY(DO_DEFAULT, "Request Impossible, Unlinked Owner!")
			}
			break;
		}
	}
	// Return false to notify caller failure.
	return false;
}

bool ResultData::isIndexRangeValid(const Range& r) const
{
	RANGE rng;
	// Apply an offset to the passed index range start because it is an index index
	rng._start = r.getStart() - 1;
	rng._stop = r.getStop();
	// Negative values are not allowed.
	if (rng._start < 0)
	{
		rng._start = 0;
	}
	if (rng._stop < 0)
	{
		rng._stop = 0;
	}
	//
	return _reference->_rangeManager->isAccessible(Range(rng));
}

bool ResultData::requestIndexRange(const Range& r)
{
	RANGE rng;
	// Apply an offset to the passed index range because it is an index index
	rng._start = r.getStart() - 1;
	rng._stop = r.getStop();
	// Negative values are not allowed.
	if (rng._start < 0)
	{
		rng._start = 0;
	}
	//
	return requestRange(Range(rng));
}

const char* ResultData::getEventName(EEvent event)
{
	const char* retval = "Unknown";
	switch (event)
	{
		case reNewId:
			retval = "NewId";
			break;

		case reFlagsChange:
			retval = "FlagsChange";
			break;

		case reAccessChange:
			retval = "AccessChange";
			break;

		case reReserve:
			retval = "Reserve";
			break;

		case reInvalid:
			retval = "Invalid";
			break;

		case reIdChanged:
			retval = "IdChanged";
			break;

		case reRemove:
			retval = "Remove";
			break;

		case reGetOwner:
			retval = "GetOwner";
			break;

		case reLostOwner:
			retval = "LostOwner";
			break;

		case reLinked:
			retval = "Linked";
			break;

		case reUnlinked:
			retval = "Unlinked";
			break;

		case reGotRange:
			retval = "GotRange";
			break;

		case reGetRange:
			retval = "GetRange";
			break;

		case reCommitted:
			retval = "Committed";
			break;

		default:
			// Check for global event.
			if (event < 0)
			{
				retval = "UserGlobal";
				break;
			}
			else
			{
				if (event >= 0 && event < reFirstPrivate)
				{
					retval = "UserLocal + ?";
				}
				else
				{
					if (event >= reUserPrivate)
					{
						retval = "UserPrivate + ?";
					}
				}
			}
	}
	return retval;
}

bool ResultData::writeUpdate(std::ostream& os) const
{
	if (!_reference)
	{
		_RTTI_NOTIFY(DO_MSGBOX | DO_CERR | DO_COUT,": TResultData::WriteUpdate() encountered nullptr\n")
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
	{ // GetResultDataById get reference to owner to beable to update.
		// Check if the reference pointer is non-zero and look only into that list for the id.
		ResultData& res((&list) ? getResultDataById(rng.getId(), list) : (ResultData&) getResultDataById(rng.getId()));
		// Check if new passed 'Stop' value is smaller then current one.
		// If so this result should be cleared. first
		if (rng.getStop() < res._reference->_rangeManager->getManaged().getStop())
		{
			res.clearValidations();
		}
		// Change the managed range
		res.setAccessRange(rng, skip_self);
		// Update the flags.
		res.updateFlags(stringToFlags(flags.c_str()), skip_self);
		// Notify caller success of this operation.
		return true;
	}
	// Notify the caller failure of this operation.
	return false;
}

bool ResultData::create(std::istream& is, Vector& list, int& err_line)
{
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
			_NORM_NOTIFY(DO_DEFAULT, "TResultData: Input stream failure")
			return false;
		}
		if (is.bad())
		{
			_NORM_NOTIFY(DO_DEFAULT, "TResultData: Bad input stream")
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
			auto* rd = new ResultData(st);
			if (rd && rd->isValid())
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

bool ResultData::isDataOwner() const
{
	return _reference->_data->isOwner() || _reference->_id == 0;
}

bool ResultData::setDataOwner(bool shared) const
{
	return _reference->_data->setOwner(shared);
}

bool ResultData::isValid() const
{
	return _reference->_valid;
}

unsigned long ResultData::getValueOffset() const
{
	return _reference->Offset;
}

unsigned long ResultData::getValueRange() const
{
	unsigned long max_val = (1UL << _reference->SignificantBits) - 1UL;
	return max_val ? max_val : ULONG_MAX;
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

ResultDataTypes::size_type ResultData::getBlockCount() const
{
	return _reference->_rangeManager->getManaged().getStop();
}

long ResultData::getSegmentSize() const
{
	return _reference->_data->getSegmentSize();
}

long ResultData::getSegmentCount() const
{
	return _reference->_data->getSegmentCount();
}

long ResultData::getReservedSize() const
{
	return _reference->_data->getSize();
}

long ResultData::getReservedBlockCount() const
{
	return _reference->_data->getBlockCount();
}

bool ResultData::isRangeValid(long ofs, long sz) const
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

long ResultData::getFlags() const
{
	return _reference->_flags;
}

long ResultData::getCurFlags() const
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
	return _reference->SignificantBits;
}

const ResultData& ResultData::getResultDataById(ResultDataTypes::id_type id)
{
	return *getReferenceById(id)->_list[0];
}

unsigned ResultData::getUsageCount() const
{
	return _reference->_list.count();
}

const ResultDataStorage& ResultData::getDataStore() const
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
