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

class Application;

class MainWindow :public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QSettings* settings, Application* application);

		~MainWindow() override;

		MultiDocInterface* openFile(const QString& filename);

		QMdiArea* getMdiArea()
		{
			return _mdiArea;
		}

		void settingsReadWrite(bool save);

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

		void createAppModuleMenus();

		sf::MultiDocInterface* createMdiChild(const QString& filename = QString());

		void switchLayoutDirection();

		void mdiSubActivated(QMdiSubWindow*);

	private:
		enum {MaxRecentFiles = 10};

		void createActions();

		void createStatusBar();

		void createDockWindows();

		MultiDocInterface* loadFile(const QString& filename);

		void prependToRecentFiles(const QString& filename);

		void setRecentFilesVisible(bool visible);

		[[nodiscard]] MultiDocInterface* activeMdiChild() const;

		[[nodiscard]] QMdiSubWindow* findMdiChild(const QString& filename) const;

		void settingsPropertySheet();

		void stateSaveRestore(bool save);

		void recentFilesReadWrite(bool save);

		Application* _application;

		QSettings* _settings;

		QMdiArea* _mdiArea{};
		QMenu* _menuSettings{};
		QMenu* _menuView{};
		QMenu* _menuTools{};
		QMenu* _menuWindow{};

		QAction* _actionNew{};
		QAction* _actionOpen{};
		QAction* _actionSave{};
		QAction* _actionSaveAs{};
		QAction* _actionRecentFile[MaxRecentFiles]{};
		QAction* _actionRecentFileSeparator{};
		QAction* _actionRecentFileSubMenu{};
		QAction* _actionCut{};
		QAction* _actionCopy{};
		QAction* _actionPaste{};
		QAction* _actionUndo{};
		QAction* _actionRedo{};
		QAction* _actionCloseWindow{};
		QAction* _actionCloseWindows{};
		QAction* _actionTileWindows{};
		QAction* _actionCascadeWindows{};
		QAction* _actionNextWindow{};
		QAction* _actionPreviousWindow{};
		QAction* _actionWindowMenuSeparator{};
		QAction* _actionModuleConfig{};
		QAction* _actionSettings{};
		QAction* _actionDevelop{};

		QStringList _recentFiles;

		void createAppModuleToolBars();
};

}
