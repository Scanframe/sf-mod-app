#include "ResultDataScriptObject.h"
#include <misc/gen/ScriptInterpreter.h>

namespace sf
{

SF_REG_CLASS(
	ScriptObject, ScriptObject::Parameters, Interface,
	ResultDataScriptObject, "ResultData", "Generic information interface result data."
)

// ResultData object members.
#define SID_ID 1
#define SID_NAME 2
#define SID_FLAGS 3
#define SID_OFFSET 4
#define SID_RANGE 5
#define SID_DATA 6
#define SID_BLOCKCOUNT 7
#define SID_REQUEST 8
// Events
#define SID_ON_ID 50
#define SID_ON_ACCESS 51
#define SID_ON_CLEAR 52
#define SID_ON_GOTRANGE 53

ScriptObject::IdInfo ResultDataScriptObject::_info[] =
	{
		{SID_ID, ScriptObject::idConstant, "Id", 0, nullptr},
		{SID_NAME, ScriptObject::idFunction, "Name", 1, nullptr},
		{SID_FLAGS, ScriptObject::idConstant, "Flags", 0, nullptr},
		{SID_OFFSET, ScriptObject::idConstant, "offset", 0, nullptr},
		{SID_RANGE, ScriptObject::idConstant, "Range", 0, nullptr},
		{SID_DATA, ScriptObject::idFunction, "Data", 1, nullptr},
		{SID_REQUEST, ScriptObject::idFunction, "Request", 2, nullptr},
		{SID_BLOCKCOUNT, ScriptObject::idVariable, "BlockCount", 0, nullptr},
		{SID_ON_ID, ScriptObject::idVariable, "OnId", 0, nullptr},
		{SID_ON_ACCESS, ScriptObject::idVariable, "OnAccess", 0, nullptr},
		{SID_ON_CLEAR, ScriptObject::idVariable, "OnClear", 0, nullptr},
		{SID_ON_GOTRANGE, ScriptObject::idVariable, "OnGotRange", 0, nullptr},
};

ResultDataScriptObject::ResultDataScriptObject(const ScriptObject::Parameters& params)
	: ScriptObject("ResultData", params._parent)
	, ResultData()
{
	// Link the handler.
	setHandler(this);
	// Need at least 2 arguments since the first is the object type name.
	if (params._arguments && !params._arguments->empty())
	{
		// When  a number assume it is an id.
		if (params._arguments->at(0).isNumber())
		{
			auto id = params._arguments->at(0).getInteger();
			// In case of a second parameter it is the id offset.
			if (params._arguments->size() > 1)
			{
				id += params._arguments->at(1).getInteger();
			}
			setup(id, true);
		}
		else
		{
			id_type id_ofs = (params._arguments->size() >= 2) ? params._arguments->at(1).getInteger() : 0;
			setup(params._arguments->at(0).getString(), id_ofs);
		}
	}
}

const ScriptObject::IdInfo* ResultDataScriptObject::getInfo(const std::string& name) const
{
	for (auto& i: _info)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<ResultDataScriptObject*>(this);
			return &i;
		}
	}
	return getInfoUnknown();
}

bool ResultDataScriptObject::getSetValue(const ScriptObject::IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	switch (info->_index)
	{
		case SID_ID: {
			if (flag_set)
			{
				setup(value->getInteger(), true);
			}
			else
			{
				value->set((Value::int_type) getId());
			}
			break;
		}

		case SID_NAME: {
			if (!flag_set)
			{
				value->set(getName((int) (*params)[0].getInteger()));
			}
			break;
		}

		case SID_OFFSET: {
			if (!flag_set)
			{
				value->set((Value::int_type) getValueOffset());
			}
			break;
		}

		case SID_RANGE: {
			if (!flag_set)
			{
				value->set((Value::int_type) getValueRange());
			}
			break;
		}

		case SID_DATA: {
			Value::int_type data = 0;
			// Check for a valid ID.
			if (getId() && getBlockSize() == 1 && getTypeSize() <= sizeof(Value::int_type))
			{
				Range::size_type index = (*params)[0].getInteger();
				// When negative take the last entered.
				if (index < 0)
				{
					index = getBlockCount() - 1;
				}
				if (index >= 0)
				{
					blockRead(index, 1, &data, false);
				}
			}
			value->set(getValue(&data));
			break;
		}

		case SID_BLOCKCOUNT: {
			value->set((Value::int_type) getBlockCount());
			break;
		}

		case SID_REQUEST: {
			Range::size_type ofs = (*params)[0].getInteger();
			Range::size_type sz = (*params)[1].getInteger();
			// When the size is negative the data till the end is requested.
			if (sz < 0)
			{
				sz = getBlockCount() - ofs;
			}
			//
			value->set(requestRange(ofs, sz));
			break;
		}

		case SID_FLAGS: {
			if (!flag_set)
			{
				value->set(getFlagsString());
			}
			break;
		}

		case SID_ON_ID: {
			if (flag_set)
			{
				OnId = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnId);
			}
			break;
		}

		case SID_ON_ACCESS: {
			if (flag_set)
			{
				OnAccess = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnAccess);
			}
			break;
		}

		case SID_ON_CLEAR: {
			if (flag_set)
			{
				OnId = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnClear);
			}
			break;
		}

		case SID_ON_GOTRANGE: {
			if (flag_set)
			{
				OnId = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnGotRange);
			}
			break;
		}

		default:
			return false;
	}
	return true;
}

void ResultDataScriptObject::resultDataEventHandler(ResultDataTypes::EEvent event, const ResultData& data, ResultData& resultData, const Range& range, bool b)
{
	auto si = dynamic_cast<ScriptInterpreter*>(getParent());
	if (event < ResultData::reFirstLocal)
	{
		return;
	}
	// Do not allow events when not compiled.
	if (si->getState() == ScriptInterpreter::esError || si->getState() == ScriptInterpreter::esCompiled)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Event avoided because script was in Error or not Initialized!\n"
										 << si->getDebugText());
		return;
	}
	//
	switch (event)
	{
		case ResultData::reAccessChange:
			if (OnAccess > 0)
			{
				si->callFunction(OnAccess, si->isStepMode());
			}
			break;

		case ResultData::reIdChanged:
			if (OnId > 0)
			{
				si->callFunction(OnId, si->isStepMode());
			}
			break;

		case ResultData::reClear:
			if (OnClear > 0)
			{
				si->callFunction(OnClear, si->isStepMode());
			}
			break;

		case ResultData::reGotRange:
			if (OnClear > 0)
			{
				si->callFunction(OnGotRange, si->isStepMode());
			}
			break;

		default:
			break;
	}
}

std::string ResultDataScriptObject::getStatusText()
{
	std::string rv;
	if (!getId())
	{
		rv.append("? ");
	}
	rv += "0x" + itostr(isOwner() ? getId() : getDesiredId(), 16);
	if (getId())
	{
		rv += ' ' + getCurFlagsString();
		rv += ' ' + getName();
	}
	return rv;
}

}// namespace sf