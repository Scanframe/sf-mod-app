#pragma once
#include <QSortFilterProxyModel>
#include <QWidget>
#include <ami/layout/LayoutEditor.h>
#include <ami/layout/ObjectHierarchyModel.h>

namespace sf
{

namespace Ui
{
class HierarchyViewer;
}

class HierarchyViewer : public QWidget
{
		Q_OBJECT

	public:
		explicit HierarchyViewer(QWidget* parent = nullptr);

		~HierarchyViewer() override;

		void setEditor(sf::LayoutEditor* editor);

		void selectObject(QObject* obj);

		/**
		 * @brief When a layout editor is present it will call its documentModified().
		 */
		void documentModified();

	Q_SIGNALS:
		/**
		 * @brief Signal emitted when the selected object has changed.
		 */
		void objectSelectChange(QObject* obj);

	private:
		void editObject();

		void addObject();

		void removeObject();

		void editorDisconnect(QObject* obj);

		/**
		 * @brief Gets the object selected.
		 * @param index
		 * @return Not NULL when an object is selected.
		 */
		QObject* objectSelected(const QModelIndex& index = {});

		Ui::HierarchyViewer* ui;

		QSortFilterProxyModel* _proxyModel{nullptr};

		QAction* _actionCollapseAll{nullptr};
		QAction* _actionExpandAll{nullptr};
		QAction* _actionEdit{nullptr};
		QAction* _actionAdd{nullptr};
		QAction* _actionRemove{nullptr};
		LayoutEditor* _layoutEditor{nullptr};
};

}// namespace sf
