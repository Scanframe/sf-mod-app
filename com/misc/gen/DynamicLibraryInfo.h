#pragma once

#include "../global.h"
#include <cstring>
#include <string>

/**
 * Separator between name and description.
 */
#define SF_DL_NAME_SEPARATOR "\n\n"

/**
 * Defines marker string used to find the start of a naming and description block in a compiled binary
 */
#define SF_DL_MARKER_BEGIN "\n#@$!*^\n"
/**
 * End marker
 */
#define SF_DL_MARKER_END "\n^*!$@#\n"

namespace sf
{

/**
 * @brief Information on a dynamic library file using the _DL_INFORMATION macro.
 */
struct _MISC_CLASS DynamicLibraryInfo
{
	/**
	 * @brief Default constructor.
	 */
	DynamicLibraryInfo() = default;

	/**
	 * @brief Copy constructor.
	 */
	DynamicLibraryInfo(const DynamicLibraryInfo&);

	/**
	 * @brief Directory of the dynamic library.
	 */
	std::string directory{};
	/**
	 * @brief Filename of the dynamic library.
	 */
	std::string filename{};
	/**
	 * @brief Name of the dynamic library.
	 */
	std::string name{};
	/**
	 * @brief Description of the dynamic library.
	 */
	std::string description{};

	/**
	 * @brief PAth to the library. Combines directory and filename.
	 */
	[[nodiscard]] std::string path() const;

	/**
	 * @brief Reads the information from the file.
	 *
	 * @param dir Directory of the
	 * @param filename Location of the dynamic library file relative to the directory.
	 * @return  True on success.
	 */
	bool read(const std::string& dir, const std::string& filename);

	/**
	 * @brief Clears all the members.
	 */
	void clear();
};

}
