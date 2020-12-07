#ifndef RT_IFACE_H
#define RT_IFACE_H

#include "global.h"
#include "misc/class_reg.h"

/**
 * This class is the interface to all derived classes and could be pure virtual.
 */
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
		// Virtual destructor required.
		virtual ~RuntimeIface();
		// Method needed to implemented in the implementation of a derived class.
		virtual const char* getString() = 0;

	// Declare the interface function.
	SF_DECL_IFACE(RuntimeIface, RuntimeIface::Parameters, Interface)
};

#endif // RT_IFACE_H
