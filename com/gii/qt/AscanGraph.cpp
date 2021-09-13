#include <QStyleOptionFrame>
#include <QStylePainter>
#include <misc/qt/Draw.h>
#include <misc/qt/Graph.h>
#include <misc/qt/qt_utils.h>
#include <gen/InformationBase.h>
#include <gen/Variable.h>
#include <gen/ResultData.h>
#include "AscanGraph.h"

namespace sf
{

struct AscanGraph::Private :ResultDataHandler, VariableHandler
{
	AscanGraph* _w;
	int margin{1};

	Graph _graph;
	QPolygon _polygon;

	ResultData _rData;
	Variable _vAmplitudeUnit;
	Variable _vTimeUnit;
	Variable _vDelay;
	Variable _vRange;
	Variable _vAttenuation;

	double _attenuation{1.0};

	bool _dataAvail{false};
	Range _lastRange;
	QRect _lastArea;

	explicit Private(AscanGraph* widget)
		:_w(widget)
		 , _graph(widget->palette())
	{
	}

	void initialize()
	{
		_rData.setHandler(this);
		for (auto& v: {&_vAmplitudeUnit, &_vTimeUnit, &_vDelay, &_vRange, &_vAttenuation})
		{
			v->setHandler(this);
		}
		setRulers();
		setGrid();
	}

	void setRulers()
	{
		_graph.setRuler(sf::Draw::roLeft, 0, 10, 2, "V");
		_graph.setRuler(sf::Draw::roBottom, 0, 100, 4, "s");
	}

	void setGrid()
	{
		_graph.setGrid(sf::Draw::goHorizontal, sf::Draw::roLeft);
		_graph.setGrid(sf::Draw::goVertical, sf::Draw::roBottom);
	}

	void invalidate(bool graphAreaOnly) const
	{
		if (graphAreaOnly)
		{
			_w->update(_graph.getPlotArea());
		}
		else
		{
			_w->update();
		}
	}

	void variableEventHandler(VariableTypes::EEvent event, const Variable& caller, Variable& link, bool sameInst) override
	{

	}

	void resultDataEventHandler(ResultDataTypes::EEvent event, const ResultData& caller, ResultData& link, const Range& range, bool sameInst) override
	{
		switch (event)
		{
			default:
				break;

			case reInvalid:
			{
				_lastRange.clear();
				// Set the flag so that the plot draw function does not use the data.
				_dataAvail = false;
				//
				invalidate(false);
				break;
			}

			case reIdChanged:
			{
				// Size the dynamic array to the needed size.
				_polygon.resize(static_cast<qsizetype>(link.getBlockSize()));
				// Paint all.
				invalidate(false);
			}
				// Run into next.
			case reClear:
			{
				_lastRange.clear();
				// Set the flag so that the plot draw function does not use the data.
				_dataAvail = false;
				// Paint only the inner part.
				invalidate(true);
				break;
			}

			case reAccessChange:
			{
				if (auto blocks = range.getStop())
				{
					// Check if the last range is valid.
					if (!link.isRangeValid(blocks - 1, 1))
					{
						link.requestRange(blocks - 1, 1);
						break;
					}
				}
				else
				{
					invalidate(true);
					break;
				}
			}
				// Run into next statement.
			case reGotRange:
				if (generateData(range, _graph.getPlotArea()))
				{
					// Paint only the inner part.
					invalidate(true);
				}
				break;
		}
	}

	bool generateData(const Range& range, const QRect& bounds)
	{
		// Do nothing when the range is empty.
		if (range.isEmpty())
		{
			return true;
		}
		// Set flag initially to false.
		_dataAvail = false;
		//
		unsigned maxValue = _rData.getValueRange();
		//
		auto blockSize = _rData.getBlockSize();
		// Get the start for reading.
		auto offset = range.getStop() - 1;
		// Get the number of bytes per value.
		auto typeSize = _rData.getTypeSize();
		// Create a temporary buffer for reading.
		DynamicBuffer buffer(_rData.getBufferSize(1));
		// On read success
		if (!_rData.blockRead(offset, 1, buffer.data()))
		{
			SF_RTTI_NOTIFY(DO_CLOG, "blockRead() Failed!");
			//PlotControl->Caption = "Error Retrieving Data !";
		}
		else
		{
			// Update the lsat range.
			_lastRange = range;
			// Get fast temporary values for width and height of the bounds.
			auto width = bounds.width();
			auto height = bounds.height() - 1;
			// Holds the previous pt.
			data_type y;
			int px, py;
			for (int x = 0; x < blockSize; x++)
			{
				// Calculate value from result.
				y = _rData.getValueU(buffer.data(x * typeSize));
				// Scale into the bounds of the plot.
				px = calculateOffset<ResultDataTypes::size_type, int>(x, 0, blockSize - 1, width, true);
				py = calculateOffset<ResultDataTypes::size_type, int>(y, 0, maxValue - 1, height, true);
				//
				if (_attenuation > 0)
				{
					height = static_cast<int>(height * _attenuation);
				}
				// Invert the value.
				py = height - py;
				// Update the polygon.
				_polygon.setPoint(x, QPoint(px, py) + bounds.topLeft());
			}
			//
			_dataAvail = true;
			return true;
		}
		//
		return false;
	}

};

SF_IMPL_INFO_ID(AscanGraph, Data, _p->_rData)

SF_IMPL_INFO_ID(AscanGraph, Attenuation, _p->_vAttenuation)

SF_IMPL_INFO_ID(AscanGraph, TimeUnit, _p->_vTimeUnit)

SF_IMPL_INFO_ID(AscanGraph, AmplitudeUnit, _p->_vAmplitudeUnit)

AscanGraph::AscanGraph(QWidget* parent)
	:QWidget(parent)
	 , ObjectExtension(this)
	 , _p(new Private(this))
{
	_p->initialize();
}

AscanGraph::~AscanGraph()
{
	delete _p;
}

void AscanGraph::initStyleOption(QStyleOptionFrame* option) const
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

void AscanGraph::paintEvent(QPaintEvent* event)
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
	inflate(rc, -_p->margin);
	// Paint the graph canvas en returns the remaining plot area.
	auto prc = _p->_graph.paint(sp, rc, event->region());
	// Generate new data when the area changed after a resize or so.
	if (prc != _p->_lastArea && !_p->_lastRange.isEmpty())
	{
		_p->_lastArea = prc;
		_p->generateData(_p->_lastRange, _p->_lastArea);
	}
	// Check if the region to update intersects with
	if (event->region().intersects(prc))
	{
		// Makes single pixel lines a bit fussy/smooth.
		sp.setRenderHint(QPainter::Antialiasing);
		//
		if (_p->_dataAvail)
		{
			auto pen = QPen(palette().color(QPalette::ColorRole::Text));
			pen.setWidth(2);
			sp.setPen(pen);
			sp.drawPolyline(_p->_polygon);
		}
		else
		{
			_p->_graph.paintPlotCross(sp, tr("No Data"));
		}
	}
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
