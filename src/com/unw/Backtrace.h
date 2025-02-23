#pragma once
#include "global.h"
#include <ostream>

namespace sf::backtrace
{

/**
 * @brief Called  when library is loaded.
 */
void initialize();

/**
 * @brief Called  when library is unloaded.
 */
void uninitialize();

/**
 * @brief Prints the call stack.
 */
_UNW_FUNC void print(std::ostream& os);

};// namespace sf::backtrace
