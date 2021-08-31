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

	private:

		Ui::HierarchyViewer* ui;

		QAction* _actionCollapseAll{nullptr};
		QAction* _actionExpandAll{nullptr};
		QAction* _actionEdit{nullptr};
		LayoutEditor* _layoutEditor{nullptr};

		void editObject();
};

}
