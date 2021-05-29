#pragma once

#include <QtUiPlugin/QDesignerExportWidget>
#include <QWidget>
#include <QPaintEvent>
/*#include "ObjectExtension.h"*/

namespace sf
{

class QDESIGNER_WIDGET_EXPORT DrawWidget :public QWidget/*, public ObjectExtension*/
{
	Q_OBJECT

	public:
		explicit DrawWidget(QWidget* parent = nullptr);

		void paintEvent(QPaintEvent* event) override;

	signals:
		void paint(QPaintEvent* event);

};

}
