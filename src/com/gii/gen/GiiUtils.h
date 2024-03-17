#pragma once

#include <misc/gen/IniProfile.h>
#include "../global.h"

namespace sf
{

/**
 * @brief Saves variable settings to the passed profile instance.
 *
 * Which variable values are saved is determined by its flags.
 */
bool _GII_FUNC SaveVariableValues(IniProfile& profile);

/**
 * Loads variable settings from the passed profile instance.
 *
 * Which variable values are loaded is determined by its flags.
 */
bool _GII_FUNC LoadVariableValues(IniProfile& profile);

}