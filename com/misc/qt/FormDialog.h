#pragma once

#include <QDialog>
#include <QScrollArea>
#include <QFile>

#include "../global.h"

namespace sf
{

class _MISC_CLASS FormDialog :public QDialog
{
	Q_OBJECT

	public:
		/**
		 * Constructor
		 */
		explicit FormDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

		/**
		 * Overridden to return the size of the contained UI file.
		 */
		[[nodiscard]] QSize sizeHint() const override;

		/**
		 * Loads UI file in the form.
		 */
		void Load(QFile file);
		void Load2(QFile file);

	Q_SIGNALS:
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
