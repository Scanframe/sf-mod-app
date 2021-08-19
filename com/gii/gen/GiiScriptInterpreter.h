#pragma once
#include <misc/gen/ScriptInterpreter.h>
#include "Variable.h"
#include "ResultData.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Script extending and linking the script to the generic information interface elements.
 */
class _GII_CLASS GiiScriptInterpreter :public ScriptInterpreter
{
	public:
		/**
		 * @brief Default constructor.
		 */
		GiiScriptInterpreter() = default;

		/**
		 * @brief Overloaded from base class.
		 */
		[[nodiscard]] strings getInfoNames() const override;

		/**
		 * @brief Overloaded from base class.
		 */
		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

	protected:

		/**
		 * @brief Gets and sets a value using a returned TInfo structure.
		 *
		 * @return True if request was answered.
		 */
		bool getSetValue(const IdInfo*, Value* value, Value::vector_type* params, bool flag_set) override;

	private:
		/**
		 * @brief Holds a static list of identifier info.
		 */
		static IdInfo _infoList[];
};

}
