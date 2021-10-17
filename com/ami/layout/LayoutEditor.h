#pragma once

#include <QWidget>
#include <QMenu>
#include <QScrollArea>
#include <QSettings>
#include <gii/qt/LayoutWidget.h>
#include <ami/iface/MultiDocInterface.h>
#include "ObjectHierarchyModel.h"

namespace sf
{

class LayoutEditor :public LayoutWidget, public MultiDocInterface
{
	Q_OBJECT

	public:
		explicit LayoutEditor(QSettings* settings, QWidget* parent);

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

		bool eventFilter(QObject* watched, QEvent* event) override;

		void popupContextMenu(QObject* target, const QPoint& pos) override;

		void openPropertyEditor(QObject* target) override;

		ObjectHierarchyModel* getHierarchyModel();

	public Q_SLOTS:

		/**
		 * @brief Send MDI signals on modification of the document.
		 */
		void documentWasModified();

		/**
		 * @brief Sets the modified flag and calls #documentWasModified().
		 */
		void documentModified();

		QDir getDirectory() const override;

	Q_SIGNALS:
		void objectSelected(QObject* obj);

	protected:
		void closeEvent(QCloseEvent* event) override;

		void resizeEvent(QResizeEvent* event) override;

	private:
		void setCurrentFile(const QString& fileName);

		[[nodiscard]] QString strippedName(const QString& fullFileName) const;

		QSettings* _settings;
		QString _curFile;
		bool _isUntitled;
		QScrollArea* _scrollArea{nullptr};
		QWidget* _widget{nullptr};
		QString _rootName;
		bool _modified{false};
		QObject* _currentTarget{nullptr};
		QMenu* _targetContextMenu{nullptr};
		ObjectHierarchyModel* _model{nullptr};
		QAction* _actionEdit{nullptr};
};

}
