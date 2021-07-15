#pragma once

#include <QWidget>
#include <QScrollArea>
#include <ami/iface/MultiDocInterface.h>

namespace sf
{

class LayoutEditor :public QWidget, public MultiDocInterface
{
	Q_OBJECT

	public:
		explicit LayoutEditor(QWidget* parent);

		void newFile() override;

		bool loadFile(const QString& fileName) override;

		bool save() override;

		bool saveAs() override;

		bool saveFile(const QString& fileName, bool keep_name = false);

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

		void develop() override;

	protected:
		void closeEvent(QCloseEvent* event) override;

		void resizeEvent(QResizeEvent* event) override;

	private slots:

		void documentWasModified();

	private:
		void setCurrentFile(const QString& fileName);

		[[nodiscard]] QString strippedName(const QString& fullFileName) const;

		QString curFile;
		bool isUntitled;
		QScrollArea* _scrollArea{nullptr};
		QWidget* _widget{nullptr};
		QString _rootName;
};

}
