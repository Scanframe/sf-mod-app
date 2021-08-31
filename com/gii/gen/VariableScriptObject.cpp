#include "VariableScriptObject.h"
#include <misc/gen/ScriptInterpreter.h>

namespace sf
{

SF_REG_CLASS
(
	ScriptObject, ScriptObject::Parameters, Interface,
	VariableScriptObject, "Variable", "Generic information interface variable."
)

// Variable script object members.
#define SID_SETUP       1
#define SID_ID          2
#define SID_NAME        3
#define SID_UNIT        4
#define SID_CUR         5
#define SID_DEF         6
#define SID_RND         7
#define SID_MIN         8
#define SID_MAX         9
#define SID_FLAGS      10
#define SID_ISFLAGS    11
#define SID_SETFLAGS   12
#define SID_UNSETFLAGS 13
#define SID_CURSTR     14
#define SID_ON_VALUE   50
#define SID_ON_FLAGS   51
#define SID_ON_ID      52
#define SID_SKIPEVENT  53

ScriptObject::IdInfo VariableScriptObject::_info[] =
	{
		{SID_SETUP, ScriptObject::idFunction, "Setup", 1, nullptr},
		{SID_ID, ScriptObject::idConstant, "Id", 0, nullptr},
		{SID_NAME, ScriptObject::idFunction, "Name", 1, nullptr},
		{SID_UNIT, ScriptObject::idConstant, "Unit", 0, nullptr},
		{SID_CUR, ScriptObject::idVariable, "Cur", 0, nullptr},
		{SID_CURSTR, ScriptObject::idConstant, "CurStr", 0, nullptr},
		{SID_RND, ScriptObject::idConstant, "Rnd", 0, nullptr},
		{SID_DEF, ScriptObject::idConstant, "Def", 0, nullptr},
		{SID_MIN, ScriptObject::idConstant, "Min", 0, nullptr},
		{SID_MAX, ScriptObject::idConstant, "Max", 0, nullptr},
		{SID_FLAGS, ScriptObject::idVariable, "Flags", 0, nullptr},
		{SID_ISFLAGS, ScriptObject::idFunction, "IsFlags", 1, nullptr},
		{SID_SETFLAGS, ScriptObject::idFunction, "SetFlags", 1, nullptr},
		{SID_UNSETFLAGS, ScriptObject::idFunction, "UnsetFlags", 1, nullptr},
		// Variable script object event related.
		{SID_ON_VALUE, ScriptObject::idVariable, "OnValue", 0, nullptr},
		{SID_ON_FLAGS, ScriptObject::idVariable, "OnFlags", 0, nullptr},
		{SID_ON_ID, ScriptObject::idVariable, "OnId", 0, nullptr},
		{SID_SKIPEVENT, ScriptObject::idVariable, "SkipEvent", 0, nullptr}
	};

VariableScriptObject::VariableScriptObject(const ScriptObject::Parameters& params)
	:ScriptObject("Variable", params._parent)
	 , Variable()
{
	// Link the handler.
	setHandler(this);
	// Need at least 2 arguments since the first is the object type name.
	if (params._arguments && !params._arguments->empty())
	{
		// When  a number assume it is an id.
		if (params._arguments->at(0).isNumber())
		{
			setup(params._arguments->at(0).getInteger(), true);
		}
		else
		{
			id_type id_ofs = (params._arguments->size() >= 2) ? params._arguments->at(1).getInteger() : 0;
			setup(params._arguments->at(0).getString(), id_ofs);
		}
	}
}

const ScriptObject::IdInfo* VariableScriptObject::getInfo(const std::string& name) const
{
	for (auto& i: _info)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<VariableScriptObject*>(this);
			return &i;
		}
	}
	return getInfoUnknown();
}

bool VariableScriptObject::getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	switch (info->_index)
	{
		default:
			// Signal that the request was not handled.
			return false;

		case SID_SETUP:
		{
			if ((*params)[0].isNumber())
			{
				value->set(setup((*params)[0].getInteger()));
			}
			else
			{
				value->set(setup((*params)[0].getString()));
			}
			break;
		}

		case SID_ID:
		{
			if (flag_set)
			{
				// Only allow setting the ID if the variable is owned.
				if (isGlobal())
				{
					setup(value->getInteger(), true);
				}
			}
			else
			{
				value->set((Value::int_type) getId());
			}
			break;
		}

		case SID_NAME:
		{
			if (!flag_set)
			{
				value->set(getName((int) (*params)[0].getInteger()));
			}
			break;
		}

		case SID_UNIT:
		{
			if (!flag_set)
			{
				value->set(getUnit());
			}
			break;
		}

		case SID_CUR:
		{
			if (flag_set)
			{
				if (!setCur(*value, _skipEvent))
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "Current value of '" << getName() << "' could not be set!");
				}
			}
			else
			{
				*value = getCur();
			}
			break;
		}

		case SID_CURSTR:
		{
			*value = getCurString();
			break;
		}

		case SID_DEF:
		{
			if (!flag_set)
			{
				*value = getDef();
			}
		}
			break;

		case SID_RND:
		{
			if (!flag_set)
			{
				*value = getRnd();
			}
			break;
		}

		case SID_MIN:
		{
			if (!flag_set)
			{
				*value = getMin();
			}
			break;
		}

		case SID_MAX:
		{
			if (!flag_set)
			{
				*value = getMax();
			}
			break;
		}

		case SID_FLAGS:
		{
			if (flag_set)
			{
				// Only allow setting the ID if the variable is owned.
				if (isGlobal())
				{
					updateFlags(Variable::toFlags(value->getString()), _skipEvent);
				}
			}
			else
			{
				*value = getCurFlagsString();
			}
			break;
		}

		case SID_ISFLAGS:
		{
			auto flags = Variable::toFlags((*params)[0].getString());
			// Check if all passed flags are set.
			value->set((getCurFlags() & flags) == flags);
			break;
		}

		case SID_SETFLAGS:
		{
			auto flags = Variable::toFlags((*params)[0].getString());
			flags |= getCurFlags();
			updateFlags(flags, _skipEvent);
			break;
		}

		case SID_UNSETFLAGS:
		{
			auto flags = Variable::toFlags((*params)[0].getString());
			flags = getCurFlags() & ~flags;
			updateFlags(flags, _skipEvent);
			break;
		}
		case SID_ON_VALUE:
		{
			if (flag_set)
			{
				_onValue = (ip_type) value->getInteger();
			}
			else
			{
				value->set(_onValue);
			}
			break;
		}

		case SID_ON_FLAGS:
		{
			if (flag_set)
			{
				_onFlags = (ip_type) value->getInteger();
			}
			else
			{
				value->set(_onFlags);
			}
			break;
		}

		case SID_ON_ID:
		{
			if (flag_set)
			{
				_onId = (ip_type) value->getInteger();
			}
			else
			{
				value->set(_onId);
			}
			break;
		}

		case SID_SKIPEVENT:
		{
			if (flag_set)
			{
				_skipEvent = value->getInteger() > 0;
			}
			else
			{
				value->set(_skipEvent);
			}
			break;
		}

	}
	// Signal that the request was handled.
	return true;
}

void VariableScriptObject::variableEventHandler(VariableTypes::EEvent event, const Variable& call_var, Variable& link_var, bool same_inst)
{
	(void) same_inst;
	auto si = dynamic_cast<ScriptInterpreter*>(getParent());
	if (event < Variable::veFirstLocal)
	{
		return;
	}
	// Do not allow events when not compiled.
	if (si->getState() == ScriptInterpreter::esError || si->getState() == ScriptInterpreter::esCompiled)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Event avoided because script was in Error or not Initialized!\n" << si->getDebugText());
		return;
	}
	switch (event)
	{
		case Variable::veValueChange:
			if (_onValue > 0)
			{
				si->callFunction(_onValue, si->isStepMode());
			}
			break;

		case Variable::veFlagsChange:
			if (_onFlags > 0)
			{
				si->callFunction(_onFlags, si->isStepMode());
			}
			break;

		case Variable::veIdChanged:
			if (_onId > 0)
			{
				si->callFunction(_onId, si->isStepMode());
			}
			break;

		default:
			break;
	}
}

}
