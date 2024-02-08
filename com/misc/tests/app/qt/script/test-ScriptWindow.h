#pragma once

#include <QMainWindow>
#include <QSettings>
#include <misc/gen/ScriptInterpreter.h>

QT_BEGIN_NAMESPACE
namespace Ui {class ScriptWindow;}
QT_END_NAMESPACE

class ScriptWindow :public QMainWindow
{
	Q_OBJECT

	public:
		explicit ScriptWindow(QSettings* settings, QWidget* parent = nullptr);

		~ScriptWindow() override;

	public:
		void onActionCompile();

		void onActionInitialize();

		void onActionStep();

		void onActionRun();

		void onClickInstruction(const QModelIndex& index);

		void stateSaveRestore(bool save);

	private:

		void updateStatus();

		// Pointer to the main window.
		Ui::ScriptWindow* ui;

		//
		sf::ScriptInterpreter _script;
		QSettings* _settings;

		void moveCursorToInstruction(int row);
};
