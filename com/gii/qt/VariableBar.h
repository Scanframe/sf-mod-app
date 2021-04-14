#pragma once

#include <QtUiPlugin/QDesignerExportWidget>
#include "VariableWidgetBase.h"

namespace sf
{

/**
 * Forward definition.
 */
class VariableBarPrivate;

/**
 * @brief Widget for showing #sf::Variable value types integer and floating point having a minimum and maximum value.
 */
class QDESIGNER_WIDGET_EXPORT VariableBar :public VariableWidgetBase
{
	Q_OBJECT

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

		friend VariableBarPrivate;

		Q_DISABLE_COPY(VariableBar)
};

}
