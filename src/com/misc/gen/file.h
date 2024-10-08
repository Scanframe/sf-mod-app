#pragma once

#include "../global.h"
#include "TStrings.h"

namespace sf
{

/**
 * @brief Return a line from the input stream.
 */
_MISC_FUNC std::string getLine(std::istream& is);

/**
 * Checks if the passed path exist (by doing a access() using F_OK).
 */
_MISC_FUNC bool fileExists(const char* path);

inline bool fileExists(const std::string& path)
{
	return fileExists(path.c_str());
}

/**
 * Same as basename() but using a std::string.
 */
_MISC_FUNC std::string fileBaseName(const std::string& path);
/**
 * Same as dirname() but using a std::string.
 */
_MISC_FUNC std::string fileDirName(const std::string& path);
/**
 * Same as unlink() but using a std::string.
 */
_MISC_FUNC bool fileUnlink(const std::string& path);
/**
 * Same as rename() but using a std::string.
 */
_MISC_FUNC bool fileRename(const std::string& old_path, const std::string& new_path);

/**
 * @brief Finds the files using the passed wildcard.
 */
_MISC_FUNC bool findFiles(sf::strings& files, const std::string& wildcard);

/**
 * @brief Returns OS dependent directory separation character.
 */
_MISC_FUNC std::string::value_type getDirectorySeparator();

/**
 * @brief Returns the temporary directory.
 */
_MISC_FUNC std::string getTemporaryDirectory();

/**
 * @brief Returns the working directory.
 */
_MISC_FUNC std::string getWorkingDirectory();

/**
 * @brief Returns the executable filepath.
 */
_MISC_FUNC std::string getExecutableFilepath();

/**
 * @brief Returns the executable directory.
 */
_MISC_FUNC std::string getExecutableDirectory();

/**
 * @brief Returns the executable name.
 */
_MISC_FUNC std::string getExecutableName();

}// namespace sf