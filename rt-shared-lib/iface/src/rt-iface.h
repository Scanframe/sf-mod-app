#ifndef RUNTIMEIFACE_H
#define RUNTIMEIFACE_H

#include "rt-iface_global.h"

class RTIFACELIB_EXPORT RuntimeIface
{
	public:
		explicit RuntimeIface();

		static const char* GetGlobalString();
};

#endif // RUNTIMEIFACE_H
