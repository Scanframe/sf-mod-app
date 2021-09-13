#pragma once

#include <utility>

#include <misc/gen/gen_utils.h>
#include <misc/gen/Value.h>
#include <misc/gen/TVector.h>
#include <misc/gen/TClassRegistration.h>
#include "global.h"

namespace sf
{

// Forward definition.
class PropertySheetDialog;
/**
 * Base class for the repetitive signal acquisition interface.
 */
class _STO_CLASS StorageInterface
{
	public:

		struct Parameters
		{
			explicit Parameters(int mode) :_mode(mode) {}

			int _mode;
		};

		/**
		 * @brief Constructor for passing general structure for derived classes.
		 */
		explicit StorageInterface(const Parameters&);

		/**
		 * @brief Virtual destructor for derived classes.
		 */
		virtual ~StorageInterface();

		/**
		 * @brief Initializes the implementation for operation. Calls DoInitialize.
		 * @return True on success.
		 */
		bool initialize();

		/**
		 * @brief Uninitializes the implementation. Calls DoInitialize.
		 * @return True on success.
		 */
		bool uinitialize();

		/**
		 * Adds controller specific property pages to the passed sheet.
		 * @param sheet
		 */
		virtual void addPropertyPages(PropertySheetDialog* sheet);

	protected:
		/**
		 * @brief Must be overridden by a derived class.
		 * Should check the hardware configuration.
		 *
		 * @param init True when initializing.
		 * @return True on success.
		 */
		virtual bool doInitialize(bool init) {return true;}

	private:
		// Holds flag of implementation initialization.
		bool _initialized;

		// Declarations of static functions and data members to be able to create registered RSA implementations.
	SF_DECL_IFACE(StorageInterface, StorageInterface::Parameters, Interface)

};

}
