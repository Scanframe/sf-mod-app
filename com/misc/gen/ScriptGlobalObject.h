#pragma once

#include "Value.h"
#include "ScriptObject.h"
#include "../global.h"

namespace sf
{

class _MISC_CLASS ScriptGlobalObject :public ScriptObject
{
	public:

		explicit ScriptGlobalObject(const Parameters&);

		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

	protected:
		void destroyObject(bool& should_delete) override
		{
			should_delete = true;
		}

	private:
		// Dynamic function table entry TInfo instance.
		IdInfo _globalIdInfo{};

		static IdInfo _objectInfo[];
};

}
