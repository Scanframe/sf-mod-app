#pragma once

#include <string>
#include "TClosure.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Type of the callback function.
 */
typedef TClosure<std::string, const std::string&> ConfigLocationClosure;

/**
 * @brief Sets the callback function for the #getConfigLocation() function.
 *
 * @param handler Callback closure.
 */
_MISC_FUNC void setConfigLocationHandler(const ConfigLocationClosure& handler = {});

/**
 * @brief Gets the configuration location/directory of the application.
 *
 * When no handler is installed the working directory is returned.
 *
 * @param option Optional option to pass to manipulate the result.
 * @param separator Terminates the string with a directory separator.
 * @return Directory
 */
_MISC_FUNC std::string getConfigLocation(const std::string& option = {}, bool separator = false);

}