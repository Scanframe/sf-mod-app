#pragma once

#include <cstring>

/**
 * Separator between name and description.
 */
#define _DL_NAME_SEPARATOR "\n\n"

/**
 * Defines marker string used to find the start of a naming and description block in a compiled binary
 */
#define _DL_MARKER_BEGIN "\n#@$!*^\n"
/**
 * End marker
 */
#define _DL_MARKER_END "\n^*!$@#\n"

namespace sf
{

/**
 * @brief
 * Information on a dynamic library Set using _DL_INFORMATION macro.
 */
struct _MISC_CLASS DynamicLibraryInfo
	{
		/**
		 * Default destructor.
		 */
		DynamicLibraryInfo() = default;
	/**
	 * Copy destructor.
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
	 * Reads the information from the file and return true on success.
	 */
	bool read(const std::string& filepath);
	/**
	 * Clears the members.
	 */
	void reset();
	};

}