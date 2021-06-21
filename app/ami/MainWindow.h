#pragma once

#include <QMainWindow>
#include <ami/iface/MultiDocInterface.h>
#include <ami/iface/AppModuleInterface.h>


class QAction;

class QMenu;

class QMdiArea;

class QMdiSubWindow;

namespace sf
{

class MainWindow :public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QSettings& settings);

		bool openFile(const QString& fileName);

	protected:
		void closeEvent(QCloseEvent* event) override;

		void showEvent(QShowEvent* event) override;

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

		void about();

		void configModules();

		void updateMenus();

		void updateWindowMenu();

		sf::MultiDocInterface* createMdiChild(const QString& filename = QString());

		void switchLayoutDirection();

	private:
		enum {MaxRecentFiles = 5};

		void createActions();

		void createStatusBar();

		void readSettings();

		void writeSettings();

		bool loadFile(const QString& filename);

		static bool hasRecentFiles();

		void prependToRecentFiles(const QString& filename);

		void setRecentFilesVisible(bool visible);

		[[nodiscard]] sf::MultiDocInterface* activeMdiChild() const;

		[[nodiscard]] QMdiSubWindow* findMdiChild(const QString& filename) const;

		QMdiArea* _mdiArea;

		QSettings& _settings;

		QMenu* _windowMenu{};
		QAction* _newAct{};
		QAction* _saveAct{};
		QAction* _saveAsAct{};
		QAction* _recentFileActs[MaxRecentFiles]{};
		QAction* _recentFileSeparator{};
		QAction* _recentFileSubMenuAct{};
		QAction* _cutAct{};
		QAction* _copyAct{};
		QAction* _pasteAct{};
		QAction* _closeAct{};
		QAction* _closeAllAct{};
		QAction* _tileAct{};
		QAction* _cascadeAct{};
		QAction* _nextAct{};
		QAction* _previousAct{};
		QAction* _windowMenuSeparatorAct{};
		QAction* _moduleConfigAct{};
};

}
