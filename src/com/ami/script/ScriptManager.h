#pragma once

#include <QTimer>
#include <QSettings>
#include <QKeySequence>
#include <QShortcut>
#include <QWidget>
#include <gsc/GlobalShortcut.h>
#include <misc/gen/ScriptInterpreter.h>

namespace sf
{

class ScriptManagerListModel;

class ScriptEntry;

/**
 * @brief Manages scripts for the application.
 */
class ScriptManager :public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit ScriptManager(QSettings* settings, QObject* parent);

		/**
		 * @brief Overridden destructor.
		 */
		~ScriptManager() override;

		/**
		 * @brief Gets the full file path of a script name passed.
		 *
		 * @param base_name Base name of the file without extension.
		 * @return Full filepath.
		 */
		[[nodiscard]] QString getScriptFilePath(const QString& base_name = QString()) const;

		/**
		 * @brief Gets the file extension used for scripts.
		 */
		static QString getFileSuffix();

		/**
		 * @brief Gets called from an entry when a shortcut is activated.

		 * @param entry
		 */
		void shortcutActivated(ScriptEntry* entry);

		/**
		 * @brief Saves and restores the entries from settings.
		 */
		void settingsReadWrite(bool save);

		/**
		 * @brief Gets status whether to save to settings is needed.
		 */
		[[nodiscard]] bool isModified() const;

		/**
		 * @brief Adds and empty entry to the list.
		 */
		void addAt(qsizetype index = -1);

		/**
		 * @brief Removes an entry in the list.
		 */
		void remove(qsizetype index);

		/**
		 * @brief Compiles and initializes script at passed position or all scripts when passing -1.
		 */
		void start(qsizetype index = -1);

		/**
		 * @brief Stops script at passed position or all scripts when passing -1.
		 */
		void stop(qsizetype index = -1);

		/**
		 * @brief Gets the entry from the list by index.
		 */
		ScriptEntry* getEntry(qsizetype index);

		/**
		 * @brief Sets the modified flag and notifies ...
		 */
		void setModified(QObject* caller);

		/**
		 * @brief Gets the entry pointer form the passed absolute file path.
		 */
		const ScriptEntry* getEntryByFilepath(const QString& filepath);

	private:
		/**
		 * @brief Called by the  timer to run the background scripts.
		 */
		void backgroundRun();

		/**
		 * @brief Gets the available file names without suffix from the scripts directory.
		 */
		[[nodiscard]] QStringList getFilenames() const;

		/**
		 * @brief Holds the settings pointer passed in the constructor.
		 */
		QSettings* _settings;
		/**
		 * @brief Holds the entries
		 */
		QList<ScriptEntry*> _list;
		/**
		 * @brief Dirty flag on when something changed that needs saving.
		 */
		bool _modified{false};
		/**
		 * Holds the base directory for all scripts.
		 */
		QString _subDirectory;
		/**
		 * @brief Timer instance calling the backgroundRun() method regularly.
		 */
		QTimer* _timer{nullptr};

		friend ScriptManagerListModel;
		friend ScriptEntry;
};

/**
 * @brief Script entry for the list contained by the manager.
 */
class ScriptEntry :public QObject
{
	Q_OBJECT

	public:

		/**
		 *@brief Available background modes.
		 */
		enum EBackgroundMode
		{
			/** No background running of the script. */
			bmNo = 0,
			/** Run the script once. */
			bmOnce,
			/** Run the script continuous*/
			bmContinuous
		};

		Q_ENUM(EBackgroundMode)

		/**
		 * @brief Constructor.
		 */
		explicit ScriptEntry(ScriptManager* parent)
			:QObject(parent)
			 , _manager(parent) {}

		/**
		 * @brief Compiles and initializes the script.
		 *
		 * @return True on success.
		 */
		bool start();

		/**
		 * @brief Stops a script from running in the background.
		 */
		void stop();

		[[nodiscard]] const QKeySequence& getKeySequence() const;

		void setKeySequence(const QKeySequence& ks);

		[[nodiscard]] QString getDisplayName() const;

		void setDisplayName(const QString& name);

		[[nodiscard]] QString getScriptName() const;

		void setScriptName(const QString& name);

		[[nodiscard]] const QString& getFilename() const
		{
			return _filename;
		}

		void setFilename(const QString& name);

		[[nodiscard]] EBackgroundMode getBackgroundMode() const
		{
			return _background;
		}

		void setBackgroundMode(EBackgroundMode bm);

		void setInterpreter(const QSharedPointer<ScriptInterpreter>& interpreter)
		{
			_interpreter = interpreter;
		}

		[[nodiscard]] const QSharedPointer<ScriptInterpreter>& getInterpreter() const
		{
			return _interpreter;
		}

		[[nodiscard]] QString getStateName() const;

		/**
		 * @brief Sets the global shortcut global flag.
		 */
		[[nodiscard]] bool isGlobal() const;

		/**
		 * @brief Sets the global shortcut global flag.
		 */
		void setGlobal(bool);

	private:
		/**
		 * @brief Name of the script file not the path.
		 */
		QString _filename{};
		/**
		 * @brief Flag to make script run in the background.
		 */
		EBackgroundMode _background{bmNo};
		/**
		 * @brief Derived or interpreter class instance.
		 */
		QSharedPointer<ScriptInterpreter> _interpreter;
		/**
		 * @brief Holds the owning manager.
		 */
		ScriptManager* _manager{nullptr};
		/**
		 * @brief Shortcut key sequence to execute the script.
		 */
		QKeySequence _keySequence{};
		/**
		 * @brief Holds the global shortcut instance.
		 */
		GlobalShortcut* _globalShortcut{nullptr};
		/**
		 * @brief Holds teh shortcut global flag.
		 */
		bool _global{false};
};

}