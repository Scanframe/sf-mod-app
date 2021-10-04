#pragma once

#include <QtUiPlugin/QDesignerExportWidget>
#include <QToolButton>
#include <QAction>
#include "ObjectExtension.h"

namespace sf
{

class QDESIGNER_WIDGET_EXPORT LayoutButton :public QToolButton, public ObjectExtension
{
	Q_OBJECT
		Q_PROPERTY(QString layoutFile MEMBER _layoutFile)

	public:
		/**
		 * @brief Constructor.
		 * @param parent
		 */
		explicit LayoutButton(QWidget* parent = nullptr);

		/**
		 * @brief overridden from base class #ObjectExtension.
		 */
		bool isRequiredProperty(const QString& name) override;

		/**
		 * @brief Adds specific property page for this widget.
		 */
		void addPropertyPages(PropertySheetDialog* sheet) override;

	protected:
		void openLayout();

	private:
		/**
		 * @brief Holds the relative file to the layout ui-file.
		 */
		QString _layoutFile;

		/**
		 * @brief Holds the widget containing layout.
		 */
		QWidget* _layoutContainer;
};

}
