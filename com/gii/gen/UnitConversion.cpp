#include <cmath>
#include <utility>
#include "misc/gen/genutils.h"
#include "misc/gen/ScriptCalc.h"

#include "UnitConversion.h"

namespace sf
{

static UnitConversionServerCallBack UnitConversionCallBack = nullptr;

static void* UnitConversionCallBackData = nullptr;

void SetUnitConversionServer(UnitConversionServerCallBack func, void* data)
{
	UnitConversionCallBack = func;
	UnitConversionCallBackData = data;
}

bool GetUnitConversion(std::string option, std::string from, int precision, double& mul, double& ofs, std::string& to, int& newprec)
{
	if (UnitConversionCallBack)
	{
		return UnitConversionCallBack(UnitConversionCallBackData, std::move(option), std::move(from), precision, mul, ofs, to, newprec);
	}
	return false;
}

bool UnitConverter::Set(const std::string& unit, int sig_digits)
{
	_originalSigDigits = sig_digits;
	_originalUnit.assign(unique(unit));
	return Set();
}

double UnitConverter::GetValue(double value) const
{
	if (_valid && fabs(_multiplier) > std::numeric_limits<double>::min())
	{
		return value * _multiplier + _offset;
	}
	return value;
}

bool UnitConverter::Set()
{
	// Get conversion values.
	_valid = GetUnitConversion(_options, _originalUnit, _originalSigDigits, _multiplier, _offset, _unit, _sigDigits);
	//
	return _valid;
}

std::string UnitConverter::GetString(double value) const
{
	value = GetValue(value);
	//
	int precision = GetSigDigits();
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

double UnitConverter::GetOrgValue(double value) const
{
	if (_valid && fabs(_multiplier) > std::numeric_limits<double>::min())
	{
		return (value - _offset) / _multiplier;
	}
	return value;
}

double UnitConverter::GetOrgValue(const std::string& value, double def) const
{
	return GetOrgValue(Calculator(value, def));
}

} // namespace


