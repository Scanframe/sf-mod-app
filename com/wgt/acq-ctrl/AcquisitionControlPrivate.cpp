#include <QLabel>
#include <QToolTip>
#include <misc/gen/gen_utils.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/Draw.h>
#include "AcquisitionControlPrivate.h"

namespace sf
{

class InfoWindow :public QLabel
{
	public:
		explicit InfoWindow(QWidget* parent)
			:QLabel(parent)
		{
		}

		void setPosition(QPoint pt)
		{
			Q_UNUSED(pt)
		}

		void setOffset(QPoint pt)
		{
			Q_UNUSED(pt)
		}

		void setActive(bool active)
		{
			Q_UNUSED(active)
		}
};

AcquisitionControl::Private::Private(AcquisitionControl* widget)
	:_w(widget)
	 , SustainEntry(this, &AcquisitionControl::Private::sustain)
	 , _infoWindow(new InfoWindow(_w))
	 , _idsTcgTime(_tcg.TimeVars)
	 , _idsTcgGain(_tcg.GainVars)
	 ,_debug(false)
{
	// Make the widget get focus when clicked in.
	_w->setFocusPolicy(Qt::StrongFocus);
	// Also, when using the keyboard.
	_w->setAttribute(Qt::WA_KeyboardFocusChange);
	//
	_w->setAttribute(Qt::WA_TranslucentBackground);
	_w->setAttribute(Qt::WA_NoSystemBackground);
	_w->setAttribute(Qt::WA_StyledBackground);
	//
	for (int i = 0; i < MaxGates; i++)
	{
		Gate& gt(_gates[i]);
		// Set the structures gate number for reference.
		gt.Gate = i;
		// Set handlers and set the data pointer for gate variables.
		gt.VDelay.setData(&gt);
		gt.VDelay.setHandler(&_gateVarHandler);
		gt.VRange.setData(&gt);
		gt.VRange.setHandler(&_gateVarHandler);
		gt.VThreshold.setData(&gt);
		gt.VThreshold.setHandler(&_gateVarHandler);
		gt.VSlavedTo.setData(&gt);
		gt.VSlavedTo.setHandler(&_gateVarHandler);
		gt.VTrackRange.setData(&gt);
		gt.VTrackRange.setHandler(&_gateVarHandler);
		// Set handler for gate results.
		gt.RAmp.setData(&gt);
		gt.RAmp.setHandler(&_gateResHandler);
		gt.RTof.setData(&gt);
		gt.RTof.setHandler(&_gateResHandler);
	}
	// Link the variable handler functions.
	_vTimeUnit.setHandler(&_rulerHandler);
	_vCopyDelay.setHandler(&_rulerHandler);
	_vCopyRange.setHandler(&_rulerHandler);
	//
	_vGateCount.setHandler(&_defaultVarHandler);
	//
	_rCopyData.setHandler(&_copyResHandler);
	_rCopyIndex.setHandler(&_copyResHandler);
	// Hook change handler to the list.
	_idsTcgTime.onChange.assign([&](void* p) {TcgIdChange(p);});
	_idsTcgGain.onChange.assign([&](void* p) {TcgIdChange(p);});
	// Hook the TCG handler to TCG variables setting the data field
	// to UINT_MAX to make a possible switch int the handler.
	_vTcgEnable.setData(std::numeric_limits<uint64_t>::max());
	_vTcgEnable.setHandler(&_tcgVarHandler);
	_vTcgDelay.setData(std::numeric_limits<uint64_t>::max());
	_vTcgDelay.setHandler(&_tcgVarHandler);
	_vTcgRange.setData(std::numeric_limits<uint64_t>::max());
	_vTcgRange.setHandler(&_tcgVarHandler);
	_vTcgSlavedTo.setData(std::numeric_limits<uint64_t>::max());
	_vTcgSlavedTo.setHandler(&_tcgVarHandler);
	// Initialize the bottom ruler.
	setBottomRuler();
	// Initialize the left ruler.
	setLeftRuler();
	// Update some fields controlled by properties. (mainly colors).
	propertyChange(nullptr);
	// Enable the grid.
	setGrid(true);
}

AcquisitionControl::Private::~Private()
{
	Q_UNUSED(this)
}

void AcquisitionControl::Private::mouseCapture(bool capture)
{
	Q_UNUSED(capture)
	// TODO: Needs implementation.
}

void AcquisitionControl::Private::invalidatePlotRect(const QRect& rect) const
{
	// When debugging invalidate it all.
	if (rect.isEmpty())
	{
		_w->update(_graph.getPlotArea());
	}
	else
	{
		_w->update(rect + _graph.getPlotArea().topLeft());
	}
}

void AcquisitionControl::Private::propertyChange(void* field)
{
	if (field == &_gripHeight)
	{
		_gripHeight = clip(_gripHeight, 1, 100);
		_flagGateVerticalPos = true;
	}
	if (field == &_thresholdDrag || field == &_thresholdLinked)
	{
		_flagGateVerticalPos = true;
	}
	if (field == &_gridEnabled)
	{
		setGrid(_gridEnabled);
	}
	if (field == &_valueMax)
	{
		setLeftRuler();
		_flagGateVerticalPos = true;
	}
	// When field is the nullptr, update it.
	if (!field || field == &_colorBackground)
	{
		_graph.setColor(Graph::cGraphBackground, _colorBackground);
	}
	if (!field || field == &_colorGridLines)
	{
		_graph.setColor(Graph::cGridLines, _colorGridLines);
	}
	if (!field || field == &_colorForeground)
	{
		_graph.setColor(Graph::cGraphForeground, _colorForeground);
	}
	// Invalidate by default.
	_w->update();
}

void AcquisitionControl::Private::TcgIdChange(void*)
{
	for (size_t i = 0; i < _tcg.TimeVars.size(); i++)
	{
		Variable& var = _tcg.TimeVars[i];
		// Set the data field to the TCG index.
		var.setData(i);
		// Hook the variable.
		var.setHandler(&_tcgVarHandler);
	}
	for (size_t i = 0; i < _tcg.GainVars.size(); i++)
	{
		Variable& var = _tcg.GainVars[i];
		// Set the data field to the TCG index.
		var.setData(i);
		// Hook the variable.
		var.setHandler(&_tcgVarHandler);
	}
}

bool AcquisitionControl::Private::setCanDraw()
{
	_flagCanDraw = false;
	// Need the copy delay.
	if (_vCopyDelay.getId())
	{
		// Can calculate the range from time unit and the array size.
		// Or can also use the range passed by the variable directly.
		if (_rCopyData.getId() && _vTimeUnit.getId() || _vCopyRange.getId())
		{
			_flagCanDraw = true;
		}
	}
	// Allow any changes to take place.
	invalidatePlotRect();
	return _flagCanDraw;
}

bool AcquisitionControl::Private::getDisplayRangeVert(Value::flt_type& minVal, Value::flt_type& maxVal) const
{
	sdata_type iMinVal;
	sdata_type iMaxVal;
	if (getDisplayRangeVert(iMinVal, iMaxVal))
	{
		minVal = static_cast<Value::flt_type>(iMinVal);
		maxVal = static_cast<Value::flt_type>(iMaxVal);
		return true;
	}
	return false;
}

bool AcquisitionControl::Private::getDisplayRangeVert(sdata_type& minVal, sdata_type& maxVal) const
{
	// When the copy data result is available.
	if (_rCopyData.getId())
	{
		// When the maximum allowed value is zero it means it is not an active property.
		if (!_valueMax)
		{
			minVal = -(sdata_type) _rCopyData.getValueOffset();
			maxVal = static_cast<sdata_type>(_rCopyData.getValueRange() - _rCopyData.getValueOffset());
		}
		else
		{  // When the current result has an offset
			if (_rCopyData.getValueOffset())
			{
				minVal = -_valueMax;
			}
			else
			{
				minVal = 0;
			}
			//
			maxVal = _valueMax;
		}
		return true;
	}
	return false;
}

void AcquisitionControl::Private::setLeftRuler()
{
	// When the copy data result is available.
	Value::flt_type minVal, maxVal;
	Value::flt_type step = 1.0;
	if (getDisplayRangeVert(minVal, maxVal))
	{
		// Set the left ruler according the maximum value.
		_graph.setRuler(sf::Draw::roLeft, minVal, maxVal, requiredDigits(step, minVal, maxVal), "Amp");
	}
	else
	{
		_graph.setRuler(sf::Draw::roLeft, 0, 100, 1, "n/a");
	}
	_w->update();
}

void AcquisitionControl::Private::setGrid(bool enabled)
{
	if (enabled)
	{
		_graph.setGrid(Draw::goVertical, Draw::roBottom);
		_graph.setGrid(Draw::goHorizontal, Draw::roLeft);
	}
	else
	{
		_graph.setGrid(Draw::goVertical, Draw::roNone);
		_graph.setGrid(Draw::goHorizontal, Draw::roNone);
	}
	_w->update();
}

void AcquisitionControl::Private::setBottomRuler()
{
	// Get the unit of the ascan.
	std::string unit = "x";
	Value::flt_type start{0.0}, range{100.0}, step{1.0};
	// When the range of the copy result id is specified the that value is used to calculate the range of plot.
	if (_vCopyRange.getId())
	{
		// Set the converted start value for the start value but when the
		// delay variable is present it is assumed to be zero.
		start = _vCopyRange.convert(_vCopyDelay.getCur()).getFloat();
		// Get the converted range value.
		range = _vCopyRange.getCur(true).getFloat();
		// Set the converted unit
		unit = _vCopyRange.getUnit(true);
	}
		// When range variable is NOT present and time unit is.
		// Then use block size and time unit to establish the range.
	else if (_vTimeUnit.getId())
	{
		// When the delay parameter is present it is used to for conversion as well.
		if (_vCopyDelay.getId())
		{
			// Set the converted start value.
			start = _vCopyDelay.getCur(true).getFloat();
			// Get the range of the ascan by getting the block size.
			range = _vCopyDelay.convert(_vTimeUnit.getCur()).getFloat() * static_cast<Value::flt_type>(_rCopyData.getBlockSize());
			// Set the converted unit
			unit = _vCopyDelay.getUnit(true);
		}
		else
		{ // Delay variable is not present so the time unit is used for conversion.
			// Get the range of the ascan by getting the block size.
			range = _vTimeUnit.getCur(true).getFloat() * static_cast<Value::flt_type>(_rCopyData.getBlockSize());
			// Set the converted unit
			unit = _vTimeUnit.getUnit(true);
		}
	}
		// Only able to use block size to set range.
		// Use the copy data block size to fill the bottom ruler.
	else if (_rCopyData.getId())
	{
		// If Delay exists the value is presumed to be an offset in samples.
		// When delay is not available the value is zero.
		start = static_cast<Value::flt_type>(_vCopyDelay.getCur().getInteger());
		range = static_cast<Value::flt_type>(_rCopyData.getBlockSize());
		unit = "x";
	}
	// Set bottom ruler.
	_graph.setRuler(sf::Draw::roBottom, start, start + range, requiredDigits(step, start, range), QString::fromStdString(unit));
	//
	SF_Q_NOTIFY("Start:" << start << "Range:" << range << "Digits:" << requiredDigits(step, start, range))
	_w->update();
}

bool AcquisitionControl::Private::generateCopyData(const Range& range)
{
	// Do nothing when the range is empty.
	if (range.isEmpty())
	{
		return true;
	}
	// Get fast local copy of data properties.
	size_type blockSize = _rCopyData.getBlockSize();
	size_type typeSize = _rCopyData.getTypeSize();
	// Get the start for reading.
	size_type ofs = range.getStop() - 1;
	// Create a temporary buffer for reading.
	DynamicBuffer buffer(_rCopyData.getBufferSize(1));
	// Size the polygon to the required size.
	_polygon.resize(static_cast<qsizetype>(blockSize));
	// On read success.
	if (!_rCopyData.blockRead(ofs, 1, buffer.data()))
	{
		SF_Q_NOTIFY("BlockRead() Failed!");
	}
	else
	{
		if (!_valueMax)
		{
			auto maxValue = _rCopyData.getValueRange();
			// Update the last range.
			_lastRange = range;
			// Get temporary values for the width and height.
			QRect bounds = _graph.getPlotArea();
			// Get fast temporary values for width and height of the plot bounds.
			int width = bounds.width();
			int height = bounds.height() - 1;
			//
			data_type y;
			int px, py;
			for (int x = 0; x < blockSize; x++)
			{
				// Get a value from the buffer.
				y = _rCopyData.getValueU(buffer.data(typeSize * x));
				// Scale into the bounds of the plot.
				px = calculateOffset<size_type, int>(x, 0, blockSize - 1, width, true);
				py = calculateOffset<size_type, int>(y, 0, maxValue - 1, height, true);
				//
				// Invert the value.
				py = height - py;
				// Update the polygon.
				_polygon.setPoint(x, QPoint(px, py));
			}
/*
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
				px = calculateOffset<size_type, int>(x, 0, blockSize - 1, width, true);
				py = calculateOffset<size_type, int>(y, 0, maxValue - 1, height, true);
				//
				if (_attenuation > 0)
				{
					height = static_cast<int>(height * _attenuation);
				}
				// Invert the value.
				py = height - py;
				// Update the polygon.
				_polygon.setPoint(x, QPoint(px, py) + bounds.topLeft());
 */
		}
		else
		{
			// Determine the maximum value to use.
			int maxVal = _valueMax;
			int minVal = (_rCopyData.getValueOffset()) ? -_valueMax : 0;
			// Update the last range.
			_lastRange = range;
			// Get temporary values for the width and height.
			QRect bounds = _graph.getPlotArea();
			// Get fast temporary values for width and height of the plot bounds.
			int width = bounds.width() - 1;
			int height = bounds.height() - 1;
			//
			ResultData::sdata_type y;
			int px, py;
			for (ResultData::size_type x = 0; x < blockSize; x++)
			{
				// Get a value from the buffer.
				y = _rCopyData.getValue(buffer.data(typeSize * x));
				// Scale into the bounds of the plot.
				px = calculateOffset<size_type, int>(x, 0L, blockSize, width, true);
				py = height - calculateOffset<size_type, int>(y, minVal, maxVal, height, true);
				//
				_polygon.append(QPoint(px, py));
			}
		}
		return true;
	}
	return false;
}

void AcquisitionControl::Private::generatePeakData()
{
	// Get temporary values for the width and height.
	QRect bounds = _graph.getPlotArea();
	// Get fast temporary values for width and height of the plot bounds.
	int width = bounds.width() - 1;
	int height = bounds.height() - 1;
	// Get fast local copies.
	double delay = _vCopyDelay.getCur().getFloat();
	double range = _vCopyRange.getCur().getFloat();
	double sampleTime = _vTimeUnit.getId() ? _vTimeUnit.getCur().getFloat() : 1.0;
	// Scale peak values in the plot area.
	for (unsigned i = 0; i < _gateCount; i++)
	{
		// Local reference.
		Gate& gi(_gates[i]);
		// Clear the screen position.
		gi.PeakPos = {0, 0};
		// Check if TOF value was present and read.
		if (gi.FlagTof)
		{
			// Calculate the time of flight of the tof in samples.
			Value::flt_type tof = sampleTime * static_cast<Value::flt_type>(gi.PeakTof);
			// Check if the gate is slaved to another.
			if (gi.SlavedTo >= 0)
			{
				int j = gi.SlavedTo;
				// if the gates are cascaded then loop and add the times up
				while (j >= 0)
				{
					// Check if TOF result is available.
					if (_gates[j].FlagTof)
					{
						tof += sampleTime * static_cast<Value::flt_type>(_gates[j].PeakTof);
					}
					//
					j = _gates[j].SlavedTo;
				}
			}
				// Check for artificial way of slaving.
				// Actual gate delay is IF delay + gate delay.
			else if (gi.SlavedTo == -2)
			{
				// Just add the delay of the IF gate.
				tof += _gates[0].VDelay.getCur().getFloat();
			}
			// Calculate the screen coord (not clipped).
			gi.PeakPos.setX(calculateOffset(tof, delay, delay + range, width, true));
		}
		// Check if AMP value was present.
		if (gi.FlagAmp)
		{
			ResultData::sdata_type minVal = 0;
			ResultData::sdata_type maxVal = 0;
			// Check if the display range could be established.
			if (getDisplayRangeVert(minVal, maxVal))
			{
				// Get the signed value.
				auto amp = gi.RAmp.getValue(&gi.PeakAmp);
				// Take the absolute value when the minimum is zero.
				// So negative values are presented as positive ones.
				if (minVal == 0)
				{
					amp = abs(amp);
				}
				// Scale it into the display range.
				gi.PeakPos.setY(height - calculateOffset(amp, minVal, maxVal, height, true));
			}
			else
			{
				// Get the current gate amplitude result value range to scale the peak on the screen to.
				auto maxValRng = gi.RAmp.getValueRange();
				gi.PeakPos.setY(height - calculateOffset<data_type, int>(gi.PeakAmp, 0, maxValRng, height, true));
			}
		}
	}
}

void AcquisitionControl::Private::generateTcgData(int point)
{
	// Test for -1 which means reinitialize from the start.
	if (point < 0)
	{
		// When the TCG enable variable is not enabled bail here.
		bool disabled = _vTcgEnable.getId() && _vTcgEnable.getCur().isZero();
		// Adjust the point list to the maximum ID's in the list.
		// Because both ID's must be available.
		Variable::PtrVector::size_type count = 0;
		// When disabled do not bother to get the count of points.
		if (!disabled)
		{
			count = std::min(_tcg.TimeVars.size(), _tcg.GainVars.size());
		}
		// Get the count for which both time and gain IDs are present and
		// the time value is non-zero.
		Value dac_last(0.0);
		for (Variable::PtrVector::size_type i = 0; i < count; i++)
		{
			// Limit the dac range when the successor point is less or
			// equal then the predecessor point.
			if (!_tcg.TimeVars[i].getId() || !_tcg.GainVars[i].getId()
				|| _tcg.TimeVars[i].getCur() <= dac_last)
			{
				count = i;
			}
			else
			{
				dac_last = _tcg.TimeVars[i].getCur();
			}
		}
		// Bail out when no valid time gain IDs are found.
		if (!count)
		{
			// Check if the TCG was draw before.
			if (!_tcg.Points.empty() || _tcg.AreaLeft != _tcg.AreaRight)
			{
				// If so invalidate the while plot to clear it.
				invalidatePlotRect();
			}
			// Clear the poly line.
			_tcg.Points.clear();
			// Clear the grip rectangles.
			_tcg.Grips.clear();
			// Clear the area for the TCG.
			_tcg.AreaLeft = _tcg.AreaRight = 0;
			// bail out here.
			return;
		}
		else
		{
			// Resize the points vector for drawing and add one extra for the starting point of a poly line.
			_tcg.Points.resize((qsizetype) count + 1);
			// Need only exact size for grips.
			_tcg.Grips.resize((qsizetype) count);
		}
	}
	// When the point is in range of the points list continue.
	if (point >= (int) _tcg.Points.size())
	{
		return;
	}
	// Get fast temporary plot range and delay.
	auto plotDelay = _vCopyDelay.getCur().getFloat();
	auto plotRange = _vCopyRange.getCur().getFloat();
	// Get temporary values for the width and height.
	QRect bounds = _graph.getPlotArea();
	// Get fast temporary values for width and height of the plot bounds.
	auto width = bounds.width() - 1;
	auto height = bounds.height() - 1;
	// Get the delay to work with.
	auto tcgDelay = _vTcgDelay.getCur().getFloat();
	// Determine to what the TCG gate is artificial.
	if (_gateCount)
	{
		// When the gate is slaved artificially the static offset of the IF delay
		// must be added here.
		if (_tcg.SlavedTo == -2)
		{
			// The offset is added in the draw function.
			tcgDelay += _gates[0].VDelay.getCur().getFloat();
			tcgDelay -= plotDelay;
		}
			// When the TCG is not slaved but from the initial pulse.
		else if (_tcg.SlavedTo == -1)
		{
			tcgDelay -= plotDelay;
		}
			// When the TCG is slaved the interface gate.
		else if (_tcg.SlavedTo == 0)
		{
			// Do nothing the offset is added in the draw function.
		}
	}
	else
	{
		// When no gates are available use calculations as for the initial pulse.
		tcgDelay -= plotDelay;
	}
	//
	//bool do_clip = !!VTcgRange.getId();
	auto tcgRange = _vTcgRange.getCur().getFloat();//do_clip ? VTcgRange.Cur->Float : 0.0;
	// Get the values for scaling the gain vertical from the first gain parameter.
	auto gainMin = _tcg.GainVars[0].getMin().getFloat();
	auto gainMax = _tcg.GainVars[0].getMax().getFloat();
	Value::flt_type tm;
	// Rectangle to invalidate.
	QRect irc;
	// Create grip rectangle just for the moving.
	QRect grc(-_tcg.GripSize, -_tcg.GripSize, _tcg.GripSize + 1, _tcg.GripSize + 1);
	// Calculate each TCG point on the plot.
	// There is always a starting point.
	for (qsizetype i = 0; i < _tcg.Points.size() - 1; i++)
	{
		// The time is absolute.
		// The TCG gate delay is start of the TCG so this is the starting point.
		tm = tcgDelay + _tcg.TimeVars[i].getCur().getFloat();
		// Clip it if a TCG range exists.
		auto t = /*do_clip ? clip(tm, tcgDelay, tcgDelay + tcgRange) : */tm;
		// Retrieve the gain of the variable.
		auto gain = _tcg.GainVars[i].getCur().getFloat();
		// Scale the values within the windows boundaries.
		_tcg.Points[i + 1].setX(calculateOffset(t, 0.0, plotRange, width, true));
		_tcg.Points[i + 1].setY(height - calculateOffset(gain, gainMin, gainMax, height, true));
		// When this is the first entry fill also the first entry.
		if (i == 0)
		{
			// The first is the TCG gate delay corrected.
			int left = calculateOffset(tcgDelay, 0.0, plotRange, width, true);
			int right = calculateOffset(tcgDelay + tcgRange, 0.0, plotRange, width, true);
			// Initialize the to invalidate rectangle.
			irc = {left, left, right, right};
			// Initialize the starting point of the TCG curve/ polyline.
			_tcg.Points[0] = irc.topLeft();
			// Calculate the left and right positions of the TCG active area.
			if (_tcg.AreaLeft != irc.left())
			{ // Add the changed part to the invalidation rectangle.
				irc |= QRect(_tcg.AreaLeft, 0, left, height).normalized();
				_tcg.AreaLeft = left;
			}
			if (_tcg.AreaRight != right)
			{ // Add the changed part to the invalidation rectangle.
				irc |= QRect(_tcg.AreaRight, 0, right, height).normalized();
				_tcg.AreaRight = right;
			}
			//SF_RTTI_NOTIFY(DO_DEFAULT, "TCG Point " << (0) << ": " << FTcg.Points[0]);
		}
		else
		{
			irc |= QRect(_tcg.Points[i], _tcg.Points[i + 1]).normalized();
		}
		// Increment the 'invalidate' rectangle so in contains the TCG grips as well.
		irc.setRight(irc.right() + _tcg.GripSize);
		irc.setTop(irc.top() - _tcg.GripSize);
		// Assigned the offset grip rectangle to the point position.
		_tcg.Grips[i] = grc + _tcg.Points[i + 1];
		// SF_RTTI_NOTIFY(DO_DEFAULT, "TCG Point " << (i+1) << ": " << FTcg.Points[i+1]);
	}
	// Invalidate only the effected area which is the previous and the new one.
	invalidatePlotRect((_tcg.Rect | irc).adjusted(1, 1, 1, 1));
	// Update the rectangle of the actual data.
	_tcg.Rect = irc;
}

void AcquisitionControl::Private::setGateVerticalPos(bool fromRect)
{
	// Get temporary values for the width and height.
	int height = _graph.getPlotArea().height() - 1;
	// Generate the gate draw points.
	for (int i = 0; i < _gateCount; i++)
	{
		// Set the default position.
		int yPos;
		// Create easy access reference.
		Gate& gt(_gates[i]);
		// Make a copy for comparison.
		auto prev_rc = gt.Rect;
		// Check if the variable is present and if it is enabled.
		if (_thresholdDrag && gt.VThreshold.getId() && height)
		{
			Value::flt_type minVal;
			Value::flt_type maxVal;
			// When the threshold is linked to the result-data it uses those results offset and range to determine the range.
			// So to say the threshold corresponds to the left ruler values.
			if (!(_thresholdLinked && getDisplayRangeVert(minVal, maxVal)))
			{
				minVal = gt.VThreshold.getMin().getFloat();
				maxVal = gt.VThreshold.getMax().getFloat();
			}
			// Check if the variables must be set from the rectangle first.
			if (fromRect)
			{
				// Get the y position of the center of the rectangle on the plot.
				int yp = gt.Rect.center().y();
				Value::flt_type threshold = ((maxVal - minVal) / height) * (height - yp);
				threshold += minVal;
				// Update the threshold variable.
				gt.VThreshold.setCur(Value(threshold), true);
			}
			// Get the y position of the center of the rect in the plot.
			// Based on the passed min and max values.
			yPos = height - calculateOffset<Value::flt_type, int>(gt.VThreshold.getCur().getFloat(), minVal, maxVal, height, false);
		}
		else
		{
			yPos = height - _gripHeight * (_gateCount - i + 1);
		}
		// Calculate the top and bottom values for that y position
		// regarding the grip height.
		gt.Rect.setTop(yPos - _gripHeight);
		gt.Rect.setBottom(yPos + _gripHeight);
		// Compare the previous and new rectangles.
		if (prev_rc != gt.Rect)
		{
			// When a change is there redraw both regions on the screen.
			invalidatePlotRect(gt.Rect);
			invalidatePlotRect(prev_rc);
		}
	}
}

void AcquisitionControl::Private::setGateHorizontalPos(bool fromRect)
{
	// Get temporary values for the width and height.
	auto bounds = _graph.getPlotArea();
	// Get fast temporary values for width and height of the plot bounds.
	auto width = bounds.width() - 1;
	// Get temporary range and delay.
	Value::flt_type plotDelay = _vCopyDelay.getCur().getFloat();
	Value::flt_type plotRange = _vCopyRange.getCur().getFloat();
	// When the range ID is not valid use time unit and block size to calculate the range.
	if (!_vCopyRange.getId())
	{
		plotRange = _vTimeUnit.getCur().getFloat() * static_cast<Value::flt_type>(_rCopyData.getBlockSize());
	}
	//
	Value::flt_type pixelRatio = 0.0;
	// Check for a division by zero.
	if (std::abs(plotRange) > std::numeric_limits<Value::flt_type>::denorm_min())
	{
		pixelRatio = double(width) / plotRange;
	}
	// Generate the gate draw points.
	for (int i = 0; i < _gateCount; i++)
	{
		// Create easy access reference.
		Gate& gt(_gates[i]);
		// Make a copy for comparison after the change.
		auto prevRect = gt.Rect;
		//
		Value::flt_type gateDelayOfs = 0.0;
		// Check if this gate is slaved to another gate.
		if (gt.SlavedTo >= 0)
		{
			// Get a fast reference to the slaved to gate.
			Gate& sgt(_gates[gt.SlavedTo]);
			// Add the delay of the slaved gate.
			gateDelayOfs += sgt.VDelay.getCur().getFloat();
		}
			// Check for artificial (-2) way of slaving.
			// Actual gate delay is IF delay + gate delay.
		else if (gt.SlavedTo == -2)
		{
			// Just add the delay of the IF gate.
			gateDelayOfs += _gates[0].VDelay.getCur().getFloat();
		}
		// Check if the variables must be set from the rectangle first.
		if (fromRect && i == _gripGate)
		{
			// Prevent division by zero.
			if (std::abs(pixelRatio) > std::numeric_limits<Value::flt_type>::denorm_min())
			{
				// Calculate variable values from plot rect coordinates.
				auto delay = static_cast<Value::flt_type>(gt.Rect.left()) / pixelRatio;
				Value::flt_type range = gt.Rect.width() / pixelRatio;
				// Slaved gates are relative to a found peak position.
				// So offsets are applied in the draw function.
				if (gt.SlavedTo < 0)
				{
					// Compensate for the plot delay and the slaved gate delay.
					delay += plotDelay;
					delay -= gateDelayOfs;
				}
				// Update delay variables.
				// When this update causes a change.
				if (gt.VDelay.setCur(Value(delay), true))
				{ // When it is the Interface gate and the TCG is artificially slaved.
					if (i == 0 && _tcg.SlavedTo == -2)
					{
						// Make the TCG curve be recalculated.
						_tcg.RecalculateFlag = true;
					}
				}
				gt.VRange.setCur(Value(range), true);
			}
		}
		// Create temporary delay and range values.
		Value::flt_type gateDelay = gt.VDelay.getCur().getFloat();
		Value::flt_type gateRange = gt.VRange.getCur().getFloat();
		// Calculate the offset of the gate in pixels
		int gateWidth = static_cast<int>(gateRange * pixelRatio);
		int gateOffset;
		if (gt.SlavedTo < 0)
		{
			gateOffset = static_cast<int>((gateDelay - plotDelay + gateDelayOfs) * pixelRatio);
		}
		else
		{
			gateOffset = static_cast<int>(gateDelay * pixelRatio);
		}
		// Adjust the rectangle.
		gt.Rect.setLeft(gateOffset);
		gt.Rect.setRight(gateWidth + gateOffset);
		// Check if this is for calculating the tracking width.
		if (gt.VTrackRange.getId())
		{
			gt.TrackWidth = static_cast<int>(gt.VTrackRange.getCur().getFloat() * pixelRatio);
			if (gt.TrackWidth > gt.Rect.width())
			{
				gt.TrackWidth = gt.Rect.width();
			}
		}
		else
		{
			// Set the width to a negative value to signal no drawing.
			gt.TrackWidth = -1;
		}
		// Compare the previous and new rectangles.
		if (prevRect != gt.Rect)
		{
			// When a change is there redraw both regions on the screen.
			invalidatePlotRect(gt.Rect);
			invalidatePlotRect(prevRect);
		}
	}
}

bool AcquisitionControl::Private::sustain(const timespec& t)
{
	if (_flagFrozen)
	{
		// Reset the timeout timer for when dragging takes a long time.
		_timeoutTimer.reset();
		return true;
	}
	// Processes the current state when not waiting for an event.
	if (_stateCurrent != psIdle)
	{
		processState();
	}
	// Recalculate vertical positions.
	if (_flagGateVerticalPos)
	{
		_flagGateVerticalPos = false;
		setGateVerticalPos(false);
	}
	// Recalculate horizontal positions onscreen.
	if (_flagGateHorizontalPos)
	{
		_flagGateHorizontalPos = false;
		setGateHorizontalPos(false);
	}
	//
	if (_tcg.RecalculateFlag)
	{
		_tcg.RecalculateFlag = false;
		generateTcgData();
	}
	// Want to be called next time too.
	return true;
}

const char* AcquisitionControl::Private::getStateName(int state)
{
	switch (state)
	{
		case psError:
			return "Error";
		case psIdle:
			return "Idle";
		case psGetCopy:
			return "GetCopy";
		case psProcessCopy:
			return "PrcCopy";
		case psTryGate:
			return "TryGate";
		case psApply:
			return "Apply";
		case psReady:
			return "Ready";
		case psWait:
			return "Wait";
		default:
			return "Unknown";
	}
}

bool AcquisitionControl::Private::_setState(EState state) // NOLINT(misc-no-recursion)
{
	// Only change the previous state when the state is not psWAIT.
	if (_stateCurrent != psWait)
	{
		_statePrevious = _stateCurrent;
	}
	// Assign the new state value.
	_stateCurrent = state;
	// No waiting when state was set with this function.
	_stateToWaitFor = psIdle;
	// This could speed up things.
	processState();
	//
	return true;
}

bool AcquisitionControl::Private::setError(const QString& txt) // NOLINT(misc-no-recursion)
{
	int oldPrevious = _statePrevious;
	// Update the previous state.
	if (_stateCurrent != psWait)
	{
		_statePrevious = _stateCurrent;
	}
	// Assign the new state first so that msg boxes can appear.
	_stateCurrent = psError;
	// Do some debug printing in case of an error.
	SF_RTTI_NOTIFY(DO_CLOG, "State Machine ran into an error!\n"
		<< txt << '\n' << "SetState("
		<< getStateName(oldPrevious) << "=>"
		<< getStateName(_statePrevious) << "=>"
		<< getStateName(_stateCurrent)
		<< ")");
	// Recover from the error.
	_setState(psIdle);
	// Always return false for being able to use it as a return value.
	return false;
}

bool AcquisitionControl::Private::waitForState(EState state)
{
//	SF_RTTI_NOTIFY(DO_CLOG, "WaitForState(" << getStateName(state) << ")" << " CopyRange" << _work.CopyRange);
	_stateToWaitFor = state;
	_stateCurrent = psWait;
	// Reset the timer to generate an error when waiting takes too long.
	_timeoutTimer.reset();
	// Always return true for being able to use it as a return value.
	return true;
}

bool AcquisitionControl::Private::processState() // NOLINT(misc-no-recursion)
{
	switch (_stateCurrent)
	{
		default:
			return true;

		case psError:
			return false;

		case psGetCopy:
		{
			// Set the maximum range to start with.
			Range rng(0, std::numeric_limits<Range::size_type>::max());
			//
			// Get the common access range of both asynchronous results.
			//
			if (_dynamicGates)
			{
				if (_rCopyIndex.getId())
				{
					rng &= _rCopyIndex.getAccessRange();
				}
				else
				{
					// Need copy index to be able to make it work at all.
					return setError("Copy index result not present");
				}
			}
			// When an a-scan result is not available just don't use it.
			if (_rCopyData.getId())
			{
				rng &= _rCopyData.getAccessRange();
			}
			// One of the copy results has no data yet.
			if (rng.getSize() == 0)
			{
				// Till next result event.
				return _setState(psIdle);
			}
			// Get the last common index to work on.
			_work.CopyRange.set({rng.getStop() - 1, rng.getStop()});
			// Not needed for dynamic gates.
			if (_dynamicGates)
			{
				// Check if the range is valid if not request it.
				if (!_rCopyIndex.isRangeValid(_work.CopyRange))
				{
					// Request the needed range.
					_work.IndexReq = _rCopyIndex.requestRange(_work.CopyRange);
					// Check for an error on making the request.
					if (!_work.IndexReq)
					{
						return setError("Failed to make request for copy index result");
					}
				}
			}
			else
			{
				// No index request was mage when no dynamic gates is selected.
				_work.IndexReq = false;
			}
			// Check if the range is valid if not, request it.
			if (!_rCopyData.isRangeValid(_work.CopyRange))
			{
				// Request the needed range.
				_work.CopyReq = _rCopyData.requestRange(_work.CopyRange);
				// Check for an error.
				if (!_work.CopyReq)
				{
					return setError("Failed to make request for copy data result");
				}
			}
			// When requests were made and thus the data was not valid yet, we must wait.
			if (_work.IndexReq || _work.CopyReq)
			{
				// Make the state machine wait for the request events.
				return waitForState(psProcessCopy);
			}
		}

			// When the data was available we skip waiting and continue to next statement.
		case psProcessCopy:
		{
			// No index request can be out for the asynchronous results at this stage.
			if (_work.IndexReq || _work.CopyReq)
			{
				return setError("Impossible request(s) are still out");
			}
			// When no gates are specified or no dynamic gates is selected all is
			// done for now and the new data can be applied.
			if (_gateCount == 0 || !_dynamicGates)
			{
				return _setState(psApply);
			}
			// Read the index from the result.
			// But initialize the 64bit value first since the read index can be 32bit.
			data_type index = 0;
			if (!_rCopyIndex.blockRead(_work.CopyRange, &index))
			{
				return setError("Copy index result failed");
			}
			// Form the range to get of all gate results.
			_work.GateRange.set({index, index + 1});
			// Run into next statement.
		}

		case psTryGate:
		{
			// The range ID is used here as a flag.
			Range rng(0, std::numeric_limits<Range::size_type>::max(), 0);
			// Iterate through the gate results to retrieve a common accessible range.
			for (int i = 0; i < _gateCount; i++)
			{
				// Fast local references.
				Gate& gi(_gates[i]);
				// Only check on those who are available.
				if (gi.RAmp.getId())
				{ // And the accessible range to create a gate common accessible range.
					rng &= gi.RAmp.getAccessRange();
					// The id is used as a flag.
					rng.setId(1);
				}
				// Only check on those who are available.
				if (gi.RTof.getId())
				{
					// And the accessible range to create a gate common accessible range.
					rng &= gi.RTof.getAccessRange();
					// The id is used as a flag.
					rng.setId(1);
				}
			}
			// Check if a common access range could be generated.
			if (!rng.getId())
			{
				// No gate results were valid at all so just apply what is generated so far.
				return _setState(psApply);
			}

			// Possibly the gates haven't some data yet.
			if (rng.getSize() == 0)
			{
				// Clear the gate event bits before waiting.
				_work.GateAccessEvent.clear();
				// Try this stage again until a timout error is generated.
				return waitForState(psTryGate);
			}
			else
			{
				// Check if the range read from the copy index result is contained by common gate range.
				if (!rng.isInRange(_work.GateRange.getStart()))
				{
					// Try this stage again until a timout error is generated.
					return waitForState(psTryGate);
				}
			}
			// When needed request gate results.
			for (int i = 0; i < _gateCount; i++)
			{
				// Fast local references.
				Gate& gi(_gates[i]);
				// Only check on those AMP's which are available.
				if (gi.RAmp.getId())
				{
					// Check if the range is valid, if not request it.
					if (!gi.RAmp.isRangeValid(_work.GateRange))
					{
						// Make the request.
						if (!gi.RAmp.requestRange(_work.GateRange))
						{
							// Set the error.
							return setError(QString("Request of gate %1 amplitude result failed").arg(i));
						}
						else
						{
							// Set the bit in the mask for this gate number.
							_work.GateAmpReq.set(i);
						}
					}
				}
				// Only check on those TOFs which are available.
				if (gi.RTof.getId())
				{
					// Check if the range is valid, if not request it.
					if (!gi.RTof.isRangeValid(_work.GateRange))
					{
						// Make the request.
						if (!gi.RTof.requestRange(_work.GateRange))
						{
							// Set the error.
							return setError(QString("Request of gate %1 time-of-flight result failed").arg(i));
						}
						else
						{
							// Set the bit in the mask for this gate number.
							_work.GateTofReq.set(i);
						}
					}
				}
			}
			// If requests were made continue with state WAIT.
			if (!_work.GateAmpReq.isClear() && !_work.GateTofReq.isClear())
			{
				// Make the plot wait for an event.
				return waitForState(psWait);
			}
			// Run into next statement.
		}

		case psApply:
		{
			if (_dynamicGates)
			{
				// Iterate through the gate results to retrieve possible range.
				for (unsigned i = 0; i < _gateCount; i++)
				{
					// Fast local references.
					Gate& gi(_gates[i]);
					// Only check if the amplitude result is available.
					if (gi.RAmp.getId())
					{
						gi.FlagAmp = gi.RAmp.blockRead(_work.GateRange, &gi.PeakAmp);
						if (!gi.FlagAmp)
						{
							setError(QString("Read of gate %1 amplitude result failed").arg(i));
						}
					}
					else
					{
						gi.FlagAmp = false;
					}
					// Only check if the time-of-flight result is available.
					if (gi.RTof.getId())
					{
						data_type tof = 0;
						gi.FlagTof = gi.RTof.blockRead(_work.GateRange, &tof);
						if (!gi.FlagTof)
						{
							gi.PeakTof = 0;
							setError(QString("Read of gate %i time-of-flight result failed").arg(i));
						}
						else
						{
							// Convert the unsigned tof data to a signed value.
							gi.PeakTof = gi.RTof.getValue(&tof);
						}
					}
					else
					{
						gi.FlagTof = false;
					}
				}
			}
			// Generate the data for the peak positions on the screen.
			generatePeakData();
			// Generate the A-scan data for the plot based on the screen information.
			if (!generateCopyData(_work.CopyRange))
			{
				setError("Generating copy data failed");
			}
			// Clear work data for next time.
			_work.Clear();
			// Make window update the plot.
			invalidatePlotRect();
			// Signal that plot is ready for next one.
			return _setState(psReady);
		}

		case psWait:
		{
			// When the wait timer timed out generate an error.
			if (_timeoutTimer)
			{
				// Disable timer to prevent reentry.
				_timeoutTimer.disable();
				return setError("Wait timer timed out");
			}
			//
			switch (_stateToWaitFor)
			{
				case psApply:
					// If requests are still out continue waiting.
					if (!_work.GateAmpReq.isClear() || !_work.GateTofReq.isClear())
					{
						break;
					}
					return _setState(psApply);

				case psProcessCopy:
					// If requests are still out continue waiting.
					if (_work.CopyReq || _work.IndexReq)
					{
						break;
					}
					return _setState(psProcessCopy);

				case psTryGate:
					// Continue as long as no gate access changes happened.
					if (_work.GateAccessEvent.isClear())
					{
						break;
					}
					// Try the state again.
					return _setState(psTryGate);

				default:
					// Cannot wait for any other state.
					return setError("Impossible state to wait on");
			}
			break;
		}

	}
	return true;
}

void AcquisitionControl::Private::CopyResHandler(ResultData::EEvent event, const ResultData& caller, ResultData& link, const Range& rng, bool sameInst)
{
	Q_UNUSED(sameInst)
	switch (event)
	{
		default:
			break;

		case ResultData::reInvalid:
			invalidatePlotRect();
			break;

		case ResultData::reIdChanged:
		{
			// Set the bottom ruler. It is possible that it is dependent on the block size.
			if (&link == &_rCopyData)
			{
				setBottomRuler();
			}
			//
			if (&link == &_rCopyIndex)
			{
				// When the copy index is not present dynamic gates can not be presented.
				_dynamicGates = _rCopyIndex.getId() ? true : false;
				// Can only draw peaks when index data is of the correct type.
				_flagCanDrawPeak = link.getTypeSize() == ResultData::rtInt32 && link.getBlockSize() == 1;
			}
			// Update the left ruler when the copy result changes.
			if (&link == &_rCopyData)
			{
				setLeftRuler();
				_flagGateVerticalPos = true;
			}
			break;
		}

		case ResultData::reClear:
		{
			// Clear the points array so no plot is drawn.
			_polygon.clear();
			_lastRange.clear();
			// TODO: Set some default peak positions.
			_flagNoData = true;
			//
			invalidatePlotRect();
			// Clear the work data structure to start over.
			_work.Clear();
			_setState(psIdle);
			break;
		}

		case ResultData::reAccessChange:
			if (rng.getSize())
			{
				_flagNoData = false;
				// Trigger the generation of new plot data.
				if (_stateCurrent == psIdle || _stateCurrent == psReady)
				{
					_setState(psGetCopy);
				}
				//
				if ((!_dynamicGates && &link == &_rCopyData) ||
					(_dynamicGates && &link == &_rCopyIndex))
				{
					// Trigger the generation of new plot data.
					if (_stateCurrent == psIdle || _stateCurrent == psReady)
					{
						_setState(psGetCopy);
					}
				}
			}
			break;

		case ResultData::reGotRange:
		{
			if (&link == &_rCopyIndex)
			{ // Was a got range event expected?
				if (_work.IndexReq)
				{ // Was this the expected range.
					if (rng == _work.CopyRange)
					{ // Request was received.
						_work.IndexReq = false;
						// If this fixes up all async result requests continue processing.
						if (_stateCurrent == psWait)
						{
							processState();
						}
					}
					else SF_RTTI_NOTIFY(DO_DEFAULT, "Expected range is not the same as the gotten range!");
				}
				else SF_RTTI_NOTIFY(DO_DEFAULT, "Did not expect a gotten range!");
			}
			else if (&link == &_rCopyData)
			{ // Was a got range event expected?
				if (_work.CopyReq)
				{ // Was this the expected range.
					if (rng == _work.CopyRange)
					{ // Request was received.
						_work.CopyReq = false;
						// If this fixes up all async result requests continue processing.
						if (_stateCurrent == psWait)
						{
							processState();
						}
					}
					else
					{
						SF_RTTI_NOTIFY(DO_DEFAULT, "Expected range is not the same as the gotten range!");
					}
				}
				else
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "Did not expect a gotten range!");
				}
			}
			break;
		}
	}
}

void AcquisitionControl::Private::GateResHandler(ResultData::EEvent event, const ResultData& caller, ResultData& link, const Range& rng, bool sameInst)
{
	Q_UNUSED(sameInst)
	// Get info pointer from assigned data set in the constructor.
	Gate& gi(*link.getData<Gate*>());
	//
	switch (event)
	{
		default:
			break;

		case ResultData::reAccessChange:
			// Set the access event bit for this gate.
			_work.GateAccessEvent.set(gi.Gate);
			// TODO: Could call ProcessState here when some conditions are met.
			break;

		case ResultData::reGotRange:
		{
			if (&link == &gi.RAmp)
			{
				// Was a got range event expected?
				if (_work.GateAmpReq.has(gi.Gate))
				{
					// Was this the expected range.
					if (rng == _work.GateRange)
					{
						// Request was received.
						_work.GateAmpReq.reset(gi.Gate);
						// Make the plot try to process the data so far.
						if (_stateCurrent == psWait)
						{
							processState();
						}
					}
					else
					{
						SF_RTTI_NOTIFY(DO_DEFAULT, "Expected range is not the same as the gotten range!");
					}
				}
				else
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "Did not expect a gotten range!");
				}
			}
			else if (&link == &gi.RTof)
			{
				// Was a got range event expected?
				if (_work.GateTofReq.has(gi.Gate))
				{
					// Was this the expected range.
					if (rng == _work.GateRange)
					{
						// Request was received.
						_work.GateTofReq.reset(gi.Gate);
						// Make the plot try to process the data so far.
						if (_stateCurrent == psWait)
						{
							processState();
						}
					}
					else
					{
						SF_RTTI_NOTIFY(DO_DEFAULT, "Expected range is not the same as the gotten range!");
					}
				}
				else
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "Did not expect a gotten range!");
				}
			}
		}
			break;
	}
}

void AcquisitionControl::Private::RulerHandler(Variable::EEvent event, const Variable& callVar, Variable& linkVar, bool sameInst)
{
	Q_UNUSED(sameInst);
	switch (event)
	{
		default:
			break;

		case Variable::veIdChanged:
		case Variable::veConverted:
		{
			// Set the FFlagCanDraw if possible.
			setCanDraw();
			// Set the bottom ruler depending on the hooked variables.
			setBottomRuler();
			break;
		}

		case Variable::veValueChange:
		{
			setBottomRuler();
			// Recalculate gate positions on screen.
			_flagGateVerticalPos = true;
			_flagGateHorizontalPos = true;
			// Tcg must be recalculated.
			_tcg.RecalculateFlag = true;
			break;
		}
	}
}

void AcquisitionControl::Private::DefaultVarHandler(Variable::EEvent event, const Variable& caller, Variable& link, bool sameInst)
{
	Q_UNUSED(sameInst)
	switch (event)
	{
		default:
			break;

		case Variable::veValueChange:
		case Variable::veIdChanged:
		{
			if (&link == &_vGateCount)
			{
				_gateCount = std::min<int>(MaxGates, (int) _vGateCount.getCur().getInteger());
				// Recalculate gate positions on screen.
				_flagGateHorizontalPos = true;
				_flagGateVerticalPos = true;
			}
			break;
		}
	}
}

void AcquisitionControl::Private::TcgVarHandler(Variable::EEvent event, const Variable&, Variable& link, bool sameInst)
{
	Q_UNUSED(sameInst)
	switch (event)
	{
		default:
			break;

		case Variable::veLinked:
		case Variable::veIdChanged:
		case Variable::veValueChange:
		{
			//
			if (&link == &_vTcgSlavedTo)
			{
				_tcg.SlavedTo = (int) _vTcgSlavedTo.getCur().getInteger();
			}
			//
			if (&link == &_vTcgEnable)
			{
				generateTcgData(-1);
				invalidatePlotRect();
			}
			else
			{
				// Make the tcg regenerate itself.
				_tcg.RecalculateFlag = true;
			}
			//if (link.Data == UINT_MAX) GenerateTcgData(-1);
			//else GenerateTcgData(link.Data);
			break;
		}

	}
}

void AcquisitionControl::Private::GateVarHandler(Variable::EEvent event, const Variable& caller, Variable& link, bool sameInst)
{
	Q_UNUSED(sameInst)
	// Cast the Data property of the variable to the local gate entry.
	auto gt = link.getData<Gate*>();
	switch (event)
	{
		default:
			break;

		case Variable::veValueChange:
		case Variable::veIdChanged:
		{
			// Is the slaved to parameter changed.
			if (&link == &gt->VSlavedTo)
			{
				// Set the value to not slaved by default.
				gt->SlavedTo = -1;
				// Is the ID valid or not.
				if (gt->VSlavedTo.getId())
				{
					// Check if this gate is slaved to another gate.
					gt->SlavedTo = static_cast<int>(gt->VSlavedTo.getCur().getInteger());
					// Check if the slaved gate is in the allowed arrays range.
					if (gt->SlavedTo >= MaxGates)
					{
						gt->SlavedTo = -1;
					}
				}
			}
			// See which function must be called.
			if (&link == &gt->VThreshold)
			{
				// Recalculate vertical positions.
				_flagGateVerticalPos = true;
			}
			else
			{
				// Recalculate horizontal positions onscreen.
				_flagGateHorizontalPos = true;
			}
			// When it is the Interface gate and the TCG is artificially slaved.
			if (gt->Gate == 0 && _tcg.SlavedTo == -2)
			{
				// Make the TCG curve be recalculated.
				_tcg.RecalculateFlag = true;
			}
			break;
		}
	}
}

AcquisitionControl::Private::EGrip AcquisitionControl::Private::GetGateGrip(const QPoint& point, int* gate)
{
	// Initialize the return values.
	EGrip rv = gNONE;
	int gt = -1;
	// Search for a grip in the list of grips to return.
	for (int i = 0; i < _gateCount; i++)
	{
		// Fast local reference.
		QRect& rc(_gates[i].GripRect);
		// Check whether the cursor in this gate area.
		if (rc.contains(point))
		{
			gt = i;
			// Divide in three section. Left middle and right.
			int w = rc.width() / 3;
			// When zero default to the middle grip.
			if (!w)
			{
				rv = gMIDDLE;
			}
			else
			{
				if (point.x() >= rc.left() && point.x() < rc.left() + w)
				{
					rv = gLEFT;
				}
				else if (point.x() >= rc.right() - w && point.x() < rc.right())
				{
					rv = gRIGHT;
				}
				else
				{
					rv = _thresholdDrag ? gMIDDLE : gRIGHT;
				}
			}
		}
	}
	// Check if pointer was set.
	if (gate)
	{
		*gate = gt;
	}
	// If non was grabbed return that as well.
	return rv;
}

Qt::CursorShape AcquisitionControl::Private::getCursorShape(EGrip grip) const
{
	Qt::CursorShape shape;
	// Set the cursor according to which grip was select.
	switch (grip)
	{
		default:
		case gNONE:
			shape = Qt::CursorShape::ArrowCursor;
			break;

		case gLEFT:
			shape = Qt::CursorShape::SplitHCursor;
			break;

		case gMIDDLE:
			shape = Qt::CursorShape::SplitVCursor;
			break;

		case gRIGHT:
			shape = Qt::CursorShape::SizeHorCursor;
			break;
	}
	return shape;
}

void AcquisitionControl::Private::setCursorShape(Qt::CursorShape shape) const
{
	// Reduce overhead, only when the shape should really change.
	if (_w->cursor().shape() != shape)
	{
		auto cc = _w->cursor();
		cc.setShape(shape);
		_w->setCursor(cc);
	}
}

void AcquisitionControl::Private::geoResize(const QSize &size, const QSize &previousSize)
{
	if (_flagCanDraw)
	{
		_graph.setBounds(QFontMetrics(_w->font()), QRect(QPoint(0, 0), size));
		generatePeakData();
		generateCopyData(_lastRange);
		generateTcgData();
		// Set gate positions from generic variables.
		_flagGateVerticalPos = true;
		_flagGateHorizontalPos = true;
		// Redraw the plot client area.
		invalidatePlotRect();
	}
}

void AcquisitionControl::Private::mouseMove(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPoint point)
{
	Q_UNUSED(button);
	Q_UNUSED(modifiers);
	// Correct for the possible rulers offset.
	auto pt = point - _graph.getPlotArea().topLeft();
	// Check if sizing.
	if (!_flagSizing)
	{
		// Set the current grip.
		EGrip grip = GetGateGrip(pt);
		// Set the cursor according to the grip.
		setCursorShape(getCursorShape(grip));
		// TODO: Should be done in separate function.
		if (grip == gNONE && !_tcg.Grips.isEmpty())
		{
			// While dragging the gate the _tcg get corrupted.
			for (auto& g: _tcg.Grips)
			{
				if (g.contains(pt))
				{
					setCursorShape(Qt::CursorShape::DragMoveCursor);
				}
			}
		}
	}
	else
	{
		// Can only drag when frozen.
		if (_flagFrozen)
		{
			// Update the hints position.
			_infoWindow->setPosition(_w->mapToGlobal(pt));
			// Hide any existing hint window.
			QToolTip::hideText();
			// Calculate the movement offset.
			_gripOffset = asQSize(pt - _grabPoint);
			QRect updateRect = _gripRectNext;
			Gate& gt(_gates[_gripGate]);
			gt.GripRect.moveTo(gt.GripOffset + _gripOffset);
			_gripRectNext = gt.GripRect;
			updateRect |= _gripRectNext;
			// Add extra 1 pixel to invalidate.
			invalidatePlotRect(inflated(updateRect, 1));
		}
	}
}

void AcquisitionControl::Private::mouseDown(Qt::MouseButton button, Qt::KeyboardModifiers modifier, QPoint point)
{
	Q_UNUSED(modifier)
	if (!_w->hasMouseTracking())
	{
		return;
	}
	// Correct for the possible rulers offset.
	auto pt = point - _graph.getPlotArea().topLeft();
	// Check for the correct button to be pressed.
	if (button == Qt::MouseButton::LeftButton)
	{
		// Always capture the mouse input when the left button is down.
		mouseCapture(true);
		if (!_debug && _w->hasMouseTracking())
		{
			// Freeze current values for the plot.
			_flagFrozen = true;
		}
		// Get the current left point for grabbing.
		_grabPoint = pt;
		// Look for a grip area.
		_gripGrabbed = GetGateGrip(_grabPoint, &_gripGate);
		//
		if (_gripGrabbed != gNONE )
		{
			// Set the sizing flag to true.
			_flagSizing = true;
			// Clear the current offset.
			_gripOffset = {0, 0};
			// Set the new grip member to the first grabbed rect so that
			// part of the plot is updated when moving.
			_gripRectNext = _gates[_gripGate].GripRect;
			// Get the offset of the grip rect when grabbed.
			_gates[_gripGate].GripOffset = _gripRectNext.topLeft();
		}
		// Set the position and offset of the hint window.
		_infoWindow->setPosition(_w->mapToGlobal(pt));
		_infoWindow->setOffset(QPoint(15, 10));
		// Get the name of the gate number which was gripped.
		QString gateName = _gripGate ? QString("Gate %1").arg(_gripGate) : QString("IF Gate");
		// Show the hint window which gate is grabbed.
		switch (_gripGrabbed)
		{
			case gNONE:
				_infoWindow->setText("");

			case gMIDDLE:
				_infoWindow->setText("Threshold " + gateName);
				break;

			case gLEFT:
				_infoWindow->setText("Position " + gateName);
				break;

			case gRIGHT:
				_infoWindow->setText("Size " + gateName);
				break;
		}
		// Activate the gate hint.
		_infoWindow->setActive(true);
	}
}

void AcquisitionControl::Private::mouseUp(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPoint point)
{
	Q_UNUSED(modifiers)
	if (!_w->hasMouseTracking())
	{
		return;
	}
	// Correct for the possible rulers offset.
	auto pt = point - _graph.getPlotArea().topLeft();
	// Check for the left mouse button getting up.
	if (button == Qt::MouseButton::LeftButton)
	{
		// After the first move change the cursor for easier positioning.
		setCursorShape(getCursorShape(GetGateGrip(pt)));
		// Disable the capturing of the mouse.
		mouseCapture(false);
		// Deactivate the hint window.
		_infoWindow->setActive(false);
		// Unfreeze the plot.
		if (!_debug)
		{
			_flagFrozen = false;
		}
		//
		if (_flagSizing)
		{
			// Set the variables according to the rectangles on the screen.
			switch (_gripGrabbed)
			{
				default:
					break;

				case gMIDDLE:
					_gates[_gripGate].Rect += QPoint(0, _gripOffset.height());
					_gripRectNext = _gates[_gripGate].Rect;
					_flagGateVerticalPos = true;
					setGateVerticalPos(true);
					break;

				case gLEFT:
					_gates[_gripGate].Rect += QPoint(_gripOffset.width(), 0);
					_gripRectNext = _gates[_gripGate].Rect;
					setGateHorizontalPos(true);
					break;

				case gRIGHT:
					_gates[_gripGate].Rect.setRight(_gates[_gripGate].Rect.right() + _gripOffset.width());
					// Do not allow negative width value.
					if (_gates[_gripGate].Rect.width() < 0)
					{
						_gates[_gripGate].Rect.setRight(_gates[_gripGate].Rect.left());
					}
					//
					_gripRectNext = _gates[_gripGate].Rect;
					setGateHorizontalPos(true);
					break;
			}
			// Reset the sizing flag.
			_flagSizing = false;
			//
			invalidatePlotRect();
		}
	}
}

void AcquisitionControl::Private::focus(bool yn)
{
	if (yn)
	{
		_w->setMouseTracking(true);
	}
	else
	{
		_w->setMouseTracking(false);
		setCursorShape(Qt::ArrowCursor);
		if (!_debug)
		{
			// Always unfreeze the plot on focus exit.
			_flagFrozen = false;
		}
	}

}

void AcquisitionControl::Private::keyDown(int key, Qt::KeyboardModifiers modifiers)
{
	Q_UNUSED(key)
	// Freeze the plot when the control key is pressed down.
	if (modifiers.testFlag(Qt::KeyboardModifier::ControlModifier))
	{
		_flagFrozen = !_flagFrozen;
	}
}

bool AcquisitionControl::Private::draw(QPainter& painter, const QRect& bounds, const QRegion& region) // NOLINT(readability-make-member-function-const)
{
	// Check if there is anything to be painted.
	if (_flagCanDraw)
	{
		// Offset for drawing the TCG part.
		QPoint tcg_ofs(0, 0);
/*
		//
		if (FTcg.SlavedTo == 0 && FGateCount)
		{
			tcg_ofs.setX(FNodataFlag ? FGates[0].Rect.left() : FGates[0].PeakPos.x());
		}
		// Get local copies to modify and to calculate with.
		QRect rc_left(bounds);
		QRect rc_right(bounds);
		// Modify rectangles
		rc_left.setRight(FTcg.AreaLeft + tcg_ofs.x());
		rc_right.setLeft(FTcg.AreaRight + tcg_ofs.x());
		//
		//painter.fillRect(bounds, FColorBackground);
		painter.fillRect(rc_left, FColorBackground);
		painter.fillRect(rc_right, FColorBackground);
		painter.fillRect(QRect(rc_left.topRight(), rc_right.bottomLeft()), FColorTcgRange);
*/
/*
		// Check, if the grid lines must be drawn at all by checking the color value.
		if (FColorGridLines.isValid() && FColorGridLines.alpha() > 0 && bounds.width() > 0)
		{
			painter.setPen(FColorGridLines);
			painter.drawLine(bounds.topLeft(), bounds.bottomRight());
			//
			int width = bounds.width();
			int height = bounds.height();
			// Skip the first line when the adjustment line is enabled.
			int i = (FColorAdjustmentLine.isValid()) ? 2 : 1;
			// Draw the vertical lines.
			for (; i < 10; i++)
			{
				// Calculate an offset position on fixed 10% of the width.
				QPoint ofs((width * i) / 10, 0);
				painter.drawLine(bounds.topLeft() + ofs, bounds.bottomLeft() + ofs);
			}
			// Draw the horizontal lines.
			for (i = 1; i < 10; i++)
			{
				// Calculate an offset position on fixed 10% of the height.
				QPoint ofs(0, (height * i) / 10);
				painter.drawLine(bounds.topLeft() + ofs, bounds.topRight() + ofs);
			}
		}
*/
		//
		// Draw TCG only when available.
		//
		if (_tcg.Points.count())
		{
			painter.save();
			// Select TCG pen color.
			painter.setPen(_colorTcgLine);
			// Only make a call to offset when there is one.
			if (tcg_ofs.x())
			{
				painter.translate(-tcg_ofs);
			}
			// Draw the poly line.
			painter.drawPolyline(_tcg.Points.data(), static_cast<int>(_tcg.Points.size()));
			// Draw then TCG grip rectangles on the line.
			for (auto & g : _tcg.Grips)
			{
				painter.fillRect(g, _colorTcgLine);
			}
			// Only make a call to restore when there was an offset set.
			painter.restore();
		}
		//
		// Draw the a-scan line when coords are available.
		//
		if (!_polygon.empty())
		{
			painter.setPen(_colorForeground);
			painter.drawPolyline(_polygon);
			// Scale peak values in the plot area.
			for (int i = 0; i < _gateCount; i++)
			{
				// Fast local reference.
				Gate& gi(_gates[i]);
				// Check if TOF value was present.
				if (gi.FlagTof)
				{
					QSize sz(6, 0);
					painter.setPen(gi._color);
					painter.drawLine(gi.PeakPos + sz, gi.PeakPos - sz);
					painter.drawLine(gi.PeakPos.x(), bounds.top(), gi.PeakPos.x(), bounds.bottom());
				}
			}
		}
		else
		{
			return false;
		}
		//
		// Draw the gate indicators.
		//
		for (int i = 0; i < (int) _gateCount; i++)
		{
			// Fast local reference.
			Gate& gt(_gates[i]);
			// When sizing the plot is frozen, so we need to use the previous information and the changed info from the dragged gate.
			if (!_flagSizing)
			{
				// Update the grip rect with the starting rectangle.
				gt.GripRect = gt.Rect;
				// Put rectangle on dynamic or static location when slaved.
				if (gt.SlavedTo >= 0)
				{
					Gate& gts(_gates[gt.SlavedTo]);
					// Calculation to get the current slaved gate position using the static positions to start with.
					int xOfs = gts.PeakPos.x();
					// Position the gate rectangle exactly where it should be located.
					gt.GripRect += QPoint(xOfs, 0);
				}
			}
			// Rectangle to work with.
			QRect grc = gt.GripRect;
			// Decrease by 1 so the line is not drawn outside the rectangle.
			grc.setRight(grc.right() - 1);
			// Set the gate color for drawing.
			painter.setPen(gt._color);
			// Draw the gate beginning.
			painter.drawLine(grc.topLeft(), grc.bottomLeft());
			// Draw the gate ending.
			painter.drawLine(grc.topRight(), grc.bottomRight());
			// Middle of the rectangle.
			QSize sz(0, grc.size().height() / 2);
			// Draw the gate line
			painter.drawLine(grc.topLeft() + sz, grc.topRight() + sz);
			// Only for gate 0 which is the interface gate.
			if (gt.TrackWidth >= 0)
			{
				int width = gt.Rect.width();
				int x1 = -gt.TrackWidth / 2;
				x1 += gt.PeakPos.x() - gt.Rect.left();
				// Clip the position.
				x1 = clip(x1, 0, width - gt.TrackWidth);
				// Decrease by 1 so the line is not drawn outside the rectangle.
				int x2 = x1 + gt.TrackWidth - 1;
				painter.drawLine(grc.topLeft() + QPoint(x1, 0), grc.bottomLeft() + QPoint(x1, 0));
				painter.drawLine(grc.topLeft() + QPoint(x2, 0), grc.bottomLeft() + QPoint(x2, 0));
			}
		}
		//
		// Check if the adjustment line must be drawn at all by checking the color value.
		//
		if (_colorAdjustmentLine.isValid() && bounds.width() > 0)
		{
			QPen pen(_colorAdjustmentLine);
			pen.setStyle(Qt::DashDotLine);
			painter.setPen(pen);
			// Calculate an offset position on fixed 10% of the width.
			QPoint ofs(bounds.width() / 10, 0);
			painter.drawLine(bounds.topLeft() + ofs, bounds.bottomLeft() + ofs);
		}
		// Draw some debugging stuff.
		if (_debug)
		{
			QColor color(QColorConstants::White);
			color.setAlpha(60);
			for (int i = 0; i < _gateCount; i++)
			{
				painter.fillRect(inflated(_gates[i].GripRect, 1), color);
			}
		}
		return true;
	}
	// Make the plot paint itself when designing.
	return false;
}

}
