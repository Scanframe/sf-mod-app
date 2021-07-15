#pragma once

#include <QPlainTextEdit>
#include "Highlighter.h"
#include <ami/iface/MultiDocInterface.h>
#include "CodeEditorConfiguration.h"

class QPaintEvent;

class QResizeEvent;

class QSize;

class QWidget;

namespace sf
{

class CodeEditor :public QPlainTextEdit, public MultiDocInterface
{
	Q_OBJECT

	public:
		// Constructor.
		explicit CodeEditor(const CodeEditorConfiguration& cfg, QWidget* parent = nullptr);

		// MultiDocInterface interface method.
		void newFile() override;

		// MultiDocInterface interface method.
		bool loadFile(const QString& fileName) override;

		// MultiDocInterface interface method.
		bool save() override;

		// MultiDocInterface interface method.
		bool saveAs() override;

		bool saveFile(const QString& fileName);

		// MultiDocInterface interface method.
		[[nodiscard]] bool isModified() const override;

		// MultiDocInterface interface method.
		[[nodiscard]] QString userFriendlyCurrentFile() const override;

		// MultiDocInterface interface method.
		[[nodiscard]] QString currentFile() const override;

		// MultiDocInterface interface method.
		[[nodiscard]] bool hasSelection() const override;

		// MultiDocInterface interface method.
		[[nodiscard]] bool isUndoRedoEnabled() const override;

		// MultiDocInterface interface method.
		[[nodiscard]] bool isUndoAvailable() const override;

		// MultiDocInterface interface method.
		[[nodiscard]] bool isRedoAvailable() const override;

		// MultiDocInterface interface method.
		void cut() override;

		// MultiDocInterface interface method.
		void copy() override;

		// MultiDocInterface interface method.
		void paste() override;

		// MultiDocInterface interface method.
		void undo() override;

		// MultiDocInterface interface method.
		void redo() override;

		void lineNumberAreaPaintEvent(QPaintEvent* event);

		int lineNumberAreaWidth();

	protected:
		void resizeEvent(QResizeEvent* event) override;

		void closeEvent(QCloseEvent* event) override;

	private slots:

		void updateLineNumberAreaWidth(int newBlockCount);

		void highlightCurrentLine();

		void updateLineNumberArea(const QRect& rect, int dy);

		void documentWasModified();

	private:
		void setCurrentFile(const QString& fileName);

		[[nodiscard]] QString strippedName(const QString& fullFileName) const;

		QString curFile;
		bool isUntitled;
		QWidget* lineNumberArea;
		int spacingNumber;
		// Color for background of current line.
		QColor curLineColor;

		/**
		 * Internal class.
		 */
		class LineNumberArea :public QWidget
		{
			public:
				explicit LineNumberArea(CodeEditor* editor)
					:QWidget(editor), codeEditor(editor) {}

				[[nodiscard]] QSize sizeHint() const override
				{
					return {codeEditor->lineNumberAreaWidth(), 0};
				}

			protected:
				void paintEvent(QPaintEvent* event) override
				{
					codeEditor->lineNumberAreaPaintEvent(event);
				}

			private:
				CodeEditor* codeEditor;
		};
};

}
