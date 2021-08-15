#pragma once

#include <QMainWindow>
#include <ami/iface/MultiDocInterface.h>
#include <ami/iface/AppModuleInterface.h>

class QShortcut;

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

		~MainWindow() override;

		MultiDocInterface* openFile(const QString& filename);

		QMdiArea* getMdiArea()
		{
			return _mdiArea;
		}

		void settingsReadWrite(bool save);

		void initialize();

	protected:
		void dragEnterEvent(QDragEnterEvent* event) override;

		void dropEvent(QDropEvent* event) override;

		void closeEvent(QCloseEvent* event) override;

	private Q_SLOTS:

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
		enum {MaxRecentFiles = 10};

		void closeDocument();

		void createActions();

		void createStatusBar();

		MultiDocInterface* loadFile(const QString& filename);

		void prependToRecentFiles(const QString& filename);

		void setRecentFilesVisible(bool visible);

		[[nodiscard]] MultiDocInterface* activeMdiChild() const;

		[[nodiscard]] QMdiSubWindow* findMdiChild(const QString& filename) const;

		ModuleConfiguration* _moduleConfiguration{nullptr};

		void settingsPropertySheet();

		void stateSaveRestore(bool save);

		void recentFilesReadWrite(bool save);

		QSettings* _settings;

		QMdiArea* _mdiArea{};
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
		QAction* _settingsAction{};
		QAction* _developAction{};

		QStringList _recentFiles;
};

}
