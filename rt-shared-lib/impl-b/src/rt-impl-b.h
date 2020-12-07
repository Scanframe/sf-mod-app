#ifndef RT_IMPL_A_H
#define RT_IMPL_A_H

#include "iface/src/rt-iface.h"

class RuntimeLibImplementationB : public RuntimeIface
{
	public:
		// Required constructor passing a parameters structure.
		explicit RuntimeLibImplementationB(const Parameters& params);
		// Implementing the the virtual method.
		const char* getString() override;

	private:
		std::string FMessage;

};

#endif // RT_IMPL_B_H
