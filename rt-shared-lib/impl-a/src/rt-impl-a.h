#ifndef RT_IMPL_B_H
#define RT_IMPL_B_H

#include <memory>

#include "rt-iface.h"

/**
 * Exporting is not needed.
 */
class RuntimeLibImplementationA : public RuntimeIface
{
	public:
		/**
		 * Required constructor passing a parameters structure.
		 * @param params
		 */
		explicit RuntimeLibImplementationA(const Parameters& params);
		/**
		 * Implementing the pure virtual method.
		 */
		const char* getString() override;

	private:
		std::string FMessage;

};

#endif // RT_IMPL_B_H
