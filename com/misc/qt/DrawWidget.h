#pragma once

#include <QtUiPlugin/QDesignerExportWidget>
#include <QWidget>
#include <QPaintEvent>

namespace sf
{

class QDESIGNER_WIDGET_EXPORT DrawWidget :public QWidget/*, public ObjectExtension*/
{
	Q_OBJECT

	public:
		explicit DrawWidget(QWidget* parent = nullptr);

		void paintEvent(QPaintEvent* event) override;

	Q_SIGNALS:
		void paint(QPaintEvent* event);

};

}
