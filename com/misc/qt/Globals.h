#pragma once

#include "../global.h"

// Forward definitions.
class QSettings;

class QUiLoader;

class QWidget;

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
 * @brief Gets global parent widget class for settings.
 *
 * @return Current global instance when set otherwise nullptr is returned.
 */
_MISC_FUNC QWidget* getGlobalParent();

/**
 * @brief Sets global UI loader class for.
 *
 * @param uiLoader New instance.
 * @return Previous instance.
 */
_MISC_FUNC QUiLoader* setGlobalUiLoader(QUiLoader* uiLoader);

/**
 * @brief Gets global UI loader instance.
 *
 * @return Current global instance when set otherwise nullptr is returned.
 */
_MISC_FUNC QUiLoader* getGlobalUiLoader();

}