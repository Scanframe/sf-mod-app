#pragma once

#include <QPlainTextEdit>
#include "MdiChild.h"
#include "Highlighter.h"

QT_BEGIN_NAMESPACE
class QPaintEvent;

class QResizeEvent;

class QSize;

class QWidget;

QT_END_NAMESPACE

class CodeEditor :public QPlainTextEdit, public MdiChild
{
	Q_OBJECT

	public:
		// Constructor.
		explicit CodeEditor(QWidget* parent = nullptr);

		// MdiChild interface method.
		void newFile() override;

		// MdiChild interface method.
		bool loadFile(const QString& fileName) override;

		// MdiChild interface method.
		bool save() override;

		// MdiChild interface method.
		bool saveAs() override;

		// MdiChild interface method.
		bool saveFile(const QString& fileName) override;

		// MdiChild interface method.
		QString userFriendlyCurrentFile() const override;

		// MdiChild interface method.
		QString currentFile() const override;

		// MdiChild interface method.
		bool hasSelection() const override;

		// MdiChild interface method.
		void cut() override;

		// MdiChild interface method.
		void copy() override;

		// MdiChild interface method.
		void paste() override;

		void lineNumberAreaPaintEvent(QPaintEvent* event);

		int lineNumberAreaWidth();

	protected:
		void resizeEvent(QResizeEvent* event) override;

	private slots:

		void updateLineNumberAreaWidth(int newBlockCount);

		void highlightCurrentLine();

		void updateLineNumberArea(const QRect& rect, int dy);

		void documentWasModified();

	private:
		void setCurrentFile(const QString& fileName);

		QString strippedName(const QString& fullFileName) const;

		QString curFile;
		bool isUntitled;
		QWidget* lineNumberArea;
		int spacingNumber;

		/**
		 * Internal class.
		 */
		class LineNumberArea :public QWidget
		{
			public:
				explicit LineNumberArea(CodeEditor* editor)
					:QWidget(editor), codeEditor(editor) {}

				QSize sizeHint() const override
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
