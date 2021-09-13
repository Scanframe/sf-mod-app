#pragma once

#include <QWidget>
#include "ObjectHierarchyModel.h"
#include "LayoutEditor.h"

namespace sf
{

namespace Ui {class HierarchyViewer;}

class HierarchyViewer :public QWidget
{
	Q_OBJECT

	public:
		explicit HierarchyViewer(QWidget* parent = nullptr);

		~HierarchyViewer() override;

		void setEditor(sf::LayoutEditor* editor);

		void selectObject(QObject* obj);

		/**
		 * When an layout editor is present it will call its documentModified().
		 */
		void documentModified();

	Q_SIGNALS:
		/**
		 * @brief Signal emitted when a the selected object has changed.
		 */
		void objectSelectChange(QObject* obj);

	private:

		void editObject();

		void addObject();

		void removeObject();

		void editorDisconnect(QObject* obj);

		/**
		 * @brief Returns the object selected.
		 * @param index
		 * @return Not null when a object is selected.
		 */
		QObject* objectSelected(const QModelIndex &index = {});

		Ui::HierarchyViewer* ui;

		QAction* _actionCollapseAll{nullptr};
		QAction* _actionExpandAll{nullptr};
		QAction* _actionEdit{nullptr};
		QAction* _actionAdd{nullptr};
		QAction* _actionRemove{nullptr};
		LayoutEditor* _layoutEditor{nullptr};
};

}
