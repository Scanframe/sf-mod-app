#pragma once

#include <stddef.h>
#include "../global.h"

_MISC_FUNC int strerror_r(int errnum, char *buf, size_t buflen);

_MISC_FUNC bool isRunningWine();

