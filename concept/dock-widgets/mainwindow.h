#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;

class QListWidget;

class QMenu;

class QTextEdit;

QT_END_NAMESPACE

class MainWindow :public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();

	private slots:

		void newLetter();

		void save();

		void print();

		void undo();

		void about();

		void insertCustomer(const QString& customer);

		void addParagraph(const QString& paragraph);

	private:
		void createActions();

		void createStatusBar();

		void createDockWindows();

		QTextEdit* _textEdit{nullptr};
		QListWidget* _customerList{nullptr};
		QListWidget* _paragraphsList{nullptr};
		QMenu* _viewMenu{nullptr};
};
