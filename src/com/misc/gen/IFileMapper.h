#pragma once

#include "../global.h"
#include <cstddef>

namespace sf
{

/**
 * @brief class for file mapping
 */
class _MISC_CLASS IFileMapper
{
	public:
		/**
		 * @brief Default constructor.
		 */
		IFileMapper() = default;

		/**
		 * @brief Virtual destructor.
		 */
		virtual ~IFileMapper() = default;

		/**
		 * @brief Initialize and prepare the underlying system when needed.
		 *
		 */
		virtual void initialize() = 0;

		/**
		 * @brief Allocates disk space the file map.
		 *
		 * @param sz Size of the view.
		 */
		virtual void createView(size_t sz) = 0;

		/**
		 * @brief Locks a pointer in memory which can be unlocked using #unmapView().
		 *
		 * @return True on success.
		 */
		virtual bool mapView() = 0;

		/**
		 * @brief Unlocks the pointer locked with #mapView().
		 *
		 * @return True on success.
		 */
		virtual bool unmapView() = 0;

		/**
		 * @brief Gets the pointer locked using method #mapView().
		 *
		 * @return True on success.
		 */
		virtual void* getPtr() = 0;

		/**
		 * @brief Gets an instance of this interface using a native or non-native (Qt) implementation.
		 *
		 * Qt non-native is only available when compiled and linked with Qt.
		 * @param native False for creating Qt implementation.
		 * @return
		 */
		static IFileMapper* instantiate(bool native);
};

}// namespace sf
