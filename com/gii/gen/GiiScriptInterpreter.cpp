#include "GiiScriptInterpreter.h"
#include "VariableScriptObject.h"

namespace sf
{

// Speed index defines
#define SID_VARIABLE   1
#define SID_RESULTDATA 2


GiiScriptInterpreter::IdInfo GiiScriptInterpreter::_infoList[] =
	{
		// Functions
		{SID_VARIABLE, idFunction, "Variable", 1, nullptr},
		{SID_RESULTDATA, idFunction, "ResultData", 1, nullptr},
	};

strings GiiScriptInterpreter::getInfoNames() const
{
	strings rv = ScriptInterpreter::getInfoNames();
	for (auto& i : _infoList)
	{
		rv.add(i._name);
	}
	return rv;
}

const GiiScriptInterpreter::IdInfo* GiiScriptInterpreter::getInfo(const std::string& name) const
{
	for (auto& i: _infoList)
	{
		if (i._name == name)
		{
			return &i;
		}
	}
	// Call the inherited method.
	return ScriptInterpreter::getInfo(name);
}

bool GiiScriptInterpreter::getSetValue(const GiiScriptInterpreter::IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	switch (info->_index)
	{
		case SID_RESULTDATA:
		case SID_VARIABLE:
		{
			// Use class factory to create derived script object.
			auto vi = ScriptObject::Interface().create(info->_name, ScriptObject::Parameters(params, this));
			// Set the pointer to the derived class.
			value->set(Value::vitCustom, &vi, sizeof(&vi));
			break;
		}

		default:
			if (ScriptInterpreter::getSetValue(info, value, params, flag_set))
			{
				return true;
			}
			return setError(aeCompilerImplementationError, info->_name);
	}
	return true;
}

}

