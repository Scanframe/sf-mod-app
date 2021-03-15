#pragma once

#include "RuntimeIface.h"

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
