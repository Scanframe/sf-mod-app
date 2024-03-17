#include <QPaintEvent>
#include "AcquisitionControl.h"
#include "AcquisitionControlPrivate.h"
#include "AcquisitionControlPropertyPage.h"
#include <misc/qt/PropertySheetDialog.h>
#include <QStylePainter>
#include <misc/qt/qt_utils.h>

namespace sf
{

AcquisitionControl::AcquisitionControl(QWidget* parent)
	:QWidget(parent)
	 , ObjectExtension(this)
	 , _p(new Private(this))
{
}

AcquisitionControl::~AcquisitionControl()
{
	delete _p;
}

void AcquisitionControl::addPropertyPages(sf::PropertySheetDialog* sheet)
{
	ObjectExtension::addPropertyPages(sheet);
	sheet->addPage(new AcquisitionControlPropertyPage(this, sheet));
}

void AcquisitionControl::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);
	_p->mouseDown(event->button(), event->modifiers(), event->pos());
}

void AcquisitionControl::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);
	_p->mouseUp(event->button(), event->modifiers(), event->pos());
}

void AcquisitionControl::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);
	_p->mouseMove(event->button(), event->modifiers(), event->pos());
}

void AcquisitionControl::initStyleOption(QStyleOptionFrame* option) const
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
	option->features = QStyleOptionFrame::Rounded;
}

void AcquisitionControl::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	QStyleOptionFrame panel;
	initStyleOption(&panel);
	// Create painter for this widget.
	QStylePainter sp(this);
	// Get the area after styling.
	auto rc = sp.style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
	// Clip all but the inner rectangle.
	sp.setClipRegion(QRegion(event->rect()).subtracted(inflated(rc, -_p->_margin)));
	// Use base function to draw the focus rectangle having the right color as a regular edit widget.
	sp.drawPrimitive(QStyle::PE_PanelLineEdit, panel);
	// Clear the clipping rea
	sp.setClipRegion(event->rect());
	//_p->_graph._debug = true;
	// Create 1 pixel spacing between border and
	inflate(rc, -_p->_margin);
	// Paint the graph canvas en returns the remaining plot area.
	auto prc = _p->_graph.paint(sp, rc, event->region());
	// Save the state of the painter for when it is not possible to draw anything.
	sp.save();
	// Get the offset of the plot area.
	auto offset = prc.topLeft();
	// Set the origin of the bounds top left.
	sp.translate(offset);
	// Paint the plot areas.
	bool ok = _p->draw(sp, prc.translated(-offset), event->region().translated(offset));
	// Restore the painter state.
	sp.restore();
	// Check if drawing did not fail.
	if (!ok)
	{
		_p->_graph.paintPlotCross(sp, tr("No Data"));
	}
}

void AcquisitionControl::keyPressEvent(QKeyEvent* event)
{
	QWidget::keyPressEvent(event);
	_p->keyDown(event->key(), event->modifiers());
}

QSize AcquisitionControl::minimumSizeHint() const
{
	return {200, 100};
}

bool AcquisitionControl::isRequiredProperty(const QString& name)
{
	return true;
}


void AcquisitionControl::focusInEvent(QFocusEvent* event)
{
	QWidget::focusInEvent(event);
	_p->focus(true);
}

void AcquisitionControl::focusOutEvent(QFocusEvent* event)
{
	QWidget::focusOutEvent(event);
	_p->focus(false);
}

void AcquisitionControl::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	_p->geoResize(event->size(), event->oldSize());
}

SF_IMPL_PROP_GSN(int, AcquisitionControl, GripHeight, _p->_gripHeight, _p->propertyChange)

SF_IMPL_PROP_GSN(int, AcquisitionControl, ValueMax, _p->_valueMax, _p->propertyChange)

SF_IMPL_PROP_GSN(bool, AcquisitionControl, ThresholdDrag, _p->_thresholdDrag, _p->propertyChange)

SF_IMPL_PROP_GSN(bool, AcquisitionControl, ThresholdLinked, _p->_thresholdLinked, _p->propertyChange)

SF_IMPL_PROP_GSN(bool, AcquisitionControl, GridEnabled, _p->_gridEnabled, _p->propertyChange)

SF_IMPL_PROP_GSN(QColor, AcquisitionControl, ColorForeground, _p->_colorForeground, _p->propertyChange)

SF_IMPL_PROP_GSN(QColor, AcquisitionControl, ColorBackground, _p->_colorBackground, _p->propertyChange)

SF_IMPL_PROP_GSN(QColor, AcquisitionControl, ColorTcgLine, _p->_colorTcgLine, _p->propertyChange)

SF_IMPL_PROP_GSN(QColor, AcquisitionControl, ColorTcgRange, _p->_colorTcgRange, _p->propertyChange)

SF_IMPL_PROP_GSN(QColor, AcquisitionControl, ColorAdjustmentLine, _p->_colorAdjustmentLine, _p->propertyChange)

SF_IMPL_PROP_GSN(QColor, AcquisitionControl, ColorGridLines, _p->_colorGridLines, _p->propertyChange)

SF_IMPL_INFO_ID(AcquisitionControl, IdTimeUnit, _p->_vTimeUnit)

SF_IMPL_INFO_ID(AcquisitionControl, IdGateCount, _p->_vGateCount)

SF_IMPL_INFO_ID(AcquisitionControl, IdIfShift, _p->_vIfShift)

SF_IMPL_INFO_ID(AcquisitionControl, IdCopyData, _p->_rCopyData)

SF_IMPL_INFO_ID(AcquisitionControl, IdCopyIndex, _p->_rCopyIndex)

SF_IMPL_INFO_ID(AcquisitionControl, IdCopyDelay, _p->_vCopyDelay)

SF_IMPL_INFO_ID(AcquisitionControl, IdCopyRange, _p->_vCopyRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdTcgEnable, _p->_vTcgEnable)

SF_IMPL_INFO_ID(AcquisitionControl, IdTcgDelay, _p->_vTcgDelay)

SF_IMPL_INFO_ID(AcquisitionControl, IdTcgRange, _p->_vTcgRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdTcgSlavedTo, _p->_vTcgSlavedTo)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate0Delay, _p->_gates[0].VDelay)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate0Range, _p->_gates[0].VRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate0Threshold, _p->_gates[0].VThreshold)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate0SlavedTo, _p->_gates[0].VSlavedTo)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate0TrackRange, _p->_gates[0].VTrackRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate0Tof, _p->_gates[0].RTof)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate0Amp, _p->_gates[0].RAmp)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate1Delay, _p->_gates[1].VDelay)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate1Range, _p->_gates[1].VRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate1Threshold, _p->_gates[1].VThreshold)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate1SlavedTo, _p->_gates[1].VSlavedTo)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate1TrackRange, _p->_gates[1].VTrackRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate1Tof, _p->_gates[1].RTof)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate1Amp, _p->_gates[1].RAmp)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate2Delay, _p->_gates[2].VDelay)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate2Range, _p->_gates[2].VRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate2Threshold, _p->_gates[2].VThreshold)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate2SlavedTo, _p->_gates[2].VSlavedTo)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate2TrackRange, _p->_gates[2].VTrackRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate2Tof, _p->_gates[2].RTof)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate2Amp, _p->_gates[2].RAmp)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate3Delay, _p->_gates[3].VDelay)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate3Range, _p->_gates[3].VRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate3Threshold, _p->_gates[3].VThreshold)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate3SlavedTo, _p->_gates[3].VSlavedTo)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate3TrackRange, _p->_gates[3].VTrackRange)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate3Tof, _p->_gates[3].RTof)

SF_IMPL_INFO_ID(AcquisitionControl, IdGate3Amp, _p->_gates[3].RAmp)

SF_IMPL_PROP_GRSN(QByteArray, AcquisitionControl, IdsTcgGain, _p->_idsTcgGain, _p->propertyChange)

SF_IMPL_PROP_GRSN(QByteArray, AcquisitionControl, IdsTcgTime, _p->_idsTcgTime, _p->propertyChange)

}
