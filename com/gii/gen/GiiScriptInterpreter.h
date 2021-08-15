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
class _GII_CLASS GiiScriptInterpreter :public ScriptInterpreter, public InformationTypes
{
	public:
		/**
		 * @brief Default constructor.
		 */
		GiiScriptInterpreter();

		/**
		 * @brief Overridden destructor.
		 */
		~GiiScriptInterpreter() override;

		/**
		 * @brief Overloaded from base class.
		 */
		[[nodiscard]] std::string getInfoNames() const override;

		/**
		 * @brief Overloaded from base class.
		 */
		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

	protected:
		/**
		 * @brief Forward definition of privately implemented type.
		 */
		struct GiiVariableInfo;
		/**
		 * @brief Forward definition of privately implemented type.
		 */
		struct GiiResultDataInfo;

		/**
		 * @brief Finds the import or exported variable by name and returns true on success.
		 *
		 * @param name
		 * @return When not found false is returned and 'ie' is set to NULL.
		 */
		GiiVariableInfo* _getVariableInfo(const std::string& name);

		// Gets and sets a value using a returned TInfo structure.
		// Returns true if request was answered.
		bool getSetValue(const IdInfo*, Value* value, Value::vector_type* params, bool flag_set) override;

		/**
		 * @brief Compile external supplied keyword.
		 *
		 * @param info
		 * @param source
		 * @return False on error.
		 */
		bool compileAdditionalStatement(const IdInfo* info, const std::string& source) override;

		/**
		 * Overloaded from base class to clear compiled data.
		 */
		void clear() override;

		/**
		 * @brief Overloaded from base class to link compiled data.
		 */
		void linkInstruction() override;

		/**
		 * @brief Overloaded is function called when script is terminated one way or an other.
		 */
		void exitFunction(EExitCode exitcode, const Value& value) override;

		/**
		 * @brief Variable event handler callback.
		 */
		void variableHandler(Variable::EEvent event, const Variable& caller, Variable& link, bool same_inst);

		/**
		 * @brief Result data event handler callback.
		 */
		void resultDataHandler(ResultData::EEvent event, const ResultData& caller, ResultData& link, const Range& rng, bool same_inst);

	private:
		/**
		 * @brief Vector of imports and exports that hold TVariableInfo entries.
		 */
		TVector<GiiVariableInfo*> _variableInfoList;
		/**
		 * @brief Vector of client results info structures which are created.
		 */
		TVector<GiiResultDataInfo*> _resultDataInfoList;
		/**
		 * @brief Dynamic function table entry IdInfo instance.
		 */
		IdInfo _infoFunction;
		/**
		 * @brief Holds a static list of identifier info.
		 */
		static IdInfo _infoList[];
		/**
		 * @brief Hook to member functions for GII interface for Variable instances.
		 */
		TVariableHandler<GiiScriptInterpreter> _varLink;
		/**
		 * @brief Hook to member functions for GII interface for ResultData instances.
		 */
		TResultDataLinkHandler<GiiScriptInterpreter> _resLink;
};

}
