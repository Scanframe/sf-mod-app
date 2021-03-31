#pragma once

#include <utility>

#include "misc/gen/TVector.h"
#include "misc/gen/Value.h"

#include "InformationBase.h"
#include "global.h"

namespace sf
{

/**
 * Forward declaration.
 */
class VariableReference;

/**
 * Forward declaration.
 */
class Variable;

/**
 * @brief This base class contains all local types of used in the Variable class.
 *
 * Multiple inheritance makes these types local for other classes as well.
 * Making code more readable and also prevents name space problems.
 */
class VariableTypes :public InformationTypes
{
	public:
		/**
		 * @brief Vector and Iterator for pointer to variables.
		 */
		typedef TVector<Variable*> Vector;

		/**
		 * @brief Types for internal use.
		 */
		typedef TVector<VariableReference*> ReferenceVector;

		/**
		 * @brief Type containing a name referencing a value and is called a state.
		 */
		struct State
		{
			/**
			 * @brief Default constructor.
			 */
			State()
				:_name(Value::_invalidStr) {}

			/**
			 * @brief Initialization constructor.
			 */
			State(std::string name, const Value& value)
				:_name(std::move(name)), _value(value) {}

			/**
			 * @brief Copy constructor.
			 */
			State(const State& s)
			{
				assign(s);
			}

			/**
			 * @brief Name of the contained value.
			 */
			std::string _name;
			/**
			 * @brief Value which belongs to the name.
			 */
			Value _value;

			/**
			 * @brief Assignment function for save copying of the structure.
			 */
			State& assign(const State& s)
			{
				_name = s._name;
				_value = s._value;
				return *this;
			}

			/**
			 * @brief Assignment operator for save copying of the structure.
			 */
			State& operator=(const State& s)
			{
				assign(s);
				return *this;
			}

			/**
			 * @brief Compare equal operator.
			 */
			bool operator==(const State& s) const
			{
				return (_name == s._name) && (_value == s._value);
			}

			/**
			 * @brief Compare unequal operator.
			 */
			bool operator!=(const State& s) const
			{
				return (_name != s._name) || (_value != s._value);
			}

			/**
			 * @brief Vector class to be able to create a state list.
			 */
			typedef TVector<State> Vector;
		};

		/**
		 * @brief Events send to the handler set with #sf::Variable::setHandler.
		 *
		 * For broadcast events the variable parameter is not the variable
		 * that called the #sf::Variable::setHandler method and the param parameter
		 * isn't also the one passed to function #sf::Variable::setHandler either.
		 */
		enum EEvent :int
		{
			/** Special event passed to the global conversion handler.*/
			veConvert = 64000,
			/** Global events for a new variable with ID.*/
			veNewId = -32000,
			/** First user global event.*/
			veUserGlobal,
			/** From here only specific variables are called.<br>Indicates first local event.*/
			veFirstLocal = 0,
			/** current flags have been changed.*/
			veFlagsChange = 1,
			/** current value has been changed.<br>Can be local when temp value is used.*/
			veValueChange = 2,
			/** Variable reference is becoming invalid for this instance. Owner is deleted.*/
			veInvalid = 3,
			/** Conversion values have been changed or conversion was enables/disabled.*/
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
		enum EField :int
		{
			/** Integer identifier */
			vfId = 0,
			/** Name(path) where '|' character is used to separate the group. */
			vfName,
			/***/
			vfUnit,
			/** Single character flags. @see EFlag.*/
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
		enum EFlag :flags_type
		{
			/**
			 * Represented by character <b>'R'</b>.<br>Only the owner can change Set it.
			 */
			flgReadonly = 1 << 0,
			/**
			 * Represented by character <b>'A'</b>.<br> Variable is needs saving when storing results.
			 */
			flgArchive = 1 << 1,
			/**
			 * Represented by character <b>'S'</b>.<br>Parameter may be exported outside of the application.
			 * When communicating over a network connection only these variables are exported.
			 */
			flgShare = 1 << 2,
			/**
			 * Represented by character <b>'L'</b>.
			 */
			flgLink = 1 << 3,
			/**
			 * Represented by character <b>'F'</b>.<br>
			 * Variable is used as a function and is reset or changed by the owning instance after execution.
			 */
			flgFunction = 1 << 4,
			/**
			 * Represented by character <b>'P'</b>.<br>Variable is a parameter and indicates that it is
			 * needed for configuration of an application.<br>
			 * This makes it possible to distinguishes between settings and parameters.
			 */
			flgParameter = 1 << 5,
			/**
			 * Represented by character <b>'H'</b>.<br>For selection of client only when listing.
			 */
			flgHidden = 1 << 6,
			/**
			 * Represented by character <b>'E'</b>.<br>Set when Local variable may be exported globally.<br>
			 * Exported local variables are readonly by default.
			 */
			flgExport = 1 << 7,
			/**
			 * Represented by character <b>'W'</b>.<br> Set when a local variable is exported globally and
			 * may be written to globally.
			 */
			flgWriteable = 1 << 8
		};
		/**
		 * Enumeration of string types used for filtering.
		 */
		enum EStringType :int
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
			 * Flag indicating this structure is valid.
			 */
			bool _valid{false};
			/**
			 * ID of the new global instance.
			 */
			id_type _id{0};
			/**
			 * Name path separated using '|' characters.
			 */
			std::string _name;
			/**
			 * Combination of EFlag flags.
			 */
			flags_type _flags{0};
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
			Value _roundValue;
			/**
			 * This is the value this instance starts with.
			 */
			Value _defaultValue;
			/**
			 * Minimum limit of this instance.<br>When the min and max limits are zero no limits are applied.
			 */
			Value _minValue;
			/**
			 * Maximum limit of this instance.<br>
			 * When the min and max limits are zero no limits are applied.
			 */
			Value _maxValue;
			/**
			 * A vector of VariableTypes::State` instances.
			 */
			State::Vector _states;
		};
};

}
