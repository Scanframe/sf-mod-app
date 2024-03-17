#pragma once
#include <misc/gen/ScriptObject.h>
#include <misc/gen/IniProfile.h>

namespace sf
{

class IniProfileObject :public ScriptObject
{
	public:

		explicit IniProfileObject(const Parameters&);

		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

	protected:
		void destroyObject(bool& should_delete) override
		{
			should_delete = true;
		}

		IniProfile _iniProfile;

		static IdInfo _objectInfo[];
};

}
