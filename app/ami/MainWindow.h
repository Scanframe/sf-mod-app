#pragma once

#include <QMainWindow>
#include <ami/iface/MultiDocInterface.h>
#include <ami/iface/AppModuleInterface.h>

class QAction;

class QMenu;

class QSettings;

class QMdiArea;

class QMdiSubWindow;

namespace sf
{

class ModuleConfiguration;

class MainWindow :public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QSettings* settings);

		bool openFile(const QString& fileName);

	protected:
		void closeEvent(QCloseEvent* event) override;

	private slots:

		void newFile();

		void open();

		void save();

		void saveAs();

		void updateRecentFileActions();

		void openRecentFile();

		void cut();

		void copy();

		void paste();

		void undo();

		void redo();

		void about();

		void configModules();

		void updateMenus();

		void updateWindowMenu();

		sf::MultiDocInterface* createMdiChild(const QString& filename = QString());

		void switchLayoutDirection();

	private:
		enum {MaxRecentFiles = 5};

		void closeDocument();

		void createActions();

		void createStatusBar();

		void readSettings();

		void writeSettings();

		bool loadFile(const QString& filename);

		static bool hasRecentFiles();

		void prependToRecentFiles(const QString& filename);

		void setRecentFilesVisible(bool visible);

		[[nodiscard]] MultiDocInterface* activeMdiChild() const;

		[[nodiscard]] QMdiSubWindow* findMdiChild(const QString& filename) const;

		ModuleConfiguration* _moduleConfiguration{nullptr};

		QMdiArea* _mdiArea{};

		QSettings* _settings;

		QMenu* _windowMenu{};

		QAction* _newAction{};
		QAction* _openAction{};
		QAction* _saveAction{};
		QAction* _saveAsAction{};
		QAction* _recentFileActions[MaxRecentFiles]{};
		QAction* _recentFileSeparator{};
		QAction* _recentFileSubMenuAction{};
		QAction* _cutAction{};
		QAction* _copyAction{};
		QAction* _pasteAction{};
		QAction* _undoAction{};
		QAction* _redoAction{};
		QAction* _closeWindowAction{};
		QAction* _closeWindowsAction{};
		QAction* _tileAction{};
		QAction* _cascadeAction{};
		QAction* _nextAction{};
		QAction* _previousAction{};
		QAction* _windowMenuSeparatorAction{};
		QAction* _moduleConfigAction{};

};

}
