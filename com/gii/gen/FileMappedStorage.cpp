#include <iostream>
#include <cstring>

#include <misc/gen/dbgutils.h>
#include <misc/gen/gen_utils.h>
#include "FileMappedStorage.h"

namespace sf
{

Mutex FileMappedStorage::_staticSync; // NOLINT(cert-err58-cpp)

FileMappedStorage::FileMappedStorage(FileMappedStorage::size_type seg_sz, FileMappedStorage::size_type blk_sz,
	FileMappedStorage::size_type recycle)
	:_segmentRecycleCount(recycle)
{
	_reference = new Reference();
	_reference->_threadId = Thread::getCurrentId();
	_reference->_referenceCount = 1;
	_reference->_blockSize = blk_sz;
	_reference->_segmentSize = seg_sz;
	// Segment and block size are not allowed to be zero.
	if (!_reference->_blockSize || !_reference->_segmentSize)
	{
		throw Exception("Segment and block size are not allowed to be zero!");
	}
}

FileMappedStorage::FileMappedStorage(const FileMappedStorage& ds)
{
	// Lock the classes when copying reference pointers.
	Mutex::Lock static_lock(_staticSync);
	// Lock also the reference data.
	Reference::MtLock lock(ds._reference->_mutex);
	// Increment the usage count.
	ds._reference->_referenceCount++;
	// Copy the reference pointer.
	_reference = ds._reference;
}

FileMappedStorage::~FileMappedStorage()
{
	// Lock the classes when copying reference pointers.
	Reference::MtLock static_lock(_staticSync);
	// Scope for lock class.
	{
		// Lock also the reference data.
		Reference::MtLock lock(_reference->_mutex);
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

void FileMappedStorage::flush()
{
	Reference::MtLock lock(_reference->_mutex);
	// Can only flush when it is the creator thread.
	if (_reference->_threadId == Thread::getCurrentHandle())
	{
		// Delete all segments instances belonging to this instance.
		unsigned count = _reference->_segmentList.count();
		// When recycling the count is different.
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
		_cachedSegmentIndex = npos;
	}
}

bool FileMappedStorage::setRecycleCount(FileMappedStorage::size_type count)
{
	Reference::MtLock lock(_reference->_mutex);
	if (_reference->_segmentList.count() == 0)
	{
		_segmentRecycleCount = count;
		return true;
	}
	SF_RTTI_NOTIFY(DO_DEFAULT, "Not allowed to set recycle count when data is already stored!")
	return false;
}

bool FileMappedStorage::reserve(FileMappedStorage::size_type block_count)
{
	Reference::MtLock lock(_reference->_mutex);
	// Check if the new value is an increments block count or not
	if (block_count < getBlockCount())
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to decrease block count by calling 'reserve()'!")
		// Do not return false since it is not a real failure.
		return true;
	}
	// Calculate the new needed amount of segments for the required blocks.
	auto seg_cnt = (block_count / _reference->_segmentSize) + ((block_count % _reference->_segmentSize) ? 1 : 0);
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
	}
	// Return true to indicate success.
	return true;
}

bool FileMappedStorage::cacheSegment(FileMappedStorage::size_type idx)
{
	// TODO: Could improve the cashed segment when recycling.
	// Check if the data is currently cached locked by this instance.
	if (_cachedSegmentIndex != idx)
	{
		// Check if a previous locked segment must be unlocked.
		if (_cachedSegmentIndex != std::numeric_limits<size_type>::max())
		{ // Unlock the segment,
			_reference->_segmentList[_cachedSegmentIndex]->doUnlockMemory();
			// Set the cached segment index to none.
			_cachedSegmentIndex = npos;
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

void FileMappedStorage::uncacheSegment(FileMappedStorage::size_type idx)
{
	(void)idx;
	// Do nothing yet.
}

bool FileMappedStorage::blockReadWrite(bool rd, FileMappedStorage::size_type ofs, FileMappedStorage::size_type sz,
	void* data)
{
	Reference::MtLock lock(_reference->_mutex);
	// Check if the size addressed data is in range
	if (ofs < 0L || sz < 0L)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, (rd ? "Read(" : "Write(") << ofs << ',' << sz << ") invalid parameters!")
		return false;
	}
	if ((ofs + sz) > getBlockCount())
	{
		SF_RTTI_NOTIFY
		(
			DO_DEFAULT,
			(rd ? "Read(" : "Write(") << ofs << ',' << sz << ") out of scope!"
		)
		SF_RTTI_NOTIFY(DO_DEFAULT, "getBlockCount() = " << getBlockCount())
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
			if (seg_i >= (int) _reference->_segmentList.count())
			{
				throw std::out_of_range(SF_RTTI_TYPENAME + "::" + __FUNCTION__ + "() block iterator out of scope!");
			}
			// Abort loop when a read/write fails.
			if
				(rd
				? !_reference->_segmentList[seg_i]->read(offset * _reference->_blockSize, blocks * _reference->_blockSize, rwp)
				: !_reference->_segmentList[seg_i]->write(offset * _reference->_blockSize, blocks * _reference->_blockSize, rwp)
				)
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, "Block" << (rd ? "Read" : "Write") << " Failed!")
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

FileMappedStorage::size_type FileMappedStorage::getBlockCount() const
{
	Reference::MtLock lock(_reference->_mutex);
	return _reference->_segmentSize * _reference->_segmentList.count();
}

FileMappedStorage::size_type FileMappedStorage::getSegmentCount() const
{
	Reference::MtLock lock(_reference->_mutex);
	return _reference->_segmentList.count();
}

FileMappedStorage::size_type FileMappedStorage::getBlockSize() const
{
	Reference::MtLock lock(_reference->_mutex);
	return _reference->_blockSize;
}

FileMappedStorage::size_type FileMappedStorage::getSegmentSize() const
{
	Reference::MtLock lock(_reference->_mutex);
	return _reference->_segmentSize;
}

FileMappedStorage::size_type FileMappedStorage::getSize() const
{
	Reference::MtLock lock(_reference->_mutex);
	auto seg_count = getSegmentCount();
	// When recycling is enabled the maximum of real segments is limited.
	if (_segmentRecycleCount && (size_type) seg_count > _segmentRecycleCount)
	{
		seg_count = _segmentRecycleCount;
	}
	return seg_count * _reference->_segmentSize * _reference->_blockSize;
}

FileMappedStorage::size_type FileMappedStorage::getSegmentLocks() const
{
	Reference::MtLock lock(_reference->_mutex);
	size_type rv = 0;
	size_type count = _reference->_segmentList.count();
	if (_segmentRecycleCount && _reference->_segmentList.count() >= _segmentRecycleCount)
	{
		count = _segmentRecycleCount;
	}
	for (size_type i = 0; i < count; i++)
	{
		rv += _reference->_segmentList[i]->_lockCount;
	}
	return rv;
}

std::ostream& FileMappedStorage::writeStatus(std::ostream& os) const
{
	return os
		<< "Total Allocated Bytes: " << this->getSize() << std::endl
		<< "Segment Size: " << this->getSegmentSize() << std::endl
		<< "Block Size: " << this->getBlockSize() << std::endl
		<< "Segment Count: " << this->getSegmentCount() << std::endl
		<< "Block Count: " << this->getBlockCount() << std::endl;
}

FileMappedStorage::Segment::Segment(FileMappedStorage::size_type sz)
	:_fileMapper(*IFileMapper::instantiate(true))
	, _size(sz)
{
	// Use the system page file.
	_fileMapper.initialize();
	// Check if the size of the segment is larger then zero before allocating
	if (_size > 0)
	{
		// Added extra size (largest integer) to file map to allow casting at the end
		// of memory possible without getting an exception.
		_fileMapper.createView(sz + sizeof(int64_t));
	}
}

FileMappedStorage::Segment::~Segment()
{
	// Clear all locks on this global handle.
	while (isLocked())
	{
		doUnlockMemory();
	}
	delete &_fileMapper;
}

bool FileMappedStorage::Segment::doLockMemory()
{
	// If the handle is already locked do not lock it again
	if (!_lockCount)
	{
		if (!_fileMapper.mapView())
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "mapView() on file mapper failed!")
			return false;
		}
		else
		{
			_dataPtr = (char*) _fileMapper.getPtr();
		}
	}
	// Increase the lock count for this instance
	_lockCount++;
	// return the pointer to the data
	return _dataPtr != nullptr;
}

void FileMappedStorage::Segment::doUnlockMemory()
{
	// Decrease lock count data member
	_lockCount--;
	// If the lock count is zero free the locked handle
	if (!_lockCount)
	{
		if (!_fileMapper.unmapView()) SF_RTTI_NOTIFY(DO_DEFAULT, "UnmapView On FileMapper Failed!")
		{
			// Null the data pointer.
			_dataPtr = nullptr;
		}
	}
	// Check if the lock count is lower then zero
	if (_lockCount < 0)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Unlocked Global Handle Too Many Times!")
		// Zero lock count for further use.
		_lockCount = 0;
	}
}

bool
FileMappedStorage::Segment::write(FileMappedStorage::size_type ofs, FileMappedStorage::size_type sz, const void* src)
{
	// Debug notify of writing zero bytes of memory.
	SF_COND_RTTI_NOTIFY(!sz, DO_DEFAULT, "Tried to write zero bytes of data!")
	if (!_dataPtr || !src)
	{
		// Return false to indicate failure.
		return false;
	}
	// Check for a write within the boundaries of this.
	if (ofs + sz > _size)
	{
		// bring size into allowed range.
		sz = clip<size_type>(sz, 0, _size - ofs);
		SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to write beyond end of memory!")
	}
	// All is well
	memcpy(_dataPtr + ofs, src, sz);
	// Return true to indicate success.
	return true;
}

bool
FileMappedStorage::Segment::read(FileMappedStorage::size_type ofs, FileMappedStorage::size_type sz, void* dst) const
{
	// Debug notify when trying to read zero bytes.
	SF_COND_RTTI_NOTIFY(!sz, DO_DEFAULT, "Tried To Read Zero Bytes of data!")
	// Debug notify when trying to read zero bytes.
	SF_COND_RTTI_NOTIFY(!dst, DO_DEFAULT, "Passed NULL pointer for storing data!")
	// Bail out here.
	if (!dst || !_dataPtr)
	{
		// Return false to indicate failure.
		return false;
	}
	// Check for a write within the boundaries of this instance.
	if (ofs + sz > _size)
	{ // bring size into allowed range
		sz = clip<size_type>(sz, 0, _size - ofs);
		SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to read beyond end of memory!")
	}
	// All is well
	memcpy(dst, _dataPtr + ofs, sz);
	// Return true to indicate success.
	return true;
}

namespace
{
int ref_counter = 0;
}

FileMappedStorage::Reference::Reference()
{
	ref_counter++;
}

FileMappedStorage::Reference::~Reference()
{
	ref_counter--;
}

bool FileMappedStorage::Lock::acquire(FileMappedStorage::size_type seg_idx)
{
	// Release the previous acquired segment first.
	release();
	// Lock the reference so we can do work safely.
	FileMappedStorage::Reference::MtLock(_store._reference->_mutex);
	// Check if the segment can be locked.
	if (seg_idx >= 0 && seg_idx < (size_type) _store._reference->_segmentList.count())
	{ // Get the segment pointer.
		_segment = _store._reference->_segmentList[seg_idx];
		// Lock the segment memory.
		_data = _segment->lockMemory();
		// Copy the index number for debugging purposes.
		_segmentIndex = seg_idx;
		(void)_segmentIndex;
		return true;
	}
	return false;
}

void* FileMappedStorage::Lock::data(FileMappedStorage::size_type blk_ofs)
{
	if (_data)
	{
		// Do a range check.
		if (blk_ofs >= 0 && blk_ofs * _blockSize < _segment->_size)
		{
			return &(static_cast<char*>(_data)[blk_ofs * _blockSize]);
		}
	}
	return nullptr;
}

void FileMappedStorage::Lock::release()
{
	if (_segment && _data)
	{
		_segment->unlockMemory();
	}
	_segment = nullptr;
	_data = nullptr;
	_segmentIndex = -1;
}

}
