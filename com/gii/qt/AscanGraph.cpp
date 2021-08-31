
#include <misc/qt/Draw.h>
#include <misc/qt/Graph.h>
#include <QStyleOptionFrame>
#include <QStylePainter>
#include <misc/qt/qt_utils.h>
#include "AscanGraph.h"

namespace sf{


struct AscanGraph::Private
{
	explicit Private(AscanGraph* widget)
	:_w(widget)
	{}

	AscanGraph* _w;

};

AscanGraph::AscanGraph(QWidget* parent)
:QWidget(parent)
 , ObjectExtension(this)
 ,_p(new Private(this))
{
}

void AscanGraph::paintEvent(QPaintEvent* event)
{
	QStyleOptionFrame so;
	so.initFrom(this);
	// Needed otherwise no focus rectangle is drawn.
	so.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &so);
	// Create painter for this widget.
	QStylePainter p(this);
	//
	//p.setRenderHint(QPainter::Antialiasing);
	// Use base function to draw the focus rectangle having the right color as a regular edit widget.
	p.drawPrimitive(QStyle::PE_PanelLineEdit, so);
	auto rc = p.style()->subElementRect(QStyle::SE_LineEditContents, &so, this);
	// Create additional spacing.
	inflate(rc, -so.lineWidth);
	// Create instance to draw the graph.
	Graph graph(palette());
	//
	graph.setRuler(sf::Draw::roLeft, 0, 10, 2, "V");
	graph.setRuler(sf::Draw::roBottom, 0, 100, 4, "s");
	//
	graph.setGrid(sf::Draw::goHorizontal, sf::Draw::roLeft);
	graph.setGrid(sf::Draw::goVertical, sf::Draw::roBottom);
	//
	//QPainter painter(this);
	graph.paint(p, rc, event->region());
}

bool AscanGraph::isRequiredProperty(const QString& name)
{
	return true;
}

QSize AscanGraph::minimumSizeHint() const
{
	return {200, 100};
}

}
