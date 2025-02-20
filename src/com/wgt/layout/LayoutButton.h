#pragma once

#include <QAction>
#include <QToolButton>
#include <QtUiPlugin/QDesignerExportWidget>
#include <gii/qt/Namespace.h>
#include <misc/qt/Macros.h>
#include <misc/qt/ObjectExtension.h>

namespace sf
{

class QDESIGNER_WIDGET_EXPORT LayoutButton
	: public QToolButton
	, public ObjectExtension
{
		Q_OBJECT
		Q_PROPERTY(QString layoutFile READ getLayoutFile WRITE setLayoutFile)
		Q_PROPERTY(gii::IdType idOffset READ getIdOffset WRITE setIdOffset)

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
		/**
		 * @brief Opens the specified layout file in a form dialog as a popup window.
		 */
		void openLayout();

		SF_DECL_PROP_GS(gii::IdType, IdOffset)

		SF_DECL_PROP_GRS(QString, LayoutFile)

	private:
		/**
		 * @brief Holds the shielded private data.
		 */
		struct Private;
		Private* _p{nullptr};
};

}// namespace sf
