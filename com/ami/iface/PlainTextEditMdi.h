#pragma once

#include <QPlainTextEdit>
#include <ami/iface/MultiDocInterface.h>
#include "global.h"

class QPaintEvent;

class QResizeEvent;

class QSize;

class QWidget;

class QSettings;

namespace sf
{

class _AMI_CLASS PlainTextEditMdi :public MultiDocInterface
{
	public:
		/**
		 * @brief Default constructor.
		 */
		PlainTextEditMdi();

		/**
		 * @brief Initializing constructor.
		 */
		explicit PlainTextEditMdi(QPlainTextEdit* editor);

		/**
		 * @brief Sets the editor for the interface to work on to set the title.
		 *
		 * @param edit Edit Editor widget.
		 * @param container Root container widget of the editor. When null the editor is used.
		 */
		void setEditor(QPlainTextEdit* edit, QWidget* container = nullptr);

		// MultiDocInterface interface method.
		void newFile() override;

		// Can be overridden to change the suffix.
		virtual QString newFileName() const;

		// MultiDocInterface interface method.
		bool loadFile(const QString& filename) override;

		// MultiDocInterface interface method.
		bool save() override;

		// MultiDocInterface interface method.
		bool saveAs() override;

		// MultiDocInterface interface method.
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

	private:

		void documentWasModified();

		void setCurrentFile(const QString& fileName);

		[[nodiscard]] QString strippedName(const QString& fullFileName) const;

		QString curFile;

		bool isUntitled{true};

		QPlainTextEdit* _editor{nullptr};

		QWidget* _container{nullptr};
};

}
