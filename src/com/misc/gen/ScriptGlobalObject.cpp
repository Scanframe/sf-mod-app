#include "ScriptGlobalObject.h"
#include "ScriptGlobalEntry.h"

namespace sf
{

SF_REG_CLASS(
	ScriptObject, ScriptObject::Parameters, Interface,
	ScriptGlobalObject, "Global", "Global entry access object."
)

ScriptGlobalObject::ScriptGlobalObject(const Parameters& params)
	: ScriptObject("Global")
{
}

// Speed index.
enum : int
{
	sidExist = 1,
	sidEntry
};

ScriptObject::IdInfo ScriptGlobalObject::_objectInfo[] = {
	{sidExist, ScriptObject::idFunction, "Exist", 1},
};

const ScriptObject::IdInfo* ScriptGlobalObject::getInfo(const std::string& name) const
{
	for (auto& i: _objectInfo)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<ScriptGlobalObject*>(this);
			//
			return &i;
		}
	}
	// Find a global entry.
	if (auto gse = ScriptGlobalEntry::getEntry(name))
	{
		auto& info = const_cast<IdInfo&>(_globalIdInfo);
		info._index = sidEntry;
		info._id = idFunction;
		info._name = gse->getName().c_str();
		info._paramCount = gse->getArgumentCount();
		info._data = gse;
		return &_globalIdInfo;
	}
	return getInfoUnknown();
}

bool ScriptGlobalObject::getSetValue(const ScriptObject::IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	switch (info->_index)
	{
		default:
			return false;

		case sidExist:
			value->set(!!ScriptGlobalEntry::getEntry((*params)[0].getString()));
			break;

		case sidEntry: {
			if (auto gse = static_cast<ScriptGlobalEntry*>(_globalIdInfo._data))
			{
				gse->call(*params, *value);
			}
			break;
		}
	}
	//
	return true;
}

}// namespace sf
