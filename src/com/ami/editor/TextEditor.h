#pragma once

#include <QTextEdit>
#include <ami/iface/MultiDocInterface.h>

namespace sf
{

class TextEditor :public QTextEdit, public MultiDocInterface
{
	Q_OBJECT

	public:
		explicit TextEditor(QWidget* parent);

		void newFile() override;

		bool loadFile(const QString& fileName) override;

		bool save() override;

		bool saveAs() override;

		bool saveFile(const QString& fileName);

		[[nodiscard]] bool isModified() const override;

		[[nodiscard]] QString userFriendlyCurrentFile() const override;

		[[nodiscard]] QString currentFile() const override;

		[[nodiscard]] bool hasSelection() const override;

		[[nodiscard]] bool isUndoRedoEnabled() const override;

		[[nodiscard]] bool isUndoAvailable() const override;

		[[nodiscard]] bool isRedoAvailable() const override;

		void cut() override;

		void copy() override;

		void paste() override;

		void undo() override;

		void redo() override;

	protected:
		void closeEvent(QCloseEvent* event) override;

	private Q_SLOTS:

		void documentWasModified();

	private:

		void setCurrentFile(const QString& fileName);

		[[nodiscard]] QString strippedName(const QString& fullFileName) const;

		QString curFile;
		bool isUntitled;
};

}
