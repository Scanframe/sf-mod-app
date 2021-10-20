#pragma once

#include <misc/gen/Sustain.h>
#include <misc/gen/TClosure.h>
#include <misc/gen/TBitSet.h>
#include <misc/gen/ElapseTimer.h>
#include <misc/qt/Graph.h>
#include <gii/gen/Variable.h>
#include <gii/gen/VariableIdList.h>
#include <gii/gen/ResultData.h>

#include "AcquisitionControl.h"

namespace sf
{

class InfoWindow;

struct AcquisitionControl::Private :QObject, InformationTypes
{
	explicit Private(AcquisitionControl* widget);

	~Private() override;

	// Holds the widget part.
	AcquisitionControl* _w;

	int _margin{1};

	Graph _graph;

	bool draw(QPainter& painter, const QRect& bounds, const QRegion& region);

	void geoResize(const QSize& size, const QSize& previousSize);

	void keyDown(int key, Qt::KeyboardModifiers);

	void mouseDown(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPoint point);

	void mouseUp(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPoint point);

	void mouseMove(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPoint point);

	void focus(bool yn);

	void mouseCapture(bool capture);

	/**
	 * @brief Invalidates the passed rectangle and when empty the whole area.
	 * @param rect Optional rectangle to invalidate.
	 */
	void invalidatePlotRect(const QRect& rect = {}) const;

	// Responds to a change in the one of the TCG Variable ID lists.
	void TcgIdChange(void* sender);

	// Sets the bottom ruler according the current variable values.
	void setBottomRuler();

	// Sets the left ruler according the current result values.
	void setLeftRuler();

	// Returns the vertical display range depending on the settings.
	bool getDisplayRangeVert(double& minVal, double& maxVal) const;

	bool getDisplayRangeVert(sdata_type& minVal, sdata_type& maxVal) const;

	// Checks the ID's if the plot can be drawn.
	bool setCanDraw();

	// Hook for the sustain interface.
	TSustain<AcquisitionControl::Private> SustainEntry;
	// Reference to the timer of the sustain entry.
	ElapseTimer _timeoutTimer{TimeSpec(2.0)};

	// Sustain function.
	bool sustain(const timespec& t);

	// List of types of grips.
	enum EGrip
	{
		gNONE = 0,
		gLEFT,   // For delay
		gMIDDLE, // For position and if so selected the threshold.
		gRIGHT,  // For range
	};

	// Check the passed point with the gate grips.
	// When gate is non-null the gate number is returned.
	EGrip GetGateGrip(const QPoint& pt, int* gate = nullptr);

	// Gets the cursor shape from the passed grip.
	[[nodiscard]] Qt::CursorShape getCursorShape(EGrip grip) const;

	// Set the cursor shape.
	void setCursorShape(Qt::CursorShape shape) const;

	// Sets the gates from value or screen rect.
	void setGateHorizontalPos(bool fromRect = false);

	void setGateVerticalPos(bool fromRect = false);

	// Signal handler for when properties change and passes the field pointer.
	void propertyChange(void* field);

	// Holds the current array of polyline points.
	QPolygon _polygon;

	// Last range that was generated.
	Range _lastRange;

	// Handles the variable which the rulers depend on.
	void RulerHandler(Variable::EEvent, const Variable&, Variable&, bool);

	TVariableHandler<AcquisitionControl::Private> _rulerHandler{this, &AcquisitionControl::Private::RulerHandler};

	// Handles the variable which the TCG drawing depends on.
	void TcgVarHandler(Variable::EEvent, const Variable&, Variable&, bool);

	TVariableHandler<AcquisitionControl::Private> _tcgVarHandler{this, &AcquisitionControl::Private::TcgVarHandler};

	// Handles the variable which others depend on.
	void DefaultVarHandler(Variable::EEvent, const Variable&, Variable&, bool);

	TVariableHandler<AcquisitionControl::Private> _defaultVarHandler{this, &AcquisitionControl::Private::DefaultVarHandler};

	// Handle gate variable changes.
	void GateVarHandler(Variable::EEvent, const Variable&, Variable&, bool);

	TVariableHandler<AcquisitionControl::Private> _gateVarHandler{this, &AcquisitionControl::Private::GateVarHandler};

	// Handles copy and copy index data result events.
	void CopyResHandler(ResultData::EEvent, const ResultData&, ResultData&, const Range&, bool);

	TResultDataHandler<AcquisitionControl::Private> _copyResHandler{this, &AcquisitionControl::Private::CopyResHandler};

	// Handles synchronous data events.
	void GateResHandler(ResultData::EEvent, const ResultData&, ResultData&, const Range&, bool);

	TResultDataHandler<AcquisitionControl::Private> _gateResHandler{this, &AcquisitionControl::Private::GateResHandler};

	// Data result.
	ResultData _rCopyData;
	ResultData _rCopyIndex;
	// A-scan time delay and range for drawing rulers.
	Variable _vCopyDelay;
	Variable _vCopyRange;
	// Variable for sample time unit.
	Variable _vTimeUnit;
	Variable _vGateCount;
	// Shift in measurements of the interface echo back in time.
	// (Probably needed for Nerason 1090 project because of probe design)
	Variable _vIfShift;
	// Variable for the TCG delay of the TCG gate.
	Variable _vTcgDelay;
	// Variable for the TCG range of the TCG gate.
	Variable _vTcgRange;
	// Variable for enabling and disabling the TCG presentation in the plot.
	Variable _vTcgEnable;
	// Variable telling how the TCG is triggered or slaved.
	Variable _vTcgSlavedTo;
	// Enumerate for the amount of implemented gates.
	enum :int
	{
		MaxGates = 4,
		MaxTcgPoints = 16
	};

	// Grouped members dealing with TCG painting.
	struct Tcg
	{
		// Holds the points calculated for drawing the TCG in the plot.
		QList<QPoint> Points;
		// Holds the markers.
		QList<QRect> Grips;
		// Hold the variables for the TCG time.
		Variable::Vector TimeVars{MaxTcgPoints};
		// Hold the variables for the TCG Gain.
		Variable::Vector GainVars{MaxTcgPoints};
		// Flag for sustain to recalculate the TCG curve.
		bool RecalculateFlag{false};
		// Holds the last rectangle containing the TCG lines.
		QRect Rect{0, 0, 0, 0};
		// When 0 if the plot data is slaved to the interface gate.
		int SlavedTo{0};
		// Size of the TCG points grips.
		int GripSize{3};
		// Active area of the TCG using delay and range.
		int AreaLeft{0};
		int AreaRight{0};
		//
	} _tcg;

	// Recalculate the gain for a specified point or for the complete list
	// when -1 is passed.
	void generateTcgData(int point = -1);

	// Structure to bind gate info and other data members.
	struct Gate
	{
		// Related variables.
		Variable VDelay;
		Variable VRange;
		Variable VThreshold;
		Variable VSlavedTo;
		// Variable for tracking gate.
		Variable VTrackRange;
		// Related results.
		ResultData RTof;
		ResultData RAmp;
		// Holds the rectangle sized and positioned to the delay and range of the gate without slaved peak information.
		QRect Rect{0, 0, 0, 0};
		// Holds the last drawn screen position. Used to find a grip after drawing.
		QRect GripRect{0, 0, 0, 0};
		// Position of the rectangle at the time of grabbing.
		QPoint GripOffset{0, 0};
		//
		bool FlagAmp{false};
		bool FlagTof{false};
		//
		data_type PeakAmp{0};
		sdata_type PeakTof{0};
		//
		int SlavedTo{-1};
		int Gate{0};
		//
		QPoint PeakPos{0, 0};
		//
		int TrackWidth{0};
		// Color of the gate.
		QColor _color{QColorConstants::Yellow};
	};
	// Create vector which checks memory overrun.
	TVector<Gate> _gates{MaxGates};

	// Triggers calling setHorizontalGatePos() in sustain.
	bool _flagGateHorizontalPos{false};
	// Triggers calling setVerticalGatePos() in sustain.
	bool _flagGateVerticalPos{false};

	// Holds the flag if the plot can be drawn or not.
	bool _flagCanDraw{false};
	// True when the peak data can be drawn.
	bool _flagCanDrawPeak{false};
	// True when dynamic gates can be drawn using the copy index result.
	bool _dynamicGates{false};
	// True when the plot is frozen.
	bool _flagFrozen{false};
	// True when no data is available to draw anything at all.
	bool _flagNoData{true};
	// Holds the actual used gate count.
	int _gateCount{0};
	// Current absolute displacement from the start of grabbing a gate.
	QSize _gripOffset{0, 0};
	// Hold the type of grip that was grabbed.
	EGrip _gripGrabbed{gNONE};
	// Holds the flag when sizing is at hand.
	bool _flagSizing{false};
	// Gate which has been grabbed.
	int _gripGate{-1};
	//
	QPoint _grabPoint{0, 0};
	QRect _gripRect{0, 0, 0, 0};
	QRect _gripRectNext{0, 0, 0, 0};
	// Special window designed to show floating hints.
	InfoWindow* _infoWindow;

	// Calculates the screen lines for drawing.
	bool generateCopyData(const Range& range);

	// Calculates the screen positions of the current peak info.
	void generatePeakData();

	// Enumerate for state levels.
	enum EState
	{
		psError = -1, // Error state for debugging.
		psIdle,       // Initial state waiting for an event.
		psGetCopy,   // Trying to get a valid new copy and index.
		psProcessCopy,   // Process the copy data.
		psTryGate,   // Try to get common gate range.
		psApply,      // Process the gate data and apply them to the plot.
		psReady,      // Ready for next plot.
		psWait,       // Waiting an event or timer.
	};

	// Returns the name of the state.
	const char* getStateName(int state);

	// Holds the current state.
	EState _stateCurrent{psIdle};
	// Holds the previous state.
	EState _statePrevious{psIdle};
	// State which the sustain wil move to requests are met.
	EState _stateToWaitFor{psIdle};

	// Sets the new state and acts on it.
	bool _setState(EState state);

	// Sets the error state.
	// Returns always false for ease in functions.
	bool setError(const QString& txt);

	// Sets the wait state member to the passed value and the state machine
	// waits for some conditions before continuing by setting the passed state.
	// When conditions are not met in time an error is generated.
	// When reset is true the timer is reset.
	bool waitForState(EState state);

	bool processState();

	// Enables or disabled grid.
	void setGrid(bool enabled);

	// Structure holding data to work on.
	struct WorkData
	{
		void Clear()
		{
			// Clear all fields.
			IndexReq = false;
			CopyReq = false;
			GateAmpReq.clear();
			GateTofReq.clear();
			GateAccessEvent.clear();
			CopyRange.clear();
			GateRange.clear();
		}

		// Range of a single entry into the asynchronous copy results.
		Range CopyRange;
		// True when an index was requested from the index result.
		bool IndexReq{false};
		// True when a copy was requested from the copy result.
		bool CopyReq{false};
		// Range of a single entry into the synchronous gate results.
		Range GateRange;
		// Bit mask for checking gate requests.
		TBitSet<MaxGates> GateAmpReq;
		TBitSet<MaxGates> GateTofReq;
		// Bit set to true when waiting gate access changed.
		TBitSet<MaxGates> GateAccessEvent;
		//
	} _work;

	//
	// Properties section
	//
	// The absolute maximum drawable signal value.
	int _valueMax{0};
	// Height for the grips.
	int _gripHeight{4};
	// Makes the threshold be changed using dragging.
	bool _thresholdDrag{true};
	// The value of the threshold is linked to the value range of the data.
	bool _thresholdLinked{true};
	// Determines if the grid is drawn.
	bool _gridEnabled{true};

	// Foreground color for the graph area.
	QColor _colorForeground{QColorConstants::Green};
	// Background color for the graph area.
	QColor _colorBackground{QColorConstants::DarkBlue};
	// Special color for the TCG line.
	QColor _colorTcgLine{QColorConstants::Blue};
	QColor _colorTcgRange{QColorConstants::Cyan};
	// Color for 10% line.
	QColor _colorAdjustmentLine{QColorConstants::LightGray};
	// Color for the grid lines.
	QColor _colorGridLines{QColorConstants::Gray};

	// Hold the TCG Time variables ID's.
	VariableIdList _idsTcgTime;
	// Hold the TCG Gain variables ID's.
	VariableIdList _idsTcgGain;

	bool _debug;
};

}
