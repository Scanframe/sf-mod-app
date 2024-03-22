#pragma once

#include "ScriptInterpreter.h"

namespace sf
{

/**
 * @brief Structure to link a Value to an IdInfo structure.
 */
struct ScriptInterpreter::VariableInfo : IdInfo
{
		// Default constructor
		VariableInfo(const char* name, Value::EType type, int index = 0)
			: _value(type)
		{
			_data = &_value;
#if IS_WIN
			_name = _strdup(name);
#else
			_name = strdup(name);
#endif

			_id = ScriptEngine::idVariable;
			_index = index;
			_paramCount = 0;
		}

		~VariableInfo()
		{
			free((char*) _name);
		}

		/**
	 * @brief Value typed data member for script variables.
	 */
		Value _value{};
};

/**
 * @brief Structure to link a labels to Info.
 */
struct ScriptInterpreter::LabelInfo : IdInfo
{
		/**
	 * Initializing constructor.
	 */
		LabelInfo(const char* name, int index, ip_type ip)
		{
#if IS_WIN
			_name = _strdup(name);
#else
			_name = strdup(name);
#endif
			// Label address is also a global constant.
			_id = ScriptEngine::idConstant;
			_index = index;
			_paramCount = 0;
			instructionPtr() = ip;
		}

		/**
	 * Destructor cleaning up the duplicated name.
	 */
		~LabelInfo()
		{
			free((char*) _name);
		}

		/**
	 * @brief Cast function of DataPtr member to ip_type.
	 *
	 * @return Reference to instruction pointer member.
	 */
		ip_type& instructionPtr()
		{
			return *(ip_type*) &_data;
		}
};

}// namespace sf