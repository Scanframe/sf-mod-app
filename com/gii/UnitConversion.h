#pragma once

#include "misc/genutils.h"
#include "misc/mcvector.h"

#include "global.h"

/*
  From -> To calculation:
    new_value = (value * Multi) + Offset;

  To -> From calculation:
    new_value = (value - Offset) / Multi;
*/

namespace sf
{

/**
 * Returns true is the system is using metric measures instead of US.
 */
_MISC_FUNC bool IsSystemMetric();

/**
 * Type of the server callback function.
 * @param option Special conversion option code.
 * @param from_unit Current unit string.
 * @param from_precision Decimals of current unit. (can be negative)
 * @param multiplier Multiplication factor.
 * @param offset Offset value.
 * @param to_unit New unit string.
 * @param to_precision New from_precision after conversion.
 * @return True on success.
 */
typedef bool (* TUnitConversionServerCallBack)
	(
		void* data,
		std::string option,
		std::string from_unit,
		int from_precision,
		double& multiplier,
		double& offset,
		std::string& to_unit,
		int& to_precision
	);

/**
 *  Sets the callback function for a security server.
 * @param func
 * @param data
 */
_MISC_FUNC void SetUnitConversionServer(TUnitConversionServerCallBack func, void* data = nullptr);
/**
 * Returns true if a conversion could be made.
 * @param option Special conversion option code.
 * @param from_unit Current unit string.
 * @param from_precision Decimals of current unit. (can be negative)
 * @param multiplier Multiplication factor.
 * @param offset Offset value.
 * @param to_unit New unit string.
 * @param to_precision New from_precision after conversion.
 * @return True on success.
 */
_MISC_FUNC bool
GetUnitConversion(std::string option, std::string from_unit, int from_precision, double& multiplier, double& offset,
	std::string& to_unit, int& to_precision);

/**
 *
 */
class _MISC_CLASS TUnitConverter
{
	public:
		/**
		 * Default constructor.
		 */
		TUnitConverter() = default;

		/**
		 * Initializing constructor.
		 * @param unit
		 * @param sig_digits
		 */
		TUnitConverter(std::string unit, int sig_digits);

		/**
		 * Sets the conversion unit.
		 * @param unit
		 * @param sig_digits
		 * @return
		 */
		bool Set(const std::string& unit, int sig_digits);

		/**
		 * Sets the unit conversion using the current set values.
		 * @return
		 */
		bool Set();

		/**
		 * Returns the converted value.
		 * If not valid the passed value is returned in changed.
		 * @param value
		 * @return
		 */
		[[nodiscard]] double GetValue(double value) const;

		/**
		 * Returns the converted value as a string.
		 * @param value
		 * @return
		 */
		[[nodiscard]] std::string GetString(double value) const;

		/**
		 * Returns the unit after conversion.
		 * If not valid the original unit is returned.
		 * @return
		 */
		[[nodiscard]] std::string GetUnit() const;

		/**
		 * Returns the significant digits after conversion.
		 * If not valid the original significant digits are returned.
		 */
		[[nodiscard]] int GetSigDigits() const;

		/**
		 * Returns if there is a valid conversion present.
		 * @return
		 */
		[[nodiscard]] bool IsValid() const;

		/**
		 * Returns original value from the past value.
		 * @param value
		 * @return
		 */
		[[nodiscard]] double GetOrgValue(double value) const;

		/**
		 * Returns original value from the past string value which may
		 * contain functions and constants like: PI and sin(PI/2).
		 * @param value
		 * @param def
		 * @return
		 */
		[[nodiscard]] double GetOrgValue(std::string value, double def = 0.0) const;

		/**
		 * Clear the conversion and make it invalid.
		 */
		void Clear();

	private:
		/**
		 * True when successfully got conversion values.
		 */
		bool FValid{false};
		/**
		 * Options
		 */
		std::string FOptions;
		/**
		 * Original significant digits.
		 */
		int FOrgSigDigits{0};
		/**
		 * Original unit.
		 */
		std::string FOrgUnit;
		/**
		 * Converted offset.
		 */
		double FOffset{0.0};
		/**
		 * Converted multiplier.
		 */
		double FMultiplier{1.0};
		/**
		 * Converted unit.
		 */
		std::string FUnit;
		/**
		 * Converted significant digits.
		 */
		int FSigDigits{0};
};

inline
TUnitConverter::TUnitConverter(std::string unit, int sig_digits)
{
	Set(std::move(unit), sig_digits);
}

inline
void TUnitConverter::Clear()
{
	FValid = false;
}

inline
bool TUnitConverter::IsValid() const
{
	return FValid;
}

inline
std::string TUnitConverter::GetUnit() const
{
	return FValid ? FUnit : FOrgUnit;
}

inline
int TUnitConverter::GetSigDigits() const
{
	return FValid ? FSigDigits : FOrgSigDigits;
}

}