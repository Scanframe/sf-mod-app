#pragma once
#pragma once

#include "Variable.h"
#include "ResultData.h"
#include "../global.h"

namespace sf
{

class _GII_CLASS InformationServer :public InformationTypes
{
	public:
		/**
		 * @brief Default constructor
		 */
		InformationServer();

		/**
		 * Destructor
		 */
		virtual ~InformationServer();

		/**
		 * @brief When device mask is non-zero, variables are added automatically on creation to this info server.
		 *
		 * @param name Name of the server.
		 * @param namePrefix Prefix of the name.
		 * @param vid Variable id of state parameter.
		 * @param deviceMask Device vid mask of this server.
		 * @param serverMask Server vid mask of this server.
		 */
		void setup(const std::string& name, const std::string& namePrefix, long vid, long deviceMask, long serverMask);

		/**
		 * @brief Clears the instance. Also called from setup.
		 */
		void flush();

		/**
		 * @brief Enumerate for state of the info server.
		 */
		enum EState
		{
			/** Not generating results. */
			issOff = 0,
			/** Generating results, and flushing. */
			issRun,
			/** Generating results, no flushing. */
			issRecord,
			/** Not generating results, saving parameters. */
			issPause,
			/** Not generating results, saving results. */
			issStop,
			/** Max states count. */
			issMaxState
		};

		/**
		 * @brief Gets the current state.
		 */
		[[nodiscard]] EState getState() const;

		/**
		 * @brief Gets the name of the passed state.
		 */
		[[nodiscard]] const char* getStateName(int state) const;

		/**
		 * @brief Sets the current state.
		 */
		void setState(EState);

		/**
		 * @brief Variable/parameter class enumerator.
		 */
		enum EClass
		{
			/** Parameters of this class will always be writable. */
			clA = 0,
			/** Parameters of this class should be read-only during measurement. */
			clB,
			/** Parameters of this class effect the result-geometry, and should be read-only during measurement and recording. */
			clC,
			/** Number of classes.*/
			clMaxClass
		};

		/**
		 * @brief Determines if the passed id belongs to this info server.
		 */
		[[nodiscard]] bool isServerId(id_type id) const;

		/**
		 * @brief Adds a variable to the variables vector.
		 */
		void attachVariable(Variable* var, EClass cls);

		/**
		 * @brief
		 */
		// Removes a variable from the variable vector.
		void detachVariable(Variable* var);

		// Adds a result to the result vector.
		/**
		 * @brief
		 */
		void attachResult(ResultData* res);

		// Removes a result from the result vector.
		/**
		 * @brief
		 */
		void detachResult(ResultData* res);

		// Reports if this server generates results depending on its current state.
		/**
		 * @brief
		 */
		bool isGeneratingResults();

		// To be overloaded by a derived class to handle a state change if necessary.
		/**
		 * @brief
		 */
		virtual void onStateChange(EState prevState, EState nextState) = 0;

		// Clears the committed ranges of the attached results.
		// This one can be overloaded by the derived class.
		// When so this function must be called as well.
		/**
		 * @brief
		 */
		virtual void clearValidations();

	private:
		/**
		 * @brief Event handler for linked state variable.
		 */
		void variableEventHandler(Variable::EEvent, const Variable&, Variable&, bool);

		/**
		 * @brief Sets or unsets the write-flag for all attached variables of a specific class.
		 */
		void setWriteFlagForClass(EClass, bool write_enable);

		/**
		 * @brief Enables or disables all archive flags from variables which have archive set at creation/setup.
		 */
		void setArchiveFlag(bool write_enable);

		/**
		 * @brief Enables or disables recycling on the results having the archive flag set.
		 * Non archived result have always the recycle mode enabled.
		 */
		void setRecycleFlag(bool recycle_enable);

		/**
		 * @brief Jump table function type.
		 */
		typedef void (InformationServer::*VoidFunction)();

		/**
		 * @brief Jump table function.
		 */
		void stateChangeUnknown();

		/**
		 * @brief Jump table function.
		 */
		void stateChangeIllegal();

		/**
		 * @brief Jump table function.
		 */
		void stateChangeStopToRun();

		/**
		 * @brief Jump table function.
		 */
		void stateChangeRunToStop();

		/**
		 * @brief Jump table function.
		 */
		void stateChangeOffToRun();

		/**
		 * @brief Jump table function.
		 */
		void stateChangeRunToRecord();

		/**
		 * @brief Jump table function.
		 */
		void stateChangeRecordToPause();

		/**
		 * @brief Jump table function.
		 */
		void stateChangePauseToStop();

		/**
		 * @brief Jump table function.
		 */
		void stateChangeAnyToOff();

		/**
		 * @brief State variable that controls this information server.
		 */
		Variable _vState;
		/**
		 * @brief Holds the previous state of the server.
		 */
		EState _prevState;
		/**
		 * @brief Holds the current state of the server.
		 */
		EState _curState;
		/**
		 * @brief Holds the combination of the server device mask of the server.
		 */
		id_type _serverDeviceMask;
		/**
		 * @brief Holds the device id of this server.
		 */
		id_type _serverDeviceId;
		/**
		 * @brief A jump table that defines the functions to be called for each possible state transition.
		 */
		VoidFunction _jumpTable[issMaxState][issMaxState]{};
		/**
		 * @brief Lists of all variables, sorted by class.
		 */
		Variable::PtrVector _variableList[clMaxClass];
		/**
		 * @brief List of result-data pointers.
		 */
		ResultData::PtrVector _resultList;
		/**
		 * @brief Declaration of variable linker so event handler is linked.
		 */
		TVariableHandler<InformationServer> _variableHandler;
};

}
