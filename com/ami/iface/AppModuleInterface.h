#pragma once

#include <QObject>
#include <QWidget>
#include <QMimeType>
#include <QSettings>
#include "misc/gen/TClassRegistration.h"

#include "global.h"

namespace sf
{

/**
 * @brief Base/Interface class for application modules.
 *
 * This class should provide functionality for loading libraries dynamically and creating implementations
 */
class _AMI_CLASS AppModuleInterface :public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Structure for passing parameters to derived classes loaded and created at run-time.
		 */
		struct Parameters
		{
			explicit Parameters(QObject* parent)
				:_parent(parent) {}

			/**
			 * @brief Parent object
			 */
			QObject* _parent;
		};

		/**
		 * @brief Constructor for passing general structure for derived classes.
		 */
		explicit AppModuleInterface(const Parameters&);

		/**
		 * @brief Virtual destructor for derived classes.
		 */
		~AppModuleInterface() override;

		/**
		 * @brief Initializes the implementation for operation.
		 *
		 * Calls `doInitialize`.
		 * @return True on success.
		 */
		bool initialize();

		/**
		 * @brief Uninitializes the implementation.
		 *
		 * Calls `doInitialize`.
		 * @return True on success.
		 */
		bool uninitialize();

		/**
		 * @brief Adds module property sheets to the passed widget parent.
		 */
		virtual void addPropertySheets(QWidget*) = 0;

		/**
		 * @brief Determined if the this module handles this mime type.
		 * @return
		 */
		[[nodiscard]] virtual bool isHandlingMime(const QMimeType& mime) const = 0;

		/**
		 * @brief Opens a dialog for module configuration.
		 *
		 * @param settings Configuration settings
		 * @param parent Parent widget.
		 */
		static void openConfiguration(QSettings& settings, QWidget* parent);

	protected:
		/**
		 * @brief performs the actual initialization of a created module.
		 *
		 * @param initialize Flag for initialization or non initialization.
		 * @return Returns true on success.
		 */
		virtual bool doInitialize(bool initialize) noexcept = 0;

		// Declarations of static functions and data members to be able to create registered RSA implementations.
	SF_DECL_IFACE(AppModuleInterface, AppModuleInterface::Parameters, Interface)
};

}
