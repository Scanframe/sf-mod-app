#pragma once

#include <QObject>
#include <QMimeType>
#include <QAbstractItemModel>
#include "misc/gen/TClassRegistration.h"
#include "MultiDocInterface.h"
#include "AppModuleFileType.h"
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
		 * @brief Creates instances of all available/loaded interface implementations.
		 */
		static void instantiate(QObject* parent);

		/**
		 * @brief Adds module property sheets to the passed widget parent.
		 */
		virtual void addPropertySheets(QWidget*) = 0;

		typedef QMap<QString, AppModuleInterface*> Map;

		/**
		 * @brief Gets the named module map.
		 */
		[[nodiscard]] static const Map& getMap();

		/**
		 * @brief Gets the description of this instance.
		 */
		[[nodiscard]] virtual QString getName() const = 0;

		/**
		 * @brief Gets the description of this instance.
		 */
		[[nodiscard]] virtual QString getDescription() const = 0;

		/**
		 * @brief Gets the library filename.
		 */
		[[nodiscard]] virtual QString getLibraryFilename() const = 0;
		/**
		 * @brief Gets svg icon resource name and can be overridden to change the default.
		 */
		[[nodiscard]] virtual QString getSvgIconResource() const;

		/**
		 * @brief Adds a file type handled by this instance.
		 *
		 * @param name Name of this file type.
		 * @param suffix File suffix like 'txt'
		 */
		void addFileType(const QString& name, const QString& suffix);
		/**
		 * @brief Adds a file type handled by this instance.
		 *
		 * @param mime Mime as a string like 'text/plain'.
		 */
		void addFileType(const QString& mime);

		/**
		 * @brief Adds a file type using the passed QMimeType instance.
		 *
		 * @param mime Mime as a string like 'text/plain'.
		 */
		void addFileType(const QMimeType& mime);
		/**
		 * @brief Find the instance handling the file using the file suffix.
		 *
		 * @return nullptr when not found.
		 */
		static AppModuleInterface* findByFile(const QString& filename);

		/**
		 * @brief Gets the filter string for a file dialog.
		 *
		 * The order of creation using #addFileType determines which of the duplicate entries get into
		 * open file dialog filter list. Duplicates can appear when an entry is created using a mime type.
		 *
		 * @return Filter string
		 */
		[[nodiscard]] QString getFileTypeFilters() const;

		/**
		 * @brief Gets the filter string for the open file dialog.
		 *
		 * The order of creation using #addFileType determines which of the duplicate entries get into
		 * open file dialog filter list. Duplicates can appear when an entry is created using a mime type.
		 *
		 * @return Filter string
		 */
		static QString getFileTypeFilters(bool all_files);

		/**
		 * @brief Determined if the this module handles this mime type.
		 * @return nullptr when cancelled
		 */
		static AppModuleInterface* selectDialog(const QString& title, QWidget* parent = nullptr);

		/**
		 * @brief Gets a list model from the available instances.
		 *
		 * @return List model .
		 */
		[[nodiscard]] static QAbstractItemModel* getListModel(QObject* parent);

		/**
		 * Calls #createChild() and assigns the this creating instance for reference.
		 * @param parent Parent and owner of the new child.
		 * @return
		 */
		[[nodiscard]] MultiDocInterface* createChild(QWidget* parent) const;

	protected:
		/**
		 * @brief Creates child of the passed parent (i.e. QMdiArea).
		 *
		 * @param parent Parent widget of the returned object.
		 * @return MDI child.
		 */
		[[nodiscard]] virtual MultiDocInterface* createWidget(QWidget* parent) const = 0;

	private:
		/**
		 * @brief Holds the file types serviced by this instance.
		 */
		QList<AppModuleFileType> _fileTypes;
		/**
		 * @brief Holds the instantiated module interfaces.
		 */
		static Map _map;

		// Declarations of static functions and data members to be able to create registered RSA implementations.
	SF_DECL_IFACE(AppModuleInterface, AppModuleInterface::Parameters, Interface)
};

}
