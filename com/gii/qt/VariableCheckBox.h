#pragma once

#include "VariableWidgetBase.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Widget for editing #sf::Variable value types integer, floating point and single line string.
 */
class QDESIGNER_WIDGET_EXPORT VariableCheckBox :public VariableWidgetBase
{
	Q_OBJECT
		Q_PROPERTY(int nameLevel READ nameLevel WRITE setNameLevel)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit VariableCheckBox(QWidget* parent = nullptr);

		/**
		 * Gets the name level shown.
		 */
		[[nodiscard]] int nameLevel() const;
		/**
		 * Sets the name level shown.
		 */
		void setNameLevel(int level = -1);

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

	/**
	 * @brief Forward definition of private implemented class to prevent interfaces exposure.
	 */
		struct Private;

		Q_DISABLE_COPY(VariableCheckBox)

};

}
