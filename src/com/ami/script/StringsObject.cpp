#include "IniProfileObject.h"
#include <misc/gen/ScriptObject.h>
#include <misc/gen/IniProfile.h>

namespace sf
{

class StringsObject :public ScriptObject
{
	public:
		// Constructor.
		StringsObject(QStringList* strings)
			:ScriptObject("Strings")
			 , Strings(strings)
			 , IsOwner(false)
		{
			// When the String's member is empty create a string list our self.
			if (!Strings)
			{
				IsOwner = true;
				Strings = new QStringList();
			}
		}

		~StringsObject()
		{ // When we own the strings instance delete it.
			if (IsOwner)
			{
				delete_null(Strings);
			}
		}

		const IdInfo* getInfo(const std::string& name) const override;

		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flagset) override;

		void destroyObject(bool& should_delete) override
		{
			should_delete = false;
		}

		// Holds pointer to the actual object.
		QStringList* Strings;
		//
		bool IsOwner;
		static IdInfo ObjectInfo[];
};

#define SID_COUNT     1
#define SID_NAMES     2
#define SID_VALUES    3
#define SID_STRINGS   4
#define SID_TEXT      5
#define SID_COMMATEXT 6

ScriptObject::IdInfo StringsObject::ObjectInfo[] =
	{
		{SID_COUNT, ScriptObject::idConstant, "Count", 0, nullptr},
		{SID_NAMES, ScriptObject::idFunction, "Names", 1, nullptr},
		{SID_VALUES, ScriptObject::idFunction, "Values", 1, nullptr},
		{SID_STRINGS, ScriptObject::idFunction, "Strings", 1, nullptr},
		{SID_TEXT, ScriptObject::idVariable, "Text", 0, nullptr},
		{SID_COMMATEXT, ScriptObject::idVariable, "CommaText", 0, nullptr},
	};

const ScriptObject::IdInfo* StringsObject::getInfo(const std::string& name) const
{
	for (auto& i: ObjectInfo)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<ScriptObject*>(this);
			//
			return &i;
		}
	}
	return ScriptObject::getInfoUnknown();
}

bool StringsObject::getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	//
	switch (info->_index)
	{
		default:
			return false;

		case SID_COUNT:
		{
			value->set((Value::int_type) Strings->size());
		}
			break;

		case SID_NAMES:
		{
			if ((*params)[0].isNumber())
			{
				auto index = (*params)[0].getInteger();
				if (index >= 0 && index < Strings->size())
				{
					value->set(Strings->at(index));
				}
				else
				{
					value->set("");
				}
			}
			else
			{
				value->set((Value::int_type) Strings->indexOf((*params)[0].getQString()));
			}
		}
			break;

		case SID_VALUES:
		{
			value->set(Strings->Values[(*params)[0].GetString()]);
		}
			break;

		case SID_STRINGS:
		{
			int index = (*params)[0].GetInteger();
			if (index >= 0 && index < Strings->Count)
			{
				value->Set(Strings->Strings[index]);
			}
			else
			{
				value->Set("");
			}
		}
			break;

		case SID_TEXT:
		{
			if (flag_set)
			{
				value->Set(Strings->Text);
			}
			else
			{
				Strings->Text = value->GetString();
			}
		}
			break;

		case SID_COMMATEXT:
		{
			if (flag_set)
			{
				value->Set(Strings->CommaText);
			}
			else
			{
				Strings->CommaText = value->GetString();
			}
		}
			break;
	}
	//
	return true;
}

}
