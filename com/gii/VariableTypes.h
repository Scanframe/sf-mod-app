#pragma once

#include "misc/mcvector.h"
#include "misc/value.h"

#include "InformationBase.h"
#include "global.h"

namespace sf
{

//
// Forward definitions of types because of pointer usage before definition.
//
class _GII_CLASS TVariableBase;

class _GII_CLASS TVariableReference;

class _GII_CLASS TVariable;

class _GII_CLASS TVariableLink;

/**
 * This base class contains all local types of used in the TVariable class.
 * Multiple inheritance makes these types local for other classes as well.
 * Making code more readable and also prevents name space problems.
 */
class TVariableTypes :public TInformationObject
{
	public:
		// Vector and Iterator for pointer to variables.
		typedef mcvector<TVariable*> TPtrVector;
		typedef TPtrVector::iter_type TPtrIterator;

		// Types for internal use.
		typedef mcvector<TVariableReference*> TRefPtrVector;
		typedef TRefPtrVector::iter_type TRefPtrIterator;

		/**
		 * Type containing a name representing a value.
		 * This is called a state.
		 */
		struct TState
		{
			TState()
			{
				Name = TValue::_invalidStr;
			}

			TState(const TState& s)
			{
				Assign(s);
			}

			/**
			 * Name of the contained value.
			 */
			std::string Name;
			/**
			 * Value which belongs to the name.
			 */
			TValue Value;

			/**
			 * Assignment function for save copying of the structure.
			 */
			TState& Assign(const TState& s)
			{
				Name = s.Name;
				Value = s.Value;
				return *this;
			}

			/**
			 * Assignment operator for save copying of the structure.
			 */
			TState& operator=(const TState& s)
			{
				Assign(s);
				return *this;
			}

			/**
			 * Compare equal operator.
			 */
			int operator==(const TState& s) const
			{
				return (Name == s.Name) && (Value == s.Value);
			}

			/**
			 * Compare unequal operator.
			 */
			int operator!=(const TState& s) const
			{
				return (Name != s.Name) || (Value != s.Value);
			}
		};

		/**
		 * Intermediate type for creating TStateArray type.
		 */
		typedef mcvector<TState> TStateVector;

		/**
		 * Vector for storing a list `LTState; TVariableTypes::TState' instances.
		 */
		class TStateArray :public TStateVector
		{
			public:
				TStateArray() :TStateVector() {}

				friend class TVariable;
		};

		/**
		 * For broadcast events the variable prameter is not the variable
		 * who set the link with function 'SetLink()' and the param parameter
		 * isn't also the one passed to function 'SetLink()' either.
		 */
		enum EEvent
		{
			/** Special event passed to the global conversion handler.*/
			veCONVERT = 64000,
			/** Global events for a new variable with ID.*/
			veNEWID = -32000,
			/** First user Global event.*/
			veUSERGLOBAL,
			/** From here only specific variables are called.<br>Indicates first local event.*/
			veFIRSTLOCAL = 0,
			/** Current flags have been changed.*/
			veFLAGSCHANGE = 1,
			/** Current value has been changed.<br>Can be local when temp value is used.*/
			veVALUECHANGE = 2,
			/** Variable reference is becoming invalid for this instance.*/
			veINVALID = 3,
			/** Conversion values have been changed.*/
			veCONVERTED = 4,
			/** First event which can be used by an implementation for its own purposes.*/
			veUSERLOCAL = 5,
			/** Indicates first private event.*/
			veFIRSTPRIVATE = 16000,
			/** This variable instance ID changed.*/
			veIDCHANGED = 16001,
			/** This variable instance desired ID changed.*/
			veDESIREDID = 16002,
			/** This variable instance is being removed.*/
			veREMOVE = 16003,
			/** This instance has become owner.*/
			veGETOWNER = 16004,
			/** This instance has lost ownership.*/
			veLOSTOWNER = 16005,
			/** This instance event handler has been linked.*/
			veLINKED = 16006,
			/** This instance has lost event handler link.*/
			veUNLINKED = 16007,
			/** This variable instance was setup.*/
			veSETUP = 16008,
			/** Start of user private event ID's.*/
			veUSERPRIVATE = 16009
		};

		/**
		 * Enumeration of the order of fields in the setup string.
		 */
		enum EField
		{
			/** Integer identifier */
			vfeID = 0,
			/** Name(path) where '|' character is used to separate the group. */
			vfeNAME,
			/***/
			vfeUNIT,
			/**Flags */
			vfeFLAGS,
			/** Description of the variable.*/
			vfeDESCR,
			/** Type of the contained value. @see TValue::EType*/
			vfeTYPE,
			/** Type of the contained value.*/
			vfeCNV,
			/** Rounding value. Makes the value a multiple of this value. Also used for displaying amount of digits.*/
			vfeRND,
			/** Default and initial*/
			vfeDEF,
			/** Minimum allowed value.*/
			vfeMIN,
			/** Maximum allowed value.*/
			vfeMAX,
			/** First state id any*/
			vfeFIRSTSTATE
		};
		/**
		 * Enumeration type for specifying flags.
		 */
		enum EFlag
		{
			/** Represented by character 'R'.<br>Only the owner can change set it.*/
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
			stNO_STRING = -1,
			/** This is a straight normal string where control characters are not converted to any multi byte translation.*/
			stNORMAL = 0,
			/** This is a multi line string with new line control characters.*/
			stMULTI,
			/** The string contains a complete file path.*/
			stPATH,
			/** The string contains a complete directory an drive letter.*/
			stDIRECTORY,
			/** The string contains a a file name.*/
			stFILENAME,
			/** The string contains a subdirectory having an ending back slash and no drive letter or starting back slash.*/
			stSUBDIRECTORY
		};
		/**
		 * Structure used to setup a variable.
		 */
		struct TDefinition
		{
			/**
			 * ID of the new global instance.
			 */
			id_type Id;
			/**
			 * Name path separated using '|' characters.
			 */
			std::string Name;
			/**
			 * Combination of EFlag flags.
			 */
			flags_type Flags;
			/**
			 * Description of the instance without comma's.
			 */
			std::string Descr;
			/**
			 * Unit preferably in SI units so conversion and calculations are simple.<br>
			 * Also used to set the string filter type.
			 */
			std::string Unit;
			/**
			 * Additional option used during conversion. (not used yet)
			 */
			std::string ConvertOption;
			/**
			 * Internal type of the instance.
			 */
			TValue::EType Type;
			/**
			 * Rounding value. The value is a multiple of this value.
			 */
			TValue Round;
			/**
			 * This is the value this instance starts with.
			 */
			TValue Default;
			/**
			 * Minimum limit of this instance.<br>When the min and max limits are zero no limits are applied.
			 */
			TValue Min;
			/**
			 * Maximum limit of this instance.<br>
			 * When the min and max limits are zero no limits are applied.
			 */
			TValue Max;
			/**
			 * A vector of TVariableTypes::TState` instances.
			 */
			TStateVector States;
		};
};

}
