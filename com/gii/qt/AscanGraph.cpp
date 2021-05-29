
#include "AscanGraph.h"

namespace sf{

AscanGraph::AscanGraph(QWidget* parent)
:QWidget(parent)
{

}

void AscanGraph::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	emit paint(event);
}

}
