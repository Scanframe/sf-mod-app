#pragma once

#include <QObject>
#include <QMimeType>
#include <QAbstractItemModel>
#include <QSettings>
#include <QDockWidget>
#include <misc/gen/TClassRegistration.h>
#include <misc/qt/PropertySheetDialog.h>
#include "MultiDocInterface.h"
#include "AppModuleFileType.h"
#include "global.h"

class QMainWindow;

namespace sf
{

/**
 * @brief Base/Interface class for application modules.
 *
 * This class should provide functionality for loading libraries dynamically and creating implementations
 */
class _AMI_CLASS AppModuleInterface :public QObject
{
	public:
		/**
		 * @brief Structure for passing parameters to derived classes loaded and created at run-time.
		 */
		struct Parameters
		{
			explicit Parameters(QSettings* settings, QObject* parent)
				:_settings(settings)
				 , _parent(parent) {}

			/**
			 * @brief Settings of application.
			 */
			QSettings* _settings;
			/**
			 * @brief Parent object.
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
		static void instantiate(QSettings* settings, QObject* parent);

		/**
		 * @brief Initializes all uninitialized instances or visa versa.
		 *
		 * @param init Determines initialization or uninitialization.
		 * @return Amount of initialized instances.
		 */
		static size_t initializeInstances(bool init = true);

		/**
		 * @brief Adds property pages from all modules to the passed sheet.
		 */
		static void addAllPropertyPages(PropertySheetDialog* sheet);

		/**
		 * Type of menu items to collect.
		 */
		enum MenuType
		{
			Settings,
			View,
			Tools
		};

		/**
		 * @brief Adds items from all modules to this menu.
		 * @param menuType Type of menu items to add.
		 * @param menu Menu to add the actions/menus to.
		 */
		static void addAllMenuItems(MenuType menuType, QMenu* menu);

		/**
		 * Map type for the application modules to their names.
		 */
		typedef QMap<QString, AppModuleInterface*> Map;

		/**
		 * @brief Gets the named module map.
		 */
		[[nodiscard]] static const Map& getMap();

		/**
		 * @brief Called when al modules are loaded or when a module added.
		 * @param init Determines initialization or uninitialization.
		 */
		virtual void initialize(bool init) = 0;

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
		 * @brief Adds module property pages to the passed sheet.
		 */
		virtual void addPropertyPages(PropertySheetDialog* sheet) = 0;

		/**
		 * @brief Adds menu items for the passed type of menu.
		 */
		virtual void addMenuItems(MenuType menuType, QMenu* menu);

		/**
		 * @brief Type definition for containing dock widgets.
		 */
		typedef QList<QDockWidget*> DockWidgetList;

		/**
		 * @brief Creates docking widgets having the passed parent.
		 */
		virtual DockWidgetList createDockingWidgets(QWidget* parent);

		/**
		 * @brief Creates all docking widgets of the loaded modules by calling #createDockingWidgets().
		 *
		 * @see createDockingWidgets()
		 */
		static DockWidgetList createAllDockingWidgets(QWidget* parent);

		/**
		 * @brief Adds toolbars to the passed main window.
		 */
		virtual void addToolBars(QMainWindow* mainWindow);

		/**
		 * @brief Creates all toolbars of the loaded modules by calling #addToolBars().
		 *
		 * @see addToolBars()
		 */
		static void addAllToolBars(QMainWindow* mainWindow);

		/**
		 * Gets if the instance has file handling types.
		 *
		 * @return True when handling file types.
		 */
		[[nodiscard]] bool hasFileTypes() const;

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
		 *
		 * @return nullptr when cancelled
		 */
		static AppModuleInterface* selectDialog(const QString& title, QSettings* settings, QWidget* parent = nullptr);

		/**
		 * @brief Gets a list model from the available instances.
		 *
		 * @param file_only True when only Modules handling files are to listed.
		 * @param parent Owning object.
		 * @return The list model.
		 */
		[[nodiscard]] static QAbstractItemModel* getListModel(bool file_only, QObject* parent);

		/**
		 * @brief Calls #createChild() and assigns the this creating instance for reference.
		 *
		 * @param parent Parent and owner of the new child.
		 * @return
		 */
		[[nodiscard]] MultiDocInterface* createChild(QWidget* parent) const;

		/**
		 * @brief Gets the settings class passed on creation.
		 */
		[[nodiscard]] QSettings* getSettings() const;

		/**
		 * @brief Makes the application open a file.
		 *
		 * @param filename Name of file to open.
		 * @param ami Force file, even when empty to open using this module.
		 * @return Not null when the type of document was handled.
		 */
		MultiDocInterface* openFile(const QString& filename, AppModuleInterface* ami = nullptr) const;

		/**
		 * @brief Called from main window when a document is activated or deactivated.
		 *
		 * @param iface MDI interface which can be dynamic casted to the actual document.
		 * @param yn
		 */
		virtual void documentActivated(MultiDocInterface* iface, bool yn) const;

		/**
		 * @brief Type definition for the callback closure.
		 */
		typedef TClosure<MultiDocInterface*, const QString&, AppModuleInterface*> OpenFileClosure;
		/**
		 * @brief Holds a callback to open a file in the application.
		 */
		static OpenFileClosure callbackOpenFile;

	protected:
		/**
		 * @brief Creates child of the passed parent (i.e. QMdiArea).
		 *
		 * @param parent Parent widget of the returned object.
		 * @return MDI child.
		 */
		[[nodiscard]] virtual MultiDocInterface* createWidget(QWidget* parent) const;

	private:
		/**
		 * @brief Settings of application.
		 */
		QSettings* _settings;
		/**
		 * @brief Holds the file types serviced by this instance.
		 */
		QList<AppModuleFileType> _fileTypes;
		/**
		 * Holds the flag if the module has been initialized.
		 */
		bool _initialized;
		/**
		 * @brief Holds the instantiated module interfaces.
		 */
		static Map _map;

		// Declarations of static functions and data members to be able to create implementations.
	SF_DECL_IFACE(AppModuleInterface, AppModuleInterface::Parameters, Interface)

};

}
