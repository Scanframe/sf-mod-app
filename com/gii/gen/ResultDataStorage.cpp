#include <iostream>
#include <cstring>

#include <misc/gen/dbgutils.h>
#include <misc/gen/gen_utils.h>
#include "ResultDataStorage.h"

namespace sf
{

CriticalSection ResultDataStorage::_staticSync; // NOLINT(cert-err58-cpp)

ResultDataStorage::ResultDataStorage(ResultDataStorage::size_type seg_sz, ResultDataStorage::size_type blk_sz,
	unsigned recycle)
	:_reference(nullptr)
	 , _cachedSegmentIndex(-1)
	 , _segmentRecycleCount(recycle)
{
	_reference = new Reference();
	_reference->_threadId = Thread::getCurrentThreadHandle();
	_reference->_referenceCount = 1;
	_reference->_blockSize = blk_sz;
	_reference->_segmentSize = seg_sz;
	_reference->_flagOwner = true;
	_reference->_flagValid = _reference->_blockSize >= 0 && _reference->_segmentSize >= 0;
}

ResultDataStorage::ResultDataStorage(const ResultDataStorage& ds)
	:_reference(nullptr), _cachedSegmentIndex(-1)
{
	// Lock the classes when copying reference pointers.
	CriticalSection::lock static_lock(_staticSync);
	// Lock also the reference data.
	Reference::MtLock lock(ds._reference->_criticalSection);
	// Increment the usage count.
	ds._reference->_referenceCount++;
	// Copy the reference pointer.
	_reference = ds._reference;
}

ResultDataStorage::~ResultDataStorage()
{
	// Lock the classes when copying reference pointers.
	Reference::MtLock static_lock(_staticSync);
	// Scope for lock class.
	{
		// Lock also the reference data.
		Reference::MtLock lock(_reference->_criticalSection);
		// Decrement the usage count.
		_reference->_referenceCount--;
		// Check if the reference must be deleted. If not bail out here.
		if (_reference->_referenceCount)
		{
			return;
		}
	}
	// Delete all segments instances belonging to this instance.
	unsigned count = _reference->_segmentList.count();
	if (_segmentRecycleCount && _reference->_segmentList.count() >= _segmentRecycleCount)
	{
		count = _segmentRecycleCount;
	}
	for (unsigned i = 0; i < count; i++)
	{
		delete_null(_reference->_segmentList[i]);
	}
	// Destroy the reference.
	delete _reference;
	_reference = nullptr;
}

void ResultDataStorage::flush()
{
	Reference::MtLock lock(_reference->_criticalSection);
	// Can only flush when it is the creator thread.
	if (_reference->_threadId == Thread::getCurrentThreadHandle())
	{
		// Delete all segments instances belonging to this instance.
		unsigned count = _reference->_segmentList.count();
		if (_segmentRecycleCount && _reference->_segmentList.count() >= _segmentRecycleCount)
		{
			count = _segmentRecycleCount;
		}
		for (unsigned i = 0; i < count; i++)
		{
			delete_null(_reference->_segmentList[i]);
		}
		// Remove all entries in the dynamic list.
		_reference->_segmentList.flush();
		// Set locked segment to none.
		_cachedSegmentIndex = -1;
	}
}

bool ResultDataStorage::setRecycleCount(unsigned count)
{
	Reference::MtLock lock(_reference->_criticalSection);
	if (_reference->_segmentList.count() == 0)
	{
		_segmentRecycleCount = count;
		return true;
	}
	_RTTI_NOTIFY(DO_DEFAULT, "Not allowed to set recycle count when data is already stored!")
	return false;
}

bool ResultDataStorage::reserve(ResultDataStorage::size_type block_count)
{
	Reference::MtLock lock(_reference->_criticalSection);
	// Only owners are allowed to reserve memory
	if (!_reference->_flagOwner)
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Only owners are allowed to reserve memory!")
		return false;
	}
	// Check if the new value is an increments block count or not
	if (block_count < getBlockCount())
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Tried to decrease block count by calling reserve!")
		// Return false to indicate failure.
		return true;
	}
	// Get the new needed amount of needed segments.
	auto seg_cnt = (unsigned) ((block_count / _reference->_segmentSize) +
		((block_count % _reference->_segmentSize) ? 1L : 0L));
	// Add enough segments to hold the amount of required blocks.
	while (_reference->_segmentList.count() < seg_cnt)
	{
		// When segments must be recycled and the current segment count is over
		// recycle amount.
		if (_segmentRecycleCount && _reference->_segmentList.count() >= _segmentRecycleCount)
		{
			size_type seg = _reference->_segmentList.count() % _segmentRecycleCount;
			//_RTTI_NOTIFY(DO_DEFAULT, "Reusing segment " << seg << " as " << Ref->SegList.Count())
			_reference->_segmentList.add(_reference->_segmentList[seg]);
		}
		else
		{
			_reference->_segmentList.add(new Segment(_reference->_segmentSize * _reference->_blockSize));
		}
		//_RTTI_NOTIFY(DO_DEFAULT, "Reserved Segment For Owned Data.")
	}
	// Return true to indicate success.
	return true;
}

bool ResultDataStorage::cacheSegment(int idx)
{
	// TODO: Could improve the cashed segment when recycling.
	// Check if the data is currently cached locked by this instance.
	if (_cachedSegmentIndex != idx)
	{
		// Check if a previous locked segment must be unlocked.
		if (_cachedSegmentIndex != -1)
		{ // Unlock the segment,
			_reference->_segmentList[_cachedSegmentIndex]->doUnlockMemory();
			// Set the cached segment index to none.
			_cachedSegmentIndex = -1;
		}
		// Try lock the segment for this data store class.
		if (_reference->_segmentList[idx]->doLockMemory())
		{
			// Set the cached locked segment.
			_cachedSegmentIndex = idx;
		}
		else
		{
			// Signal failure.
			return false;
		}
	}
	return true;
}

void ResultDataStorage::uncacheSegment(int)
{
	// Do nothing yet.
}

bool ResultDataStorage::blockReadWrite(bool rd, ResultDataStorage::size_type ofs, ResultDataStorage::size_type sz,
	void* data)
{
	Reference::MtLock lock(_reference->_criticalSection);
	// Check if the size addressed data is in range
	if (ofs < 0L || sz < 0L)
	{
		_RTTI_NOTIFY(DO_DEFAULT, (rd ? "Read(" : "Write(") << ofs << ',' << sz << ") invalid parameters!")
		return false;
	}
	if ((ofs + sz) > getBlockCount())
	{
		_RTTI_NOTIFY
		(
			DO_DEFAULT,
			(rd ? "Read(" : "Write(") << ofs << ',' << sz << ") out of scope!"
		)
		_RTTI_NOTIFY(DO_DEFAULT, "getBlockCount() = " << getBlockCount())
		return false;
	}
	// Calculate starting segment from offset.
	size_type seg_i = ofs / _reference->_segmentSize;
	// Calculate offset for segment from the passed offset.
	size_type offset = ofs % _reference->_segmentSize;
	// Copy the passed block count value.
	size_type count = sz;
	// Create temporary read pointer initialized with the 'data' address.
	auto rwp = static_cast<char*>(data);
	// Check if count is non-zero before entering the do-while loop.
	if (count)
	{
		// Write as long as count is larger then zero.
		do
		{ // Lock the segment if it hasn't been locked yet.
			cacheSegment(seg_i);
			// Calculate the amount of blocks to write for write in loop.
			// Calculate the amount of blocks to write for first write in loop.
			size_type blocks = (offset)
				? (_reference->_segmentSize - offset) > count ? count : (_reference->_segmentSize - offset)
				: (count < _reference->_segmentSize) ? count : _reference->_segmentSize;
			// Check iterator scope.
			_COND_RTTI_THROW(seg_i >= (int) _reference->_segmentList.count(),
				(rd ? "Read" : "Write") << " block iterator out of scope!")
			// Abort loop when a read/write fails.
			if
				(rd
				? !_reference->_segmentList[seg_i]->read(offset * _reference->_blockSize, blocks * _reference->_blockSize, rwp)
				: !_reference->_segmentList[seg_i]->write(offset * _reference->_blockSize, blocks * _reference->_blockSize, rwp)
				)
			{
				_RTTI_NOTIFY(DO_DEFAULT, "Block" << (rd ? "Read" : "Write") << " Failed!")
				// Return false indicating failure.
				return false;
			}
			// When any segments should be unlocked.
			uncacheSegment(seg_i);
			// Set offset to zero so next segment starts at offset zero.
			if (offset)
			{ // Clear offset
				offset = 0;
			}
			// Correct read/write pointer
			rwp += (blocks * _reference->_blockSize);
			// Decrement the amount of blocks with that still must be written.
			count -= blocks;
			// Increment the segment counter for the loop.
			seg_i++;
		}
		while (count > 0);
	}
	// Return true on success.
	return true;
}

bool ResultDataStorage::blockRead(size_type ofs, size_type sz, void* src) const
{
	return const_cast<ResultDataStorage*>(this)->blockReadWrite(true, ofs, sz, src);
}

ResultDataStorage::size_type ResultDataStorage::getSegmentLocks() const
{
	Reference::MtLock lock(_reference->_criticalSection);
	size_type rv = 0;
	size_type count = _reference->_segmentList.count();
	if (_segmentRecycleCount && _reference->_segmentList.count() >= _segmentRecycleCount)
	{
		count = _segmentRecycleCount;
	}
	for (size_type i = 0; i < count; i++)
	{
		rv += _reference->_segmentList[i]->LockCount;
	}
	return rv;
}

/**
 * Implementation of functions of Class 'TDataStore::TSegment'.
 */
ResultDataStorage::Segment::Segment(ResultDataStorage::size_type sz)
:_fileMapper(*IFileMapper::instantiate(true))
{
	FlagOwner = true;
	// Check if the size of the segment is larger then zero before allocating
	if (Size > 0)
	{
		// Use the system page file.
		_fileMapper.initialize();
		// Added extra size to file map to make casting of smaller int pointers possible.
		_fileMapper.createView(sz + sizeof(ResultDataStorage::size_type));
	}
}

ResultDataStorage::Segment::~Segment()
{
	// Clear all locks on this global handle.
	while (isLocked())
	{
		doUnlockMemory();
	}
	delete &_fileMapper;
}

bool ResultDataStorage::Segment::doLockMemory()
{
	// If the handle is already locked do not lock it again
	if (!LockCount)
	{
		if (!_fileMapper.mapView())
		{
			_RTTI_NOTIFY(DO_DEFAULT, "MapView on file mapper failed!")
			// Return NULL pointer to indicate failure to lock
			return false;
		}
		else
		{
			DataPtr = (char*) _fileMapper.getPtr();
		}
	}
	// Increase the lock count for this instance
	LockCount++;
	// return the pointer to the data
	return DataPtr != nullptr;
}

void ResultDataStorage::Segment::doUnlockMemory()
{
	// Decrease lock count data member
	LockCount--;
	// If the lock count is zero free the locked handle
	if (!LockCount)
	{
		if (!_fileMapper.unmapView()) _RTTI_NOTIFY(DO_DEFAULT, "UnmapView On FileMapper Failed!")
		{
			// Null the data pointer.
			DataPtr = nullptr;
		}
	}
	// Check if the lock count is lower then zero
	if (LockCount < 0)
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Unlocked Global Handle Too Many Times!")
		// Zero lock count for further use.
		LockCount = 0;
	}
}

bool
ResultDataStorage::Segment::write(ResultDataStorage::size_type ofs, ResultDataStorage::size_type sz, const void* src)
{
	// Can only write to owned global handle
	if (!FlagOwner)
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Tried To Write To Not Owned Memory!")
		return false;
	}
	// Debug notify of writing zero bytes of memory.
	_COND_RTTI_NOTIFY(!sz, DO_DEFAULT, "Tried To Write Zero Bytes of data!")
	if (!DataPtr || !src)
	{
		// Return false to indicate failure.
		return false;
	}
	// Check for a write within the boundaries of this.
	if (ofs + sz > Size)
	{
		// bring size into allowed range.
		sz = clip<size_type>(sz, 0, Size - ofs);
		_RTTI_NOTIFY(DO_DEFAULT, "Tried To Write Beyond End Of Memory!")
	}
	// All is well
	memcpy(DataPtr + ofs, src, sz);
	// Return true to indicate success.
	return true;
}

bool
ResultDataStorage::Segment::read(ResultDataStorage::size_type ofs, ResultDataStorage::size_type sz, void* dest) const
{
	// Debug notify when trying to read zero bytes.
	_COND_RTTI_NOTIFY(!sz, DO_DEFAULT, "Tried To Read Zero Bytes of data!")
	// Debug notify when trying to read zero bytes.
	_COND_RTTI_NOTIFY(!dest, DO_DEFAULT, "Passed NULL pointer for storing data!")
	// Bail out here.
	if (!dest || !DataPtr)
	{
		// Return false to indicate failure.
		return false;
	}
	// Check for a write within the boundaries of this instance.
	if (ofs + sz > Size)
	{ // bring size into allowed range
		sz = clip<size_type>(sz, 0, Size - ofs);
		_RTTI_NOTIFY(DO_DEFAULT, "Tried to read beyond end of memory!")
	}
	// All is well
	memcpy(dest, DataPtr + ofs, sz);
	// Return true to indicate success.
	return true;
}

namespace
{
int ref_counter = 0;
}

ResultDataStorage::Reference::Reference()
{
	ref_counter++;
}

ResultDataStorage::Reference::~Reference()
{
	ref_counter--;
}

bool ResultDataStorage::Lock::acquire(ResultDataStorage::size_type seg_idx)
{
	// Release the previous acquired segment first.
	release();
	// Lock the reference so we can do work safely.
	ResultDataStorage::Reference::MtLock(_store._reference->_criticalSection);
	// Check if the segment can be locked.
	if (seg_idx >= 0 && seg_idx < (size_type) _store._reference->_segmentList.count())
	{ // Get the segment pointer.
		_segment = _store._reference->_segmentList[seg_idx];
		// Lock the segment memory.
		_data = _segment->lockMemory();
		// Copy the index number for debugging purposes.
		_index = seg_idx;
		return true;
	}
	return false;
}

void ResultDataStorage_Test()
{
	auto ds = new ResultDataStorage(100000L, sizeof(int32_t));
	ds->reserve(1000000L);
	std::cout << "Total Allocated Bytes: " << ds->getSize() << '\n';

	std::cout << "Segment Size: " << ds->getSegmentSize() << '\n';
	std::cout << "Block Size: " << ds->getBlockSize() << '\n';

	std::cout << "Segment Count: " << ds->getSegmentCount() << '\n';
	std::cout << "Block Count: " << ds->getBlockCount() << '\n';

	typedef ResultDataStorage::size_type size_type;
	size_type size = ds->getBlockCount();
	auto* buf = new size_type[size];

	bool rw_ok = true;

	// Introduce errors at the beginning.
	for (size_type i = 0; i < size; i++) {buf[i] = 10000000L + i;}
	{
		rw_ok &= ds->blockWrite(0, 25, buf);
	}

	for (size_type k = 25; k < size; k++) {buf[k] = k;}
	{
		rw_ok &= ds->blockWrite(25, size - 25, &buf[25]);
	}

	// Check the data for errors
	for (long j = 0; j < size; j++) {buf[j] = 0;}
	{
		rw_ok &= ds->blockRead(0, size, buf);
	}

	// Only Check when writing and reading went okay.
	if (rw_ok)
	{
		long fail_count = 0;
		for (size_type t = 0; t < size; t++)
		{
			if (buf[t] != t)
			{
				std::cout << "Incorrect return value [" << t << "] = " << buf[t] << '\n';

				if (fail_count++ > 200)
				{
					std::cout << "Maximum amount of printed errors reached!\n";
					break;
				}
			}
		}
	}
	else
	{
		std::cout << "blockRead Or blockWrite Failed!\n";
	}

	delete ds;
	delete[] buf;
}

}
