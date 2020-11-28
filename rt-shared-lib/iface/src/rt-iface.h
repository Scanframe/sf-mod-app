#ifndef RT_IFACE_H
#define RT_IFACE_H

#include "global.h"
#include "com/misc/class_reg.h"
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
		explicit RuntimeIface(const Parameters& params);
		// Method needed to implemented in the implementation of a derived class.
		virtual const char* getString() = 0;
		//
		static misc::TClassRegistration<RuntimeIface, Parameters> ClassRegistration;

	DECLARE_REGISTER_BASE(RuntimeIface);
};

//_IFACE_CLASS extern misc::TClassRegistration<RuntimeIface, RuntimeIface::Parameters> ClassRegistration;


#endif // RT_IFACE_H
