#pragma once

#include "VariableWidgetBase.h"
#include "../global.h"

namespace sf
{

/**
 * Forward definition of private implemented class to prevent interfaces exposure.
 */
class VariableEditPrivate;

/**
 * @brief Widget for editing #sf::Variable value types integer, floating point and single line string.
 */
class QDESIGNER_WIDGET_EXPORT VariableEdit :public VariableWidgetBase
{
	Q_OBJECT
		Q_PROPERTY(bool focusFrame READ hasFocusFrame WRITE setFocusFrame)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit VariableEdit(QWidget* parent = nullptr);

		/**
		 * Sets the edit frame or no frame property.
		 * @param yn True when a focus frame is wanted.
		 */
		void setFocusFrame(bool yn);

		/**
		 * Gets the focus frame status.
		 * @return True when a focus frame is used.
		 */
		[[nodiscard]] bool hasFocusFrame() const;

	protected:
		/**
		 * @brief Overridden from base class '#ObjectExtension'.
		 */
		bool isRequiredProperty(const QString& name) override;

		/**
		 * @brief Overridden from base class '#VariableWidgetBase'.
		 */
		void applyReadOnly(bool yn) override;

	private:

		friend VariableEditPrivate;

		Q_DISABLE_COPY(VariableEdit)

};

}
