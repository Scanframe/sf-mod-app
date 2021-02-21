#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include "MdiChild.h"

class TextEditor :public QTextEdit, public MdiChild
{
	Q_OBJECT

	public:
		TextEditor();

		void newFile() override;

		bool loadFile(const QString& fileName) override;

		bool save() override;

		bool saveAs() override;

		bool saveFile(const QString& fileName) override;

		QString userFriendlyCurrentFile() const override;

		QString currentFile() const override;

		bool hasSelection() const override;

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

		QString strippedName(const QString& fullFileName) const;

		QString curFile;
		bool isUntitled;
};

#endif
