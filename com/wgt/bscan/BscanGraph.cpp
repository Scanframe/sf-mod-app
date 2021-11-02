#include <QStyleOptionFrame>
#include <QStylePainter>
#include <misc/qt/PropertySheetDialog.h>
#include <misc/qt/qt_utils.h>
#include <QGuiApplication>
#include "BscanGraph.h"
#include "BscanGraphPrivate.h"
#include "BscanPropertyPage.h"

namespace sf
{

SF_IMPL_INFO_ID(BscanGraph, IdData, _p->_rData)

SF_IMPL_INFO_ID(BscanGraph, IdIndex, _p->_rIndex)

SF_IMPL_INFO_ID(BscanGraph, IdTimeUnit, _p->_vTimeUnit)

SF_IMPL_INFO_ID(BscanGraph, IdTimeDelay, _p->_vTimeDelay)

SF_IMPL_INFO_ID(BscanGraph, IdTimeRange, _p->_vTimeRange)

SF_IMPL_INFO_ID(BscanGraph, IdCursorEvent, _p->_cursor.VEvent)

SF_IMPL_INFO_ID(BscanGraph, IdCursorFraction, _p->_cursor.VFraction)

SF_IMPL_INFO_ID(BscanGraph, IdCursorIndex, _p->_cursor.VIndex)

SF_IMPL_INFO_ID(BscanGraph, IdCursorPopIndex, _p->_cursor.VPopIndex)

SF_IMPL_INFO_ID(BscanGraph, IdScanDelay, _p->_scan.VDelay)

SF_IMPL_INFO_ID(BscanGraph, IdScanRange, _p->_scan.VRange)

SF_IMPL_INFO_ID(BscanGraph, IdScanLeft, _p->_scan.VLeft)

SF_IMPL_INFO_ID(BscanGraph, IdScanRight, _p->_scan.VRight)


BscanGraph::BscanGraph(QWidget* parent)
	:QWidget(parent)
	 , ObjectExtension(this)
	 , _p(new Private(this))
{
	_p->initialize();
}

BscanGraph::~BscanGraph()
{
	delete _p;
}

void BscanGraph::initStyleOption(QStyleOptionFrame* option) const
{
	if (!option)
	{
		return;
	}
	option->initFrom(this);
	option->rect = contentsRect();
	// Needed otherwise no focus rectangle is drawn.
	option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
	option->midLineWidth = 0;
	option->state |= QStyle::State_Sunken;
	option->features = QStyleOptionFrame::None;
}

void BscanGraph::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	QStyleOptionFrame panel;
	initStyleOption(&panel);
	// Create painter for this widget.
	QStylePainter sp(this);
	// Use base function to draw the focus rectangle having the right color as a regular edit widget.
	sp.drawPrimitive(QStyle::PE_PanelLineEdit, panel);
	//
	auto rc = sp.style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
	// Create 1 pixel spacing between border and
	inflate(rc, -_p->_margin);
	// Paint the graph canvas en returns the remaining plot area.
	auto prc = _p->_graph.paint(sp, rc, event->region());
	// Check if the region to update intersects with
	if (event->region().intersects(prc))
	{
		if (!_p->draw(sp, prc, event->region()))
		{
			_p->_graph.paintPlotCross(sp, tr("No Data"));
		}
	}
}

void BscanGraph::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
//  GenerateData(RData, FLastRange, true);
	// Make the bitmap resize.
	_p->_flagResized = true;
	// Make the control draw the plot.
	_p->_sustain.enable();
}

void BscanGraph::addPropertyPages(sf::PropertySheetDialog* sheet)
{
	sheet->addPage(new BscanPropertyPage(this, sheet));
}

bool BscanGraph::isRequiredProperty(const QString& name)
{
	return true;
}

QSize BscanGraph::minimumSizeHint() const
{
	return {200, 100};
}

void BscanGraph::mousePressEvent(QMouseEvent* event)
{
	// Set the capture flag.
	_p->_cursor.Captured = true;
	// Pass the event to the mouse handling routine.
	_p->localMouse(Private::MousePress, event->modifiers(), event->buttons(), event->pos(), true);
}

void BscanGraph::mouseReleaseEvent(QMouseEvent* event)
{
	// Reset the capture flag.
	_p->_cursor.Captured = false;
	// Pass the event to the cursor handling routine.
	_p->localMouse(Private::MouseRelease, event->modifiers(), event->buttons(), event->pos(), true);
}

void BscanGraph::mouseMoveEvent(QMouseEvent* event)
{
	if (_p->_cursor.Captured)
	{
		_p->localMouse(Private::MouseMove, event->modifiers(), event->buttons(), event->pos(), true);
	}
}

void BscanGraph::keyPressEvent(QKeyEvent* event)
{
	_p->localMouse(Private::KeyPress, event->modifiers(), QGuiApplication::mouseButtons(),{}, true);
}

void BscanGraph::keyReleaseEvent(QKeyEvent* event)
{
	_p->localMouse(Private::KeyRelease, event->modifiers(), QGuiApplication::mouseButtons(),{}, true);
}

}
