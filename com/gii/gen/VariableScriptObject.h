#pragma once

#include <misc/gen/ScriptObject.h>
#include "Variable.h"

namespace sf
{

/**
 * @brief Script object for exporting GII parameters to a script object.
 */
class VariableScriptObject :public ScriptObject, public Variable, private VariableHandler
{
	public:
		explicit VariableScriptObject(const Parameters& params);

	protected:
		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		void destroyObject(bool& should_delete) override
		{
			should_delete = true;
		}

		// Overloaded from base class.
		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

	private:
		void variableEventHandler(EEvent event, const Variable& call_var, Variable& link_var, bool same_inst) override;
		// Skips event for this instance.
		bool _skipEvent{false};

		// Instruction pointer to jump to on a change of value. 0 means disabled.
		ip_type _onValue{0};
		// Instruction pointer to jump to on a change of flags. 0 means disabled.
		ip_type _onFlags{0};
		// Instruction pointer to jump to on a change of ID. 0 means disabled.
		ip_type _onId{0};

		static ScriptObject::IdInfo _info[];
};

}