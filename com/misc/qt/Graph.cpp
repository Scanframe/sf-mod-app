#include "Graph.h"

namespace sf
{

Graph::Graph()
	:_colors({QColorConstants::White, QColorConstants::Yellow, QColorConstants::LightGray, QColorConstants::DarkBlue,  QColorConstants::DarkGray})
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
	// Setting information enables the ruler too when the amount of digits is non zero.
	ri->enabled = digits > 0;
	ri->start = start;
	ri->stop = stop;
	ri->digits = digits;
	ri->unit = unit;
}

void Graph::paint(QPainter& painter, const QRect& bounds, const QRegion& region)
{
	Draw draw;
	// Get font sizes to calculate needed widths and heights.
	QFontMetrics fm(painter.font());
	// Font relative ruler sizes.
	_left.size = fm.averageCharWidth() * 10 + _left.digits;
	_right.size = fm.averageCharWidth() * 10 + _right.digits;
	_top.size = _bottom.size = fm.height() * 3 * 6 / 7;
	// Calculate the graph area.
	_graphArea = bounds.adjusted(
		_left.enabled ? _left.size : 0,
		_top.enabled ? _top.size : 0,
		_right.enabled ? -_right.size : 0,
		_bottom.enabled ? -_bottom.size : 0);
/*
	painter.setPen(QPen(Qt::cyan));
	painter.drawRect(QRect(graph_area.topLeft(), graph_area.size() - QSize(1, 1)));
*/
	// When the background is a valid color paint int.
	if (_colors[cGraphBackground].isValid())
	{
		painter.fillRect(_graphArea, _colors[cGraphBackground]);
	}
	// When there is a left ruler.
	if (_left.enabled)
	{
		_left.rect = bounds;
		_left.rect.setWidth(_left.size);
		_left.bounds = _left.rect;
		// The area allowed to paint in.
		auto area = _left.bounds;
		// Draw background of ruler area.
		if (_colors[cRulerBackground].isValid())
		{
			painter.fillRect(_left.rect, _colors[cRulerBackground]);
		}
		// Correction when having a top ruler.
		if (_top.enabled)
		{
			_left.rect.setY(_top.size);
		}
		// Correction when having a bottom ruler.
		if (_bottom.enabled)
		{
			_left.rect.setBottom(_left.rect.bottom() - _bottom.size);
		}
		// Draw the actual ruler elements.
		draw.Ruler(painter, Draw::roLeft, _colors[cLine], _colors[cText],
			_left.rect, area, _left.start, _left.stop, _left.digits, _left.unit);
	}
	// When there is a right ruler.
	if (_right.enabled)
	{
		_right.rect = bounds;
		_right.rect.setWidth(_right.size);
		_right.rect.moveTo(QPoint(bounds.width() - _right.size, 0));
		_right.bounds = _right.rect;
		// The area allowed to paint in.
		auto area = _right.bounds;
		// Draw background of ruler area.
		if (_colors[cRulerBackground].isValid())
		{
			painter.fillRect(_right.rect, _colors[cRulerBackground]);
		}
		// Correction when having a top ruler.
		if (_top.enabled)
		{
			_right.rect.setY(_top.size);
		}
		// Correction when having a bottom ruler.
		if (_bottom.enabled)
		{
			_right.rect.setBottom(_right.rect.bottom() - _bottom.size);
		}
		// Draw the actual ruler elements.
		draw.Ruler(painter, Draw::roRight, _colors[cLine], _colors[cText],
			_right.rect, area, _right.start, _right.stop, _right.digits, _right.unit);
	}
	// When there is a top ruler.
	if (_top.enabled)
	{
		_top.rect = bounds;
		_top.rect.setHeight(_top.size);
		_top.bounds = _top.rect;
		// The area allowed to paint in.
		auto area = _top.bounds;
		// Correction when having a left ruler.
		if (_left.enabled)
		{
			_top.rect.setX(_left.size);
		}
		// Correction when having a right ruler.
		if (_right.enabled)
		{
			_top.rect.setRight(_top.rect.right() - _right.size);
		}
		// Draw background of area.
		if (_colors[cRulerBackground].isValid())
		{
			painter.fillRect(_top.rect, _colors[cRulerBackground]);
		}
		// Draw the actual ruler elements.
		draw.Ruler(painter, Draw::roTop, _colors[cLine], _colors[cText],
			_top.rect, area, _top.start, _top.stop, _top.digits, _top.unit);
	}
	// When there is a bottom ruler.
	if (_bottom.enabled)
	{
		_bottom.rect = bounds;
		_bottom.rect.moveTo(0, _bottom.rect.height() - _bottom.size);
		_bottom.rect.setHeight(_bottom.size);
		_bottom.bounds = _bottom.rect;
		// The area allowed to paint in.
		auto area = _bottom.bounds;
		// Correction when having a left ruler.
		if (_left.enabled)
		{
			_bottom.rect.setX(_left.size);
		}
		// Correction when having a right ruler.
		if (_right.enabled)
		{
			_bottom.rect.setRight(_bottom.rect.right() - _right.size);
		}
		// Draw background of area.
		if (_colors[cRulerBackground].isValid())
		{
			painter.fillRect(_bottom.rect, _colors[cRulerBackground]);
		}
		// Draw the actual ruler elements.
		draw.Ruler(painter, Draw::roBottom, _colors[cLine], _colors[cText],
			_bottom.rect, area, _bottom.start, _bottom.stop, _bottom.digits, _bottom.unit);
	}
	// Check if horizontal grid is enabled.
	if (_horizontal)
	{
		auto ri = getRulerInfo(_horizontal);
		draw.GridLines(painter, Draw::goHorizontal, _colors[cGrid], _graphArea, ri->start, ri->stop, ri->digits);
	}
	// Check if vertical grid is enabled.
	if (_vertical)
	{
		auto ri = getRulerInfo(_vertical);
		draw.GridLines(painter, Draw::goVertical, _colors[cGrid], _graphArea, ri->start, ri->stop, ri->digits);
	}
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

const QRect& Graph::getGraphArea() const
{
	return _graphArea;
}

}