#pragma once

#include <iostream>
#include <misc/gen/TVector.h>
#include <misc/gen/Thread.h>
#include <misc/gen/IFileMapper.h>
#include "../global.h"

namespace sf
{

/**
 * Storage
 */
class _GII_CLASS ResultDataStorage
{
	public:
		/**
		 * Type used for sizes.
		 */
		typedef int64_t size_type;

		/**
		 * Initializing constructor.
		 * @param seg_sz Sets the segment size of storage in blocks.
		 * @param blk_sz Is in bytes.
		 * @param recycle Is the maximum amount of segments allowed to be used for storing. Additional segments.
		 */
		ResultDataStorage(size_type seg_sz, size_type blk_sz, unsigned recycle = 0);

		/**
		 * Copy constructor.
		 */
		ResultDataStorage(const ResultDataStorage&);

		/**
		 * Virtual Destructor constructor.
		 */
		virtual ~ResultDataStorage();

		/**
		 * Returns true if it succeeded to increment the storage space.
		 * If the block count is zero this function wil set the owner flag as well.
 		 * @param block_count
		 * @return
		 */
		bool reserve(size_type block_count);

		/**
		 * Flushes all data stored by this instance.
		 */
		void flush();

		/**
		 * Function that store blocks in several segments when needed.
		 * @param ofs
		 * @param sz
		 * @param src
		 * @return
		 */
		bool blockWrite(size_type ofs, size_type sz, const void* src);

		/**
		 * Function that store blocks in several segments when needed.
		 * @param ofs
		 * @param sz
		 * @param dest
		 * @return
		 */
		bool blockRead(size_type ofs, size_type sz, void* dest) const;

		/**
		 * Returns the Block Count of this instance.
		 * @return
		 */
		[[nodiscard]] size_type getBlockCount() const;

		/**
		 * Returns the amount of segments of this instance.
		 * @return
		 */
		[[nodiscard]] size_type getSegmentCount() const;

		/**
		 * Returns the accumulation of locks on segments.
		 * @return
		 */
		[[nodiscard]] size_type getSegmentLocks() const;

		/**
		 * Returns the Block size in bytes of this instance.
		 * @return
		 */
		[[nodiscard]] size_type getBlockSize() const;

		/**
		 * Returns the size of the segments in blocks of this instance.
		 * @return
		 */
		[[nodiscard]] size_type getSegmentSize() const;

		/**
		 * Returns the total amount of bytes handled by this instance.
		 * Taking recycling into account.
		 * @return
		 */
		[[nodiscard]] size_type getSize() const;

		/**
		 * Returns the amount of segments being recycled.
		 * @return
		 */
		[[nodiscard]] unsigned getRecycleCount() const;

		/**
		 * Sets the recycle segment count.
		 * @param count
		 * @return True on success.
		 */
		bool setRecycleCount(unsigned count);

		/**
		 * Returns true if the global handle is owned by this instance.
		 * @return
		 */
		[[nodiscard]] bool isOwner() const;

		/**
		 * Sets the owner ship of the data.
		 * This is only possible when no segments are allocated yet.
		 * By default the instance is the data owner.
		 * @param yn
		 * @return Status of the owner.
		 */
		bool setOwner(bool yn);

		/**
		 * Returns true if the instance was constructed well.
		 * @return
		 */
		[[nodiscard]] bool isValid() const;

	private:
		/**
		 * Function used in blockRead and Block Write
		 * @param rd
		 * @param ofs
		 * @param sz
		 * @param src
		 * @return
		 */
		bool blockReadWrite(bool rd, size_type ofs, size_type sz, void* src);

		/**
		 * This class is used to hook data
		 */
		class Segment
		{
			public:
				/**
				 * Constructor that owns this data
				 * @param sz
				 */
				explicit Segment(size_type sz);

				/**
				 * Destructor
				 */
				~Segment();

				/**
				 * Returns true if the global handle is locked.
				 * @return
				 */
				[[nodiscard]] bool isLocked() const;

				/**
				 * Returns true if the global handle is owned by this instance.
				 * @return
				 */
				[[nodiscard]] bool isOwner() const;

				/**
				 * Returns the size this instance.
				 * @return
				 */
				[[nodiscard]] size_type getSize() const;

				/**
				 * Locks the global handle and returns the pointer to it.
				 * @return NULL on failure.
				 */
				void* lockMemory();

				/**
				 * Locks the global handle and returns the pointer to it.
				 * Read only (const version)
				 * @return NULL on failure.
				 */
				[[nodiscard]] const void* lockMemory() const;

				/**
				 * Unlocks the memory.
				 */
				void unlockMemory() const;

				/**
				 * Function that writes to the memory.
				 * @param ofs
				 * @param sz
				 * @param src
				 * @return
				 */
				bool write(size_type ofs, size_type sz, const void* src);

				/**
				 * Function that reads the memory.
				 * @param ofs
				 * @param sz
				 * @param dest
				 * @return
				 */
				bool read(size_type ofs, size_type sz, void* dest) const;

				/**
				 * Prevents copying of this class.
				 */
				Segment(Segment&) = delete;

				/**
				 * Prevents copying of this class.
				 * @return
				 */
				Segment& operator=(Segment&) = delete;

			protected:
				/**
				 * Private function to lock the memory in const functions.
				 * @return
				 */
				bool doLockMemory();

				/**
				 * Private function to be used in const functions to unlock memory.
				 */
				void doUnlockMemory();

			private:
				/**
				 * Holds the count of locks on the MyGlobalHandle.
				 */
				int LockCount{0};
				/**
				 * Holds the size of this global handle.
				 */
				size_type Size{0};
				/**
				 * Pointer to the data when it is locked.
				 */
				char* DataPtr{nullptr};
				/**
				 * Tells the destructor if this global handle is owned by the instance.
				 * By default this flag is true meaning the data is owned.
				 */
				bool FlagOwner{false};
				/**
				 * Underlying class for storing data on the page/swap file to be accessed by other processes too.
				 */
				IFileMapper& _fileMapper;

				friend class ResultDataStorage;

				friend class TDataStoreLock;

				friend void ResultDataStorage_Test();
		};

		/**
		 * Referenced structure.
		 */
		struct Reference
		{
			/**
			 * Constructor.
			 */
			Reference();

			/**
			 * Destructor.
			 */
			~Reference();

			/**
			 * Tread {DWORD} ID of the thread who created the data store reference.
			 * by other processes too.
			 */
			Thread::handle_type _threadId{0};
			/**
			 * Contains the block size of this instance.
			 */
			size_type _blockSize{0};
			/**
			 * Contains the segment size of this instance.
			 */
			size_type _segmentSize{0};
			/**
			 * Flag that indicates that this instance owns its stored data.
			 */
			bool _flagOwner{false};
			/**
			 * Flag that tells if the instance was constructed well.
			 */
			bool _flagValid{false};
			/**
			 * Holds the amount of times this instance is referenced.
			 */
			int _referenceCount{0};
			/**
			 * Contains a vector of pointers to all segments of this instance.
			 */
			TVector<Segment*> _segmentList;
			/**
			 * Mutex for MT safety.
			 */
			CriticalSection _criticalSection;
			/**
			 * Lock type for multi threading safety.
			 */
			typedef CriticalSection::Lock MtLock;

		} * _reference;

	public:
		/**
		 * Provides easy way to lock a pointer to the memory.
		 */
		class Lock
		{
			public:
				/**
				 * Constructor.
				 * @param ds Data store instance.
				 */
				explicit Lock(ResultDataStorage& ds);

				/**
				 * Constructor.
				 * @param ds Data store instance.
				 * @param seg_idx Designated segment.
				 */
				Lock(ResultDataStorage& ds, size_type seg_idx);

				/**
				 * Destructor.
				 */
				~Lock();

				/**
				 * Tries to lock the segment index of the data store and returns true when successful.
				 * @param seg_idx
				 * @return
				 */
				bool acquire(size_type seg_idx);

				/**
				 * Returns true when the segment could be acquired.
				 * @return
				 */
				bool isAcquired();

				/**
				 * Overloaded function operator to access the segments locked data pointer passing the offset in blocks.
				 * @param blk_ofs
				 * @return
				 */
				void* operator()(size_type blk_ofs);

				/**
				 * Unlocks memory locked by the segment.
				 */
				void release();

			private:
				/**
				 * Holds the pointer to the data store.
				 */
				ResultDataStorage& _store;
				/**
				 * Index in to the segment array.
				 */
				size_type _index{0};
				/**
				 * Local fast version of the data stores block size.
				 */
				size_type _blockSize{0};
				/**
				 * Pointer to locked segment.
				 */
				Segment* _segment{nullptr};
				/**
				 * Pointer to lock data from segment.
				 */
				void* _data{nullptr};
		};

	private:

		/**
		 * Optimization functions for limiting the amount of locks and unlocks.
		 * @param idx
		 * @return
		 */
		bool cacheSegment(int idx);

		/**
		 * @param idx
		 */
		void uncacheSegment(int idx);

		/**
		 * Holds the segment which is locked by this class instance.
		 * Where -1 means no segment is locked.
		 */
		size_type _cachedSegmentIndex{-1};
		/**
		 * Maximum amount of segments to be used when storage is recycled.
		 */
		unsigned _segmentRecycleCount{0};
		/**
		 * Should prevent changing a refs reference counter.
		 */
		static CriticalSection _staticSync;

		friend void ResultDataStorage_Test();
};

inline
ResultDataStorage::size_type ResultDataStorage::Segment::getSize() const
{
	return Size;
}

inline
bool ResultDataStorage::Segment::isLocked() const
{
	return (LockCount > 0);
}

inline
bool ResultDataStorage::Segment::isOwner() const
{
	return FlagOwner;
}

inline
void* ResultDataStorage::Segment::lockMemory()
{
	doLockMemory();
	return DataPtr;
}

inline
const void* ResultDataStorage::Segment::lockMemory() const
{
	const_cast<ResultDataStorage::Segment*>(this)->doLockMemory();
	return const_cast<ResultDataStorage::Segment*>(this)->DataPtr;
}

inline
void ResultDataStorage::Segment::unlockMemory() const
{
	const_cast<ResultDataStorage::Segment*>(this)->doUnlockMemory();
}

inline
ResultDataStorage::size_type ResultDataStorage::getBlockCount() const
{
	Reference::MtLock lock(_reference->_criticalSection);
	return _reference->_segmentSize * _reference->_segmentList.count();
}

inline
ResultDataStorage::size_type ResultDataStorage::getSegmentCount() const
{
	Reference::MtLock lock(_reference->_criticalSection);
	return _reference->_segmentList.count();
}

inline
ResultDataStorage::size_type ResultDataStorage::getBlockSize() const
{
	Reference::MtLock lock(_reference->_criticalSection);
	return _reference->_blockSize;
}

inline
ResultDataStorage::size_type ResultDataStorage::getSegmentSize() const
{
	Reference::MtLock lock(_reference->_criticalSection);
	return _reference->_segmentSize;
}

inline
ResultDataStorage::size_type ResultDataStorage::getSize() const
{
	Reference::MtLock lock(_reference->_criticalSection);
	ResultDataStorage::size_type seg_count = getSegmentCount();
	// When recycling is enabled the maximum of real segments is limited.
	if (_segmentRecycleCount && (unsigned) seg_count > _segmentRecycleCount)
	{
		seg_count = _segmentRecycleCount;
	}
	return seg_count * _reference->_segmentSize * _reference->_blockSize;
}

inline
bool ResultDataStorage::isOwner() const
{
	Reference::MtLock lock(_reference->_criticalSection);
	return _reference->_flagOwner;
}

inline
bool ResultDataStorage::setOwner(bool yn)
{
	Reference::MtLock lock(_reference->_criticalSection);
	if (!_reference->_segmentList.count())
	{
		_reference->_flagOwner = yn;
	}
	return _reference->_flagOwner;
}

inline
bool ResultDataStorage::isValid() const
{
	Reference::MtLock lock(_reference->_criticalSection);
	return _reference->_flagValid;
}

inline
bool ResultDataStorage::blockWrite(ResultDataStorage::size_type ofs, ResultDataStorage::size_type sz, const void* src)
{
	// Lock of the reference is done in this function.
	return blockReadWrite(false, ofs, sz, (void*) src);
}

inline
unsigned ResultDataStorage::getRecycleCount() const
{
	return _segmentRecycleCount;
}

inline
ResultDataStorage::Lock::Lock(ResultDataStorage& ds)
	:_store(ds), _blockSize(ds.getBlockSize()) {}

inline
ResultDataStorage::Lock::Lock(ResultDataStorage& ds, ResultDataStorage::size_type seg_idx)
	:_store(ds), _index(-1), _blockSize(ds.getBlockSize())
{
	acquire(seg_idx);
}

inline
ResultDataStorage::Lock::~Lock()
{
	release();
}

inline
bool ResultDataStorage::Lock::isAcquired()
{
	return _data != nullptr;
}

inline
void* ResultDataStorage::Lock::operator()(ResultDataStorage::size_type blk_ofs)
{
	if (_data)
	{
		// Do a range check.
		if (blk_ofs >= 0 && blk_ofs * _blockSize < _segment->Size)
		{
			return &((char*) _data)[blk_ofs * _blockSize];
		}
	}
	return nullptr;
}

inline
void ResultDataStorage::Lock::release()
{
	// When locked the segment data is unlocked and all members.
	if (_segment && _data)
	{
		_segment->unlockMemory();
	}
	_segment = nullptr;
	_data = nullptr;
	_index = -1;
}

/**
 * TODO: Move to unit test.
 * Test function.
 */
void ResultDataStorage_Test();

}
