#include "win_utils.h"
#include "string.h"

int strerror_r(int errnum, char *buf, size_t buflen)
{
	return strerror_s(buf, buflen, errnum);
}


