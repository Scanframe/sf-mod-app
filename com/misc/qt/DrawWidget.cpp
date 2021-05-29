
#include "DrawWidget.h"

namespace sf{

DrawWidget::DrawWidget(QWidget* parent)
:QWidget(parent)
{

}

void DrawWidget::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	emit paint(event);
}

}
