#include "ScriptGlobalEntry.h"

namespace sf
{

namespace
{

typedef std::map<std::string, ScriptGlobalEntry*> MapType;

// Pointer to the default vector which is automatically created when
MapType* mapGlobalScriptEntry;

}

ScriptGlobalEntry::ScriptGlobalEntry(const std::string& name, const std::string& description, int argumentCount) // NOLINT(modernize-pass-by-value)
	:_name(name)
	 , _argumentCount(argumentCount)
	 , _description(description)
{
	if (!mapGlobalScriptEntry)
	{
		mapGlobalScriptEntry = new MapType;
	}
	mapGlobalScriptEntry->insert(MapType::value_type(name, this));
}

ScriptGlobalEntry::~ScriptGlobalEntry()
{
	// Check if the vector still exists to detach from.
	if (mapGlobalScriptEntry)
	{
		mapGlobalScriptEntry->erase(_name);
	}
	// When the map is empty delete it.
	if (mapGlobalScriptEntry->empty())
	{
		delete_null(mapGlobalScriptEntry);
	}
}

ScriptGlobalEntry* ScriptGlobalEntry::getEntry(const std::string& name)
{
	if (mapGlobalScriptEntry)
	{
		auto it = mapGlobalScriptEntry->find(name);
		if (it != mapGlobalScriptEntry->end())
		{
			return it->second;
		}
	}
	return nullptr;
}

bool ScriptGlobalEntry::call(const std::string& name, const Value::vector_type& arguments, Value& result)
{
	// Check if the vector still exists to detach from.
	if (auto entry = getEntry(name))
	{
		entry->call(arguments, result);
		return true;
	}
	return false;
}

}
