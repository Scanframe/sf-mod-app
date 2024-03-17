#pragma once

#include <utility>

#include "misc/gen/gen_utils.h"
#include "misc/gen/TVector.h"
#include "misc/gen/TClosure.h"

#include "../global.h"

namespace sf
{

/**
 * @brief Structure passed to handle unit conversions.
 */
struct UnitConversionEvent
{
	/**
	 * @brief Constructor.
	 */
	UnitConversionEvent(std::string option, std::string from_unit, int from_precision,
		double& multiplier, double& offset, std::string& to_unit, int& to_precision)
		:_option(std::move(option)), _from_unit(std::move(from_unit)), _from_precision(from_precision)
		 , _multiplier(multiplier), _offset(offset), _to_precision(to_precision), _to_unit(to_unit) {}

	/** @brief Special conversion option code. */
	std::string _option;
	/** @brief current unit string. */
	std::string _from_unit;
	/** @brief  Decimals of current unit. (can be negative) */
	int _from_precision;
	/** @brief Multiplication factor. */
	double& _multiplier;
	/** @brief Offset value. */
	double& _offset;
	/** @brief New precision after conversion. */
	std::string& _to_unit;
	/** @brief New unit string. */
	int& _to_precision;
};

/**
 * @brief Type of the server callback function.
 *
 * @return True on successful conversion.
 */
typedef TClosure<bool, UnitConversionEvent&> UnitConversionServerClosure;

/**
 * @brief Sets the callback function for a security server.
 *
 * @param closure Callback closure.
 */
_GII_FUNC void setUnitConversionHandler(const UnitConversionServerClosure& closure = {});

/**
 * @brief Returns true if a conversion could be made.
 * <br>
 * Calculation to convert are:<br>
 *   From -> To calculation:<br>
 *     new_value = (value * multiplier) + offset;<br>
 *   To -> From calculation:<br>
 *     new_value = (value - offset) / multiplier;<br>
 *
 * @param option Special conversion option code.
 * @param from_unit current unit string.
 * @param from_precision Decimals of current unit. (can be negative)
 * @param multiplier Multiplication factor.
 * @param offset Offset value.
 * @param to_unit New unit string.
 * @param to_precision New precision after conversion.
 * @return True on success.
 */
_GII_FUNC bool getUnitConversion
	(
		const std::string& option,
		const std::string& from_unit,
		int from_precision,
		double& multiplier,
		double& offset,
		std::string& to_unit,
		int& to_precision
	);

/**
 * @brief Unit conversion class to easy the pain of conversions in dialogs.
 *
 * Used dialogs to convert SI-unit's to be displayed as required and to store them as SI-units.<br>
 * When performing scientific formula calculations in applications there is no need for obscure
 * conversions since all values are in SI-units.
 */
class _GII_CLASS UnitConverter
{
	public:
		/**
		 * @brief Default constructor.
		 */
		UnitConverter() = default;

		/**
		 * @brief Initializing constructor.
		 *
		 * @param unit Original unit.
		 * @param sig_digits Original significant digits.
		 */
		inline UnitConverter(const std::string& unit, int sig_digits);

		/**
		 * @brief Retrieves the unit conversion using the passed values.
		 *
		 * @param unit Original unit.
		 * @param sig_digits Original significant digits.
		 * @return True when successful.
		 */
		bool set(const std::string& unit, int sig_digits);

		/**
		 * @brief Retrieves the unit conversion using the current set values<br>
		 * and passes them to the global function #getUnitConversion
		 *
		 * @return True when successful.
		 */
		bool set();

		/**
		 * @brief Gets the converted value from an original one.
		 *
		 * If not valid the passed value is returned in changed.
		 * @param value Original value.
		 * @return Converted value of the passed value.
		 */
		[[nodiscard]] double getValue(double value) const;

		/**
		 * @brief Returns the converted value as a string using the digits.
		 *
		 * Used to display this value appropriately.
		 * @param value Value to turn into string.
		 * @return Value formatted into string using the digits.
		 */
		[[nodiscard]] std::string getString(double value) const;

		/**
		 * @brief Returns the unit after conversion.
		 *
		 * @return If no valid conversion the original unit is returned.
		 */
		[[nodiscard]] inline std::string getUnit() const;

		/**
		 * @brief Gets the significant digits after conversion.
		 *
		 * @return If no valid conversion the original unit is returned.
		 */
		[[nodiscard]] inline int getSigDigits() const;

		/**
		 * @brief Returns if there is a valid conversion present.
		 *
		 * @return True when valid.
		 */
		[[nodiscard]] inline bool isValid() const;

		/**
		 * @brief  Gets original value from the past converted value.
		 *
		 * Turns converted value into original one.
		 * @param value Converted value.
		 * @return Original non converted value.
		 */
		[[nodiscard]] double getOrgValue(double value) const;

		/**
		 * @brief Gets the original value from the past string which may contain a calculation.
		 *
		 * Calculations like:
		 *  * `"2*20+3"`
		 *  * `"PI and sin(PI/2)"`
		 *
		 * This method parses the string and calls the other #getOrgValue().<br>
		 * @param value String value to parse and to convert.
		 * @param def Default value when parsing the string fails.
		 * @return Original non converted value.
		 */
		[[nodiscard]] double getOrgValue(const std::string& value, double def = 0.0) const;

		/**
		 * @brief Clear the conversion and make it invalid.
		 *
		 * Clearing makes this class pass through values unconverted.
		 */
		inline void clear();

	private:
		/**
		 * True when successfully got conversion values.
		 */
		bool _valid{false};
		/**
		 * Options
		 */
		std::string _options;
		/**
		 * Original significant digits.
		 */
		int _originalSigDigits{0};
		/**
		 * Original unit.
		 */
		std::string _originalUnit;
		/**
		 * Converted offset.
		 */
		double _offset{0.0};
		/**
		 * Converted multiplier.
		 */
		double _multiplier{1.0};
		/**
		 * Converted unit.
		 */
		std::string _unit;
		/**
		 * Converted significant digits.
		 */
		int _sigDigits{0};
};

inline
UnitConverter::UnitConverter(const std::string& unit, int sig_digits)
{
	set(unit, sig_digits);
}

inline
void UnitConverter::clear()
{
	_valid = false;
}

inline
bool UnitConverter::isValid() const
{
	return _valid;
}

inline
std::string UnitConverter::getUnit() const
{
	return _valid ? _unit : _originalUnit;
}

inline
int UnitConverter::getSigDigits() const
{
	return _valid ? _sigDigits : _originalSigDigits;
}

}
