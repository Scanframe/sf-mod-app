#pragma once

#include <cstddef>
#include "../global.h"

namespace sf
{

/**
 * Interface class for file mapping
 */
class _MISC_CLASS IFileMapper
{
	public:
		/**
		 * Default constructor.
		 */
		IFileMapper() = default;

		/**
		 * Virtual destructor.
		 */
		virtual ~IFileMapper() = default;

		virtual void initialize() = 0;

		virtual void createView(size_t sz) = 0;

		virtual bool mapView() = 0;

		virtual bool unmapView() = 0;

		virtual void* getPtr() = 0;

		static IFileMapper* instantiate(bool native);

};

}
