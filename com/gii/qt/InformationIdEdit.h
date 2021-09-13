#pragma once
#include <QtUiPlugin/QDesignerExportWidget>
#include <QLineEdit>
#include <misc/qt/ObjectExtension.h>
#include <gii/gen/InformationBase.h>
#include <gii/qt/InformationSelectDialog.h>
#include "Namespace.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Line edit having a button
 */
class QDESIGNER_WIDGET_EXPORT InformationIdEdit :public QLineEdit, public ObjectExtension
{
	public:
		// Declarations in these macros are made private.
	Q_OBJECT
		// Need full namespace for this property macro.
		Q_PROPERTY(sf::Gii::TypeId typeId READ getTypeId WRITE setTypeId);

	public:
		/**
		 * Standard QWidget constructor.
		 */
		explicit InformationIdEdit(QWidget* parent = nullptr);

		/**
		 * @brief Sets the Id in the line edit.
		 */
		void setId(Gii::IdType id);

		/**
		 * @brief Gets the Id from the text in the line edit.
		 */
		Gii::IdType getId();

		/**
		 * @brief Opens the Id selection dialog.
		 *
		 * @param  parent Optional, when nullptr the tool-button is the parent.
		 * @return True when accepted.
		 */
		bool selectDialog(QWidget* parent = nullptr);

		/**
		 * @brief Overridden from base class  #sf::ObjectExtension
		 */
		bool isRequiredProperty(const QString& name) override {return true;};

		/**
		 * @brief Set the type of id set with #setId().
		 */
		void setTypeId(Gii::TypeId typeId);

		/**
		 * @brief Gets the type of id set with #setId().
		 */
		[[nodiscard]] Gii::TypeId getTypeId() const;

	protected:
		/**
		 * @brief overridden from base class.
		 */
		void focusOutEvent(QFocusEvent* event) override;

	protected:
		/**
		 * @brief Overridden from base class QLineEdit
		 */
		void resizeEvent(QResizeEvent*) override;

	private:
		/**
		 * @brief Holds the tool button.
		 */
		QToolButton* btnOpenDialog;
		/**
		 * @brief Holds the type of the Id.
		 */
		Gii::TypeId _typeId;
};

}
