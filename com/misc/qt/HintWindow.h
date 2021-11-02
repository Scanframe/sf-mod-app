#pragma once

#include <QLabel>
#include "../global.h"

namespace sf
{
/**
 * @brief Floating window like tool tip available to the script engine as well.
 */
class _MISC_CLASS HintWindow :public QLabel
{
	public:
		/**
		 * @brief Constructor.
		 */
		explicit HintWindow(QWidget* parent);

		/**
		 * @brief Sets the global position to appear at.
		 */
		void setPosition(const QPoint& pt);

		/**
		 * @brief Sets the offset from the given position to appear at.
		 */
		void setOffset(const QPoint& pt);

		/**
		 * @brief Sets the offset from the given position to appear at.
		 */
		void setOffset(int x, int y);

		/**
		 * @brief Activates or deactivates the window (calls setVisible() for now).
		 */
		void setActive(bool active);

	protected:
		/**
		 * @brief Holds the offset for the position.
		 */
		QPoint _offset{0, 0};
};

}
