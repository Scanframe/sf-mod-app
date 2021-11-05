#include "Graph.h"
#include "qt_utils.h"

namespace sf
{

Graph::Graph(const QPalette& palette)
	:_colors({
//	cRulerText,
	palette.color(QPalette::ColorRole::WindowText),
//	cRulerLine,
	palette.color(QPalette::ColorRole::Dark),
//	cGrid,
	palette.color(QPalette::ColorRole::AlternateBase),
//	cRulerBackground,
	palette.color(QPalette::ColorRole::Window),
//	cGraphBackground,
	palette.color(QPalette::ColorRole::Base),
//	cGraphForeground,
	palette.color(QPalette::ColorRole::Text)
})
{
}

Graph::RulerInfo* Graph::getRulerInfo(Draw::ERulerOrientation ro)
{
	RulerInfo* rv = nullptr;
	if (ro & Draw::roLeft)
	{
		rv = &_left;
	}
	else if (ro & Draw::roRight)
	{
		rv = &_right;
	}
	else if (ro & Draw::roTop)
	{
		rv = &_top;
	}
	else if (ro & Draw::roBottom)
	{
		rv = &_bottom;
	}
	return rv;
}

void Graph::setColor(Graph::EColor index, QColor color)
{
	_colors[index] = color;
}

void Graph::setRuler(Draw::ERulerOrientation ro, double start, double stop, int digits, const QString& unit)
{
	auto ri = getRulerInfo(ro);
	// Setting information enables the ruler too when the amount of digits is non-zero.
	ri->enabled = digits > 0;
	ri->start = start;
	ri->stop = stop;
	// Do not allow
	ri->digits = clip(digits, 1, 10);
	ri->unit = unit;
}

void Graph::setBounds(const QFontMetrics& fm, const QRect& bounds)
{
	// Font relative ruler sizes.
	_left.size = fm.averageCharWidth() * 10 + _left.digits;
	_right.size = fm.averageCharWidth() * 10 + _right.digits;
	_top.size = _bottom.size = fm.height() * 3 * 6 / 7;
	// Calculate the graph area.
	_plotArea = bounds.adjusted(
		_left.enabled ? _left.size : 0,
		_top.enabled ? _top.size : 0,
		_right.enabled ? -_right.size : 0,
		_bottom.enabled ? -_bottom.size : 0);
}

const QRect& Graph::paint(QPainter& painter, const QRect& bounds, const QRegion& region)
{
	Draw draw;
	// Get font sizes to calculate needed widths and heights.
	setBounds(QFontMetrics(painter.font()), bounds);
	// When the background is a valid color paint int.
	if (_colors[cGraphBackground].isValid())
	{
		painter.fillRect(_plotArea, _colors[cGraphBackground]);
	}
	// When there is a left ruler.
	if (_left.enabled)
	{
		_left.rect = bounds;
		_left.rect.setWidth(_left.size);
		_left.bounds = _left.rect;
		// The area allowed painting in.
		auto area = _left.bounds;
		// Draw background of ruler area.
		if (_colors[cRulerBackground].isValid() && !_debug)
		{
			painter.fillRect(_left.rect, _colors[cRulerBackground]);
		}
		// Correction when having a top ruler.
		if (_top.enabled)
		{
			_left.rect.setY(_top.size + bounds.y());
		}
		// Correction when having a bottom ruler.
		if (_bottom.enabled)
		{
			_left.rect.setBottom(_left.rect.bottom() - _bottom.size);
		}
		// Draw the actual ruler elements.
		if (!_debug)
		{
			draw.ruler(painter, Draw::roLeft, _colors[cRulerLine], _colors[cRulerText],
				_left.rect, area, _left.start, _left.stop, _left.digits, _left.unit);
		}
	}
	// When there is a right ruler.
	if (_right.enabled)
	{
		_right.rect = bounds;
		_right.rect.setWidth(_right.size);
		_right.rect.moveTo(QPoint(bounds.width() - _right.size, 0) + bounds.topLeft());
		_right.bounds = _right.rect;
		// The area allowed painting in.
		auto area = _right.bounds;
		// Draw background of ruler area.
		if (_colors[cRulerBackground].isValid() && !_debug)
		{
			painter.fillRect(_right.rect, _colors[cRulerBackground]);
		}
		// Correction when having a top ruler.
		if (_top.enabled)
		{
			_right.rect.setY(_top.size + bounds.y());
		}
		// Correction when having a bottom ruler.
		if (_bottom.enabled)
		{
			_right.rect.setBottom(_right.rect.bottom() - _bottom.size);
		}
		// Draw the actual ruler elements.
		if (!_debug)
		{
			draw.ruler(painter, Draw::roRight, _colors[cRulerLine], _colors[cRulerText],
				_right.rect, area, _right.start, _right.stop, _right.digits, _right.unit);
		}
	}
	// When there is a top ruler.
	if (_top.enabled)
	{
		_top.rect = bounds;
		_top.rect.setHeight(_top.size);
		_top.bounds = _top.rect;
		// The area allowed painting in.
		auto area = _top.bounds;
		// Correction when having a left ruler.
		if (_left.enabled)
		{
			_top.rect.setX(_left.size + bounds.x());
		}
		// Correction when having a right ruler.
		if (_right.enabled)
		{
			_top.rect.setRight(_top.rect.right() - _right.size);
		}
		// Draw background of area.
		if (_colors[cRulerBackground].isValid() && !_debug)
		{
			painter.fillRect(_top.rect, _colors[cRulerBackground]);
		}
		// Draw the actual ruler elements.
		if (!_debug)
		{
			draw.ruler(painter, Draw::roTop, _colors[cRulerLine], _colors[cRulerText],
				_top.rect, area, _top.start, _top.stop, _top.digits, _top.unit);
		}
	}
	// When there is a bottom ruler.
	if (_bottom.enabled)
	{
		_bottom.rect = bounds;
		_bottom.rect.moveTo(QPoint(0, _bottom.rect.height() - _bottom.size) + bounds.topLeft());
		_bottom.rect.setHeight(_bottom.size);
		_bottom.bounds = _bottom.rect;
		// The area allowed painting in.
		auto area = _bottom.bounds;
		// Correction when having a left ruler.
		if (_left.enabled)
		{
			_bottom.rect.setX(_left.size + bounds.x());
		}
		// Correction when having a right ruler.
		if (_right.enabled)
		{
			_bottom.rect.setRight(_bottom.rect.right() - _right.size);
		}
		// Draw background of area.
		if (_colors[cRulerBackground].isValid() && !_debug)
		{
			painter.fillRect(_bottom.rect, _colors[cRulerBackground]);
		}
		// Draw the actual ruler elements.
		if (!_debug)
		{
			draw.ruler(painter, Draw::roBottom, _colors[cRulerLine], _colors[cRulerText],
				_bottom.rect, area, _bottom.start, _bottom.stop, _bottom.digits, _bottom.unit);
		}
	}
	// Check if horizontal grid is enabled.
	if (_horizontal && !_debug)
	{
		auto ri = getRulerInfo(_horizontal);
		draw.gridLines(painter, Draw::goHorizontal, _colors[cGridLines], _plotArea, ri->start, ri->stop, ri->digits);
	}
	// Check if vertical grid is enabled.
	if (_vertical && !_debug)
	{
		auto ri = getRulerInfo(_vertical);
		draw.gridLines(painter, Draw::goVertical, _colors[cGridLines], _plotArea, ri->start, ri->stop, ri->digits);
	}
	if (_debug)
	{
		for (auto& i: {_left, _right, _top, _bottom})
		{
			if (i.enabled)
			{
				painter.setPen(Qt::blue);
				painter.drawRect(inflated(i.rect, -1));
				painter.setPen(QPen(Qt::red, 1, Qt::PenStyle::DashDotLine));
				painter.drawRect(i.bounds);
			}
		}
		painter.setPen(Qt::green);
		painter.drawRect(_plotArea);
		painter.setPen(Qt::darkGray);
		painter.drawRect(inflated(bounds, 1));
	}
	return _plotArea;
}

void Graph::paintPlotCross(QPainter& painter, const QString& text)
{
	Draw().textCross(painter, _plotArea, text, _colors[cGraphForeground]);
}

void Graph::setGrid(Draw::EGridOrientation go, Draw::ERulerOrientation ro)
{
	if (go == Draw::goHorizontal)
	{
		_horizontal = ro;
	}
	else if (go == Draw::goVertical)
	{
		_vertical = ro;
	}
}

const QRect& Graph::getPlotArea() const
{
	return _plotArea;
}

}