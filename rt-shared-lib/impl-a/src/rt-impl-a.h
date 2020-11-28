#ifndef RT_IMPL_A_H
#define RT_IMPL_A_H

#include <memory>
#include "global.h"

#include "iface/src/rt-iface.h"

class _IMPL_A_CLASS RuntimeLibImplementationA : public RuntimeIface
{
	public:
		// Required constructor passing a parameters structure.
		explicit RuntimeLibImplementationA(const Parameters& params);
		// Implementing the the virtual method.
		const char* getString() override;

	private:
		std::string FMessage;

/*
	Defines to declare and implement registration for objects
		DECLARE_REGISTER_BASE(T)
		DECLARE_REGISTER_OBJECT(T)
		DECLARE_REGISTER_BASE_AND_OBJECT(T)
*/
	DECLARE_REGISTER_OBJECT(RuntimeLibImplementationA);
};

#endif // RT_IMPL_A_H
