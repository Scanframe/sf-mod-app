#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <utility>

#include "dbgutils.h"
#include "genutils.h"
#include "value.h"
#include "arith.h"

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

//IMPLEMENT_REGISTERBASE(TScriptObject);

const char* TScriptCalc::GetArithErrorText(EArithError error_value) const
{
	switch (error_value)
	{
		case aeSUCCESS:
			return "Success";
		case aeEXPECTED_RIGHT_PARENTHESIS:
			return "A right parenthesis was expected at the end";
		case aeEXPECTED_LEFT_PARENTHESIS:
			return "A left parenthesis was expected at the end";
		case aeEXPECTED_DELIMITER:
			return "Expected delimiter ';'";
		case aeUNEXPECTED_END:
			return "Unexpected end";
		case aeUNEXPECTED_CHARACTER:
			return "Unexpected character";
		case aeSTRING_TOO_LONG:
			return "String too long";
		case aeDIVISION_BY_ZERO:
			return "Division by zero";
		case aeUNKNOWN_FUNCTION:
			return "Unknown function";
		case aeUNKNOWN_CONSTANT:
			return "Unknown constant";
		case aeASSIGN_CONSTANT:
			return "Cannot assign a value to a constant";
		case aeUNKNOWN_VARIABLE:
			return "Unknown variable";
		case aeUNKNOWN_SYMBOL:
			return "Unknown symbol";
		case aeUNKNOWN_IDENTIFIER:
			return "Unknown identifier";
		case aeUNKNOWN_OBJECTMEMBER:
			return "Unknown object member";
		case aeMAX_IDENTIFIER_LENGTH:
			return "Identifier length exceeded";
		case aeEXPECTED_FUNCTION:
			return "Function expected";
		case aeUNEXPECTED_KEYWORD:
			return "Unexpected keyword";
		case aeUNEXPECTED_IDENTIFIER:
			return "Unexpected identifier";
		case aeEXPECTED_IDENTIFIER:
			return "Expected identifier";
		case aeMULTIPLE_DECLARATION:
			return "Multiple declaration";
		case aeTOO_MANY_PARAMS:
			return "Too many parameters in function";
		case aeTOO_FEW_PARAMS:
			return "Too few parameters in function";
		case aeLABEL_NOT_FOUND:
			return "Label Not Found";
		case aeIP_STACK:
			return "IP Stack Error";
		case aeEXTERNAL_KEYWORD:
			return "Malformed Additional statement";
		case aeSCRIPT_TIMEOUT:
			return "Script took to long to execute";
		case aeFUNCTION_ERROR:
			return "Function error";
		case aeNOT_LVALUE:
			return "Not a left value";
		case aeIS_NOT_AN_OBJECT:
			return "Identifier references NOT a valid object";
		case aeCOMPILER_IMPLEMENTATION_ERROR:
			return "Compiler implementation fault";
		default:
			return "Unknown error";
	}
}

// Speed index defines.
#define SID_SCRIPT_START   -20000

#define SID_INT       SID_SCRIPT_START -  1
#define SID_FLOAT     SID_SCRIPT_START -  2
#define SID_STRING    SID_SCRIPT_START -  3
#define SID_UNDEF     SID_SCRIPT_START -  4
#define SID_OBJECT    SID_SCRIPT_START -  5
// String functions.
#define SID_SUBSTR    SID_SCRIPT_START - 13
#define SID_STRLEN    SID_SCRIPT_START - 14
#define SID_FINDSTR   SID_SCRIPT_START - 15
#define SID_STRIP     SID_SCRIPT_START - 16
#define SID_TO_UPPER  SID_SCRIPT_START - 17
#define SID_TO_LOWER  SID_SCRIPT_START - 18
//
#define SID_PI        SID_SCRIPT_START - 20
#define SID_SIN       SID_SCRIPT_START - 21
#define SID_COS       SID_SCRIPT_START - 22
#define SID_TAN       SID_SCRIPT_START - 23
#define SID_ACOS      SID_SCRIPT_START - 24
#define SID_ASIN      SID_SCRIPT_START - 25
#define SID_ATAN      SID_SCRIPT_START - 26
#define SID_ATAN2     SID_SCRIPT_START - 27
//
#define SID_CMP       SID_SCRIPT_START - 31
//
#define SID_CEIL      SID_SCRIPT_START - 41
#define SID_FLOOR     SID_SCRIPT_START - 42
#define SID_ABS       SID_SCRIPT_START - 43
#define SID_ROUND     SID_SCRIPT_START - 44
#define SID_MOD       SID_SCRIPT_START - 45
#define SID_STR       SID_SCRIPT_START - 46
#define SID_TO_INT    SID_SCRIPT_START - 47
#define SID_TO_FLOAT  SID_SCRIPT_START - 48
//
#define SID_LOG       SID_SCRIPT_START - 50
#define SID_EXP       SID_SCRIPT_START - 51
#define SID_LOG10     SID_SCRIPT_START - 52
#define SID_LOGX      SID_SCRIPT_START - 53
#define SID_POW       SID_SCRIPT_START - 54
#define SID_SQRT      SID_SCRIPT_START - 55

TScriptCalc::TInfo TScriptCalc::Info[] =
	{
		// Typedefs.
		{SID_INT, idTYPEDEF, "int", TValue::vitINTEGER, nullptr},
		{SID_FLOAT, idTYPEDEF, "float", TValue::vitFLOAT, nullptr},
		{SID_STRING, idTYPEDEF, "string", TValue::vitSTRING, nullptr},
		{SID_UNDEF, idTYPEDEF, "undef", TValue::vitUNDEF, nullptr},
		{SID_OBJECT, idTYPEDEF, "object", TValue::vitCUSTOM, nullptr},
		// Constants
		{SID_PI, idCONSTANT, "PI", 0, nullptr},
		// Functions
		{SID_SIN, idFUNCTION, "sin", 1, nullptr},
		{SID_COS, idFUNCTION, "cos", 1, nullptr},
		{SID_TAN, idFUNCTION, "cos", 1, nullptr},
		{SID_ACOS, idFUNCTION, "acos", 1, nullptr},
		{SID_ASIN, idFUNCTION, "asin", 1, nullptr},
		{SID_ATAN, idFUNCTION, "atan", 1, nullptr},
		{SID_ATAN2, idFUNCTION, "atan2", 2, nullptr},
		{SID_CMP, idFUNCTION, "cmp", 3, nullptr},
		{SID_SIN, idFUNCTION, "sin", 1, nullptr},
		{SID_ABS, idFUNCTION, "abs", 1, nullptr},
		{SID_LOG, idFUNCTION, "log", 1, nullptr},
		{SID_EXP, idFUNCTION, "exp", 1, nullptr},
		{SID_LOGX, idFUNCTION, "logx", 2, nullptr},
		{SID_LOG10, idFUNCTION, "log10", 1, nullptr},
		{SID_POW, idFUNCTION, "pow", 2, nullptr},
		{SID_SQRT, idFUNCTION, "sqrt", 1, nullptr},
		{SID_CEIL, idFUNCTION, "ceil", 1, nullptr},
		{SID_FLOOR, idFUNCTION, "floor", 1, nullptr},
		{SID_ROUND, idFUNCTION, "round", 2, nullptr},
		{SID_MOD, idFUNCTION, "mod", 2, nullptr},
		{SID_STR, idFUNCTION, "str", 1, nullptr},
		{SID_TO_INT, idFUNCTION, "to_int", 1, nullptr},
		{SID_TO_FLOAT, idFUNCTION, "to_float", 1, nullptr},
		{SID_SUBSTR, idFUNCTION, "substr", 3, nullptr},
		{SID_STRLEN, idFUNCTION, "strlen", 1, nullptr},
		{SID_FINDSTR, idFUNCTION, "findstr", 2, nullptr},
		{SID_STRIP, idFUNCTION, "strip", 3, nullptr},
		{SID_TO_UPPER, idFUNCTION, "to_upper", 1, nullptr},
		{SID_TO_LOWER, idFUNCTION, "to_lower", 1, nullptr},
		//
	};

TScriptCalc::TScriptCalc()
	:TScriptObject("ScriptCalc")
{
}

const TScriptObject::TInfo* TScriptObject::GetInfoUnknown()
{
	static TInfo info = {0, idUNKNOWN, nullptr, 0, nullptr};
	return &info;
}

const TScriptCalc::TInfo* TScriptCalc::GetInfo(const std::string& name) const
{
	// Find the name of the identifier.
	for (auto& i : Info)
	{
		if (i.Name == name)
		{
			return &i;
		}
	}
	// Return the unknown Info entry of the static list.
	return GetInfoUnknown();
}

TScriptObject* TScriptCalc::CastToObject(const TValue& value)
{
	// When the variable is of the special type an object is referenced.
	if (value.GetType() == TValue::vitCUSTOM && value.GetSize() == sizeof(void*))
	{
		struct TData {TScriptObject* Object;} * data;
		// Cast binary data to the pointer type.
		data = (TData*) value.GetData();
		// Check for a null pointer.
		return data ? data->Object : nullptr;
	}
	// Failed to cast the value.
	return nullptr;
}

std::string TScriptCalc::GetInfoNames() const
{
	std::string s;
	// Find the name of the identifier.
	for (auto& i : Info)
	{
		s += i.Name;
		s += "\n";
	}
	return s;
}

bool TScriptCalc::GetSetValue
	(
		const TInfo* info,
		TValue* result,
		TValue::vector_type* param,
		bool flagset
	)
{
	_COND_RTTI_THROW(!info, "GetSetValue() 'info' is nullptr")
	// Check if the amount of parameters is correct.
	if (info->Id == idFUNCTION) _COND_RTTI_THROW(!param, "GetSetValue() 'param' is nullptr")
	// Check if the info is own info structure by checking the speed index.
	if (info->Index <= SID_SCRIPT_START)
	{
		switch (info->Id)
		{
			default:
				break;

			case idCONSTANT:
			{
				// Check if a constant is assigned or not.
				if (flagset)
				{
					SetErrorValue(aeASSIGN_CONSTANT, info->Name);
				}
				else
				{
					switch (info->Index)
					{
						case SID_PI:
							result->Assign(M_PI);
							break;

						default:
							SetErrorValue(aeUNKNOWN_CONSTANT, info->Name);
					}
				}
			}
				break;

			case idFUNCTION:
			{
				switch (info->Index)
				{
					case SID_ABS:
						if ((*param)[0].GetType() == TValue::vitINTEGER)
						{
							result->Set(abs((*param)[0].GetInteger()));
						}
						else
						{
							result->Set(fabs((*param)[0].GetFloat()));
						}
						break;

					case SID_ROUND:
						if ((*param)[0].GetType() == TValue::vitINTEGER)
						{
							result->Set((*param)[0].Round(TValue(std::abs((*param)[1].GetInteger()))));
						}
						else
						{
							result->Set((*param)[0].Round(TValue(std::fabs((*param)[1].GetFloat()))));
						}
						break;

					case SID_MOD:
						result->Set(fmod((*param)[0].GetFloat(), (*param)[1].GetFloat()));
						break;

					case SID_STR:
						result->Set((*param)[0]);
						result->SetType(TValue::vitSTRING);
						break;

					case SID_TO_INT:
						result->Set((*param)[0].GetInteger());
						break;

					case SID_TO_FLOAT:
						result->Set((*param)[0].GetFloat());
						break;

					case SID_SIN:
						result->Set(sin((*param)[0].GetFloat()));
						break;

					case SID_COS:
						result->Set(cos((*param)[0].GetFloat()));
						break;

					case SID_TAN:
						result->Set(tan((*param)[0].GetFloat()));
						break;

					case SID_ACOS:
						result->Set(acos((*param)[0].GetFloat()));
						break;

					case SID_ASIN:
						result->Set(asin((*param)[0].GetFloat()));
						break;

					case SID_ATAN:
						result->Set(atan((*param)[0].GetFloat()));
						break;

					case SID_ATAN2:
						result->Set(atan2((*param)[0].GetFloat(), (*param)[1].GetFloat()));
						break;

					case SID_CMP:
						// param[0] must be at the end of the comparison to get it .right
						result->Set(((*param)[1] <= (*param)[0] && (*param)[2] >= (*param)[0]) ? 1 : 0);
						break;

					case SID_SUBSTR:
					{
						std::string s = (*param)[0].GetString();
						int pos = (*param)[1].GetInteger();
						int len = (*param)[2].GetInteger();
						int sz = s.length();
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
							result->Set(s.substr(pos, len));
						}
						else
						{
							result->Set("");
						}
					}
						break;

					case SID_STRLEN:
						result->Set((int) (*param)[0].GetString().length());
						break;

					case SID_FINDSTR:
					{
						size_t pos = (*param)[0].GetString().find((*param)[1].GetString());
						result->Set(long((pos == std::string::npos) ? -1 : pos));
					}
						break;

					case SID_STRIP:
					{
						if ((*param)[0].GetType() == TValue::vitSTRING)
						{
							// String to strip.
							std::string str = (*param)[0].GetString();
							// Characters to strip.
							std::string chars = (*param)[1].GetString();
							// Type of strip mode.
							int type = (*param)[2].GetInteger();
							// Select string side to strip.
							switch (type)
							{
								// left trim
								case -1:
									str = trim_left(str, chars);
									break;

									// right trim
								case 1:
									str = trim_right(str, chars);
									break;

									// left and right trim.
								default:
									str = trim(str, chars);
									break;
							}
							// Result is the stripped string.
							result->Set(str);
						}
						else
						{
							// Just return the passed value for stripping unchanged.
							result->Set((*param)[0]);
						}
						break;
					}

					case SID_TO_UPPER:
					{
						std::string s = (*param)[0].GetString();
						for (char& c : s)
						{
							c = (char) std::toupper(int(c));
						}
					}
						break;

					case SID_TO_LOWER:
					{
						std::string s = (*param)[0].GetString();
						for (char& c : s)
						{
							c = (char) std::tolower(int(c));
						}
						result->Set(s);
					}
						break;

					case SID_CEIL:
					{
						if ((*param)[0].GetType() == TValue::vitFLOAT)
						{
							result->Set(ceil((*param)[0].GetFloat()));
						}
						else
						{
							result->Set((*param)[0]);
						}
					}
						break;

					case SID_FLOOR:
					{
						if ((*param)[0].GetType() == TValue::vitFLOAT)
						{
							result->Set(floor((*param)[0].GetFloat()));
						}
						else
						{
							result->Set((*param)[0]);
						}
					}
						break;

					case SID_LOG:
					{
						double value = (*param)[0].GetFloat();
						value = log(value);
						result->Set(value);
					}
						break;

					case SID_EXP:
					{
						double value = (*param)[0].GetFloat();
						value = exp(value);
						result->Set(value);
					}
						break;

					case SID_LOG10:
					{
						double value = (*param)[0].GetFloat();
						value = log10(value);
						result->Set(value);
					}
						break;

					case SID_LOGX:
					{
						double n = (*param)[0].GetFloat();
						double value = (*param)[1].GetFloat();
						value = log(value) / log(n);
						result->Set(value);
					}
						break;

					case SID_POW:
					{
						double value = (*param)[0].GetFloat();
						double exp = (*param)[1].GetFloat();
						value = pow(value, exp);
						result->Set(value);
					}
						break;

					case SID_SQRT:
					{
						double value = (*param)[0].GetFloat();
						value = sqrt(value);
						result->Set(value);
					}
						break;

					default:
						SetErrorValue(aeUNKNOWN_FUNCTION);
				}
			}
				break;

			case idVARIABLE:
				SetErrorValue(aeUNKNOWN_VARIABLE, info->Name);
				break;

			case idTYPEDEF:
			{
				switch (info->Index)
				{
					case SID_INT:
					case SID_FLOAT:
					case SID_STRING:
					case SID_UNDEF:
					case SID_OBJECT:
						SetErrorValue(aeUNKNOWN_IDENTIFIER, info->Name);
				}
			}

		}
		return true;
	}
	return false;
}

void TScriptCalc::GetNumber(TValue& result)
{
	char* dp = nullptr;
	char* lp = nullptr;
	double d = strtod(&Cmd[Pos], &dp);
	long l = strtol(&Cmd[Pos], &lp, 0);
	// see which one converted the most characters
	if (dp > lp)
	{
		result.Set(d);
	}
	else
	{
		dp = lp;
		result.Set(l);
	}
	// Give index 'Pos' the exact number
	while (&Cmd[Pos] != dp && Cmd[Pos] != '\0')
	{
		Pos++;
	}
}

bool TScriptCalc::GetName(std::string& name)
{
	name = "";
	int p = 0;
	while (IsAlpha(Cmd[Pos + p]) || isalnum(Cmd[Pos + p]))
	{
		if (++p > MAX_IDENT_LENGTH)
		{
			return SetErrorValue(aeMAX_IDENTIFIER_LENGTH);
		}
	}
	//
	name.append(Cmd, Pos, p);
	Pos += p;
	EatWhite();
	return true;
}

bool TScriptCalc::IsAlpha(char ch)
{
	return (isalpha(ch) || strchr("_", ch) && ch);
}

void TScriptCalc::EatWhite()
{
	while (Cmd[Pos] && strchr("\r\n\t ", Cmd[Pos]))
	{
		Pos++;
	}
}

bool TScriptCalc::Operator(TValue& result, TDataCode& left)
{ // Get operator character
	char oper1 = Cmd[Pos];
	char oper2 = '\0';
	// Skip if no operator character was found
	if (!oper1 || !strchr("<>=&|^/*!%", oper1))
	{
		return false;
	}
	Pos++;
	// Look for second charater of the operator
	if (strchr(">=|^&", Cmd[Pos]))
	{
		oper2 = Cmd[Pos++];
	}
	// Get the next value to complete the operation
	TValue tmpres;
	//
	if (oper1 == '=' && oper2 == '\0')
	{ // Check if + - operators follow and call Partial as long as these
		// are there.
		do
		{
			if (!Partial(tmpres))
			{
				return false;
			}
		}
		while (strchr("+-", Cmd[Pos]) && Cmd[Pos]);
	}
	else
	{
		TDataCode lvalue(this);
		//
		if (!Arith(tmpres, lvalue))
		{
			return false;
		}
	}
	// Convert types if needed
	if (strchr("<>=", oper2))
	{
		if (result.GetType() == TValue::vitFLOAT ||
			tmpres.GetType() == TValue::vitFLOAT)
		{
			result.SetType(TValue::vitFLOAT);
			tmpres.SetType(TValue::vitFLOAT);
		}
	}
	// Do the actual operation
	switch (oper1)
	{
		case '>':
		{ // Check for operator '>='
			if (oper2 == '=')
			{
				result.Set(result >= tmpres);
			}
			else
			{
				result.Set(result > tmpres);
			}
			break;
		}

		case '<':
		{
			switch (oper2)
			{ // check for operator '<='
				case '=':
					result.Set(result <= tmpres);
					break;
					// check for operator '<>'
				case '>':
					result.Set(result != tmpres);
					break;

					// defaul for operator '<'
				default:
					result.Set(result < tmpres);
					break;
			}
			break;
		}

		case '!':
		{ // check for operator '!='
			if (oper2 == '=')
			{
				result.Set(result != tmpres);
			}
			else
			{
				SetErrorValue(aeUNEXPECTED_CHARACTER);
			}
			break;
		}

		case '=':
		{ // Check for a compare operator.
			if (oper2 == '=')
			{
				result.Set(result == tmpres);
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
						left.Object->GetSetValue(left.Info, &tmpres, nullptr, true);
						// Assign the new value.
						result.Assign(tmpres);
					}
					else
					{
						SetErrorValue(aeNOT_LVALUE);
					}
				}
				else
				{
					SetErrorValue(aeUNEXPECTED_CHARACTER);
				}
			}
			break;
		}

		case '&':
		{
			if (oper2 == '&')
			{
				result.Set((int) (!result.IsZero() && !tmpres.IsZero()));
			}
			else
			{
				result.Set(result.GetInteger() & tmpres.GetInteger());
			}
			break;
		}

		case '|':
		{
			if (oper2 == '|')
			{
				result.Set((int) (!result.IsZero() || !tmpres.IsZero()));
			}
			else
			{
				result.Set(result.GetInteger() | tmpres.GetInteger());
			}
			break;
		}

			// exclusive 'OR' operator
		case '^':
		{
			if (oper2 == '^')
			{
				result.Set((int) (!result.IsZero() ^ !tmpres.IsZero()));
			}
			else
			{
				result.Set(result.GetInteger() ^ tmpres.GetInteger());
			}
		}
			break;

			// multiplication operator
		case '*':
		{
			result *= tmpres;
			break;
		}

		case '/':
		{ // check for division by zero
			if (tmpres == TValue(0.0))
			{
				SetErrorValue(aeDIVISION_BY_ZERO);
			}
			else
			{
				result /= tmpres;
			}
			break;
		}

		case '%':
		{ // check for division by zero
			if (tmpres == TValue(0.0))
			{
				SetErrorValue(aeDIVISION_BY_ZERO);
			}
			else
			{
				result %= tmpres;
			}
			break;
		}
	}
	// Clear left value if an operator has passed.
	left.Clear(this);
	// Signal success.
	return true;
}

bool TScriptCalc::Partial(TValue& result)
{
	TValue locres;
	TDataCode left(this);
	if (!Arith(locres, left))
	{
		return false;
	}
	while (Operator(locres, left) && !ErrorValue) {}
	if (ErrorValue)
	{
		return false;
	}
	result += locres;
	return true;
}

bool TScriptCalc::Arith(TValue& result, TDataCode& left)
{
	EatWhite();
	if (Cmd[Pos] == '(')
	{
		Pos++;
		while (!ErrorValue && Cmd[Pos] != ')')
		{
			if (!Partial(result))
			{
				if (ErrorValue == aeUNEXPECTED_END)
				{
					return SetErrorValue(aeEXPECTED_RIGHT_PARENTHESIS);
				}
				else
				{
					return false;
				}
			}
		}
		//
		if (Cmd[Pos] == ')')
		{
			Pos++;
		}
		EatWhite();
		return true;
	}
	//
	if (Cmd[Pos] == '!')
	{
		TValue tmpres;
		TDataCode left(this);
		Pos++;
		Arith(tmpres, left);
		tmpres.Set(tmpres.IsZero());
		result += tmpres;
		return true;
	}
	//
	if (Cmd[Pos] == '+' || Cmd[Pos] == '-')
	{
		TValue tmpres;
		TDataCode left(this);
		char posneg = Cmd[Pos];
		Pos++;
		if (!Arith(tmpres, left))
		{
			return false;
		}
		// if (result.GetType()==TValue::vitSTRING)
		result += tmpres * TValue(((posneg == '+') ? 1.0 : -1.0));
		return true;
	}
	//
	if (isdigit(Cmd[Pos]))
	{
		TValue locres;
		GetNumber(locres);
		EatWhite();
		Operator(locres, left);
		if (ErrorValue)
		{
			return false;
		}
		result += locres;
		return true;
	}
	//
	if (IsAlpha(Cmd[Pos]))
	{
		std::string name;
		if (!GetName(name))
		{
			return false;
		}
		// Local storage of data and code.
		TDataCode datacode(this);
		// Get identifier info and number of parameters in case of a function
		datacode.Info = GetInfo(name);
		// When a value had a member that was dereferenced.
		bool startover;
		do
		{
			startover = false;
			//
			switch (datacode.Info->Id)
			{
				case idUNKNOWN:
					SetErrorValue(aeUNKNOWN_SYMBOL, name);
					break;

				case idCONSTANT:
				case idVARIABLE:
				{
					// Check the next character
					if (strchr("<>^&|= +-/*)!,%;", Cmd[Pos]))
					{
						datacode.Object->GetSetValue(datacode.Info, &result, nullptr, false);
						// Set the left value.
						left = datacode;
					}
					else
						// Check for dereferenced object.
					if (Cmd[Pos] == '.')
					{
						// Skip the dot character.
						Pos++;
						EatWhite();
						// Get 'self' pointer from the object.
						TValue objself;
						// Is the current info structure already derived.
						datacode.Object->GetSetValue(datacode.Info, &objself, nullptr, false);
						// Cast the value to a info pointer type.
						datacode.Object = CastToObject(objself);
						// Generate an error when the object is not valid.
						if (!datacode)
						{
							return SetErrorValue(aeIS_NOT_AN_OBJECT, name);
						}
						// Get the member name.
						GetName(name);
						// Get the info struture of the passed object member name.
						datacode.Info = datacode.Object->GetInfo(name);
						// Check for an error and an unknown ID.
						if (!datacode.Info || datacode.Info->Id == idUNKNOWN)
						{
							return SetErrorValue(aeUNKNOWN_OBJECTMEMBER, name);
						}
						else
						{
							// Make the same loop a gain.
							startover = true;
						}
					}
					else
					{
						return SetErrorValue(aeUNEXPECTED_CHARACTER, std::string(1, Cmd[Pos]));
					}
				}
					break;

				case idFUNCTION:
				{
					// Check if the next character is a '(' one
					if (Cmd[Pos] == '(')
					{
						// Create an array of TValue for parameter storage.
						TValue::vector_type params;
						// Check if there are more then one parameters
						if (Cmd[Pos + 1] != ')')
						{
							// Get parameters in the array.
							do
							{
								Pos++;
								TValue loc_res;
								GetParameter(loc_res);
								if (ErrorValue)
								{
									return false;
								}
								params.Add(loc_res);
							}
							while (Cmd[Pos] != ')');
						}
						else
						{
							// Skip ')' character in case of function having no parameters.
							Pos++;
						}
						// Check if there are limits to the amount of parameters passed to the function.
						if (datacode.Info->ParamCount != INT_MAX)
						{
							// Get the minimum amount of parameters needed for this function
							uint n = abs(datacode.Info->ParamCount);
							// Check in any case if there are sufficient parameters passed
							if (params.Count() > n && datacode.Info->ParamCount > 0)
							{
								SetErrorValue(aeTOO_MANY_PARAMS, datacode.Info->Name);
							}
							// Check the minimum amount of parameters needed
							if (params.Count() < n)
							{
								SetErrorValue(aeTOO_FEW_PARAMS, datacode.Info->Name);
							}
							if (GetErrorValue() != aeSUCCESS)
							{
								return true;
							}
						}
						// Prevent exception from continuing from here.
						bool berr = false;
						try
						{
							datacode.Object->GetSetValue(datacode.Info, &result, &params, false);
						}
						catch (...) //(Exception& e)
						{
							berr = true;
						}
						//
						if (berr)
						{
							_RTTI_NOTIFY(DO_DEFAULT, "Error in function ' " << datacode.Info->Name << " ' !")
							return SetErrorValue(aeFUNCTION_ERROR, datacode.Info->Name);
						}
						else
						{
							Pos++;
							if (Cmd[Pos] == '.')
							{
								// Skip the dot character.
								Pos++;
								EatWhite();
								// Cast the value to a info pointer type.
								datacode.Object = CastToObject(result);
								// Generate an error when the object is not valid.
								if (!datacode)
								{
									return SetErrorValue(aeIS_NOT_AN_OBJECT, name);
								}
								// Get the member name.
								GetName(name);
								// Get the info struture of the passed object member name.
								datacode.Info = datacode.Object->GetInfo(name);
								// Check for an error and an unknown ID.
								if (!datacode.Info || datacode.Info->Id == idUNKNOWN)
								{
									return SetErrorValue(aeUNKNOWN_OBJECTMEMBER, name);
								}
								else
								{
									// Make the same loop a gain.
									startover = true;
								}
								//
								result.SetType(TValue::vitUNDEF);
							}
						}
					}
					else
					{
						return SetErrorValue(aeEXPECTED_FUNCTION, datacode.Info->Name);
					}
				}
					break;

				default:
					return SetErrorValue(aeUNEXPECTED_IDENTIFIER, name);
			} // switch
			//
			EatWhite();
		}
		while (startover);
		//
		return true;
	}
	// Start of string parameter i.e. "\"mijn naam\"" " is haas\n" .
	if (Cmd[Pos] == '"')
	{
		// Holds the characters.
		std::string s;
		// Flag to indicate regular expression.
		bool escaped = false;
		// Iterate as long as double quoted regions are available.
		while (Cmd[Pos] == '"')
		{
			int pos = Pos++;
			// As long as the end of the script is not reached.
			do
			{ // Check for escaped expression sequence.
				if (Cmd[pos++] == '\\')
				{
					// Set the flag that a escaped expression has passed.
					escaped = true;
					// Always skip the next character.
					pos++;
				}
			}
			while (Cmd[pos] && Cmd[pos] != '\"');
			// Add the string to the resulting string.
			s.append(Cmd, Pos, pos - Pos);
			// Update the position index.
			Pos = pos + 1;
			// Eat the white between the succesive double quoted regions.
			EatWhite();
		}
		// Check the max length.
		if (s.length() > MAX_STRING_LENGTH)
		{
			return SetErrorValue(aeSTRING_TOO_LONG, std::string(Cmd, Pos, MAX_STRING_LENGTH));
		}
		// Convert the string if needed.
		if (escaped)
		{
			result.Set(unescape(s));
		}
		else
		{
			result.Set(s);
		}
		return true;
	}
	//
	return SetErrorValue(aeUNEXPECTED_CHARACTER);
}

bool TScriptCalc::GetParameter(TValue& value)
{
	value.SetType(value.vitUNDEF);
	do
	{
		if (!Partial(value))
		{
			return false;
		}
		if (')' == Cmd[Pos])
		{
			return true;
		}
		if (',' == Cmd[Pos])
		{
			break;
		}
	}
	while (!ErrorValue && Cmd[Pos]);
	//
	return false;
}

bool TScriptCalc::Calculate(const std::string& script, TValue& result)
{
	const char* prev_cmd = Cmd;
	int prev_pos = Pos;
	Cmd = script.c_str();
	Pos = 0;
	SetErrorValue(aeSUCCESS);
	result.SetType(result.vitUNDEF);
	// execute until character '\0'
	while (Cmd[Pos])
	{
		Partial(result);
		// check for error so far
		if (ErrorValue)
		{
			break;
		}
		// Check for unwanted ending characters.
		if (!strchr("+-/*&|^=<>!%", Cmd[Pos]))
		{ // report an error
			SetErrorValue(aeEXPECTED_DELIMITER);
			return false;
		}

	}
	// If there is an error return flase whithout restoring the previous situation.
	if (ErrorValue)
	{
		return false;
	}
	// Restore previous command string and position when there is no error.
	Cmd = prev_cmd;
	Pos = prev_pos;
	return true;
}

bool TScriptCalc::SetErrorValue(EArithError error_value, std::string reason)
{
	bool retval = error_value == aeSUCCESS;
	if (ErrorValue != error_value)
	{
		ErrorValue = error_value;
		ErrorReason = std::move(reason);
		_COND_RTTI_NOTIFY(!retval, DO_DEFAULT,
			"Error in script: '" << ErrorReason << "' " << GetArithErrorText(ErrorValue));
	}
	return retval;
}

std::string TScriptCalc::GetErrorReason() const
{
	return ErrorReason;
}

class TFunctionScriptCalc :public TScriptCalc
{
	public:
		TFunctionScriptCalc()
		{
			memset(FValue, 0, sizeof(FValue));
		}

		//
		void SetValue(int index, double value) {FValue[index] = value;}

	protected:
		// Gets identifier information.
		[[nodiscard]] const TInfo* GetInfo(const std::string& name) const override;

		// Overloaded from base class.
		bool GetSetValue(const TInfo*, TValue*, TValue::vector_type*, bool) override;

	private:
		enum {MAX_VALUES = 3};
		// Holds the level passed in the GetLevelIntensity().
		double FValue[MAX_VALUES]{};
};

bool TFunctionScriptCalc::GetSetValue(const TInfo* info, TValue* result,
	TValue::vector_type* param, bool flag_set)
{
	if (info->Index > 0 && info->Index < MAX_VALUES)
	{
		result->Set(FValue[info->Index - 1]);
		return true;
	}
	return TScriptCalc::GetSetValue(info, result, param, flag_set);
}

double ScriptCalc(const std::string& script, double def)
{
	TValue result(0.0);
	TFunctionScriptCalc fsc;
	if (fsc.Calculate(script, result) && result.IsNumber())
	{
		return result.GetFloat();
	}
	return def;
}

const TFunctionScriptCalc::TInfo* TFunctionScriptCalc::GetInfo(const std::string& name) const
{
	static TScriptCalc::TInfo Info[] =
		{
			{1, idCONSTANT, "x", 0, nullptr},
			{2, idCONSTANT, "y", 0, nullptr},
			{3, idCONSTANT, "z", 0, nullptr}
		};
	for (auto& i : Info)
	{
		if (i.Name == name)
		{
			return &i;
		}
	}
	return TScriptCalc::GetInfo(name);
}

double ScriptCalc(const std::string& script, double def, double x, double y, double z)
{
	TValue result(0.0);
	TFunctionScriptCalc fsc;
	fsc.SetValue(0, x);
	fsc.SetValue(1, y);
	fsc.SetValue(2, z);
	if (fsc.Calculate(script, result) && result.IsNumber())
	{
		return result.GetFloat();
	}
	return def;
}

}
