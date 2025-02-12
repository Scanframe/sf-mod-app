#pragma once
#include <QDialog>
#include <QFile>
#include <QScrollArea>

namespace sf
{

class AxesCoordDialog : public QDialog
{
		Q_OBJECT

	public:
		/**
		 * Constructor
		 */
		explicit AxesCoordDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

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

	private:
		QScrollArea* _scrollArea;

		QWidget* _widget{nullptr};

		void connectChildren();
};

}// namespace sf
