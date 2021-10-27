#pragma once
#include <misc/qt/Graph.h>
#include <gii/gen/ResultData.h>
#include <gii/gen/Variable.h>
#include <gen/ResultDataRequester.h>
#include "BscanGraph.h"

namespace sf
{

struct BscanGraph::Private
{
	BscanGraph* _w;
	int _margin{1};

	Graph _graph;

	ResultData _rIndex;
	ResultData _rData;
	Variable _vTimeUnit;
	Variable _vTimeDelay;
	Variable _vTimeRange;

	// Handles request for index bundled data.
	ResultDataRequester _requester;

	// Constructor.
	explicit Private(BscanGraph* widget);

	// Destructor.
	~Private();

	void setRulerLeft();

	void setRulerBottom();

	void invalidate(bool graphAreaOnly) const;

	void invalidate(const QRect& rect = {}) const;

	// Link all handlers.
	void initialize();

	// Handles events of the data result.
	void handlerResultData(ResultDataTypes::EEvent event, const ResultData& caller, ResultData& link, const Range& range, bool sameInst);

	TResultDataHandler<BscanGraph::Private> _handlerResultData{this, &BscanGraph::Private::handlerResultData};

	// Handles events of time variables.
	void handlerVariable(VariableTypes::EEvent event, const Variable& caller, Variable& link, bool sameInst);

	TVariableHandler<BscanGraph::Private> _handlerVariable{this, &BscanGraph::Private::handlerVariable};

	// Handlers for scan variables
	void handlerScanVariable(Variable::EEvent event, const Variable& caller, Variable& link, bool sameInst);

	TVariableHandler<BscanGraph::Private> _handlerScan{this, &BscanGraph::Private::handlerScanVariable};

	// Handlers for mark variables
	void handlerMarkVariable(Variable::EEvent event, const Variable& caller, Variable& link, bool sameInst);

	TVariableHandler<BscanGraph::Private> _handlerMarker{this, &BscanGraph::Private::handlerMarkVariable};

	// Draw the graph area.
	bool draw(QPainter& painter, const QRect& rect, const QRegion& region);

	// Checks if the plot can be drawn on the basis of available information.
	bool setCanDraw();

	// Recalculate all static values. When true is passed all is initialized to the initial state.
	void reinitialize(bool clear);

	// Sustain function.
	bool sustain(const timespec& ts);

	//
	bool getData(const Range& index);

	// Called using the valid access data range.
	bool generateData(const Range& rng);

	// Maximum allowed scan lines.
	static constexpr int MaxScanSize{3000};

	// Hook for the sustain interface.
	TSustain<BscanGraph::Private> _sustain;

	// True when plot area has been resized.
	bool _flagResized{false};
	// True when condition for drawing have been met.
	bool _flagCanDraw{false};
	// True when data result is not yet up to date.
	bool _flagWaitForData{false};
	// Bitmap container.
	QImage _image;
	// Range which is currently generated.
	Range _rangeCurrent;
	// Range next to be generated.
	Range _rangeNext;
	// Index skipped when mouse was active. Is -1 if not.
	Range _skippedAccessRange;
	// Range in the data result currently visible non zoomed.
	Range _dataRange;
	// Flag indicating that the data must be generated from the draw function.
	bool _flagGenerate{false};
	// Reference to the timer of the sustain entry.
	ElapseTimer _generateTimer{TimeSpec(0.1)};
	// Holds the data for the bitmap.
	DynamicBuffer _imageData;
	// Structure holding all scan related members.
	struct
	{
		// Current Delay ( in samples ) of data to draw.
		Value::int_type Delay{0};
		// Current range of scan lines able to draw.
		Value::int_type Range{1};
		//
		Value::flt_type Left{0};
		//
		Value::flt_type Right{1.0};
		// Variables for synchronising the data members with when they exist.
		Variable VDelay, VRange, VLeft, VRight;
		//
	} _scan;
	// Structure holding all cursor related members.
	struct
	{
		// Holds the pop index in the pop data of the current cursor position.
		Value::int_type PopIndex{0};
		// Holds the skipped index range of a cursor event.
		Range SkippedPopIndex{};
		// Holds the fraction of the scan line at the cursor position.
		Value::flt_type Fraction{0};
		// Holds the index in the data of the cursor position.
		Value::int_type Index{0};
		// Holds button up down and ctrl/shift/alt key status.
		Value::int_type Event{0};
		// True when mouse is dragging.
		bool Active{false};
		// True when mouse events are captured. Which means this plot is the one changing the attached variables.
		bool Captured{false};
		// Variables for synchronising the data members with when they exist.
		Variable VPopIndex, VEvent, VFraction, VIndex;
		//
	} _cursor;
	// Mark point.
	struct Marker
	{
		bool Active{false};
		QPoint Pt{0, 0};
		int Radius{40};
		QRect Rect{0, 0, 0, 0};
		bool Update{false};
	} _mark;
	// True when zooming is enabled.
	bool _zoomActive{false};

	// Marks a point.
	void mark(const QPoint& pt, bool active);

	// Draw a mark.
	void drawMark(QPainter& painter, Marker& mark) const;

	// Converts to a POP index and a fraction, updates appropriate parameters.
	bool calculatePosIndexAt(Marker& mark);

	// Draw or remove a zoom box on the screen.
	void zoomBox(const QPoint& pt, bool down);

	// Event enumerate for passing to the event function.
	enum EMouseEvent {meDOWN, meMOVE, meUP, meKEY};

	// Event handling routine for local or remote events.
	void localMouse(BscanGraph::Private::EMouseEvent me, Qt::KeyboardModifiers modifiers, Qt::MouseButtons buttons, const QPoint& pt, bool local);
};

}
