#pragma once
/**
 * @file misc/gen/PropertySheetDialog.h
 * <pre>
 * This file is to intercept non QT builds property sheet dialog inclusion replacing it with
 * forwarded classes so they still can be used as pointers or references in headers.
 * This in order to keep code clean of define checking.
 * To get the actual implementation of #sf:PropertySheetDialog the file 'misc/qt/PropertyPage.h' must be included.
 * </pre>
 */

#include <misc/global.h>

namespace sf
{

// Forward definition
class _MISC_CLASS PropertyPage;

// Forward definition
class _MISC_CLASS PropertySheetDialog;

}// namespace sf
