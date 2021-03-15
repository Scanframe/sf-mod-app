#pragma once

#include <string>
#include "../gen/IFileMapper.h"
#include "../global.h"

namespace sf::win
{

/**
 * @brief Class wrapper around the Windows 32-bit file mapping functions which allow
 * memory allocation using the page file for controlled swapping. This is
 * especially useful for managing huge memory blocks.
 */
class _MISC_CLASS FileMapper :public IFileMapper
{
	public:
		/**
		 * Default constructor
		 */
		FileMapper();

		/**
		 * Destructor
		 */
		~FileMapper() override;

		/**
		 * Makes this instance use the the passed file
		 * @param filename
		 * @return True on success.
		 */
		bool createMapFile(const char* filename);

		/**
		 * Makes this instance create a map that is backed by the systems page file.
		 * @return
		 */
		void initialize() override;

		/**
		 * Sets the part of the file to be mapped.
		 * Calls #createMap
		 */
		void createView(size_t sz) override;

		/**
		 * Closes the opened file handle by 'createMapFile'.
		 * @return True on success.
		 */
		bool closeFile();

		/**
		 * Returns true if a map file is open. (pagefile or not.)
		 * @return True when open.
		 */
		[[nodiscard]] bool isFileOpen() const;


		/**
		 * Called by a data server to create a named file map.
		 * When 'unique' is true there is no other map named as 'map_name' before calling this function.
		 * @param map_name
		 * @param map_size
		 * @param unique
		 * @param readonly
		 * @return
		 */
		bool createMap(const char* map_name, size_t map_size, bool unique , bool readonly);

		/**
		 * Called by a data client to open an existing a named file map.
		 * When 'readonly' is true this instance opens the map in readonly mode.
		 * Writing to the map has no effect.
		 * @param map_name
		 * @param map_size
		 * @param readonly
		 * @return
		 */
		bool openMap(const char* map_name, unsigned long map_size, bool readonly = true);

		/**
		 * Closes the map handle created by calling CreateMap or OpenMap.
		 * @return
		 */
		bool closeMap();

		/**
		 * Returns true if the file map is open.
		 * @return When open True.
		 */
		[[nodiscard]] bool isMapOpen() const;

		/**
		 * Called by a server and client to gain access of the map.
		 * @return True on success.
		 */
		bool mapView() override;

		/**
		 * Called by a server and client to unaccess the map.
		 * @return True on success.
		 */
		bool unmapView() override;

		/**
		 * Writes to the disk a byte range within a mapped view of the file.
		 * @return True on success.
		 */
		bool flushView();

		/**
		 * Returns the file name when 'CreateMapFile' was called.
		 * @return
		 */
		[[nodiscard]] std::string getFileName() const;

		/**
		 * Returns the map name when 'CreateMap' was called.
		 * @return
		 */
		[[nodiscard]] std::string getMapName() const;

		/**
		 * Returns the size of the map.
		 * @return
		 */
		[[nodiscard]] size_t getMapSize() const;

		/**
		 * Returns a pointer to the data if MapView is called.
		 * @return Pointer
		 */
		[[nodiscard]] void* getPtr() override;

		/**
		 * Returns a pointer to the data if MapView is called.
		 * @return Const Pointer
		 */
		[[nodiscard]] const void* getPtr() const;

		/**
		 * Deleted copy constructor to prevent copying.
		 */
		FileMapper(const FileMapper&) = delete;

		/**
		 * Deleted assignment operator to prevent copying.
		 * @return
		 */
		FileMapper& operator=(const FileMapper&) = delete;

	private:

		/**
		 * Holds the implementation dependent data.
		 */
		struct FileMapperPrivate* _data{nullptr};
};

}