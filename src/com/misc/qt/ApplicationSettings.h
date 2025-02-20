#pragma once
#include "qt_utils.h"
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QTreeView>

namespace sf
{

/**
 * @brief Keeps the application up-to-date with changes in the settings file.
 *
 * Sets the styling and the color from an ini file.
 * Used to quickly create test applications.
 */
class _MISC_CLASS ApplicationSettings final : public QObject
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit ApplicationSettings(QObject* parent = nullptr);

		/**
		 * @brief Destructor.
		 */
		~ApplicationSettings() override;

		/**
		 * @brief Sets the fileInfo using the filepath.
		 * @param filepath Path to the ini-file.
		 * @param watch Tell it to be watched for external changes.
		 */
		void setFilepath(const QString& filepath, bool watch = false);

		/**
		 * @brief Gets a QSettings instance using the set file path.
		 */
		QSettings getSettings() const;

		/**
		 * @brief Gets the fileInfo.
		 */
		[[nodiscard]] const QFileInfo& fileInfo() const;

		/**
		 * @brief Sets the window position and size from the settings file onto the passed widget.
		 * @param win_name Name of the window.
		 * @param window Window widget.
		 */
		void restoreWindowRect(const QString& win_name, QWidget* window) const;

		/**
		 * @brief Sets the window position and size from the settings file onto the passed widget.
		 * @param win_name Name of the window.
		 * @param window Window widget.
		 */
		void saveWindowRect(const QString& win_name, QWidget* window) const;

		void restoreTreeViewColumns(const QString& name, const QTreeView* tv) const;

		void saveTreeViewColumns(const QString& name, const QTreeView* tv) const;

	private Q_SLOTS:

		/**
		 * @brief Triggered when a watched files changes.
		 */
		void onFileChance(const QString& file);

	private:
		/**
		 * @brief Called from setFilepath and the event handler.
		 * @param watch
		 */
		void doStyleApplication(bool readOnly, bool watch);

		/**
		 * Save and restores the window state of the passed widget.
		 * @param name Name of the window widget.
		 * @param widget The window widget.
		 * @param save True for saving and false for restoring.
		 */
		void windowState(const QString& name, QWidget* widget, bool save) const;

		/**
		 * Save and restores the columns width of the passed treeview.
		 * @param name Name of the tree view.
		 * @param tv The tree view.
		 * @param save True for saving and false for restoring.
		 */
		void treeViewColumns(const QString& name, const QTreeView* tv, bool save) const;

		/**
		 * @brief File watcher instance.
		 */
		QFileSystemWatcher* _watcher;
		/**
		 * @brief Holds the timestamp of the last processed and watched file.
		 */
		QDateTime _lastModified;
		/**
		 * @brief File info structure of the ini-file.
		 */
		QFileInfo _fileInfo;
		/**
		 * @brief Holds system colors from startup.
		 *
		 * When having a different palette then the default one the icons in the file open dialog
		 * do not have the right color.
		 * This class provides a solution to store the initial palette for example.
		 */
		PaletteColors _systemColors;
};

}// namespace sf
