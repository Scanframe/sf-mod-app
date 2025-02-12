#pragma once
#include <misc/gen/IFileMapper.h>
#include <misc/global.h>
#include <string>

namespace sf::win
{

/**
 * @brief Class wrapper around the Windows 32-bit file mapping functions which allow
 * memory allocation using the page file for controlled swapping. This is
 * especially useful for managing huge memory blocks.
 */
class _MISC_CLASS FileMapper : public IFileMapper
{
	public:
		/**
		 * @brief Default constructor
		 */
		FileMapper();

		/**
		 * @brief Destructor
		 */
		~FileMapper() override;

		/**
		 * @brief Makes this instance use the passed file
		 * @param filename
		 * @return True on success.
		 */
		bool createMapFile(const char* filename);

		/**
		 * @brief Makes this instance create a map that is backed by the systems page file.
		 */
		void initialize() override;

		/**
		 * @brief Sets the part of the file to be mapped.
		 * Calls #createMap
		 */
		void createView(size_t sz) override;

		/**
		 * @brief Closes the opened file handle by 'createMapFile'.
		 * @return True on success.
		 */
		bool closeFile();

		/**
		 * @brief Returns true if a map file is open. (pagefile or not.)
		 * @return True when open.
		 */
		[[nodiscard]] bool isFileOpen() const;

		/**
		 * @brief Called by a data server to create a named file map.
		 * When 'unique' is true there is no other map named as 'map_name' before calling this function.
		 * @param map_name
		 * @param map_size
		 * @param unique
		 * @param readonly
		 * @return
		 */
		bool createMap(const char* map_name, size_t map_size, bool unique, bool readonly);

		/**
		 * @brief Called by a data client to open an existing a named file map.
		 * When 'readonly' is true this instance opens the map in readonly mode.
		 * Writing to the map has no effect.
		 * @param map_name
		 * @param map_size
		 * @param readonly
		 * @return
		 */
		bool openMap(const char* map_name, unsigned long map_size, bool readonly = true);

		/**
		 * @brief Closes the map handle created by calling CreateMap or OpenMap.
		 * @return
		 */
		bool closeMap();

		/**
		 * @brief Returns true if the file map is open.
		 * @return When open True.
		 */
		[[nodiscard]] bool isMapOpen() const;

		/**
		 * @brief Called by a server and client to gain access of the map.
		 * @return True on success.
		 */
		bool mapView() override;

		/**
		 * @brief Called by a server and client to unaccess the map.
		 * @return True on success.
		 */
		bool unmapView() override;

		/**
		 * @brief Writes to the disk a byte range within a mapped view of the file.
		 * @return True on success.
		 */
		bool flushView();

		/**
		 * @brief Gets the file name when 'CreateMapFile' was called.
		 */
		[[nodiscard]] std::string getFileName() const;

		/**
		 * @brief Gets the map name when 'CreateMap' was called.
		 */
		[[nodiscard]] std::string getMapName() const;

		/**
		 * @brief Gets the size of the map.
		 */
		[[nodiscard]] size_t getMapSize() const;

		/**
		 * @brief Gets a pointer to the data if MapView is called.
		 * @return Pointer
		 */
		[[nodiscard]] void* getPtr() override;

		/**
		 * @brief Returns a pointer to the data if MapView is called.
		 * @return Const Pointer
		 */
		[[nodiscard]] const void* getPtr() const;

		/**
		 * @brief Deleted copy constructor to prevent copying.
		 */
		FileMapper(const FileMapper&) = delete;

		/**
		 * @brief Deleted assignment operator to prevent copying.
		 */
		FileMapper& operator=(const FileMapper&) = delete;

	private:
		/**
		 * Holds the implementation dependent data.
		 */
		struct FileMapperPrivate* _data{nullptr};
};

}// namespace sf::win