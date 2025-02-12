#pragma once
#include <gii/global.h>
#include <gii/qt/VariableWidgetBase.h>

namespace sf
{

/**
 * @brief Widget for editing #sf::Variable value types integer, floating point and single line string.
 */
class QDESIGNER_WIDGET_EXPORT VariableEdit : public VariableWidgetBase
{
		Q_OBJECT
		Q_PROPERTY(bool focusFrame READ hasFocusFrame WRITE setFocusFrame)
		Q_PROPERTY(int nameLevel READ nameLevel WRITE setNameLevel)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit VariableEdit(QWidget* parent = nullptr);

		/**
		 * @brief Sets the edit frame or no frame property.
		 * @param yn True when a focus frame is wanted.
		 */
		void setFocusFrame(bool yn);

		/**
		 * @brief Gets the focus frame status.
		 * @return True when a focus frame is used.
		 */
		[[nodiscard]] bool hasFocusFrame() const;

		/**
		 * @brief Gets the name level shown.
		 */
		[[nodiscard]] int nameLevel() const;
		/**
		 * @brief Sets the name level shown.
		 */
		void setNameLevel(int level = -1);

	protected:
		/**
		 * @brief Overridden from base class '#sf::ObjectExtension'.
		 */
		bool isRequiredProperty(const QString& name) override;

		/**
		 * @brief Overridden from base class '#sf::VariableWidgetBase'.
		 */
		void applyReadOnly(bool yn) override;

	private:
		/**
	 * @brief Forward definition of private implemented class to prevent interfaces exposure.
	 */
		struct Private;

		Q_DISABLE_COPY(VariableEdit)
};

}// namespace sf
