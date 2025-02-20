#pragma once
#include <QDialog>
#include <QFile>
#include <QScrollArea>
#include <misc/global.h>

namespace sf
{

/**
 * @brief Creates a dialog from ui-type file.
 */
class _MISC_CLASS FormDialog : public QDialog
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructor
		 */
		explicit FormDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

		/**
		 * @brief Overridden to return the size of the contained UI file.
		 */
		[[nodiscard]] QSize sizeHint() const override;

		/**
		 * @brief Loads UI file in the form.
		 */
		void Load(QFile file);
		void Load2(QFile file);

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
