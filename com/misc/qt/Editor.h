#pragma once

#include <QPlainTextEdit>
#include "ObjectExtension.h"
#include "../global.h"

class QPaintEvent;

class QResizeEvent;

class QSize;

class QWidget;

class QSettings;

namespace sf
{

class _MISC_CLASS Editor :public QPlainTextEdit, public ObjectExtension
{
	Q_OBJECT

	public:
		class _MISC_CLASS Configuration
		{
			public:
				explicit Configuration(QSettings* settings = nullptr);

				void settingsReadWrite(bool save);

				QString fontType{"Monospace"};
				int fontSize{10};
				bool showGutter{true};
				bool darkMode{false};
				bool useHighLighting{true};

			private:
				QSettings* _settings;
		};

		// Constructor.
		explicit Editor(QWidget* parent = nullptr);

		void setConfiguration(const Configuration& cfg);

		void lineNumberAreaPaintEvent(QPaintEvent* event);

		int lineNumberAreaWidth();

		/**
		 * @brief Called on a close event. To prevent losing changes made to the document.
		 *
		 * @return True when it can be closed.
		 */
		virtual bool canClose();

		/**
		 * Overridden from base class ObjectExtension.
		 */
		bool isRequiredProperty(const QString& name) override;

	protected:
		/**
		 * Overridden from base class QPlainTextEdit.
		 */
		void resizeEvent(QResizeEvent* event) override;

		/**
		 * Overridden from base class QPlainTextEdit.
		 */
		void closeEvent(QCloseEvent* event) override;

	private:
		void updateLineNumberAreaWidth(int newBlockCount);

		void highlightCurrentLine();

		void updateLineNumberArea(const QRect& rect, int dy);

		// Forward definition.
		class LineNumberArea;

		QString curFile;
		bool isUntitled;
		QWidget* lineNumberArea;
		int spacingNumber;
		// Color for background of current line.
		QColor curLineColor;
};

}
