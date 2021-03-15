#pragma once

#include "VariableTypes.h"
#include "../global.h"

namespace sf
{

/**
 * Class referenced by the Variable class which carries the actual data.
 * This type is never directly used.
 */
struct VariableReference :public VariableTypes
{
	/**
	 * Constructor when global is true the reference is added to the global list.
	 * @param global
	 */
	explicit VariableReference(bool global);

	/**
	 * Destructor.
	 */
	~VariableReference();

	/**
	 * Copies the members to this instance except for the list and global members.
	 * @param ref
	 */
	void copy(const VariableReference& ref);

	/** Holds the flag about the global status of this reference.*/
	bool _global{false};
	/** Holds the exported flag for local applications.*/
	bool _exported{false};
	/** Holds the valid status of this reference.*/
	bool _valid{false};
	/** In case of a global instance this value is a unique ID.*/
	id_type _id{0};
	/** Holds the flags at creation.*/
	flags_type _flags{0};
	/** Holds the current flags.*/
	flags_type _curFlags{0};
	/** Holds the internal type of the variable.*/
	Value::EType _type{Value::vitUndefined};
	/** Name path separated by '|' characters.*/
	std::string _name;
	/** Describes the*/
	std::string _description;
	/** Contains the SI unit or the string filter.*/
	std::string _unit;
	/** current value.*/
	Value _curValue;
	/** Default value.*/
	Value _defValue;
	/** Maximum value.*/
	Value _maxValue;
	/** Minimum value.*/
	Value _minValue;
	/** Rounding value.*/
	Value _rndValue;
	/** Significant digits based on the round value.*/
	int _sigDigits{0};
	/** Vector which holds all states of this instance.*/
	StateVector _states;
	/** list of variables attached to this reference.*/
	Vector _list;
	/** Static this counter is increased if a local event is generated.<br>
	 * It is decreased when it returns from the event handler.*/
	int _localActive{0};
	/** Conversion option.*/
	std::string _convertOption;
	/** Converted unit.
	 * String length of unit is used as a flag for conversion enabling.*/
	std::string _convertUnit;
	/** Converted current value.*/
	Value _convertCurValue;
	/** Converted default value.*/
	Value _convertDefValue;
	/** Converted maximum value.*/
	Value _convertMaxValue;
	/** Converted minimum value.*/
	Value _convertMinValue;
	/** Converted rounding value.*/
	Value _convertRndValue;
	/** Significant digits after conversion.*/
	int _convertSigDigits{0};
	/** Multiplication value for convert calculation.*/
	Value _convertMultiplier;
	/** Offset value for convert calculation.*/
	Value _convertOffset;
};

}
