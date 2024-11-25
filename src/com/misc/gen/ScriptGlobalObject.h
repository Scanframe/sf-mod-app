#pragma once
#include <misc/gen/ScriptObject.h>
#include <misc/gen/Value.h>
#include <misc/global.h>

namespace sf
{

/**
 * @brief Base class for a script to implementing
 */
class _MISC_CLASS ScriptGlobalObject
	: public ScriptObject
{
	public:
		explicit ScriptGlobalObject(const Parameters&);

		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

	protected:
		void destroyObject(bool& should_delete) override;

	private:
		/**
		 * @brief Dynamic function table entry instance.
		 */
		IdInfo _globalIdInfo{};

		static IdInfo _objectInfo[];
};

}// namespace sf
