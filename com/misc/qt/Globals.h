#pragma once

#include <QSettings>
#include "../global.h"

namespace sf
{

/**
 * @brief Sets global settings class for settings.

 * @param settings New instance.
 * @return Previous instance.
 */
_MISC_FUNC QSettings* setGlobalSettings(QSettings* settings);

/**
 * @brief Gets global settings class for settings.
 *
 * @return Current global instance when set otherwise nullptr is returned.
 */
_MISC_FUNC QSettings* getGlobalSettings();

/**
 * @brief Sets global parent widget class for dialogs.
 *
 * @param parent New instance.
 * @return Previous instance.
 */
_MISC_FUNC QWidget* setGlobalParent(QWidget* parent);

/**
 * @brief Gets global parent widget  class for settings.
 *
 * @return Current global instance when set otherwise nullptr is returned.
 */
_MISC_FUNC QWidget* getGlobalParent();

}