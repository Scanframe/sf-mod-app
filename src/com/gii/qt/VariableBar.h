#pragma once

#include <QtUiPlugin/QDesignerExportWidget>
#include "VariableWidgetBase.h"

namespace sf
{

/**
 * @brief Widget for showing #sf::Variable value types integer and floating point having a minimum and maximum value.
 */
class QDESIGNER_WIDGET_EXPORT VariableBar :public VariableWidgetBase
{
	Q_OBJECT
		Q_PROPERTY(int nameLevel READ nameLevel WRITE setNameLevel)

	public:
		/**
		 * @brief Constructor.
		 */
		explicit VariableBar(QWidget* parent = nullptr);

		/**
		 * @brief Overridden from base class.
		 */
		void keyPressEvent(QKeyEvent* event) override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QSize minimumSizeHint() const override;

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

		/**
		 * @brief Overridden from base class.
		 */
		void paintEvent(QPaintEvent* event) override;

	private:

		void initStyleOption(QStyleOptionFrame *option) const;

		struct Private;

		Q_DISABLE_COPY(VariableBar)
};

}
