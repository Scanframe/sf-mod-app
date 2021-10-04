#pragma once

#include <iostream>
#include <misc/gen/TVector.h>
#include <misc/gen/Thread.h>
#include <misc/gen/IFileMapper.h>
#include "../global.h"

namespace sf
{

/**
 * @brief This class provides storing of huge amounts of data using file mapping.<br>
 * Mapping only what is needed into memory.
 *
 * Typically used for data acquisition using digitisers with a vast stream of data in multiple channels.<br>
 * In some cases the stored data must be kept together in blocks.<br>
 * A block is a fixed length array of a certain type (mostly integers).<br>
 * Blocks need to be atomic to for processing later.<br>
 * All functions are block size oriented and multi-threaded safe and allowing data be processed in other threads.
 * @image html "com/gii/doc/FileMappedStorage-1.svg"
 *
 * The embedded #sf::FileMappedStorage::Lock class provides a segment locking mechanism.
 *
 * The example code below shows it simple usage.
 * @code
 * // Where `_store` is `FileMappedStorage` data member.
 * bool MyProcess::process(size_type segment)
 * {
 * 	// Create and try locking.
 * 	Lock lock(_store, segment);
 * 	// Boolean operator calling isAcquired() under the hood.
 * 	if (lock)
 * 	{
 * 		// Get a typed pointer.
 * 		auto p = lock.ptr<uint32_t>();
 * 		//
 * 		// Do the processing here.
 * 		//
 * 		// Signal processing success.
 * 		return true;
 * 	}
 * 	// Signal segment was not acquired to be processed. Try again later.
 * 	return false;
 * }
 * @endcode
 *
 * File mapping is done using a #sf::IFileMapper derived class has implementations per OS and also a Qt version.
 * @see sf::FileMappedStorage::Lock
 * @see sf::IFileMapper
 * @see sf::lnx::FileMapper
 * @see sf::win::FileMapper
 * @see sf::qt::FileMapper
 */
class _GII_CLASS FileMappedStorage
{
	public:
		/**
		 * @brief Type used for sizes of storage.
		 */
		typedef uint64_t size_type;

		/**
		 * @brief Unsigned size value indicating not found or no index.
		 */
		static constexpr size_t npos = std::numeric_limits<size_type>::max();

		/**
		 * Initializing constructor.
		 * @param seg_sz Sets the segment size of storage in blocks.
		 * @param blk_sz Is in bytes.
		 * @param recycle Is the maximum amount of segments allowed to be used for storing.
		 */
		FileMappedStorage(size_type seg_sz, size_type blk_sz, size_type recycle = 0);

		/**
		 * @brief Copy constructor.
		 */
		FileMappedStorage(const FileMappedStorage&);

		/**
		 * @brief Virtual destructor.
		 */
		virtual ~FileMappedStorage();

		/**
		 * @brief Reserves an amount of blocks and rounds it up to the nearest segment size.
		 *
		 * Shrinking is not possible with this method.
 		 * @param block_count Amount of blocks to reserve.
		 * @return True if it succeeded to fulfill the request.
		 */
		bool reserve(size_type block_count);

		/**
		 * @brief Flushes all data stored by this instance.
		 *
		 * After this all segments are no longer and #reserve() must be called again.
		 */
		void flush();

		/**
		 * @brief Function that store blocks in several segments when needed.
		 * @param ofs Offset in blocks
		 * @param sz Size in blocks
		 * @param src Pointer to source of data.
		 * @return True on success.
		 */
		inline bool blockWrite(size_type ofs, size_type sz, const void* src);

		/**
		 * @brief Function that store blocks in several segments when needed.
		 * @param ofs Offset in blocks
		 * @param sz Size in blocks
		 * @param dst Pointer to destination of data.
		 * @return True on success.
		 */
		inline bool blockRead(size_type ofs, size_type sz, void* dst) const;

		/**
		 * @brief Gets the amount of blocks reserved.
		 *
		 * @return Amount blocks.
		 * @see #reserve
		 */
		[[nodiscard]] size_type getBlockCount() const;

		/**
		 * @brief Gets the amount of segments used by this instance.
		 */
		[[nodiscard]] size_type getSegmentCount() const;

		/**
		 * @brief Gets the accumulation of locks on segments.
		 */
		[[nodiscard]] size_type getSegmentLocks() const;

		/**
		 * @brief Gets the block size in bytes of this instance.
		 */
		[[nodiscard]] size_type getBlockSize() const;

		/**
		 * @brief Gets the size of the segments in blocks of this instance.
		 */
		[[nodiscard]] size_type getSegmentSize() const;

		/**
		 * Gets the total amount of bytes handled by this instance.
		 * Taking recycling into account.
		 * @return Bytes
		 */
		[[nodiscard]] size_type getSize() const;

		/**
		 * @brief Gets the amount of segments being recycled.
		 */
		[[nodiscard]] inline FileMappedStorage::size_type getRecycleCount() const;

		/**
		 * @brief Sets the recycle segment count.
		 *
		 * @param count Amount of segments to recycle.
		 * @return True on success.
		 */
		bool setRecycleCount(size_type count);

		/**
		 * @brief Writes the status to the output stream.
		 */
		std::ostream& writeStatus(std::ostream& os) const;

	private:
		/**
		 * @brief Method used in blockRead and Block read
		 * @param rd True makes it read and false write.
		 * @param ofs Offset in blocks
		 * @param sz Size in blocks
		 * @param src Source data.
		 * @return True on success.
		 */
		bool blockReadWrite(bool rd, size_type ofs, size_type sz, void* src);

		/**
		 * This class is used to hook data
		 */
		class Segment
		{
			public:
				/**
				 * @brief Constructor passing the size.
				 * @param sz Size in bytes.
				 */
				explicit Segment(size_type sz);

				/**
				 * @brief Destructor.
				 */
				~Segment();

				/**
				 * @brief Gets the locked states.
				 *
				 * @return True if the when locked.
				 */
				[[nodiscard]] inline bool isLocked() const;

				/**
				 * @brief Gets the size this instance.
				 *
				 * @return Size in bytes.
				 */
				[[nodiscard]] inline size_type getSize() const;

				/**
				 * @brief Locks the global handle and returns the pointer to it.
				 *
				 * @return NULL on failure.
				 */
				[[nodiscard]] inline void* lockMemory();

				/**
				 * @brief Locks the file map and returns the pointer to the memory.
				 * Read only (const version)
				 * @return NULL on failure.
				 */
				[[nodiscard]] inline const void* lockMemory() const;

				/**
				 * @brief Unlocks the pointer to memory.
				 */
				inline void unlockMemory() const;

				/**
				 * @brief Writes to the file mapped memory.
				 * @param ofs Offset in bytes.
				 * @param sz Size in bytes to write.
				 * @param src Pointer to source location.
				 * @return True on success.
				 */
				bool write(size_type ofs, size_type sz, const void* src);

				/**
				 * Function that reads the memory.
				 * @param ofs Offset
				 * @param sz Size in bytes.
				 * @param dst Pointer to destination location.
				 * @return True on success.
				 */
				bool read(size_type ofs, size_type sz, void* dst) const;

				/**
				 * @brief Prevents copying of this class.
				 */
				Segment(Segment&) = delete;

				/**
				 * Prevents copying of this class.
				 * @return
				 */
				Segment& operator=(Segment&) = delete;

			protected:
				/**
				 * @brief Method to lock the memory in const functions.
				 * @return True on success.
				 */
				bool doLockMemory();

				/**
				 * @brief Method to be used in const method to unlock memory.
				 */
				void doUnlockMemory();

			private:
				/**
				 * Holds the count of locks on the MyGlobalHandle.
				 */
				int _lockCount{0};
				/**
				 * Holds the size of this global handle.
				 */
				size_type _size{0};
				/**
				 * Pointer to the data when it is locked.
				 */
				char* _dataPtr{nullptr};
				/**
				 * Underlying class for storing data on the page/swap file to be accessed by other processes too.
				 */
				IFileMapper& _fileMapper;

				friend class FileMappedStorage;
		};

		/**
		 * Referenced structure.
		 */
		struct Reference
		{
			/**
			 * @brief Constructor.
			 */
			Reference();

			/**
			 * @brief Destructor.
			 */
			~Reference();

			/**
			 * @brief Thread id of the thread who created the data store reference.
			 */
			Thread::id_type _threadId{0};
			/**
			 * @brief Contains the block size of this instance.
			 */
			size_type _blockSize{0};
			/**
			 * @brief Contains the segment size of this instance.
			 */
			size_type _segmentSize{0};
			/**
			 * @brief Holds the amount of times this instance is referenced.
			 */
			int _referenceCount{0};
			/**
			 * @brief Contains a vector of pointers to all segments of this instance.
			 */
			TVector<Segment*> _segmentList;
			/**
			 * @brief Mutex for MT safety.
			 */
			Mutex _mutex;
			/**
			 * @brief Lock type for multi threading safety.
			 */
			typedef Mutex::Lock MtLock;

		};

		/**
		 * @brief Reference structure for the storage class.
		 */
		Reference* _reference{nullptr};

	public:
		/**
		 * @brief Provides easy way to lock a pointer to the memory.
		 */
		class _GII_CLASS Lock
		{
			public:
				/**
				 * @brief Constructor.
				 *
				 * @param ds Data store instance.
				 */
				explicit inline Lock(FileMappedStorage& ds);

				/**
				 * @brief Constructor.
				 *
				 * @param ds Data store instance.
				 * @param seg_idx Designated segment.
				 */
				inline Lock(FileMappedStorage& ds, size_type seg_idx);

				/**
				 * @brief Destructor.
				 */
				~Lock();

				/**
				 * @brief Tries to lock the segment index of the data store and returns true when successful.
				 *
				 * @param seg_idx Segment index.
				 * @return Return true when acquired.
				 */
				bool acquire(size_type seg_idx);

				/**
				 * Gets the segment acquired status.
				 * @return True when acquired.
				 */
				[[nodiscard]] bool isAcquired() const;

				/**
				 * Gets the segment acquired status.
				 * @return True when acquired.
				 */
				explicit operator bool() const;

				/**
				 * @brief Returns  pointer to .
				 *
				 * @param blk_ofs Offset in blocks.
				 * @return Data pointer
				 */
				void* data(size_type blk_ofs);

				/**
				 * Typed template method.
				 * @tparam T Type of the return value.
				 * @param blk_ofs Offset in to the data.
				 * @return Typed pointer.
				 */
				template<typename T = void*>
				T* ptr(size_type blk_ofs)
				{
					return static_cast<T*>(data(blk_ofs));
				}

				/**
				 * @brief Unlocks memory locked by the segment.
				 */
				void release();

			private:
				/**
				 * @brief Holds the pointer to the data store.
				 */
				FileMappedStorage& _store;
				/**
				 * @brief Index into the segment array.
				 */
				size_type _segmentIndex{0};
				/**
				 * @brief Local fast version of the data stores block size.
				 */
				size_type _blockSize{0};
				/**
				 * @brief Pointer to locked segment.
				 */
				Segment* _segment{nullptr};
				/**
				 * @brief Pointer to lock data from segment.
				 */
				void* _data{nullptr};
		};

	private:

		/**
		 * Optimization functions for limiting the amount of locks and unlocks.
		 */
		bool cacheSegment(size_type idx);

		/**
		 * Optimization functions for limiting the amount of locks and unlocks.
		 */
		void uncacheSegment(size_type idx);

		/**
		 * Holds the segment which is locked by this class instance.
		 * Where max() means no segment is locked.
		 */
		size_type _cachedSegmentIndex{npos};
		/**
		 * Maximum amount of segments to be used when storage is recycled.
		 */
		size_type _segmentRecycleCount{0};
		/**
		 * Should prevent changing a refs reference counter.
		 */
		static Mutex _staticSync;
};

inline
FileMappedStorage::size_type FileMappedStorage::Segment::getSize() const
{
	return _size;
}

inline
bool FileMappedStorage::Segment::isLocked() const
{
	return (_lockCount > 0);
}

inline
void* FileMappedStorage::Segment::lockMemory()
{
	doLockMemory();
	return _dataPtr;
}

inline
const void* FileMappedStorage::Segment::lockMemory() const
{
	const_cast<FileMappedStorage::Segment*>(this)->doLockMemory();
	return const_cast<FileMappedStorage::Segment*>(this)->_dataPtr;
}

inline
void FileMappedStorage::Segment::unlockMemory() const
{
	const_cast<FileMappedStorage::Segment*>(this)->doUnlockMemory();
}

inline
bool FileMappedStorage::blockWrite(FileMappedStorage::size_type ofs, FileMappedStorage::size_type sz, const void* src)
{
	return blockReadWrite(false, ofs, sz, (void*) src);
}

inline
bool FileMappedStorage::blockRead(FileMappedStorage::size_type ofs, FileMappedStorage::size_type sz, void* dst) const
{
	return const_cast<FileMappedStorage*>(this)->blockReadWrite(true, ofs, sz, dst);
}

inline
FileMappedStorage::size_type FileMappedStorage::getRecycleCount() const
{
	return _segmentRecycleCount;
}

inline
FileMappedStorage::Lock::Lock(FileMappedStorage& ds)
	:_store(ds), _blockSize(ds.getBlockSize()) {}

inline
FileMappedStorage::Lock::Lock(FileMappedStorage& ds, FileMappedStorage::size_type seg_idx)
	:_store(ds), _segmentIndex(-1), _blockSize(ds.getBlockSize())
{
	acquire(seg_idx);
}

inline
FileMappedStorage::Lock::~Lock()
{
	release();
}

inline
bool FileMappedStorage::Lock::isAcquired() const
{
	return _data != nullptr;
}

inline
FileMappedStorage::Lock::operator bool() const
{
	return isAcquired();
}

}
