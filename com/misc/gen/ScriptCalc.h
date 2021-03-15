#pragma once

#include <cstring>
#include "gen_utils.h"
#include "Value.h"
#include "../global.h"

namespace sf
{

/**
 * Info structure for objects used in scripts.
 */
class _MISC_CLASS ScriptObject
{
	public:
		/**
		 * Type to pass to registered classes.
		 */
		typedef ScriptObject* TParameters;
		//
		enum EIdentifier
		{
			idUNKNOWN = 0,
			idCONSTANT,
			idVARIABLE,
			idFUNCTION,
			idTYPEDEF,
			idKEYWORD
		};

		/**
		 * Used to create static lookup lists.
		 */
		struct TInfo
		{
			/**
			 * Index for decoding. Could be regarded as function or variable address.
			 */
			int Index;
			/**
			 * Function of the entry.
			 */
			EIdentifier Id;
			/**
			 * Name of the entry.
			 */
			const char* Name;
			/**
			 * In case of a function the amount of parameters where:
			 */
			int ParamCount;
			/**
			 * INTMAX is infinite -2 is at least 2 and when 3 is exactly 3 parameters are required.
			 * Pointer referring to TInfoObject if nullptr it is a static entry.
			 */
			void* Data;
		};

		/**
		 * Default constructor.
		 * @param type_name
		 */
		explicit ScriptObject(const char* type_name)
		:TypeName(type_name)
		{}
		/**
		 * Virtual destructor which can be overloaded to clean up objects.
		 */
		virtual ~ScriptObject() = default;
		/**
		 * Must be overloaded for member namespace.
		 */
		[[nodiscard]] virtual const TInfo* GetInfo(const std::string& name) const = 0;
		/**
		 * Gets or sets the a passed data member.
		 * Must be overloaded in derived class.
		 */
		virtual bool GetSetValue(const TInfo* info, Value* value, Value::vector_type* params, bool flag_set) = 0;
		/**
		 * Asks if the object should be deleted after having made this call.
		 */
		virtual void DestroyObject(bool& should_delete) = 0;
		/**
		 * Returns the reference count.
		 */
		[[nodiscard]] int GetRefCount() const
		{
			return RefCount;
		}
		/**
		 * Cast operator to be able to return this instance as a Value.
		 */
		explicit operator Value() const
		{
			const ScriptObject* so = this;
			if (so)
			{
				return Value(Value::vitCustom, &so, sizeof(&so));
			}
			// Return zero integer when the object is valid.
			return Value(0);
		}
		/**
		 * Returns the static TInfo structure for unknowns.
		 */
		static const TInfo* GetInfoUnknown();
		/**
		 * Returns the type name Set at the constructor.
		 */
		[[nodiscard]] std::string GetTypeName() const
		{
			return TypeName;
		}
		/**
		 * Returns the script object owner.
		 */
		ScriptObject* GetOwner()
		{
			return Owner;
		}

	protected:
		/**
		 * Makes this object the owner of the other object.
		 */
		void MakeOwner(ScriptObject* obj)
		{
			obj->Owner = this;
		}

	private:
		/**
		 * Reference count for internal use to determine if this instance is to be deleted.
		 */
		int RefCount{0};
		/**
		 * Type name used in the script for debugging.
		 */
		const char* TypeName{nullptr};
		/**
		 * Script object which owns/created this instance.
		 * Used to call labels of for events.
		 */
		ScriptObject* Owner{nullptr};

	//DECLARE_REGISTERBASE(TScriptObject);

	friend class ScriptCalc;
};

/**
 *
 */
class _MISC_CLASS ScriptCalc :public ScriptObject
{
	public:
		/**
		 * Constructor.
		 */
		ScriptCalc();

		/**
		 *
		 */
		enum EArithError
		{
			aeSUCCESS = 0,
			aeSTRING_TOO_LONG,

			aeUNEXPECTED_END,
			aeUNEXPECTED_CHARACTER,
			aeUNEXPECTED_IDENTIFIER,

			aeEXPECTED_RIGHT_PARENTHESIS,
			aeEXPECTED_LEFT_PARENTHESIS,
			aeEXPECTED_DELIMITER,
			aeEXPECTED_FUNCTION,
			aeEXPECTED_IDENTIFIER,

			aeUNKNOWN_FUNCTION,
			aeUNKNOWN_CONSTANT,
			aeUNKNOWN_VARIABLE,
			aeUNKNOWN_SYMBOL,
			aeUNKNOWN_IDENTIFIER,
			aeUNKNOWN_OBJECTMEMBER,

			aeASSIGN_CONSTANT,
			aeMAX_IDENTIFIER_LENGTH,

			aeTOO_MANY_PARAMS,
			aeTOO_FEW_PARAMS,

			aeMULTIPLE_DECLARATION,
			aeUNEXPECTED_KEYWORD,
			aeDIVISION_BY_ZERO,
			aeLABEL_NOT_FOUND,
			aeIP_STACK,
			aeEXTERNAL_KEYWORD,
			aeSCRIPT_TIMEOUT,
			aeFUNCTION_ERROR,
			aeNOT_LVALUE,
			aeIS_NOT_AN_OBJECT,

			aeCOMPILER_IMPLEMENTATION_ERROR
		};

		/**
		 * Calculates the passed script.
		 */
		bool Calculate(const std::string& script, Value& result);
		/**
		 * Returns the error value of this instance.
		 */
		[[nodiscard]] EArithError GetErrorValue() const
		{
			return ErrorValue;
		}
		/**
		 * Returns the associated error reason string.
		 */
		[[nodiscard]] std::string GetErrorReason() const;
		/**
		 * Sets the error value for this instance
		 */
		bool SetErrorValue(EArithError error_value, std::string reason = std::string());
		/**
		 * Returns the arithmetic error.
		 * @param error_value
		 * @return
		 */
		[[nodiscard]] const char* GetArithErrorText(EArithError error_value) const;
		// Gets the current position.
		[[nodiscard]] int GetPos() const
		{
			return Pos;
		}
		/**
		 * Returns names of info structures in lines.
		 */
		[[nodiscard]] virtual std::string GetInfoNames() const;
		/**
		 * Functions to be overloaded in derived classes to add functions,
		 * parameters, constants and keywords.
		 *
		 * Gets identifier information
		 * Can be virtual overloaded in derived class to add function names
		 */
		[[nodiscard]] const TInfo* GetInfo(const std::string& name) const override;

	protected:
		/**
		 * Casts a vitCustom typed variable to a TObjInfo typed pointer.
		 */
		ScriptObject* CastToObject(const Value& value);
		/**
		 * Gets and sets a value using a returned TInfo structure.
		 * Returns true if request was answered.
		 */
		bool GetSetValue
		(
			const TInfo* info,
			Value* value,
			Value::vector_type* params,
			bool flagset
		) override;
		// Determines if the passed character is an alpha one.
		bool IsAlpha(char ch);

	private:
		/**
		 * Overloaded from base class.
		 * Do nothing because it cannot be done.
		 */
		void DestroyObject(bool& should_delete) override
		{
			should_delete = false;
		}
		/**
		 * Internally used structure. For binding object data and code.
		 */
		struct TDataCode
		{
			/**
			 * Default constructor.
			 */
			explicit TDataCode(ScriptObject* object)
			{
				Clear(object);
			}
			/**
			 *
			 */
			void Clear(ScriptObject* object)
			{
				Info = nullptr;
				Object = object;
			}
			/**
			 *
			 */
			explicit operator bool() const
			{
				return Info && Object;
			}
			/**
			 *
			 */
			const TInfo* Info{nullptr};
			ScriptObject* Object{nullptr};
		};
		/**
		 * Skip white and comment at current position
		 */
		void EatWhite();
		/**
		 * Get a number at the current position.
		 */
		void GetNumber(Value& d);
		/**
		 * Get a name at the current position.
		 */
		bool GetName(std::string& name);
		/**
		 * Get parameter from called function
		 */
		bool GetParameter(Value& result);
		/**
		 * Process from current position
		 */
		bool Arith(Value& result, TDataCode& left);
		// Process partial part. is called from within Arith(...)
		bool Partial(Value& result);
		/**
		 * Checks for operator and processes it.
		 * Returns true on processing else false.
		 */
		bool Operator(Value& result, TDataCode& left);
		//
		int Pos{0};
		const char* Cmd{};
		EArithError ErrorValue{aeSUCCESS};
		std::string ErrorReason;
		static TInfo Info[];
};

/**
 * Calculator function that converts a formula in the passed string to a double value.
 */
_MISC_FUNC double Calculator(const std::string& script, double def);

/**
 * Same as above only the passed variable 'x', 'y', 'z' are available as a constant in the script.
 */
_MISC_FUNC double Calculator(const std::string& script, double def,	double x,	double y = 0.0,	double z = 0.0);

}// namespace
