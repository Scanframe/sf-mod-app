#include <cmath>
#include <utility>
#include "misc/genutils.h"
#include "misc/arith.h"

#include "UnitConversion.h"

namespace sf
{

static TUnitConversionServerCallBack UnitConversionCallBack = nullptr;

static void* UnitConversionCallBackData = nullptr;

void SetUnitConversionServer(TUnitConversionServerCallBack func, void* data)
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

bool TUnitConverter::Set(const std::string& unit, int sig_digits)
{
	FOrgSigDigits = sig_digits;
	FOrgUnit.assign(unique(unit));
	return Set();
}

double TUnitConverter::GetValue(double value) const
{
	if (FValid && fabs(FMultiplier) > std::numeric_limits<double>::min())
	{
		return value * FMultiplier + FOffset;
	}
	return value;
}

bool TUnitConverter::Set()
{
	// Get conversion values.
	FValid = GetUnitConversion(FOptions, FOrgUnit, FOrgSigDigits, FMultiplier, FOffset, FUnit, FSigDigits);
	//
	return FValid;
}

std::string TUnitConverter::GetString(double value) const
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

double TUnitConverter::GetOrgValue(double value) const
{
	if (FValid && fabs(FMultiplier) > std::numeric_limits<double>::min())
	{
		return (value - FOffset) / FMultiplier;
	}
	return value;
}

double TUnitConverter::GetOrgValue(std::string value, double def) const
{
	return GetOrgValue(ScriptCalc(std::move(value), def));
}

#if IS_WIN_TARGET
_MISC_FUNC bool IsSystemMetric()
 {
	static int value = -1;
	// When not initialized.
	if (value == -1)
		value = GetProfileInt("intl", "iMeasure", 0);
	// Get from the registry if the system is Metric or US measurement.
	return value == 0;
}
#endif

} // namespace


