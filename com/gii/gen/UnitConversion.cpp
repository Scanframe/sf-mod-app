#include <cmath>
#include "misc/gen/gen_utils.h"
#include "misc/gen/ScriptEngine.h"

#include "UnitConversion.h"

namespace sf
{

static UnitConversionServerClosure UnitConversionClosure;

void setUnitConversionHandler(const UnitConversionServerClosure& closure)
{
	UnitConversionClosure.assign(closure);
}

bool getUnitConversion(const std::string& option, const std::string& from_unit, int from_precision, double& multiplier, double& offset, std::string& to_unit, int& to_precision)
{
	// When set call the handler.
	if (UnitConversionClosure)
	{
		UnitConversionEvent ev(option, from_unit, from_precision, multiplier, offset, to_unit, to_precision);
		return UnitConversionClosure(ev);
	}
	return false;
}

bool UnitConverter::set(const std::string& unit, int sig_digits)
{
	_originalSigDigits = sig_digits;
	_originalUnit.assign(unique(unit));
	return set();
}

double UnitConverter::getValue(double value) const
{
	if (_valid && fabs(_multiplier) > std::numeric_limits<double>::min())
	{
		return value * _multiplier + _offset;
	}
	return value;
}

bool UnitConverter::set()
{
	// Get conversion values using global function.
	return getUnitConversion(_options, _originalUnit, _originalSigDigits, _multiplier, _offset, _unit, _sigDigits);
}

std::string UnitConverter::getString(double value) const
{
	value = getValue(value);
	//
	int precision = getSigDigits();
	//
	if (precision != std::numeric_limits<int>::max())
	{
		precision = clip(precision, 0, std::numeric_limits<double>::max_digits10 - 1);
		return stringf("%.*lf", precision, value);
	}
	else
	{
		// Create buffer large enough to hold all digits and signs including exponent 'e' and decimal dot '.'.
		char buf[std::numeric_limits<double>::max_digits10 + std::numeric_limits<double>::max_exponent10 + 5];
		// It seems the last digit is not reliable so the 'max_digits10 - 1' is given.
		std::string s(gcvt(value, std::numeric_limits<double>::digits10, buf));
		// Only needed for Windows since it adds a trailing '.' even when not required.
		return s.erase(s.find_last_not_of('.') + 1);
	}
}

double UnitConverter::getOrgValue(double value) const
{
	if (_valid && fabs(_multiplier) > std::numeric_limits<double>::min())
	{
		return (value - _offset) / _multiplier;
	}
	return value;
}

double UnitConverter::getOrgValue(const std::string& value, double def) const
{
	return getOrgValue(calculator(value, def));
}

} // namespace


