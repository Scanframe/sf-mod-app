#ifndef RT_IFACE_H
#define RT_IFACE_H

#include "global.h"
#include "com/misc/regobj.h"

class _IFACE_CLASS RuntimeIface
{
	public:
		struct Parameters
		{
			explicit Parameters(int h) : Handle(h) {}

			int Handle;
		};

		// Required type of constructor passing a parameters structure.
		explicit RuntimeIface(Parameters& params);

		// Method needed to implemented in the implementation of a derived class.
		virtual const char* getString() = 0;

	DECLARE_REGISTER_BASE(RuntimeIface);
};

#endif // RT_IFACE_H
