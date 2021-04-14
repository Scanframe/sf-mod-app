#pragma once

#include <cstring>

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
	 * Default constructor.
	 */
	DynamicLibraryInfo() = default;

	/**
	 * Copy constructor.
	 */
	DynamicLibraryInfo(const DynamicLibraryInfo&);

	/**
	 * Path of the dynamic library.
	 */
	std::string Path;
	/**
	 * Name of the dynamic library.
	 */
	std::string Name;
	/**
	 * Description of the dynamic library.
	 */
	std::string Description;

	/**
	 * Reads the information from the file.
	 * @param filepath Location of the dynamic library file.
	 * @return  True on success.
	 */
	bool read(const std::string& filepath);

	/**
	 * Clears the members.
	 */
	void reset();
};

}
