#pragma once

#include <QMainWindow>
#include <QLayout>
#include <QScrollArea>

QT_BEGIN_NAMESPACE
namespace Ui{class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		/**
		 * Constructor.
		 */
		explicit MainWindow(QWidget* parent = nullptr);
		/**
		 * Overridden virtual destructor
		 */
		~MainWindow() override;

	private Q_SLOTS:
		/**
		 * Create onCreateFormDialog using the UI file sizing to contain the UI-file entirely.
		 * When the onCreateFormDialog is sided smaller scrollbars appear.
		 */
		void onCreateFormDialog();
		/**
		 * Reads UI-file into the an area of the main window.
		 */
		void onReadFromFile();
		/**
		 * Writes UI-file from the area of the main window.
		 */
		void onWriteToFile();

	private:
		Ui::MainWindow* ui;
		/**
		 * Returns the uiFilepath to read.
		 */
		QString uiFilepath();

		const QString rootName;
};

