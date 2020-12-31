#ifndef _QFORMDIALOG_H
#define _QFORMDIALOG_H

#include <QDialog>

class QFormDialog :public QDialog
{
	public:
		/**
		 * Constructor
		 */
		explicit QFormDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

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

#endif // _QFORMDIALOG_H
