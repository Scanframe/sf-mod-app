#pragma once

#include <QtUiPlugin/QDesignerExportWidget>
#include <QWidget>
#include <QPaintEvent>
/*#include "ObjectExtension.h"*/

namespace sf
{

class QDESIGNER_WIDGET_EXPORT AscanGraph :public QWidget/*, public ObjectExtension*/
{
	Q_OBJECT

	public:
		explicit AscanGraph(QWidget* parent = nullptr);

		void paintEvent(QPaintEvent* event) override;

	signals:
		void paint(QPaintEvent* event);

};

}
