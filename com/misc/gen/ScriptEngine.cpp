#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "ScriptEngine.h"
#include "Value.h"
#include "dbgutils.h"
#include "gen_utils.h"

namespace sf
{

/**
 * Set the maximum identifier length for the script interpreter for error generation.
 */
constexpr size_t MAX_IDENT_LENGTH = 64;

/**
 * Set the maximum length for the script interpreter for error generation.
 */
constexpr size_t MAX_STRING_LENGTH = (1024 * 4);

const char* ScriptEngine::getErrorText(EArithError error_value) const
{
	if (error_value == aeCurrent)
	{
		error_value = _errorValue;
	}
	switch (error_value)
	{
		case aeSuccess:
			return "Success";
		case aeExpectedRightParenthesis:
			return "A right parenthesis was expected at the end";
		case aeExpectedLeftParenthesis:
			return "A left parenthesis was expected at the end";
		case aeExpectedDelimiter:
			return "Expected delimiter ';'";
		case aeUnexpectedEnd:
			return "Unexpected end";
		case aeUnexpectedCharacter:
			return "Unexpected character";
		case aeStringTooLong:
			return "String is too long";
		case aeDivisionByZero:
			return "Division by zero";
		case aeUnknownFunction:
			return "Unknown function";
		case aeUnknownConstant:
			return "Unknown constant";
		case aeAssignConstant:
			return "Cannot assign a value to a constant";
		case aeUnknownVariable:
			return "Unknown variable";
		case aeUnknownSymbol:
			return "Unknown symbol";
		case aeUnknownIdentifier:
			return "Unknown identifier";
		case aeUnknownObjectMember:
			return "Unknown object member";
		case aeMaxIdentifierLength:
			return "Identifier length exceeded";
		case aeExpectedFunction:
			return "Expected function";
		case aeUnexpectedKeyword:
			return "Unexpected keyword";
		case aeUnexpectedIdentifier:
			return "Unexpected identifier";
		case aeExpectedIdentifier:
			return "Expected identifier";
		case aeMultipleDeclaration:
			return "Multiple declaration";
		case aeTooManyParameters:
			return "Too many parameters in function";
		case aeTooFewParameters:
			return "Too few parameters in function";
		case aeLabelNotFound:
			return "Label not found";
		case aeIpStack:
			return "IP stack error";
		case aeExternalKeyword:
			return "Malformed additional statement";
		case aeScriptTimeout:
			return "Script took to long to execute";
		case aeFunctionError:
			return "Function error";
		case aeNotLValue:
			return "Not a left value";
		case aeNotObject:
			return "Identifier references NOT an object";
		case aeCompilerImplementationError:
			return "Compiler implementation fault";
		default:
			return "Unknown error";
	}
}

// Speed index defines.
#define SID_SCRIPT_START   (-20000)

#define SID_INT       (SID_SCRIPT_START -  1)
#define SID_FLOAT     (SID_SCRIPT_START -  2)
#define SID_STRING    (SID_SCRIPT_START -  3)
#define SID_UNDEF     (SID_SCRIPT_START -  4)
#define SID_OBJECT    (SID_SCRIPT_START -  5)
// String functions.
#define SID_SUBSTR    (SID_SCRIPT_START - 13)
#define SID_STRLEN    (SID_SCRIPT_START - 14)
#define SID_FINDSTR   (SID_SCRIPT_START - 15)
#define SID_STRIP     (SID_SCRIPT_START - 16)
#define SID_TO_UPPER  (SID_SCRIPT_START - 17)
#define SID_TO_LOWER  (SID_SCRIPT_START - 18)
//
#define SID_PI        (SID_SCRIPT_START - 20)
#define SID_SIN       (SID_SCRIPT_START - 21)
#define SID_COS       (SID_SCRIPT_START - 22)
#define SID_TAN       (SID_SCRIPT_START - 23)
#define SID_ACOS      (SID_SCRIPT_START - 24)
#define SID_ASIN      (SID_SCRIPT_START - 25)
#define SID_ATAN      (SID_SCRIPT_START - 26)
#define SID_ATAN2     (SID_SCRIPT_START - 27)
//
#define SID_CMP       (SID_SCRIPT_START - 31)
//
#define SID_CEIL      (SID_SCRIPT_START - 41)
#define SID_FLOOR     (SID_SCRIPT_START - 42)
#define SID_ABS       (SID_SCRIPT_START - 43)
#define SID_ROUND     (SID_SCRIPT_START - 44)
#define SID_MOD       (SID_SCRIPT_START - 45)
#define SID_STR       (SID_SCRIPT_START - 46)
#define SID_TO_INT    (SID_SCRIPT_START - 47)
#define SID_TO_FLOAT  (SID_SCRIPT_START - 48)
//
#define SID_LOG       (SID_SCRIPT_START - 50)
#define SID_EXP       (SID_SCRIPT_START - 51)
#define SID_LOG10     (SID_SCRIPT_START - 52)
#define SID_LOGX      (SID_SCRIPT_START - 53)
#define SID_POW       (SID_SCRIPT_START - 54)
#define SID_SQRT      (SID_SCRIPT_START - 55)

ScriptEngine::IdInfo ScriptEngine::_idInfo[] =
	{
		// Typedefs.
		{SID_INT, idTypedef, "int", Value::vitInteger, nullptr},
		{SID_FLOAT, idTypedef, "float", Value::vitFloat, nullptr},
		{SID_STRING, idTypedef, "string", Value::vitString, nullptr},
		{SID_UNDEF, idTypedef, "undef", Value::vitUndefined, nullptr},
		{SID_OBJECT, idTypedef, "object", Value::vitCustom, nullptr},
		// Constants
		{SID_PI, idConstant, "PI", 0, nullptr},
		// Functions
		{SID_SIN, idFunction, "sin", 1, nullptr},
		{SID_COS, idFunction, "cos", 1, nullptr},
		{SID_TAN, idFunction, "cos", 1, nullptr},
		{SID_ACOS, idFunction, "acos", 1, nullptr},
		{SID_ASIN, idFunction, "asin", 1, nullptr},
		{SID_ATAN, idFunction, "atan", 1, nullptr},
		{SID_ATAN2, idFunction, "atan2", 2, nullptr},
		{SID_CMP, idFunction, "cmp", 3, nullptr},
		{SID_SIN, idFunction, "sin", 1, nullptr},
		{SID_ABS, idFunction, "abs", 1, nullptr},
		{SID_LOG, idFunction, "log", 1, nullptr},
		{SID_EXP, idFunction, "exp", 1, nullptr},
		{SID_LOGX, idFunction, "logx", 2, nullptr},
		{SID_LOG10, idFunction, "log10", 1, nullptr},
		{SID_POW, idFunction, "pow", 2, nullptr},
		{SID_SQRT, idFunction, "sqrt", 1, nullptr},
		{SID_CEIL, idFunction, "ceil", 1, nullptr},
		{SID_FLOOR, idFunction, "floor", 1, nullptr},
		{SID_ROUND, idFunction, "round", 2, nullptr},
		{SID_MOD, idFunction, "mod", 2, nullptr},
		{SID_STR, idFunction, "str", 1, nullptr},
		{SID_TO_INT, idFunction, "to_int", 1, nullptr},
		{SID_TO_FLOAT, idFunction, "to_float", 1, nullptr},
		{SID_SUBSTR, idFunction, "substr", 3, nullptr},
		{SID_STRLEN, idFunction, "strlen", 1, nullptr},
		{SID_FINDSTR, idFunction, "findstr", 2, nullptr},
		{SID_STRIP, idFunction, "strip", 3, nullptr},
		{SID_TO_UPPER, idFunction, "to_upper", 1, nullptr},
		{SID_TO_LOWER, idFunction, "to_lower", 1, nullptr},
		//
	};

ScriptEngine::ScriptEngine()
	:ScriptObject("ScriptEngine")
{
}

const ScriptEngine::IdInfo* ScriptEngine::getInfo(const std::string& name) const
{
	// Find the name of the identifier.
	for (auto& i : _idInfo)
	{
		if (i._name == name)
		{
			return &i;
		}
	}
	// Return the unknown Info entry of the static list.
	return getInfoUnknown();
}

strings ScriptEngine::getIdentifiers(ScriptObject::EIdentifier id) const
{
	sf::strings rv;
	for (auto& i: _idInfo)
	{
		if (i._id == id)
		{
			rv.add(i._name);
		}
	}
	return rv;
}

strings ScriptEngine::getInfoNames() const
{
	strings rv;
	for (auto& i : _idInfo)
	{
		rv.add(i._name);
	}
	return rv;
}

bool ScriptEngine::getSetValue
	(
		const IdInfo* info,
		Value* result,
		Value::vector_type* param,
		bool flag_set
	)
{
	if (!info)
	{
		throw std::invalid_argument(std::string(__FUNCTION__) + "() 'info' is nullptr!");
	}
	// Check if the amount of parameters is correct.
	if (info->_id == idFunction && !param)
	{
		throw std::invalid_argument(std::string(__FUNCTION__) + "() 'param' is nullptr!");
	}
	// Check if the info is own info structure by checking the speed index.
	if (info->_index <= SID_SCRIPT_START)
	{
		switch (info->_id)
		{
			default:
				break;

			case idConstant:
			{
				// Check if a constant is assigned or not.
				if (flag_set)
				{
					setError(aeAssignConstant, info->_name);
				}
				else
				{
					switch (info->_index)
					{
						case SID_PI:
							result->assign(M_PI);
							break;

						default:
							setError(aeUnknownConstant, info->_name);
					}
				}
			}
				break;

			case idFunction:
			{
				switch (info->_index)
				{
					case SID_ABS:
						if ((*param)[0].getType() == Value::vitInteger)
						{
							result->set(abs((*param)[0].getInteger()));
						}
						else
						{
							result->set(fabs((*param)[0].getFloat()));
						}
						break;

					case SID_ROUND:
						if ((*param)[0].getType() == Value::vitInteger)
						{
							result->set((*param)[0].round(Value(std::abs((*param)[1].getInteger()))));
						}
						else
						{
							result->set((*param)[0].round(Value(std::fabs((*param)[1].getFloat()))));
						}
						break;

					case SID_MOD:
						result->set(fmod((*param)[0].getFloat(), (*param)[1].getFloat()));
						break;

					case SID_STR:
						result->set((*param)[0]);
						result->setType(Value::vitString);
						break;

					case SID_TO_INT:
						result->set((*param)[0].getInteger());
						break;

					case SID_TO_FLOAT:
						result->set((*param)[0].getFloat());
						break;

					case SID_SIN:
						result->set(sin((*param)[0].getFloat()));
						break;

					case SID_COS:
						result->set(cos((*param)[0].getFloat()));
						break;

					case SID_TAN:
						result->set(tan((*param)[0].getFloat()));
						break;

					case SID_ACOS:
						result->set(acos((*param)[0].getFloat()));
						break;

					case SID_ASIN:
						result->set(asin((*param)[0].getFloat()));
						break;

					case SID_ATAN:
						result->set(atan((*param)[0].getFloat()));
						break;

					case SID_ATAN2:
						result->set(atan2((*param)[0].getFloat(), (*param)[1].getFloat()));
						break;

					case SID_CMP:
						// param[0] must be at the end of the comparison to get it .right
						result->set(((*param)[1] <= (*param)[0] && (*param)[2] >= (*param)[0]) ? 1 : 0);
						break;

					case SID_SUBSTR:
					{
						std::string s = (*param)[0].getString();
						std::string::size_type pos = (*param)[1].getInteger();
						std::string::size_type len = (*param)[2].getInteger();
						std::string::size_type sz = s.length();
						if (pos >= 0)
						{ // Clip the position
							if (pos > sz)
							{
								pos = sz;
								len = 0;
							}
							else
							{
								if (pos + len > sz)
								{
									len = sz - pos;
								}
							}
							result->set(s.substr(pos, len));
						}
						else
						{
							result->set("");
						}
						break;
					}

					case SID_STRLEN:
						result->set((int) (*param)[0].getString().length());
						break;

					case SID_FINDSTR:
					{
						auto pos = (*param)[0].getString().find((*param)[1].getString());
						result->set(Value::int_type((pos == std::string::npos) ? -1 : pos));
						break;
					}

					case SID_STRIP:
					{
						if ((*param)[0].getType() == Value::vitString)
						{
							// String to strip.
							std::string str = (*param)[0].getString();
							// Characters to strip.
							std::string chars = (*param)[1].getString();
							// Type of strip mode.
							ssize_t type = (*param)[2].getInteger();
							// Select string side to strip.
							switch (type)
							{
								// left trim
								case -1:
									str = trimLeft(str, chars);
									break;

									// right trim
								case 1:
									str = trimRight(str, chars);
									break;

									// left and right trim.
								default:
									str = trim(str, chars);
									break;
							}
							// Result is the stripped string.
							result->set(str);
						}
						else
						{
							// Just return the passed value for stripping unchanged.
							result->set((*param)[0]);
						}
						break;
					}

					case SID_TO_UPPER:
					{
						std::string s = (*param)[0].getString();
						for (char& c : s)
						{
							c = (char) std::toupper(int(c));
						}
						break;
					}

					case SID_TO_LOWER:
					{
						std::string s = (*param)[0].getString();
						for (char& c : s)
						{
							c = (char) std::tolower(int(c));
						}
						result->set(s);
						break;
					}

					case SID_CEIL:
					{
						if ((*param)[0].getType() == Value::vitFloat)
						{
							result->set(ceil((*param)[0].getFloat()));
						}
						else
						{
							result->set((*param)[0]);
						}
						break;
					}

					case SID_FLOOR:
					{
						if ((*param)[0].getType() == Value::vitFloat)
						{
							result->set(floor((*param)[0].getFloat()));
						}
						else
						{
							result->set((*param)[0]);
						}
						break;
					}

					case SID_LOG:
					{
						double value = (*param)[0].getFloat();
						value = log(value);
						result->set(value);
						break;
					}

					case SID_EXP:
					{
						double value = (*param)[0].getFloat();
						value = exp(value);
						result->set(value);
						break;
					}

					case SID_LOG10:
					{
						double value = (*param)[0].getFloat();
						value = log10(value);
						result->set(value);
						break;
					}

					case SID_LOGX:
					{
						double n = (*param)[0].getFloat();
						double value = (*param)[1].getFloat();
						value = log(value) / log(n);
						result->set(value);
						break;
					}

					case SID_POW:
					{
						double value = (*param)[0].getFloat();
						double exp = (*param)[1].getFloat();
						value = pow(value, exp);
						result->set(value);
						break;
					}

					case SID_SQRT:
					{
						double value = (*param)[0].getFloat();
						value = sqrt(value);
						result->set(value);
						break;
					}

					default:
						setError(aeUnknownFunction);
				}
				break;
			}

			case idVariable:
				setError(aeUnknownVariable, info->_name);
				break;

			case idTypedef:
			{
				switch (info->_index)
				{
					case SID_INT:
					case SID_FLOAT:
					case SID_STRING:
					case SID_UNDEF:
					case SID_OBJECT:
						setError(aeUnknownIdentifier, info->_name);
				}
			}

		}
		return true;
	}
	return false;
}

void ScriptEngine::getNumber(Value& result)
{
	char* dp = nullptr;
	char* lp = nullptr;
	double d = sf::stod(&_cmd[_pos], &dp);
	Value::int_type l = std::strtol(&_cmd[_pos], &lp, 0);
	// see which one converted the most characters
	if (dp > lp)
	{
		result.set(d);
	}
	else
	{
		dp = lp;
		result.set(l);
	}
	// Give index 'Pos' the exact number
	while (&_cmd[_pos] != dp && _cmd[_pos] != '\0')
	{
		_pos++;
	}
}

bool ScriptEngine::getName(std::string& name)
{
	name = "";
	std::string::size_type p = 0;
	while (isAlpha(_cmd[_pos + p]) || isalnum(_cmd[_pos + p]))
	{
		if (++p > MAX_IDENT_LENGTH)
		{
			return setError(aeMaxIdentifierLength);
		}
	}
	//
	name.append(_cmd, _pos, p);
	_pos += p;
	eatWhite();
	return true;
}

bool ScriptEngine::isAlpha(char ch)
{
	return isalpha(ch) || (strchr("_", ch) && ch);
}

void ScriptEngine::eatWhite()
{
	while (_cmd[_pos] && strchr("\r\n\t ", _cmd[_pos]))
	{
		_pos++;
	}
}

bool ScriptEngine::operator_(Value& result, DataCode& left) // NOLINT(misc-no-recursion)
{
	// Get operator character
	char oper1 = _cmd[_pos];
	char oper2 = '\0';
	// Skip if no operator character was found
	if (!oper1 || !strchr("<>=&|^/*!%", oper1))
	{
		return false;
	}
	_pos++;
	// Look for second character of the operator
	if (strchr(">=|^&", _cmd[_pos]))
	{
		oper2 = _cmd[_pos++];
	}
	// Get the next value to complete the operation
	Value tmpres;
	//
	if (oper1 == '=' && oper2 == '\0')
	{ // Check if + - operators follow and call Partial as long as these
		// are there.
		do
		{
			if (!partial(tmpres))
			{
				return false;
			}
		}
		while (strchr("+-", _cmd[_pos]) && _cmd[_pos]);
	}
	else
	{
		DataCode lvalue(this);
		//
		if (!arith(tmpres, lvalue))
		{
			return false;
		}
	}
	// Convert types if needed
	if (strchr("<>=", oper2))
	{
		if (result.getType() == Value::vitFloat ||
			tmpres.getType() == Value::vitFloat)
		{
			result.setType(Value::vitFloat);
			tmpres.setType(Value::vitFloat);
		}
	}
	// Do the actual operation
	switch (oper1)
	{
		case '>':
		{ // Check for operator '>='
			if (oper2 == '=')
			{
				result.set(result >= tmpres);
			}
			else
			{
				result.set(result > tmpres);
			}
			break;
		}

		case '<':
		{
			switch (oper2)
			{ // check for operator '<='
				case '=':
					result.set(result <= tmpres);
					break;
					// check for operator '<>'
				case '>':
					result.set(result != tmpres);
					break;

					// defaul for operator '<'
				default:
					result.set(result < tmpres);
					break;
			}
			break;
		}

		case '!':
		{ // check for operator '!='
			if (oper2 == '=')
			{
				result.set(result != tmpres);
			}
			else
			{
				setError(aeUnexpectedCharacter);
			}
			break;
		}

		case '=':
		{ // Check for a compare operator.
			if (oper2 == '=')
			{
				result.set(result == tmpres);
			}
			else
			{
				// If no left value is available and second operator not available.
				if (oper2 == '\0')
				{ // Was a left value available for assigning.
					if (left)
					{
						// Assign also the value to the left variable.
						// The result contains the current value of left info.
						left._object->getSetValue(left._info, &tmpres, nullptr, true);
						// Assign the new value.
						result.assign(tmpres);
					}
					else
					{
						setError(aeNotLValue);
					}
				}
				else
				{
					setError(aeUnexpectedCharacter);
				}
			}
			break;
		}

		case '&':
		{
			if (oper2 == '&')
			{
				result.set((int) (!result.isZero() && !tmpres.isZero()));
			}
			else
			{
				result.set(result.getInteger() & tmpres.getInteger());
			}
			break;
		}

		case '|':
		{
			if (oper2 == '|')
			{
				result.set((int) (!result.isZero() || !tmpres.isZero()));
			}
			else
			{
				result.set(result.getInteger() | tmpres.getInteger());
			}
			break;
		}

			// exclusive 'OR' operator
		case '^':
		{
			if (oper2 == '^')
			{
				result.set((int) (!result.isZero() ^ !tmpres.isZero()));
			}
			else
			{
				result.set(result.getInteger() ^ tmpres.getInteger());
			}
			break;
		}

			// multiplication operator
		case '*':
		{
			result *= tmpres;
			break;
		}

		case '/':
		{
			// check for division by zero
			if (tmpres == Value(0.0))
			{
				setError(aeDivisionByZero);
			}
			else
			{
				result /= tmpres;
			}
			break;
		}

		case '%':
		{
			// check for division by zero
			if (tmpres == Value(0.0))
			{
				setError(aeDivisionByZero);
			}
			else
			{
				result %= tmpres;
			}
			break;
		}

		default:
			break;
	}
	// Clear left value if an operator has passed.
	left.Clear(this);
	// Signal success.
	return true;
}

bool ScriptEngine::partial(Value& result) // NOLINT(misc-no-recursion)
{
	Value locres;
	DataCode left(this);
	if (!arith(locres, left))
	{
		return false;
	}
	while (operator_(locres, left) && !_errorValue) {}
	if (_errorValue)
	{
		return false;
	}
	result += locres;
	return true;
}

bool ScriptEngine::arith(Value& result, DataCode& left) // NOLINT(misc-no-recursion)
{
	eatWhite();
	if (_cmd[_pos] == '(')
	{
		_pos++;
		while (!_errorValue && _cmd[_pos] != ')')
		{
			if (!partial(result))
			{
				if (_errorValue == aeUnexpectedEnd)
				{
					return setError(aeExpectedRightParenthesis);
				}
				else
				{
					return false;
				}
			}
		}
		//
		if (_cmd[_pos] == ')')
		{
			_pos++;
		}
		eatWhite();
		return true;
	}
	//
	if (_cmd[_pos] == '!')
	{
		Value tmpres;
		DataCode left_(this);
		_pos++;
		arith(tmpres, left_);
		tmpres.set(tmpres.isZero());
		result += tmpres;
		return true;
	}
	//
	if (_cmd[_pos] == '+' || _cmd[_pos] == '-')
	{
		Value tmpres;
		DataCode left_(this);
		char posneg = _cmd[_pos];
		_pos++;
		if (!arith(tmpres, left_))
		{
			return false;
		}
		// if (result.GetType()==Value::vitString)
		result += tmpres * Value(((posneg == '+') ? 1.0 : -1.0));
		return true;
	}
	//
	if (isdigit(_cmd[_pos]))
	{
		Value locres;
		getNumber(locres);
		eatWhite();
		operator_(locres, left);
		if (_errorValue)
		{
			return false;
		}
		result += locres;
		return true;
	}
	//
	if (isAlpha(_cmd[_pos]))
	{
		std::string name;
		if (!getName(name))
		{
			return false;
		}
		// Local storage of data and code.
		DataCode data_code(this);
		// Get identifier info and number of parameters in case of a function
		data_code._info = getInfo(name);
		// When a value had a member that was dereferenced.
		bool start_over = true;
		do
		{
			start_over = false;
			//
			switch (data_code._info->_id)
			{
				case idUnknown:
				{
					setError(aeUnknownSymbol, name);
					break;
				}

				case idConstant:
				case idVariable:
					// Check the next character
					if (strchr("<>^&|= +-/*)!,%;", _cmd[_pos]))
					{
						data_code._object->getSetValue(data_code._info, &result, nullptr, false);
						// Set the left value.
						left = data_code;
					}
						// Check for dereferenced object.
					else if (_cmd[_pos] == '.')
					{
						// Skip the dot character.
						_pos++;
						eatWhite();
						// Get 'self' pointer from the object.
						Value objself;
						// Is the current info structure already derived.
						data_code._object->getSetValue(data_code._info, &objself, nullptr, false);
						// Cast the value to a info pointer type.
						data_code._object = castToObject(objself);
						// Generate an error when the object is not valid.
						if (!data_code)
						{
							return setError(aeNotObject, name);
						}
						// Get the member name.
						getName(name);
						// Get the info structure of the passed object member name.
						data_code._info = data_code._object->getInfo(name);
						// Check for an error and an unknown ID.
						if (!data_code._info || data_code._info->_id == idUnknown)
						{
							return setError(aeUnknownObjectMember, name);
						}
						else
						{
							// Make the same loop a gain.
							start_over = true;
						}
					}
					else
					{
						return setError(aeUnexpectedCharacter, std::string(1, _cmd[_pos]));
					}
					break;

				case idFunction:
				{
					// Check if the next character is a '(' one
					if (_cmd[_pos] == '(')
					{
						// Create an array of Value for parameter storage.
						Value::vector_type params;
						// Check if there are more then one parameters
						if (_cmd[_pos + 1] != ')')
						{
							// Get parameters in the array.
							do
							{
								_pos++;
								Value loc_res;
								getParameter(loc_res);
								if (_errorValue)
								{
									return false;
								}
								params.add(loc_res);
							}
							while (_cmd[_pos] != ')');
						}
						else
						{
							// Skip ')' character in case of function having no parameters.
							_pos++;
						}
						// Check if there are limits to the amount of parameters passed to the function.
						if (data_code._info->_paramCount != std::numeric_limits<int>::max())
						{
							// Get the minimum amount of parameters needed for this function
							auto n = std::abs(data_code._info->_paramCount);
							// Check in any case if there are sufficient parameters passed
							if (params.count() > n && data_code._info->_paramCount > 0)
							{
								setError(aeTooManyParameters, data_code._info->_name);
							}
							// Check the minimum amount of parameters needed
							if (params.count() < n)
							{
								setError(aeTooFewParameters, data_code._info->_name);
							}
							if (getError() != aeSuccess)
							{
								return true;
							}
						}
						// Prevent exception from continuing from here.
						bool b_err = false;
						try
						{
							data_code._object->getSetValue(data_code._info, &result, &params, false);
						}
						catch (...) //(Exception& e)
						{
							b_err = true;
						}
						//
						if (b_err)
						{
							SF_RTTI_NOTIFY(DO_DEFAULT, "Error in function ' " << data_code._info->_name << " ' !")
							return setError(aeFunctionError, data_code._info->_name);
						}
						else
						{
							_pos++;
							if (_cmd[_pos] == '.')
							{
								// Skip the dot character.
								_pos++;
								eatWhite();
								// Cast the value to a info pointer type.
								data_code._object = castToObject(result);
								// Generate an error when the object is not valid.
								if (!data_code)
								{
									return setError(aeNotObject, name);
								}
								// Get the member name.
								getName(name);
								// Get the info structure of the passed object member name.
								data_code._info = data_code._object->getInfo(name);
								// Check for an error and an unknown ID.
								if (!data_code._info || data_code._info->_id == idUnknown)
								{
									return setError(aeUnknownObjectMember, name);
								}
								else
								{
									// Make the same loop a gain.
									start_over = true;
								}
								//
								result.setType(Value::vitUndefined);
							}
						}
					}
					else
					{
						return setError(aeExpectedFunction, data_code._info->_name);
					}
					break;
				}

				default:
					return setError(aeUnexpectedIdentifier, name);
			} // switch
			//
			eatWhite();
		}
		while (start_over);
		//
		return true;
	}
	// Start of string parameter i.e. "\"my great\"" " example\n" .
	if (_cmd[_pos] == '"')
	{
		// Holds the characters.
		std::string s;
		// Flag to indicate regular expression.
		bool escaped = false;
		// Iterate as long as double quoted regions are available.
		while (_cmd[_pos] == '"')
		{
			auto pos = _pos++;
			// As long as the end of the script is not reached.
			do
			{ // Check for escaped expression sequence.
				if (_cmd[pos++] == '\\')
				{
					// Set the flag that a escaped expression has passed.
					escaped = true;
					// Always skip the next character.
					pos++;
				}
			}
			while (_cmd[pos] && _cmd[pos] != '\"');
			// Add the string to the resulting string.
			s.append(_cmd, _pos, pos - _pos);
			// Update the position index.
			_pos = pos + 1;
			// Eat the white between the succesive double quoted regions.
			eatWhite();
		}
		// Check the max length.
		if (s.length() > MAX_STRING_LENGTH)
		{
			return setError(aeStringTooLong, std::string(_cmd, _pos, MAX_STRING_LENGTH));
		}
		// Convert the string if needed.
		if (escaped)
		{
			result.set(unescape(s));
		}
		else
		{
			result.set(s);
		}
		return true;
	}
	//
	return setError(aeUnexpectedCharacter);
}

bool ScriptEngine::getParameter(Value& value) // NOLINT(misc-no-recursion)
{
	value.setType(value.vitUndefined);
	do
	{
		if (!partial(value))
		{
			return false;
		}
		if (')' == _cmd[_pos])
		{
			return true;
		}
		if (',' == _cmd[_pos])
		{
			break;
		}
	}
	while (!_errorValue && _cmd[_pos]);
	//
	return false;
}

bool ScriptEngine::calculate(const std::string& script, Value& result)
{
	const char* prev_cmd = _cmd;
	auto prev_pos = _pos;
	_cmd = script.c_str();
	_pos = 0;
	setError(aeSuccess);
	result.setType(result.vitUndefined);
	// execute until character '\0'
	while (_cmd[_pos])
	{
		partial(result);
		// check for error so far
		if (_errorValue)
		{
			break;
		}
		// Check for unwanted ending characters.
		if (!strchr("+-/*&|^=<>!%", _cmd[_pos]))
		{ // report an error
			setError(aeExpectedDelimiter);
			return false;
		}

	}
	// If there is an error return false without restoring the previous situation.
	if (_errorValue)
	{
		return false;
	}
	// Restore previous command string and position when there is no error.
	_cmd = prev_cmd;
	_pos = prev_pos;
	return true;
}

bool ScriptEngine::setError(EArithError error_value, const std::string& reason)
{
	bool rv = error_value == aeSuccess;
	if (_errorValue != error_value)
	{
		_errorValue = error_value;
		_errorReason = reason;
		SF_COND_RTTI_NOTIFY(!rv, DO_DEFAULT,
			"Error in script: '" << _errorReason << "' " << getErrorText(_errorValue));
	}
	return rv;
}

std::string ScriptEngine::getErrorReason() const
{
	return _errorReason;
}

class ScriptCalcFunction :public ScriptEngine
{
	public:
		ScriptCalcFunction()
		{
			memset(_value, 0, sizeof(_value));
		}

		void SetValue(int index, double value) {_value[index] = value;}

	protected:
		// Gets identifier information.
		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		// Overloaded from base class.
		bool getSetValue(const IdInfo*, Value*, Value::vector_type*, bool) override;

	private:
		enum {MAX_VALUES = 3};
		// Holds the level passed in the GetLevelIntensity().
		double _value[MAX_VALUES]{};
};

bool ScriptCalcFunction::getSetValue(const IdInfo* info, Value* result,
	Value::vector_type* param, bool flag_set)
{
	if (info->_index > 0 && info->_index < MAX_VALUES)
	{
		result->set(_value[info->_index - 1]);
		return true;
	}
	return ScriptEngine::getSetValue(info, result, param, flag_set);
}

Value calculator(const std::string& script, const Value& def)
{
	Value result;
	return ScriptCalcFunction().calculate(script, result) ? result : def;
}

Value::flt_type calculator(const std::string& script, Value::flt_type def)
{
	Value result(0.0);
	ScriptCalcFunction fsc;
	if (fsc.calculate(script, result) && result.isNumber())
	{
		return result.getFloat();
	}
	return def;
}

const ScriptCalcFunction::IdInfo* ScriptCalcFunction::getInfo(const std::string& name) const
{
	static ScriptEngine::IdInfo Info[] =
		{
			{1, idConstant, "x", 0, nullptr},
			{2, idConstant, "y", 0, nullptr},
			{3, idConstant, "z", 0, nullptr}
		};
	for (auto& i : Info)
	{
		if (i._name == name)
		{
			return &i;
		}
	}
	return ScriptEngine::getInfo(name);
}

Value::flt_type calculator(const std::string& script, Value::flt_type def, Value::flt_type x, Value::flt_type y, Value::flt_type z)
{
	Value result(0.0);
	ScriptCalcFunction fsc;
	fsc.SetValue(0, x);
	fsc.SetValue(1, y);
	fsc.SetValue(2, z);
	if (fsc.calculate(script, result) && result.isNumber())
	{
		return result.getFloat();
	}
	return def;
}

}
