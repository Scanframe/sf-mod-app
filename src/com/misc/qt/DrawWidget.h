#pragma once
#include <QPaintEvent>
#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>

namespace sf
{

/**
 * @brief Simple widget which redirects the painting event to a Qt signal method.
 */
class QDESIGNER_WIDGET_EXPORT DrawWidget final : public QWidget /*, public ObjectExtension*/
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructor passing owner/parent.
		 * @param parent Parent widget.
		 */
		explicit DrawWidget(QWidget* parent = nullptr);

		/**
		 * @brief Emits the #paint signal when called.
		 * @param event
		 */
		void paintEvent(QPaintEvent* event) override;

		/**
		 * @brief Signal for painting.
		 */
		// ReSharper disable once CppFunctionIsNotImplemented
		Q_SIGNAL void paint(QPaintEvent* event);
};

}// namespace sf
