#include "SimplePalette.h"
#include <misc/gen/ScriptEngine.h>

namespace sf
{

SimplePalette::SimplePalette(const PaletteInterface::Parameters& params)
	:PaletteInterface(params)
{
}

SF_IMPL_PROP_GSN(QColor, SimplePalette, ColorFrom, _colorFrom, notify)

SF_IMPL_PROP_GSN(QColor, SimplePalette, ColorTo, _colorTo, notify)

SF_IMPL_PROP_GSN(QColor, SimplePalette, ColorCenter, _colorCenter, notify)

SF_IMPL_PROP_GSN(SimplePalette::Mode, SimplePalette, Mode, _mode, notify)

SF_IMPL_PROP_GRSN(QString, SimplePalette, Formula, _formula, notify)

void SimplePalette::addPropertyPages(PropertySheetDialog* sheet)
{
	PaletteInterface::addPropertyPages(sheet);
}

void SimplePalette::notify(void*)
{
	// Check if callback is assigned.
	if (_callback)
	{
		_callback();
	}
}

qreal SimplePalette::getCurveLevel(qreal x) const
{
	// Calculate the level from the formula.
	return calculator(_formula.toStdString(), x, x);
}

ColorTable SimplePalette::getColorTable() const
{
	// Initialize the return value.
	ColorTable rv(_colorsSize, QColor(Qt::magenta).rgba());
	ColorShiftCurve curve;
	curve.assign(this, &SimplePalette::getCurveLevel, std::placeholders::_1);
	// No color.
	QColor noColor(0, 0, 0, 0);
	auto center = _colorsUsed / 2;
	//
	switch (_mode)
	{
		case ShiftRgb:
			// Check if the color is invalid.
			if (_colorCenter == noColor)
			{
				shiftRgb(_colorFrom, _colorTo, rv.begin(), rv.begin() + _colorsUsed, curve);
			}
			else
			{
				shiftRgb(_colorFrom, _colorCenter, rv.begin(), rv.begin() + center, curve);
				shiftRgb(_colorCenter, _colorTo, rv.begin() + center, rv.begin() + _colorsUsed, curve);
			}
			break;

		case ShiftHsv:
			// Check if the color is invalid.
			if (_colorCenter == noColor)
			{
				shiftHsv(_colorFrom, _colorTo, rv.begin(), rv.begin() + _colorsUsed, curve);
			}
			else
			{
				shiftHsv(_colorFrom, _colorCenter, rv.begin(), rv.begin() + center, curve);
				shiftHsv(_colorCenter, _colorTo, rv.begin() + center, rv.begin() + _colorsUsed, curve);
			}
			break;

		case ShiftHsl:
			// Check if the color is invalid.
			if (_colorCenter == noColor)
			{
				shiftHsl(_colorFrom, _colorTo, rv.begin(), rv.begin() + _colorsUsed, curve);
			}
			else
			{
				shiftHsl(_colorFrom, _colorCenter, rv.begin(), rv.begin() + center, curve);
				shiftHsl(_colorCenter, _colorTo, rv.begin() + center, rv.begin() + _colorsUsed, curve);
			}
			break;
	}
	// Return the color table.
	return rv;
}

}
