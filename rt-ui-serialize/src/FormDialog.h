#pragma once

#include <QDialog>

class FormDialog :public QDialog
{
	public:
		/**
		 * Constructor
		 */
		explicit FormDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

		/**
		 * Overridden to return the size of the contained UI file.
		 */
		QSize sizeHint() const override;

	Q_OBJECT

	signals:

		/**
		 * Signal to resize content.
		 */
		void resizing(QResizeEvent* event);

	protected:
		// Overridden to emit resizing signal.
		void resizeEvent(QResizeEvent* event) override;

};
