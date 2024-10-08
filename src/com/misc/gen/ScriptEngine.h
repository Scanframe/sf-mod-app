#pragma once

#include "../global.h"
#include "ScriptObject.h"
#include "TStrings.h"

namespace sf
{

/**
 * @brief Simple script engine able.
 */
class _MISC_CLASS ScriptEngine : public ScriptObject
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
			/** Used default for retrieving the current error text.*/
			aeCurrent = -1,
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
		[[nodiscard]] EArithError getError() const
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
		bool setError(EArithError error_value, const std::string& reason = std::string());

		/**
		 * Returns the arithmetic error.
		 * @param error_value
		 * @return Error text.
		 */
		[[nodiscard]] const char* getErrorText(EArithError error_value = EArithError::aeCurrent) const;

		/**
		 * @brief Gets the current position.
		 */
		[[nodiscard]] pos_type getPos() const
		{
			return _pos;
		}

		/**
		 * @brief Gets names of info structures in lines.
		 */
		[[nodiscard]] virtual strings getInfoNames() const;

		/**
		 * @brief Functions to be overloaded in derived classes to add functions, parameters, constants and keywords.
		 *
		 * Gets identifier information
		 * Can be virtual overloaded in derived class to add function names
		 */
		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		/**
		 * @brief Gets a list of available identifiers.
		 *
		 * Used for syntax high lighting for example or code completion.
		 */
		[[nodiscard]] virtual strings getIdentifiers(EIdentifier id) const;

	protected:
		/**
		 * @brief Gets and sets a value using a returned Info structure.
		 *
		 * @return True if request was answered.
		 */
		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

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

				const IdInfo* _info{nullptr};

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
		pos_type _pos{0};
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
		/**
		 * @brief Static list of keywords, constants and functions.
		 */
		static IdInfo _idInfo[];
};

/**
 * @brief Calculator function that converts a formula of the passed string to a #::sf::Value.
 * @param script Script text to calculate.
 * @param def Default value.
 * @return Result or default value depending on success or not.
 */
_MISC_FUNC Value calculator(const std::string& script, const Value& def);

/**
 * @brief Calculator function that converts a formula of the passed string to a floating point value.
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
_MISC_FUNC Value::flt_type calculator(const std::string& script, Value::flt_type def, Value::flt_type x, Value::flt_type y = 0.0, Value::flt_type z = 0.0);

}// namespace sf
