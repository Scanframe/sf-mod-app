#pragma once

#include <QWidget>
#include <QDialog>
#include <QSettings>

#include "../global.h"

namespace sf
{

class _MISC_CLASS ModuleConfiguration :public QObject
{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param settings Settings for loading and storing the module configuration.
		 * @param parent Owning object.
		 */
		explicit ModuleConfiguration(QSettings* settings, QObject* parent = nullptr);

		/**
		 * @brief Virtual Destructor.
		 */
		~ModuleConfiguration() override;

		/**
		 * @brief Opens a dialog for module configuration.
		 *
		 * @param parent Parent widget.
		 */
		void openDialog(QWidget* parent = nullptr);

		/**
		 * @brief Mapped array for passing module library files.
		 *
		 * The key is the (relative) path to the dynamic library file.
		 * The pair contains the module name of and the description.
		 */
		typedef QMap<QString, QString> ModuleListType;

		/**
		 * @brief Gets the configured modules in a list.
		 *
		 * @return Mapped list of module
		 */
		[[nodiscard]] ModuleListType getList() const;

		/**
		 * @brief Loads the module configuration from the settings.
		 */
		void load();

		/**
		 * @brief Saves the module configuration to the settings.
		 */
		void save(const ModuleListType& list);

		/**\
		 * @brief Gets the module library directory.
		 */
		QString getModuleDir();

	private:
		/**
		 * @brief Holds the dialog pointer when a dialog exists.
		 */
		QDialog* _dialog;
		/**
		 * @brief Holds the settings reference for the module config.
		 */
		QSettings* _settings;
};

}
