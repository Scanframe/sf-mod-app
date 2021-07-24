#pragma once

#include <QMainWindow>
#include <QSettings>
#include <misc/gen/ScriptInterpreter.h>
#include <misc/qt/Editor.h>
#include <ami/iface/PlainTextEditMdi.h>

namespace Ui {class ScriptEditor;}

namespace sf
{

class ScriptEditor :public QWidget, public PlainTextEditMdi
{
	Q_OBJECT

	public:
		explicit ScriptEditor(QSettings* settings, QWidget* parent = nullptr);

		~ScriptEditor() override;

		[[nodiscard]] Editor* getEditor() const;

		[[nodiscard]] ScriptInterpreter* getScriptInterpreter() const;

		void setInterpreter(const QSharedPointer<ScriptInterpreter>& interpreter);

		void updateStatus();

		void moveCursorToInstruction(int row);

	public:
		void onActionCompile();

		void onActionInitialize();

		void onActionStep();

		void onActionRun();

		void onActionStop();

		void onClickInstruction(const QModelIndex& index);

		void stateSaveRestore(bool save) override;

		void develop() override;

	protected:
		void closeEvent(QCloseEvent* event) override;

	private:
		Ui::ScriptEditor* ui;

		struct Private;
		Private& _p;
		friend Private;
};

}
