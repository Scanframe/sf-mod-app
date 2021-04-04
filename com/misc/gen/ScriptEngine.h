#pragma once

#include <cstring>
#include "gen_utils.h"
#include "Value.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Info structure for objects used in scripts.
 */
class _MISC_CLASS ScriptObject
{
	public:
		/**
		 * @brief Keyword identifiers.
		 */
		enum EIdentifier
		{
			/** Unknown to script */
			idUnknown = 0,
			/** Is a constant like PI.*/
			idConstant,
			/** Is a variable which cn be assigned.*/
			idVariable,
			/** Is a callable function.*/
			idFunction,
			/** Is a type definition like 'int', 'float', string, 'undef' and 'object'.*/
			idTypedef,
			/** Keyword like 'if' and not used at the moment.*/
			idKeyword
		};

		/**
		 * @brief Used to create static lookup lists.
		 */
		struct Info
		{
			/**
			 * @brief Index for decoding. Could be regarded as function or variable address.
			 */
			int _index{0};
			/**
			 * @brief Function of the entry.
			 */
			EIdentifier _id{idUnknown};
			/**
			 * @brief Name of the entry.
			 */
			const char* _name{nullptr};
			/**
			 * @brief In case of a function the amount of parameters where std::numeric_limits<int>::max()
			 * is infinite -2 is at least 2 and when 3 is exactly 3 parameters are required.
			 */
			int _paramCount{0};
			/**
			 * @brief Pointer referring to TInfoObject if nullptr it is a static entry.
			 */
			void* _data{nullptr};
		};

		/**
		 * Default constructor.
		 * @param type_name
		 */
		explicit ScriptObject(const char* type_name)
			:_typeName(type_name) {}

		/**
		 * @brief Virtual destructor which can be overloaded to clean up objects.
		 */
		virtual ~ScriptObject() = default;

		/**
		 * @brief Must be overloaded for member namespace.
		 */
		[[nodiscard]] virtual const Info* getInfo(const std::string& name) const = 0;

		/**
		 * @brief Gets or sets the a passed data member. Must be overloaded in derived class.
		 */
		virtual bool getSetValue(const Info* info, Value* value, Value::vector_type* params, bool flag_set) = 0;

		/**
		 * @brief Asks if the object should be deleted after having made this call.
		 */
		virtual void destroyObject(bool& should_delete) = 0;

		/**
		 * @brief Gets the reference count.
		 */
		[[nodiscard]] int getRefCount() const
		{
			return _refCount;
		}

		/**
		 * @brief Cast operator to be able to return this instance as a Value.
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
		 * @brief Returns the static Info structure for unknowns.
		 */
		static const Info* getInfoUnknown();

		/**
		 * @brief Returns the type name Set at the constructor.
		 */
		[[nodiscard]] std::string getTypeName() const
		{
			return _typeName;
		}

		/**
		 * @brief Gets the script object owner.
		 */
		ScriptObject* getOwner()
		{
			return _owner;
		}

	protected:
		/**
		 * @brief Makes this object the owner of the other object.
		 */
		void makeOwner(ScriptObject* obj)
		{
			obj->_owner = this;
		}

	private:
		/**
		 * Reference count for internal use to determine if this instance is to be deleted.
		 */
		int _refCount{0};
		/**
		 * Type name used in the script for debugging.
		 */
		const char* _typeName{nullptr};
		/**
		 * Script object which owns/created this instance.
		 * Used to call labels of for events.
		 */
		ScriptObject* _owner{nullptr};

		friend class ScriptEngine;
};

/**
 * @brief Simple script engine able.
 */
class _MISC_CLASS ScriptEngine :public ScriptObject
{
	public:
		/**
		 * @brief Default constructor.
		 */
		ScriptEngine();

		/**
		 * @brief Errors when compiling a script.
		 */
		enum EArithError
		{
			/** Success and no errors encountered.*/
			aeSuccess = 0,
			/** String is too long.*/
			aeStringTooLong,

			/** Unexpected end.*/
			aeUnexpectedEnd,
			/** Unexpected character.*/
			aeUnexpectedCharacter,
			/** Unexpected identifier.*/
			aeUnexpectedIdentifier,

			/** A right parenthesis was expected at the end.*/
			aeExpectedRightParenthesis,
			/** A left parenthesis was expected at the end.*/
			aeExpectedLeftParenthesis,
			/** Expected delimiter ';'.*/
			aeExpectedDelimiter,
			/** Expected function.*/
			aeExpectedFunction,
			/** Expected identifier.*/
			aeExpectedIdentifier,

			/** Unknown function.*/
			aeUnknownFunction,
			/** Unknown constant.*/
			aeUnknownConstant,
			/** Unknown variable.*/
			aeUnknownVariable,
			/** Unknown symbol.*/
			aeUnknownSymbol,
			/** Unknown identifier.*/
			aeUnknownIdentifier,
			/** Unknown object member.*/
			aeUnknownObjectMember,

			/** Cannot assign a value to a constant.*/
			aeAssignConstant,
			/** Identifier length exceeded.*/
			aeMaxIdentifierLength,

			/** Too many parameters in function.*/
			aeTooManyParameters,
			/** Too few parameters in function.*/
			aeTooFewParameters,

			/** Multiple declaration.*/
			aeMultipleDeclaration,
			/** Unexpected keyword.*/
			aeUnexpectedKeyword,
			/** Division by zero.*/
			aeDivisionByZero,
			/** Label not found.*/
			aeLabelNotFound,
			/** IP stack error.*/
			aeIpStack,
			/** Malformed Additional statement.*/
			aeExternalKeyword,
			/** Script took to long to execute.*/
			aeScriptTimeout,
			/** Function error.*/
			aeFunctionError,
			/** Not a left value.*/
			aeNotLValue,
			/** Identifier references NOT an object.*/
			aeNotObject,

			/** Compiler implementation fault.*/
			aeCompilerImplementationError
		};

		/**
		 * @brief Calculates the passed script.
		 */
		bool calculate(const std::string& script, Value& result);

		/**
		 * @brief Gets the error value of this instance.
		 */
		[[nodiscard]] EArithError getErrorValue() const
		{
			return _errorValue;
		}

		/**
		 * @brief Gets the associated error reason string.
		 */
		[[nodiscard]] std::string getErrorReason() const;

		/**
		 * @brief Sets the error value for this instance
		 */
		bool setErrorValue(EArithError error_value, std::string reason = std::string());

		/**
		 * Returns the arithmetic error.
		 * @param error_value
		 * @return
		 */
		[[nodiscard]] const char* getArithErrorText(EArithError error_value) const;

		/**
		 * @brief Gets the current position.
		 */
		[[nodiscard]] int getPos() const
		{
			return _pos;
		}

		/**
		 * @brief Gets names of info structures in lines.
		 */
		[[nodiscard]] virtual std::string getInfoNames() const;

		/**
		 * @brief Functions to be overloaded in derived classes to add functions, parameters, constants and keywords.
		 *
		 * Gets identifier information
		 * Can be virtual overloaded in derived class to add function names
		 */
		[[nodiscard]] const Info* getInfo(const std::string& name) const override;

	protected:
		/**
		 * @brief Casts a #sf::Value::vitCustom typed #sf::Value to a #ScriptObject typed pointer.
		 */
		ScriptObject* castToObject(const Value& value);

		/**
		 * @brief Gets and sets a value using a returned Info structure.
		 *
		 * @return True if request was answered.
		 */
		bool getSetValue(const Info* info, Value* value, Value::vector_type* params, bool flagset) override;

		/**
		 * @brief Determines if the passed character is an alpha one.
		 */
		bool isAlpha(char ch);

	private:
		/**
		 * Overloaded from base class. Do nothing because it cannot be done.
		 */
		void destroyObject(bool& should_delete) override
		{
			should_delete = false;
		}

		/**
		 * Internally used structure for binding object data and code.
		 */
		struct DataCode
		{
			explicit DataCode(ScriptObject* object)
			{
				Clear(object);
			}

			void Clear(ScriptObject* object)
			{
				_info = nullptr;
				_object = object;
			}

			explicit operator bool() const
			{
				return _info && _object;
			}

			const Info* _info{nullptr};

			ScriptObject* _object{nullptr};
		};

		/**
		 * Skip white and comment at current position
		 */
		void eatWhite();

		/**
		 * Get a number at the current position.
		 */
		void getNumber(Value& d);

		/**
		 * Get a name at the current position.
		 */
		bool getName(std::string& name);

		/**
		 * Get parameter from called function
		 */
		bool getParameter(Value& result);

		/**
		 * Process from current position
		 */
		bool arith(Value& result, DataCode& left);

		/**
		 * Process partial part and is called from within Arith(...)
		 */
		bool partial(Value& result);

		/**
		 * Checks for operator and processes it.
		 * Returns true on processing else false.
		 */
		bool operator_(Value& result, DataCode& left);

		/**
		 * Current position.
		 */
		int _pos{0};
		/**
		 * Command pointer.
		 */
		const char* _cmd{};
		/**
		 * Last error value.
		 */
		EArithError _errorValue{aeSuccess};
		/**
		 * Last error reason string.
		 */
		std::string _errorReason;
		static Info _info[];
};

/**
 * @brief Calculator function that converts a formula in the passed string to a #::sf::Value.
 * @param script Script text to calculate.
 * @param def Default value.
 * @return Result or default value depending on success or not.
 */
_MISC_FUNC Value calculator(const std::string& script, const Value& def);

/**
 * @brief Calculator function that converts a formula in the passed string to a floating point value.
 * @param script Script text to calculate.
 * @param def Default value.
 * @return Result or default value depending on success or not.
 */
_MISC_FUNC Value::flt_type calculator(const std::string& script, Value::flt_type def);

/**
 * @brief Same as #calculator() only the passed variable 'x', 'y', 'z' are available as a constant in the script.
 * @todo This function should be replaced by passing a vector of pairs instead.
 * @param script Script text to calculate.
 * @param def Default value.
 * @param x constant value.
 * @param y constant value.
 * @param z constant value.
 * @return Result or default value depending on success or not.
 */
_MISC_FUNC Value::flt_type calculator(const std::string& script, Value::flt_type def,
	Value::flt_type x, Value::flt_type y = 0.0, Value::flt_type z = 0.0);

}
