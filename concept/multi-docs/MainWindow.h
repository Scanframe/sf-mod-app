#pragma once

#include <QMainWindow>
#include "MdiChild.h"

class MdiChild;

QT_BEGIN_NAMESPACE
class QAction;

class QMenu;

class QMdiArea;

class QMdiSubWindow;

QT_END_NAMESPACE

class MainWindow :public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();

		bool openFile(const QString& fileName);

	protected:
		void closeEvent(QCloseEvent* event) override;

	private Q_SLOTS:

		void newFile();

		void open();

		void save();

		void saveAs();

		void updateRecentFileActions();

		void openRecentFile();

#ifndef QT_NO_CLIPBOARD

		void cut();

		void copy();

		void paste();

#endif

		void about();

		void updateMenus();

		void updateWindowMenu();

		MdiChild* createMdiChild();

		void switchLayoutDirection();

	private:
		enum {MaxRecentFiles = 5};

		void createActions();

		void createStatusBar();

		void readSettings();

		void writeSettings();

		bool loadFile(const QString& fileName);

		static bool hasRecentFiles();

		void prependToRecentFiles(const QString& fileName);

		void setRecentFilesVisible(bool visible);

		MdiChild* activeMdiChild() const;

		QMdiSubWindow* findMdiChild(const QString& fileName) const;

		QMdiArea* mdiArea;

		QMenu* windowMenu{};
		QAction* newAct{};
		QAction* saveAct{};
		QAction* saveAsAct{};
		QAction* recentFileActs[MaxRecentFiles]{};
		QAction* recentFileSeparator{};
		QAction* recentFileSubMenuAct{};
		QAction* cutAct{};
		QAction* copyAct{};
		QAction* pasteAct{};
		QAction* closeAct{};
		QAction* closeAllAct{};
		QAction* tileAct{};
		QAction* cascadeAct{};
		QAction* nextAct{};
		QAction* previousAct{};
		QAction* windowMenuSeparatorAct{};
};
