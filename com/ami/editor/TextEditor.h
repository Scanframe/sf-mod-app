#pragma once

#include <QTextEdit>
#include <ami/iface/MultiDocInterface.h>

namespace sf
{

class TextEditor :public QTextEdit, public MultiDocInterface
{
	Q_OBJECT

	public:
		TextEditor();

		void newFile() override;

		bool loadFile(const QString& fileName) override;

		bool save() override;

		bool saveAs() override;

		bool saveFile(const QString& fileName) override;

		[[nodiscard]] QString userFriendlyCurrentFile() const override;

		[[nodiscard]] QString currentFile() const override;

		[[nodiscard]] bool hasSelection() const override;

		void cut() override;

		void copy() override;

		void paste() override;

	protected:
		void closeEvent(QCloseEvent* event) override;

	private slots:

		void documentWasModified();

	private:
		bool maybeSave();

		void setCurrentFile(const QString& fileName);

		[[nodiscard]] QString strippedName(const QString& fullFileName) const;

		QString curFile;
		bool isUntitled;
};

}
