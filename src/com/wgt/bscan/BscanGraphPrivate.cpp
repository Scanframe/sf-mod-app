#include "BscanGraphPrivate.h"
#include <QImage>
#include <misc/gen/Sustain.h>
#include <misc/gen/TSet.h>
#include <misc/gen/dbgutils.h>
#include <misc/qt/Draw.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

constexpr Qt::KeyboardModifier ModifierList[] = {
	Qt::MetaModifier,
	Qt::ShiftModifier,
	Qt::ControlModifier,
	Qt::AltModifier
};

constexpr Qt::MouseButton MouseButtonList[] = {
	Qt::LeftButton,
	Qt::RightButton,
	Qt::MiddleButton
};

Value::int_type toBitMask(Qt::KeyboardModifiers modifiers, Qt::MouseButtons buttons, bool buttonDown)
{
	TSet<Value::int_type, Value::int_type> mask;
	// First bit signals down.
	if (buttonDown)
	{
		mask.set(0);
	}
	int i = 0;
	for (auto m: ModifierList)
	{
		i++;
		if (modifiers.testFlag(m))
		{
			mask.set(i);
		}
	}
	for (auto b: MouseButtonList)
	{
		i++;
		if (buttons.testFlag(b))
		{
			mask.set(i);
		}
	}
	//
	return mask._bits;
}

// TODO: Must be moved to a library eventually.
bool bitMaskToButtonDown(Value::int_type bitMask)
{
	// Test the fist bit for the button down.
	return (bitMask & 0x1) < 0;
}

// TODO: Must be moved to a library eventually.
QPair<Qt::KeyboardModifiers, Qt::MouseButtons> bitMaskToModifiers(Value::int_type bitMask)
{
	TSet<Value::int_type, Value::int_type> mask(bitMask);
	Qt::KeyboardModifiers modifiers;
	Qt::MouseButtons buttons;
	int i = 0;
	for (auto m: ModifierList)
	{
		i++;
		if (mask.has(i))
		{
			modifiers.setFlag(m);
		}
	}
	for (auto b: MouseButtonList)
	{
		i++;
		if (mask.has(i))
		{
			buttons.setFlag(b);
		}
	}
	return {modifiers, buttons};
}

BscanGraph::Private::Private(BscanGraph* widget)
	: _w(widget)
	, _graph(widget->palette())
	, _paletteServer()
	, _sustain{this, &BscanGraph::Private::sustain, SustainBase::spTimer}
{
	// Make the widget get focus when clicked in.
	_w->setFocusPolicy(Qt::StrongFocus);
	// Also, when using the keyboard.
	_w->setAttribute(Qt::WA_KeyboardFocusChange);
	// Do not automatically fil the background.
	_w->setAttribute(Qt::WA_NoSystemBackground);
}

BscanGraph::Private::~Private()
{
	// Release handlers from index and data results.
	// Prevents crashing because of the right order of instances destruction.
	_requester.release();
}

void BscanGraph::Private::initialize()
{
	for (auto v: {&_vTimeUnit, &_vTimeDelay, &_vTimeRange})
	{
		v->setHandler(&_handlerVariable);
	}
	// Hook the scan variables.
	for (auto r: {&_scan.VDelay, &_scan.VRange, &_scan.VLeft, &_scan.VRight})
	{
		r->setHandler(&_handlerScan);
	}
	for (auto r: {&_cursor.VPopIndex, &_cursor.VFraction, &_cursor.VIndex, &_cursor.VEvent})
	{
		r->setHandler(&_handlerMarker);
	}
	// Set result handler to request instance which will route the events to the
	_requester.setHandler(&_handlerResultData);
	// Bind the index result to the request handler object.
	_requester.attachIndex(&_rIndex);
	// Bind the data result to the request handler object.
	_requester.attachData(&_rData);
	//
	connect(&_paletteServer, &PaletteServer::changed, [&]() {
		if (_rcPalette.isValid())
		{
			_w->update(_rcPalette);
		}
		_w->update(_graph.getPlotArea());
	});
	//
	setRulerLeft();
	setRulerBottom();
}

void BscanGraph::Private::setRulerLeft()
{
	QString unit = "deg";
	Value::flt_type start = 0;
	Value::flt_type stop = 360;
	Value::flt_type step = 10;
	_graph.setRuler(sf::Draw::roLeft, start, stop, requiredDigits(step, start, stop), unit);
}

void BscanGraph::Private::setRulerBottom()
{
	// Get the unit of the A-scan.
	QString unit = "%";
	Value::flt_type start = 0.0;
	Value::flt_type range = 100.0;
	Value::flt_type step = 1.0;
	// When data is available.
	if (_rData.getId())
	{
		// When the range of the copy result id is specified the value is used to calculate the range of graph.
		if (_vTimeRange.getId())
		{
			// Set the converted start value for the start value but when the
			// delay variable is present it is assumed to be zero.
			start = _vTimeRange.convert(_vTimeDelay.getCur()).getFloat();
			// Get the converted range value.
			range = _vTimeRange.getCur(true).getFloat();
			// Set the converted unit
			unit = QString::fromStdString(_vTimeRange.getUnit(true));
		}
		// When range variable is NOT present and time unit is.
		// Then use block size and time unit to establish the range.
		else if (_vTimeUnit.getId())
		{
			// When the delay parameter is present it is used to for conversion as well.
			if (_vTimeDelay.getId())
			{
				// Set the converted start value.
				start = _vTimeDelay.getCur(true).getFloat();
				// Get the range of the A-scan by getting the block size.
				range = _vTimeDelay.convert(_vTimeUnit.getCur()).getFloat() * static_cast<Value::flt_type>(_rData.getBlockSize());
				// Set the converted unit
				unit = QString::fromStdString(_vTimeDelay.getUnit(true));
			}
			else
			{
				// Delay variable is not present so the time unit is used for conversion.
				// Get the range of the A-scan by getting the block size.
				range = _vTimeUnit.getCur(true).getFloat() * static_cast<Value::flt_type>(_rData.getBlockSize());
				// Set the converted unit
				unit = QString::fromStdString(_vTimeUnit.getUnit(true));
			}
		}
		// Only able to use block size to set range.
		// Use the copy data block size to fill the bottom ruler.
		else
		{
			// If Delay exists the value is presumed to be an offset in samples.
			// When delay is not available the value is zero.
			start = _vTimeDelay.getCur().getFloat();
			range = static_cast<Value::flt_type>(_rData.getBlockSize());
			unit = "x";
		}
	}
	_graph.setRuler(sf::Draw::roBottom, start, start + range, requiredDigits(step, start, range), unit);
}

void BscanGraph::Private::updatePlotArea(const QRect& rect) const
{
	// When debugging invalidate it all.
	if (!rect.isEmpty())
	{
		// Update rectangle withing the plot area by adding the offset.
		_w->update(rect + _graph.getPlotArea().topLeft());
	}
}

void BscanGraph::Private::handlerVariable(VariableTypes::EEvent event, const Variable& caller, Variable& link, bool sameInst)
{
	Q_UNUSED(sameInst)
	switch (event)
	{
		default:
			break;

		case Variable::veIdChanged:
		case Variable::veValueChange:
		case Variable::veConverted: {
			setRulerBottom();
			// Redraw the ruler by updating the whole area.
			_w->update(_area);
			break;
		}
	}
}

void BscanGraph::Private::handlerResultData(ResultDataTypes::EEvent event, const ResultData& caller, ResultData& link, const Range& range, bool sameInst)
{
	Q_UNUSED(sameInst)
	switch (event)
	{
		default:
			break;

		case ResultData::reIdChanged: {
			// Check if drawing is possible.
			setCanDraw();
			break;
		}

		case ResultData::reClear: {
			// Makes sure that no new data is drawn until an access change event has passed.
			_rangeNext.clear();
			reinitialize(true);
			break;
		}

		case ResultData::reAccessChange:
			if (&link == &_rIndex)
			{
				if (!_cursor.Active)
				{
					// When the control is covered updating must be skipped.
					if (!_w->visibleRegion().isEmpty())
					{
						// Just make the requester gets it data.
						if (getData(range))
						{
							break;
						}
					}
				}
				// If failing to do so make the sustain or draw wait for it.
				_skippedAccessRange = range;
			}
			break;

		case (ResultData::EEvent) ResultDataRequester::reDataValid: {
			//SF_RTTI_NOTIFY(DO_CLOG, "Event reDataValid " << range);
			// The requested index range is passed in 'rng' and is assigned to the new plot range.
			_rangeNext = range;
			// Enable the timer since a new range can be generated.
			_generateTimer.enable();
			break;
		}
	}
}

bool BscanGraph::Private::draw(QPainter& painter, const QRect& rect, const QRegion& region)
{
	// Check if there is anything to be painted.
	if (_flagCanDraw)
	{
		// Window not visible, do nothing, wait until it is visible !
		if (_w->visibleRegion().isEmpty())
		{
			return true;
		}
		// Reset the timer for the next image to be generated.
		_generateTimer.reset();
	}
	else
	{
		return false;
	}
	// Were any access changes skipped during an active cursor.
	if (!_cursor.Active && !_skippedAccessRange.isEmpty())
	{
		// Try the request again here.
		if (getData(_skippedAccessRange))
		{
			// On success prevent reentry be clearing the range.
			_skippedAccessRange.clear();
		}
	}
	// Is there any data to draw.
	if (_rangeCurrent.getSize())
	{
		// Draw the image into the graph area.
		if (!_image.isNull())
		{
			// Check if the palette server has an implementation.
			if (_paletteServer.isAvailable())
			{
				// Convert the image to an 8-bit indexed format and set the color table for each index.
				auto img = _image.convertToFormat(QImage::Format_Indexed8);
				img.setColorTable(_paletteServer.getColorTable());
				painter.drawImage(rect, img);
			}
			else
			{
				painter.drawImage(rect, _image);
			}
		}
		// Now we draw other things
		drawMark(painter, _mark);
		return true;
	}
	return false;
}

bool BscanGraph::Private::generateData(const Range& range)
{
	// Read the data range to be accessed and check if it was valid before continuing.
	if (_rIndex.readIndexRange(range, _dataRange) && _rData.isRangeValid(_dataRange))
	{
		// Prevent problems when generating too much data cause by corrupted data.
		if (_dataRange.getSize() > MaxScanSize)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Lines size exceeds the maximum allowed scan size of " << MaxScanSize << "!")
			return false;
		}
	}
	// Vertical zoom into the range.
	Range rng(
		_dataRange.getStart() + calculateOffset(_scan.Left, 0.0, 1.0, _dataRange.getSize(), true),
		_dataRange.getStart() + calculateOffset(_scan.Right, 0.0, 1.0, _dataRange.getSize(), true)
	);
	// Adjust the data buffer size to hold the image data.
	_imageData.grow(_rData.getBlockSize() * (rng.getSize() + 1));
	// Create new image.
	QImage img(static_cast<const uchar*>(_imageData.data()), static_cast<int>(_rData.getBlockSize()), static_cast<int>(rng.getSize()), static_cast<qsizetype>(_rData.getBlockSize()), QImage::Format_Grayscale8);
	// Swap the old with the new image.
	_image.swap(img);
	// Fill the dib data buffer at the specified position.
	if (!_rData.blockRead(rng, _imageData.data()))
	{
		// Notify an error which actually cannot occur.
		SF_RTTI_NOTIFY(DO_CLOG, "Retrieving data range " << rng << " !");
		return false;
	}
	return true;
}

bool BscanGraph::Private::sustain(const timespec& ts)
{
	// Check if it is time to generate when there is a next range for it.
	if (_generateTimer(ts))
	{
		// Do not generate new data until enabled to do so when the data is drawn.
		_generateTimer.disable();
		// Check if a next range to generate is valid for data generation.
		if (_rangeCurrent != _rangeNext)
		{
			// Generate the new data from the range.
			if (generateData(_rangeNext))
			{
				// Update the current range also used as a sentry.
				_rangeCurrent = _rangeNext;
				// Invalidate plot area.
				_w->update(_graph.getPlotArea());
			}
		}
	}
	// Were any access changes skipped during an active cursor.
	if (!_cursor.Active)
	{
		if (!_skippedAccessRange.isEmpty())
		{
			// Try the request again here.
			if (getData(_skippedAccessRange))
			{
				// On success prevent reentry be clearing the range.
				_skippedAccessRange.clear();
			}
		}
	}
	else
	{
		// Handle changes of the cursor pop index.
		if (!_cursor.SkippedPopIndex.isEmpty())
		{
			// Try the request again here.
			if (getData(_cursor.SkippedPopIndex))
			{
				// On success prevent reentry be clearing the range.
				_cursor.SkippedPopIndex.clear();
			}
		}
	}
	// Check if the parameters of the mark must be updated.
	if (_mark.Update)
	{
		calculatePosIndexAt(_mark);
	}
	//
	return true;
}

void BscanGraph::Private::handlerMarkVariable(Variable::EEvent event, const Variable& caller, Variable& link, bool sameInst)
{
	switch (event)
	{
		default:
			break;

		case Variable::veIdChanged:
		case Variable::veValueChange: {
			// Prevent looping when even came from within.
			if (!sameInst)
			{
				// The index has changed
				if (&link == &_cursor.VIndex)
				{
					// Keep local copy
					_cursor.Index = _cursor.VIndex.getCur().getInteger();
					break;
				}
				//-----------------------------------
				// Common calculations.
				//-----------------------------------
				// Get B-scan area rectangle.
				QRect rc(_graph.getPlotArea());
				// Calculate cursor point for passing to the event function.
				QPoint pt(rc.width() / 2, calculateOffset(_cursor.VFraction.getCur().getFloat(), _scan.Left, _scan.Right, rc.height(), true));
				//-----------------------------------
				// Check for a mouse event.
				//-----------------------------------
				if (&link == &_cursor.VEvent)
				{
					// Keep local copy
					_cursor.Event = _cursor.VEvent.getCur().getInteger();
					// Get the shift state from the current mouse click param.
					auto modifiers = bitMaskToModifiers(_cursor.Event);
					// Is event from button down.
					if (bitMaskToButtonDown(_cursor.Event))
					{
						// Check button left button is active.
						if (modifiers.second.testAnyFlags(Qt::LeftButton | Qt::RightButton))
						{
							localMouse(MousePress, modifiers.first, modifiers.second, pt, false);
						}
					}
					else
					{
						localMouse(MouseRelease, modifiers.first, modifiers.second, pt, false);
					}
					break;
				}
				//-----------------------------------
				// The Pop index has changed
				//-----------------------------------
				if (&link == &_cursor.VPopIndex)
				{// Set local copy.
					_cursor.PopIndex = _cursor.VPopIndex.getCur().getInteger();
					if (_cursor.PopIndex < 0)
					{
						_cursor.PopIndex = 0;
					}
					// Set the new plot range which is picked up in the sustain function.
					_cursor.SkippedPopIndex.assign(_cursor.PopIndex, _cursor.PopIndex + 1);
					//
					break;
				}
				//-----------------------------------
				// Check for a ClickFraction event.
				//-----------------------------------
				if (&link == &_cursor.VFraction)
				{
					// Keep local copy
					_cursor.Fraction = _cursor.VFraction.getCur().getFloat();
					if (_cursor.Active)
					{
						localMouse(MouseMove, bitMaskToModifiers(_cursor.Event).first, Qt::MouseButtons(), pt, false);
					}
					break;
				}
			}
			break;
		}
	}
}

void BscanGraph::Private::localMouse(EKeyMouseEvent me, Qt::KeyboardModifiers modifiers, Qt::MouseButtons buttons, const QPoint& pt, bool local)
{
	switch (me)
	{
		case KeyPress:
		case KeyRelease:
			if (_cursor.Active)
			{
				// Set the left button too.
				buttons.setFlag(Qt::LeftButton);
				// Generate a variable value from the shift set.
				_cursor.Event = toBitMask(modifiers, buttons, true);
				// Update the click event param
				_cursor.VEvent.setCur(Value(_cursor.Event), true);
			}
			break;

		case MousePress:
			// Cursor is active on left button
			if (buttons.testFlag(Qt::LeftButton))
			{
				_cursor.Active = true;
				// Draw a circle on the screen.
				mark(pt, true);
			}
			// Only when this is a local call.
			if (local)
			{
				// Generate a variable value from the shift set.
				_cursor.Event = toBitMask(modifiers, buttons, true);
				// Update
				calculatePosIndexAt(_mark);
				// Update the click event param
				_cursor.VEvent.setCur(Value(_cursor.Event), true);
				// Zoom is active on left button + Control key.
				if (buttons.testFlag(Qt::LeftButton) && modifiers.testFlag(Qt::ControlModifier))
				{
					// Mouse is active ( stops scroll )
					_cursor.Active = true;
					// Zoom is active
					_zoomActive = true;
					// Draw zoom box;
					//ZoomBox(pt, true);
				}
			}
			break;

		case MouseMove:
			if (_cursor.Active)
			{
				if (_zoomActive)
				{
					zoomBox(pt, true);
				}
				else
				{
					// Draw a Cursor on the screen.
					mark(pt, true);
				}
				// Only when this is a local call.
				if (local)
				{
					_cursor.Event = toBitMask(modifiers, buttons, true);
					// Update the click event param
					_cursor.VEvent.setCur(Value(_cursor.Event), true);
					// Make variables update from the sustain.
					_mark.Update = true;
				}
			}
			break;

		case MouseRelease:
			if (_cursor.Active)
			{
				_cursor.Active = false;
				if (_zoomActive)
				{
					_zoomActive = false;
					// Deactivate Zoom.
					if (_mark.Active)
					{
						zoomBox(pt, false);
					}
				}
				else
				{
					// Deactivate mark.
					if (_mark.Active)
					{
						mark(pt, false);
					}
				}
			}
			// Only when this is a local call.
			if (local)
			{
				// Update
				calculatePosIndexAt(_mark);
				// When the key is up no events are generated.
				_cursor.Event = toBitMask(modifiers, buttons, false);
				// Update the click event param
				_cursor.VEvent.setCur(Value(_cursor.Event), true);
			}
			break;
	}
}

void BscanGraph::Private::handlerScanVariable(Variable::EEvent event, const Variable& caller, Variable& link, bool sameInst)
{
	Q_UNUSED(sameInst)
	switch (event)
	{
		default:
			break;

		case Variable::veIdChanged:
		case Variable::veValueChange: {
			// ScanDelay has changed
			if (&link == &_scan.VDelay)
			{
				// Do not allow negative values.
				_scan.Delay = clip<Value::int_type>(link.getCur().getInteger(), 0, std::numeric_limits<Value::int_type>::max());
				// Make the draw function call for new data.
				Range rng = _rIndex.getAccessRange();
				// When maximum delay at this time has been reached.
				if (_scan.Delay >= rng.getStop())
				{
					_requester.reset();
					_rangeNext.clear();
					_generateTimer.reset();
				}
				else
				{
					// Trigger update in sustain by setting this member.
					_skippedAccessRange = rng;
				}
				break;
			}
			// ScanRange has changed
			if (&link == &_scan.VRange)
			{
				// If the ID of the variable is present
				if (_scan.VRange.getId())
				{
					// Use that value to assign.
					_scan.Range = _scan.VRange.getCur().getInteger();
				}
				// Reinitialize will set the FScan.Range member.
				reinitialize(true);
				break;
			}
			// ScanLeft has changed
			if (&link == &_scan.VLeft)
			{
				_scan.Left = clip(link.getCur().getFloat(), 0.0, 1.0);
				// Reinitialize and redraw the plot.
				reinitialize(true);
				break;
			}
			// ScanRight has changed
			if (&link == &_scan.VRight)
			{
				if (link.getId())
				{
					_scan.Right = clip(link.getCur().getFloat(), 0.0, 1.0);
				}
				else
				{
					_scan.Right = 1.0;
				}
				// Reinitialize and redraw the plot.
				reinitialize(true);
				break;
			}
			// Reinitialize will set the FScan.Range member.
			reinitialize(true);
			break;
		}
	}
}

bool BscanGraph::Private::setCanDraw()
{
	_flagCanDraw = false;
	// Need the copy delay.
	if (_rData.getId() && _rIndex.getId())
	{
		// Need block size of 1 (single byte)
		if (_rIndex.getBlockSize() == 1 && (_rIndex.getType() == ResultData::rtInt32 || _rIndex.getType() == ResultData::rtInt64))
		{
			// Type of 8-bit allowed for now.
			if (_rData.getType() == ResultData::rtInt8)
			{
				_flagCanDraw = true;
			}
		}
	}
	return _flagCanDraw;
}

void BscanGraph::Private::reinitialize(bool clear)
{
	if (clear)
	{
		// Clear the bitmaps with background color.
		//FDib.clear();
		// Force GenerateData to rebuild array content by clearing PlotRange.
		_rangeCurrent.clear();
		// Also clear the new range.
		_rangeNext.clear();
		// Make the draw function call for new data.
		_skippedAccessRange = _rIndex.getAccessRange();
		// Clear plot before use.
		_w->update(_graph.getPlotArea());
		// Clear the current request.
		_requester.reset();
		// Update rulers with new values.
		setRulerLeft();
		setRulerBottom();
	}
	// Make the plot regenerate its data.
	_generateTimer.reset();
}

void BscanGraph::Private::mark(const QPoint& pt, bool active)
{
	_mark.Active = active;
	// Get plot rectangle.
	QRect rc(_graph.getPlotArea());
	// Make sure the point is clipped within the area of the b-scan.
	_mark.Pt = QPoint(clip(pt.x(), rc.left(), rc.right()), clip(pt.y(), rc.top(), rc.bottom()));
	if (active)
	{
		// Invalidate old rect.
		updatePlotArea(_mark.Rect);
		// Calculate new rect.
		_mark.Rect = inflated({_mark.Pt, _mark.Pt}, _mark.Radius);
		// Invalidate new rect.
		updatePlotArea(_mark.Rect);
	}
	else
	{// Make the plot redraw the affected area.
		updatePlotArea(_mark.Rect);
		// Clear accumulated rectangle.
		_mark.Rect = {};
	}
}

bool BscanGraph::Private::calculatePosIndexAt(BscanGraph::Private::Marker& mark)
{
	// Get fast accessible plot rectangle.
	QRect rc(_graph.getPlotArea());
	// Convert Y to a fraction.
	_cursor.Fraction = static_cast<double>(mark.Pt.y()) / rc.height();
	// Use zoomed value.
	_cursor.Fraction = _scan.Left + _cursor.Fraction * (_scan.Right - _scan.Left);
	// Get PopIndex at Y position.
	// Now we convert pixels to pop indexes (lines)
	int pi = 1;
	// Convert to data range.
	_cursor.PopIndex = static_cast<Value::int_type>(_rangeCurrent.getStop() - pi);
	// Update fraction variable.
	_cursor.VFraction.setCur(Value(_cursor.Fraction), true);
	// Update pop index variable.
	_cursor.VPopIndex.setCur(Value(_cursor.PopIndex), true);
	// Get the Actual data Index of this position
	_cursor.Index = static_cast<Value::int_type>(_dataRange.getStart() + static_cast<Range::size_type>(_cursor.Fraction * static_cast<Value::flt_type>(_dataRange.getSize())));
	// Update ClickIndex value.
	_cursor.VIndex.setCur(Value(_cursor.Index), true);
	// Clear update flag because it has been updated one way or the other.
	mark.Update = false;
	// Ready
	return false;
}

void BscanGraph::Private::zoomBox(const QPoint& pt, bool down)
{
	// First time !
	if (!_mark.Active)
	{
		// Store current point as Anchor for drawing zoom box
		_mark.Pt = pt;
	}
	// New status
	_mark.Active = down;
	// Draw new ZoomBox when active.
	if (_mark.Active)
	{
		// Make plot erase old zoom box.
		updatePlotArea(inflated(_mark.Rect, 1));
		// Make new zoom rect.
		_mark.Rect = QRect(_mark.Pt, pt).normalized();
	}
	else
	{
		/*
		//----------------------------------------
		// Execute actual zoom.
		// Get C-scan area rectangle.
		QRect rc(_graph.getPlotArea());
		//----------------------------------------
		// Calculate vertical zoom.
		// Calculate New Scan Delay
		FScan.Delay += FMark.Rect.Top() * FLineDensity;
		//----------------------------------------
		// Calculate New Scan Range
		FScan.Range = (double)FMark.Rect.Height() / rc.Height() * FScan.Range + 1;
		if (FScan.Range < 6)
			FScan.Range = 6;
		//----------------------------------------
		// Calculate Horizontal zoom.
		// Calculate left and right fractions.
		double scan_sz = FScan.Right - FScan.Left;
		// Calculate right fraction first before the current left value is lost.
		FScan.Right = FScan.Left + (scan_sz * FMark.Rect.right) / rc.Width();
		// Secondly correct the left value.
		FScan.Left += (scan_sz * FMark.Rect.left) / rc.Width();
		//----------------------------------------
		// Update all associated variables
		FScan.VDelay.SetCur(FScan.Delay);
		FScan.VRange.SetCur(FScan.Range);
		FScan.VLeft.SetCur(FScan.Left);
		FScan.VRight.SetCur(FScan.Right);
		// Reinitialize and redraw the plot.
		Reinitialize(true);
		*/
		// Invalidate old rect.
		updatePlotArea(_mark.Rect);
	}
}

void BscanGraph::Private::drawMark(QPainter& p, BscanGraph::Private::Marker& mark) const
{
	// Now we draw other things
	if (mark.Active)
	{
		if (!_zoomActive)
		{
			auto rc = inflated({mark.Pt, mark.Pt}, mark.Radius);
			QPoint ctr(mark.Pt);
			p.save();
			// Invert the color on what is drawn.
			p.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
			// TODO: Use color from palette.
			p.setPen(QColor(0xff, 0xff, 0xff));
			// Draw circle.
			p.drawArc(rc, 0, 16 * 360);
			// Start drawing a cross without a center.
			p.drawLine(ctr.x(), rc.top(), ctr.x(), rc.top() + rc.height() / 4);
			p.drawLine(ctr.x(), rc.bottom() - rc.height() / 4, ctr.x(), rc.bottom());
			p.drawLine(rc.left(), ctr.y(), rc.left() + rc.width() / 4, ctr.y());
			p.drawLine(rc.right() - rc.width() / 4, ctr.y(), rc.right(), ctr.y());
			// Restore pen copy style to copy.
			p.restore();
		}
	}
}

bool BscanGraph::Private::getData(const Range& index)
{
	// Do nothing when the range is empty or plot can not be drawn.
	if (index.isEmpty() || !_flagCanDraw)
	{
		return false;
	}
	// Form a new plot range from the passed index argument.
	Range req(index.getStop() - 1, index.getStop());
	// Clip it on the allowed index range.
	req &= index;
	// When the range is empty signal success.
	if (req.isEmpty())
	{
		return true;
	}
	// Initiate the retrieval of a plot range.
	if (!_requester.requestIndex(req))
	{
		//SF_RTTI_NOTIFY(DO_CLOG, "RequestIndex of new plot index range " << req << " failed!");
		return false;
	}
	return true;
}

void BscanGraph::Private::paint(QPainter& painter, QPaintEvent* event)
{
	auto rect = _area;
	// When the color table is enabled.
	if (_heightPalette)
	{
		auto p2 = rect.bottomRight();
		// Adjust for the color table at the bottom.
		rect.adjust(0, 0, 0, -_heightPalette);
		// Set the color table bounds.
		_rcPalette = QRect(rect.bottomLeft(), p2);
	}
	else
	{
		// Make the palette rectangle invalid.
		_rcPalette = {};
	}
	// Paint the graph canvas en returns the remaining plot area.
	auto prc = _graph.paint(painter, rect, event->region());
	// Check if the region to update intersects with
	if (event->region().intersects(prc))
	{
		if (!draw(painter, prc, event->region()))
		{
			_graph.paintPlotCross(painter, tr("No Data"));
		}
	}
	// Check if the region needs updating.
	if (_heightPalette && event->region().intersects(_rcPalette))
	{
		_paletteServer.paint(painter, _rcPalette);
	}
}

}// namespace sf
