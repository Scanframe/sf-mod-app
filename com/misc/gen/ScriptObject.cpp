#include "ScriptObject.h"

namespace sf
{

SF_IMPL_IFACE(ScriptObject, ScriptObject::Parameters, Interface)

ScriptObject::ScriptObject(const char* type_name, ScriptObject* parent)
	:_typeName(type_name)
	,_parent(parent)
{}

const ScriptObject::IdInfo* ScriptObject::getInfoUnknown()
{
	static IdInfo info = {0, idUnknown, nullptr, 0, nullptr};
	return &info;
}

ScriptObject* ScriptObject::castToObject(const Value& value)
{
	// When the variable is of the special type an object is referenced.
	if (value.getType() == Value::vitCustom && value.getSize() == sizeof(void*))
	{
		struct TData {ScriptObject* Object;} * data;
		// Cast binary data to the pointer type.
		data = (TData*) value.getData();
		// Check for a null pointer.
		return data ? data->Object : nullptr;
	}
	// Failed to cast the value.
	return nullptr;
}

ScriptObject::operator Value() const
{
	const ScriptObject* so = this;
	if (so)
	{
		return {Value::vitCustom, &so, sizeof(&so)};
	}
	// Return zero integer when the object is valid.
	return Value(0);
}

std::string ScriptObject::getTypeName() const
{
	return _typeName;
}

void ScriptObject::makeParent(ScriptObject* so)
{
	so->_parent = this;
}

void ScriptObject::setParent(ScriptObject* parent)
{
	_parent = parent;
}

}
