#pragma once

#include <QDialog>
#include <QScrollArea>
#include <QFile>
#include "../global.h"

namespace sf
{

class _GII_CLASS FormDialog :public QDialog
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

		/**
		 * Loads UI file in the form.
		 */
		void Load(QFile file);
		void Load2(QFile file);

	Q_OBJECT

	signals:

		/**
		 * Signal to resize content.
		 */
		void resizing(QResizeEvent* event);

	protected:
		// Overridden to emit resizing signal.
		void resizeEvent(QResizeEvent* event) override;

	private:
		QScrollArea* _scrollArea;

		QWidget* _widget{nullptr};

		void connectChildren();
};

}
