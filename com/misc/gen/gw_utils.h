#pragma once

#include "../global.h"
#include <cstddef>


namespace gw
{

/**
 * Alternative implementation for MinGW faulting function fcvt_r.
 */
_MISC_FUNC int fcvt_r(double value, int ndigit, int* decpt, int* sign, char* buf, size_t len);

/**
 * Alternative implementation for MinGW faulting function ecvt_r.
 */
_MISC_FUNC int ecvt_r(double value, int ndigit, int* decpt, int* sign, char* buf, size_t len);

}
