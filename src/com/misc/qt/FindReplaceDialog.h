#pragma once

#include <QDialog>
#include <QTextCursor>

class QPlainTextEdit;

class QCheckBox;

class QDialogButtonBox;

class QGroupBox;

class QLabel;

class QLineEdit;

class QPushButton;

namespace sf
{

/**
 * @brief Find and replace dialog for a QPlainTextEdit widget.
 */
class FindReplaceDialog :public QDialog
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit FindReplaceDialog(QWidget* parent = nullptr);

		/**
		 * @brief Destructor.
		 */
		~FindReplaceDialog() override;

		/**
		 * @brief Sets the editor widget to act on.
		 */
		void setEditor(QPlainTextEdit* editor)
		{
			_editor = editor;
		}

		/**
		 * @brief Gets the editor widget it acts on.
		 */
		QPlainTextEdit* getEditor()
		{
			return _editor;
		}

	protected:
		void showEvent(QShowEvent* event) override;

	private Q_SLOTS:

		bool find();

		void replace();

		void replaceAll();

		void regexMode();

	private:

		QPlainTextEdit* _editor{nullptr};

		QLabel* lblFind;
		QLabel* lblReplace;
		QLineEdit* leFind;
		QCheckBox* cbRegex;
		QLineEdit* leReplaceField;
		QCheckBox* cbCase;
		QCheckBox* cbWholeWords;
		QCheckBox* cbSearchSelection;
		QCheckBox* cbBackward;
		QDialogButtonBox* buttonBox;
		QPushButton* btnFind;
		QPushButton* btnReplace;
		QPushButton* btnReplaceAll;
		QPushButton* btnShowMore;
		QWidget* extension;

		QTextCursor cursor;

		void resizeToContent();
};

}
