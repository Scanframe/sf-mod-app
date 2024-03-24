#pragma once

#include <misc/gen/ScriptObject.h>
#include "ResultData.h"

namespace sf
{

/**
 * @brief Script object for exporting GII parameters to a script object.
 */
class ResultDataScriptObject :public ScriptObject, public ResultData, private ResultDataHandler
{
	public:
		explicit ResultDataScriptObject(const Parameters& params);

		std::string getStatusText() override;

	protected:

		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		void destroyObject(bool& should_delete) override
		{
			should_delete = true;
		}

		// Overloaded from base class.
		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

	private:
		void resultDataEventHandler(EEvent event, const ResultData& data, ResultData& resultData, const Range& range, bool b) override;

		// Instruction pointer to jump to on a change of ID. 0 means disabled.
		ip_type OnId{0};
		// Instruction pointer to jump to on a change of access range. 0 means disabled.
		ip_type OnAccess{0};
		// Instruction pointer to jump to on a clear event. 0 means disabled.
		ip_type OnClear{0};
		// Instruction pointer to jump to on a got data event. 0 means disabled.
		ip_type OnGotRange{0};

		static ScriptObject::IdInfo _info[];
};

}