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

		/**
		 * @brief Signal to resize content.
		 */
		// ReSharper disable once CppFunctionIsNotImplemented
		Q_SIGNAL void resizing(QResizeEvent* event);

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
