#pragma once

#include <QDialog>

class FormDialog :public QDialog
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructor
		 */
		explicit FormDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

		/**
		 * Overridden to return the size of the contained UI file.
		 */
		[[nodiscard]] QSize sizeHint() const override;

	Q_SIGNALS:

		/**
		 * @brief Signal to resize content.
		 */
		void resizing(QResizeEvent* event);

	protected:
		/**
		 * @brief Overridden to emit resizing signal.
		 */

		void resizeEvent(QResizeEvent* event) override;

};
