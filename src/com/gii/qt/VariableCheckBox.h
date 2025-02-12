#pragma once
#include <gii/global.h>
#include <gii/qt/VariableWidgetBase.h>

namespace sf
{

/**
 * @brief Widget for editing #sf::Variable value types integer, floating point and single line string.
 */
class QDESIGNER_WIDGET_EXPORT VariableCheckBox : public VariableWidgetBase
{
		Q_OBJECT
		Q_PROPERTY(int nameLevel READ nameLevel WRITE setNameLevel)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit VariableCheckBox(QWidget* parent = nullptr);

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

		Q_DISABLE_COPY(VariableCheckBox)
};

}// namespace sf
