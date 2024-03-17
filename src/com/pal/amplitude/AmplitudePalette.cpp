#include "AmplitudePalette.h"
#include <misc/gen/ScriptEngine.h>

namespace sf
{

AmplitudePalette::AmplitudePalette(const PaletteInterface::Parameters& params)
	:PaletteInterface(params)
	 , _callback(params._callback)
{
	for (auto v: {&_vCalibMin, &_vCalibMax, &_vCalibPos})
	{
		v->setHandler(this);
	}
}

SF_IMPL_PROP_GRSN(QColor, AmplitudePalette, ColorFrom, _colorFrom, notify)

SF_IMPL_PROP_GRSN(QColor, AmplitudePalette, ColorTo, _colorTo, notify)

SF_IMPL_PROP_GRSN(QColor, AmplitudePalette, ColorCalib, _colorCalib, notify)

SF_IMPL_PROP_GRSN(QColor, AmplitudePalette, ColorOver, _colorOver, notify)

SF_IMPL_PROP_GSN(double, AmplitudePalette, SizeCalib, _sizeCalib, notify)

SF_IMPL_INFO_ID(AmplitudePalette, IdCalibMin, _vCalibMin)

SF_IMPL_INFO_ID(AmplitudePalette, IdCalibMax, _vCalibMax)

SF_IMPL_INFO_ID(AmplitudePalette, IdCalibPos, _vCalibPos)

void AmplitudePalette::addPropertyPages(PropertySheetDialog* sheet)
{
	PaletteInterface::addPropertyPages(sheet);
}

void AmplitudePalette::notify(void*)
{
	// Check if callback is assigned before calling it.
	if (_callback)
	{
		_callback();
	}
}

qreal AmplitudePalette::getCurveLevel(qreal x) const
{
	// Calculate the level from the formula.
	return x;
}

ColorTable AmplitudePalette::getColorTable() const
{
	// Create a color table using the calibration color.
	ColorTable rv(_colorsSize, _colorCalib.rgba());
	// Get the amount of index colors for the calibration color.
	auto calibCount = clip<qsizetype>(static_cast<qsizetype>(_colorsUsed * _sizeCalib), 1, _colorsUsed / 10); // NOLINT(cppcoreguidelines-narrowing-conversions)
	//
	auto calibIndex = calculateOffset<double, qsizetype>(_calibLevel, 0.0, 1.0, _colorsUsed - 1, true); // NOLINT(cppcoreguidelines-narrowing-conversions)
	// Fill the lower under calibration threshold colors.
	shiftRgb(_colorFrom, _colorTo, rv.begin(), rv.begin() + (calibIndex - calibCount + 1));
	// Fill the upper over calibration threshold colors.
	shiftSaturation(_colorOver, rv.begin() + calibIndex + 1, rv.begin() + _colorsUsed);
	// Return the color table.
	return rv;
}

void AmplitudePalette::variableEventHandler(VariableTypes::EEvent event, const Variable& caller, Variable& link, bool sameInst)
{
	switch (event)
	{
		default:
			break;

		case veValueChange:
		case veIdChanged:
		{
			// Set some defaults.
			Value::flt_type minLim(0.0);
			Value::flt_type  maxLim(1.0);
			Value::flt_type pos = _calibLevel;
			// Adjust the defaults whe corresponding ids are valid.
			if (_vCalibMin.getId())
			{
				minLim = _vCalibMin.getCur().getFloat();
			}
			else if (_vCalibPos.getId())
			{
				minLim = _vCalibPos.getMin().getFloat();
			}
			if (_vCalibMax.getId())
			{
				maxLim = _vCalibMax.getCur().getFloat();
			}
			else if (_vCalibPos.getId())
			{
				maxLim = _vCalibPos.getMax().getFloat();
			}
			if (_vCalibPos.getId())
			{
				pos = _vCalibPos.getCur().getFloat();
			}
			// Calculate the position from the available information.
			_calibLevel = calculateOffset<Value::flt_type, double>(pos, minLim, maxLim, 1.0, true);
			// Notify the change.
			if (_callback)
			{
				_callback();
			}
			break;
		}
	}
}

double AmplitudePalette::getCalibLevel() const
{
	return _calibLevel;
}

void AmplitudePalette::setCalibLevel(double level)
{
	// Only when no variable is attached it can be set manually.
	if (!_vCalibPos.getId())
	{
		level = clip<double>(level, 0.0, 1.0);
		if (level != _calibLevel)
		{
			_calibLevel = level;
			notify(&_calibLevel);
		}
	}
}

}
