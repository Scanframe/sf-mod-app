#pragma once

#include "misc/gen/TVector.h"
#include "misc/gen/Value.h"

#include "InformationBase.h"
#include "global.h"

namespace sf
{

/**
 * Forward declaration.
 */
class _GII_CLASS VariableReference;

/**
 * Forward declaration.
 */
class _GII_CLASS Variable;

/**
 * This base class contains all local types of used in the Variable class.
 * Multiple inheritance makes these types local for other classes as well.
 * Making code more readable and also prevents name space problems.
 */
class VariableTypes :public InformationTypes
{
	public:
		/**
		 * Vector and Iterator for pointer to variables.
		 */
		typedef TVector<Variable*> Vector;

		/**
		 * Types for internal use.
		 */
		typedef TVector<VariableReference*> ReferenceVector;

		/**
		 * Type containing a name representing a value.
		 * This is called a state.
		 */
		struct State
		{
			State()
			{
				_name = Value::_invalidStr;
			}

			State(const State& s)
			{
				Assign(s);
			}

			/**
			 * Name of the contained value.
			 */
			std::string _name;
			/**
			 * Value which belongs to the name.
			 */
			Value _value;

			/**
			 * Assignment function for save copying of the structure.
			 */
			State& Assign(const State& s)
			{
				_name = s._name;
				_value = s._value;
				return *this;
			}

			/**
			 * Assignment operator for save copying of the structure.
			 */
			State& operator=(const State& s)
			{
				Assign(s);
				return *this;
			}

			/**
			 * Compare equal operator.
			 */
			bool operator==(const State& s) const
			{
				return (_name == s._name) && (_value == s._value);
			}

			/**
			 * Compare unequal operator.
			 */
			bool operator!=(const State& s) const
			{
				return (_name != s._name) || (_value != s._value);
			}
		};

		/**
		 * Intermediate type for creating TStateArray type.
		 */
		typedef TVector<State> StateVector;

		/**
		 * For broadcast events the variable parameter is not the variable
		 * who Set the link with function 'SetLink()' and the param parameter
		 * isn't also the one passed to function 'SetLink()' either.
		 */
		enum EEvent
		{
			/** Special event passed to the global conversion handler.*/
			veConvert = 64000,
			/** Global events for a new variable with ID.*/
			veNewId = -32000,
			/** First user global event.*/
			veUserGlobal,
			/** From here only specific variables are called.<br>Indicates first local event.*/
			veFirstLocal = 0,
			/** Current flags have been changed.*/
			veFlagsChange = 1,
			/** Current value has been changed.<br>Can be local when temp value is used.*/
			veValueChange = 2,
			/** Variable reference is becoming invalid for this instance.*/
			veInvalid = 3,
			/** Conversion values have been changed.*/
			veConverted = 4,
			/** First event which can be used by an implementation for its own purposes.*/
			veUserLocal = 5,
			/** Indicates first private event.*/
			veFirstPrivate = 16000,
			/** This variable instance ID changed.*/
			veIdChanged = 16001,
			/** This variable instance desired ID changed.*/
			veDesiredId = 16002,
			/** This variable instance is being removed.*/
			veRemove = 16003,
			/** This instance has become owner.*/
			veGetOwner = 16004,
			/** This instance has lost ownership.*/
			veLostOwner = 16005,
			/** This instance event handler has been linked.*/
			veLinked = 16006,
			/** This instance has lost event handler link.*/
			veUnlinked = 16007,
			/** This variable instance was setup.*/
			veSetup = 16008,
			/** Start of user private event ID's.*/
			veUserPrivate = 16009
		};

		/**
		 * Enumeration of the order of fields in the setup string.
		 */
		enum EField
		{
			/** Integer identifier */
			vfId = 0,
			/** Name(path) where '|' character is used to separate the group. */
			vfName,
			/***/
			vfUnit,
			/**_flags */
			vfFlags,
			/** Description of the variable.*/
			vfDescription,
			/** Type of the contained value. @see Value::EType*/
			vfType,
			/** Type of the contained value.*/
			vfConversionType,
			/** Rounding value. Makes the value a multiple of this value. Also used for displaying amount of digits.*/
			vfRound,
			/** Default and initial*/
			vfDefault,
			/** Minimum allowed value.*/
			vfMinimum,
			/** Maximum allowed value.*/
			vfMaximum,
			/** First state id any*/
			vfFirstState
		};
		/**
		 * Enumeration type for specifying flags.
		 */
		enum EFlag
		{
			/** Represented by character 'R'.<br>Only the owner can change Set it.*/
			flgREADONLY = 1 << 0,
			/** Represented by character 'A'.<br> Variable is needs saving when storing results.*/
			flgARCHIVE = 1 << 1,
			/** Represented by character 'S'.<br>Parameter may be exported out side the application.*/
			flgSHARE = 1 << 2,
			/** Represented by character 'L'.*/
			flgLINK = 1 << 3,
			/** Represented by character 'F'.<br>
			 * Variable is used as a function and is reset or changed by the owning instance after execution. */
			flgFUNCTION = 1 << 4,
			/** Represented by character 'P'.<br>Variable is a parameter and indicates that it is needed to configuration. (Filter for settings files)*/
			flgPARAMETER = 1 << 5,
			/** Represented by character 'H'.<br>For selection of client only when listing.*/
			flgHIDDEN = 1 << 6,
			/** Represented by character 'E'.<br>Set when Local variable may be exported globally.<br>
			 * Exported local variables are readonly by default. */
			flgEXPORT = 1 << 7,
			/** Represented by character 'W'.<br> Set when a local variable is exported globally and may be written to globally.*/
			flgWRITEABLE = 1 << 8
		};
		/**
		 * Enumeration of string types used for filtering.
		 */
		enum EStringType
		{
			/** When the instance is not of type string.*/
			stNoString = -1,
			/** This is a straight normal string where control characters are not converted to any multi byte translation.*/
			stNormal = 0,
			/** This is a multi line string with new line control characters.*/
			stMulti,
			/** The string contains a complete file path.*/
			stPath,
			/** The string contains a complete directory an drive letter.*/
			stDirectory,
			/** The string contains a a file name.*/
			stFilename,
			/** The string contains a subdirectory having an ending back slash and no drive letter or starting back slash.*/
			stSubdirectory
		};
		/**
		 * Structure used to setup a variable.
		 */
		struct Definition
		{
			/**
			 * ID of the new global instance.
			 */
			id_type _id;
			/**
			 * Name path separated using '|' characters.
			 */
			std::string _name;
			/**
			 * Combination of EFlag flags.
			 */
			flags_type _flags;
			/**
			 * Description of the instance without comma's.
			 */
			std::string _description;
			/**
			 * Unit preferably in SI units so conversion and calculations are simple.<br>
			 * Also used to Set the string filter type.
			 */
			std::string _unit;
			/**
			 * Additional option used during conversion. (not used yet)
			 */
			std::string _convertOption;
			/**
			 * Internal type of the instance.
			 */
			Value::EType _type;
			/**
			 * Rounding value. The value is a multiple of this value.
			 */
			Value _round;
			/**
			 * This is the value this instance starts with.
			 */
			Value _default;
			/**
			 * Minimum limit of this instance.<br>When the min and max limits are zero no limits are applied.
			 */
			Value _min;
			/**
			 * Maximum limit of this instance.<br>
			 * When the min and max limits are zero no limits are applied.
			 */
			Value _max;
			/**
			 * A vector of VariableTypes::State` instances.
			 */
			StateVector _states;
		};
};

}
