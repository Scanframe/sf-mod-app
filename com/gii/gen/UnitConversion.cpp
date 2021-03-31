#include <cmath>
#include <utility>
#include "misc/gen/gen_utils.h"
#include "misc/gen/ScriptCalc.h"

#include "UnitConversion.h"

namespace sf
{

static UnitConversionServerCallBack UnitConversionCallBack = nullptr;

static void* UnitConversionCallBackData = nullptr;

void setUnitConversionHandler(UnitConversionServerCallBack func, void* data)
{
	UnitConversionCallBack = func;
	UnitConversionCallBackData = data;
}

bool getUnitConversion(const std::string& option, const std::string& from, int precision, double& mul, double& ofs, std::string& to, int& to_precision)
{
	if (UnitConversionCallBack)
	{
		return UnitConversionCallBack(UnitConversionCallBackData, option, from, precision, mul, ofs, to, to_precision);
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
	if (precision != INT_MAX)
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
	return getOrgValue(Calculator(value, def));
}

} // namespace


