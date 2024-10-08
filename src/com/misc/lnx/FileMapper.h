#pragma once

#include "../gen/IFileMapper.h"
#include "../global.h"
#include "File.h"
#include <cstddef>

namespace sf::lnx
{

/**
 * Wrapper for the linux mmap.
 */
class _MISC_CLASS FileMapper : public IFileMapper
{
	public:
		/**
		 * Default constructor.
		 */
		FileMapper();

		/**
		 * Initializing constructor.
		 */
		explicit FileMapper(const std::string& filepath, size_t ofs = 0, size_t sz = 0);

		/**
		 * Use temporary files to map.
		 */
		void initialize() override;

		/**
		 * Destructor.
		 */
		~FileMapper() override;

		/**
		 * Initializes the mapper passing a file instance.
		 */
		void initialize(const std::string& filepath, size_t ofs = 0, size_t sz = 0);

		/**
		 * Sets the size of the file to be mapped.
 		*/
		void createView(size_t size) override;

		/**
		 * Sets the part of the file to be mapped.
		 */
		void setView(size_t ofs, size_t sz);

		/**
		 * Unlocks the locked file and removes the file reference.
		 */
		void reset();

		/**
		 * Returns the pointer of the mapped area.
		 */
		void* getPtr() override;

		/**
		 * Locks the set area.
		 * Returns NULL on failure.
		 * @tparam T Type of the pointer
		 * @param readonly Flag making the resulting pointer readonly.
		 * @return Pointer of specified type.
		 */
		template<class T>
		T* lock(bool readonly = false)
		{
			return static_cast<T*>(doLock(readonly));
		}

		/**
		 * Unlocks the set area and syncs the data when sync is set.
		 */
		void unlock(bool sync = false);

		bool mapView() override;

		bool unmapView() override;

		/**
		 * Returns the underlying file.
		 * @return File instance reference.
		 */
		const File& getFile()
		{
			return _file;
		}

		/**
		 * Returns true when locked.
		 */
		[[nodiscard]] bool isLocked() const;

		/**
		 * Returns true on success to invalidated data so it is written to disk.
		 * When async is false the function returns after the data is written to
		 * disk. When true the function returns immediately.
		 */
		void sync(bool async);

		/**
		 * Returns the set size.
		 */
		[[nodiscard]] size_t getSize() const;

	private:
		/**
		 * Locks the set area.
		 * Returns NULL on failure.
		 */
		void* doLock(bool readonly);

		/**
		 * Holds the flag on whether a temporary file is used.
		 */
		bool _temp{false};
		/**
		 * Holds the file handle/descriptor.
		 */
		File _file{};
		/**
		 * Holds the pointer when locked.
		 */
		char* _ptr{nullptr};
		/**
		 * Holds the amount of times the instance is locked.
		 */
		int _counter{0};
		/**
		 * Size of the locked area.
		 */
		size_t _size{0};
		/**
		 * offset of the locked area.
		 */
		size_t _offset{0};
		/**
		 * Real page aligned offset into the file.
		 */
		size_t _realOffset{0};
};

}// namespace sf::lnx
