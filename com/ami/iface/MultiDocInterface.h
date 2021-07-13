#pragma once

#include <QObject>
#include <QString>
#include <QWidget>
#include "global.h"

namespace sf
{

// Forward definition.
class AppModuleInterface;

/**
 * @brief Class to connect signals to the clients of a document.
 *
 * Since the class #sf::MultiDocInterface cannot inheriting QObject because of Qt not
 * allowing multiple inheritance of a QObject derived class.
 * So this class is an intermediate.
 */
class _AMI_CLASS MultiDocInterfaceSignals :public QObject
{
	Q_OBJECT

	/*
	 * Most signals from class QPlainTextEdit are implemented.
	 */
	Q_SIGNALS:
		void textChanged();
		void undoAvailable(bool b);
		void redoAvailable(bool b);
		void copyAvailable(bool b);
		void selectionChanged();
		void cursorPositionChanged();
		void modificationChanged(bool);
};

/**
 * @brief Interface for MDI document widgets handling files.
 */
class _AMI_CLASS MultiDocInterface
{
	public:
		/**
		 * @brief Constructor.
		 *
		 * When the passed document inherits QPlainTextEdit or QTextEdit the MDI signals are automatically connected.
		 * For other implementations you need to connect them manually.
		 * @param editor Widget representing the actual MDI child.
		 */
		explicit MultiDocInterface(QWidget* editor);

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual void newFile() = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual bool loadFile(const QString& fileName) = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual bool save() = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual bool saveAs() = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual bool saveFile(const QString& fileName) = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		[[nodiscard]] virtual QString userFriendlyCurrentFile() const = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		[[nodiscard]] virtual QString currentFile() const = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		[[nodiscard]] virtual bool isModified() const = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		[[nodiscard]] virtual bool hasSelection() const = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		[[nodiscard]] virtual bool isUndoRedoEnabled() const = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		[[nodiscard]] virtual bool isUndoAvailable() const = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		[[nodiscard]] virtual bool isRedoAvailable() const = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual void cut() = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual void copy() = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual void paste() = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual void undo() = 0;

		/**
		 * @brief Pure virtual needs to be implemented.
		 */
		virtual void redo() = 0;

		/**
		 * @brief When the document is modified this function shows a message box with 3 options to save or not or to cancel closing the document.
		 *
		 * It returns true when saved or not saved and false when cancelled.
		 * Can be overridden for other purposes.
		 *
		 * @return True when it can be closed.
		 */
		[[nodiscard]] virtual bool canClose() const;

		/**
		 * Gets the file type filters from module.
		 */
		[[nodiscard]] QString getFileTypeFilters() const;

		/**
		 * @brief Instance to connect signal handlers onto.
		 */
		MultiDocInterfaceSignals mdiSignals;

	private:
		/**
		 * @brief Holds the editor widget passed in the constructor.
		 */
		QWidget* _editor{nullptr};
		/**
		 * @brief Holds the editor widget passed in the constructor.
		 */
		const AppModuleInterface* _module{nullptr};

		friend AppModuleInterface;
};

}
