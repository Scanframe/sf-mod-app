#pragma once

#include <gii/gen/InformationServer.h>
#include "RsaTypes.h"
#include "global.h"

namespace sf
{

class RsaInterface;
/**
 * @brief RSA implementation of a information server.
 */
class _RSA_CLASS RsaServer :public InformationServer, public RsaTypes
{
	public:
		/**
		 * @brief Constructor.
		 *
		 * @param compatible Compatibility switch. 1=Strip-chart 0=Current 2=Nerason
		 * @param deviceNumber The device number used for creating an implementation. (default is UT)
		 * @param serverName Server name added to the settings and results. (Default is 'Acquisition')
		 */
		explicit RsaServer(int compatible, int deviceNumber = -1, const std::string& serverName = {});

		/**
		 * Destructor.
		 */
		~RsaServer() override;

		/**
		 * Gets the name given at the constructor.
		 */
		[[nodiscard]] std::string getServerName() const;

		/**
		 * @brief Creates an attached implementation by name.
		 */
		bool createImplementation(const std::string& name);

		/**
		 * @brief Gets the name of the current implementation.
		 */
		[[nodiscard]] std::string getImplementationName() const;

		/**
		 * @brief Destroys the created implementation.
		 */
		void destroyImplementation()
		{
			createImplementation(-1);
		}

		/**
		 * @brief Gets a pointer to the current acquisition implementation.
		 */
		RsaInterface* getAcquisition() {return _acquisition;}

		/**
		 * @brief Returns true whe all parameters are locked.
		 */
		[[nodiscard]] bool isLocked() const {return _lock;}

		/**
		 * @brief Locks the variables by making them all read only.
		 */
		void setLocked(bool);

	protected:
		/**
		 * @brief Creates the implementation of index on the registered implementations.
		 * @return True on success.
		 */
		bool createImplementation(int index);

		/**
		 * @brief Create interface parameters at construction once.
		 */
		void createInterface();

		/**
		 * @brief Remove all parameters and results.
		 */
		void destroyInterface();

		/**
		 * @brief Looks up a parameter which is represented by the interface id.
		 */
		[[nodiscard]] Variable::Vector::size_type variableListFind(id_type id) const;

		/**
		 * @brief Creates a variable with a extra info structure attached.
		 */
		bool createVariable(Variable*& var, RsaTypes::ParamInfo& info, const std::string& setup);

		/**
		 * @brief Destroys a variable including the attached extra info structure.
		 */
		void destroyVariable(Variable* var);

		/**
		 * @brief Creates a setup string from the passed parameter info structure.
		 */
		std::string createSetupString(const RsaTypes::ParamInfo& info, long vid);

		/**
		 * @brief Evaluate parameters after the configuration has changed.
		 */
		void evaluateInterfaceParams();

		/**
		 * @brief Gets the name of the channel using the device name.
		 */
		std::string getNameOffset(const RsaTypes::ParamInfo& info);

		/**
		 * @brief Gets the description prefix of the channel.
		 */
		std::string getDescription(const RsaTypes::ParamInfo& info);

		/**
		 * @brief Looks up a result data which is represented by the interface id.
		 */
		[[nodiscard]] ResultData::Vector::size_type resultListFind(id_type id) const;

		/**
		 * @brief Creates a variable with a extra info structure attached.
		 */
		bool createResultData(ResultData*& res, RsaTypes::ResultInfo& info, const std::string& setup);

		/**
		 * @brief Destroys a variable including the attached extra info structure.
		 */
		void destroyResultData(ResultData* var);

		/**
		 * @brief Creates a setup string from the passed result info structure.
		 */
		std::string createSetupString(const RsaTypes::ResultInfo& info,long vid);

		/**
		 * @brief Evaluate results after the configuration has changed.
		 */
		void evaluateInterfaceResults();

		/**
		 * @brief Returns the name of the channel using the device name.
		 */
		std::string getNameOffset(const RsaTypes::ResultInfo& info);

		/**
		 * @brief Returns the description prefix of the channel.
		 */
		std::string getDescription(const RsaTypes::ResultInfo& info);

		/**
		 * @brief Structure placed in the Variable's Data field for fast access to information.
		 */
		struct ExtraInfo
		{
			/** Holds the channel number. */
			unsigned _channel;
			/** Holds the interface id. */
			IdType _id;
			/** Holds the last setup-string for comparison. */
			std::string _setupString;
			/** Flags copied from the ParamInfo structure. */
			int _flags;
		};

		/**
		 * @brief Easy to use function to access the extra info from a variable.
		 */
		ExtraInfo* castExtraInfo(const Variable* var);

		/**
		 * @brief Easy to use function to access the extra info from a result data.
		 */
		ExtraInfo* castExtraInfo(const ResultData* var);

		/**
		 * @brief Overloaded from TInfoServer.
		 */
		void onStateChange(EState prevState, EState nextState) override;

		/**
		 * @brief Hook function indirectly called from the implementation.
		 */
		void paramNotify(id_type id);

		/**
		 * @brief Hook function indirectly called from the implementation.
		 */
		void resultNotify(id_type id);

		/**
		 * @brief Sets the variables to readonly based on the flags and lock status.
		 */
		void checkReadOnly();

		/**
		 * @brief Parameter hook function called directly by the implementation.
		 */
		static void paramNotifyProc(void* data, id_type id);

		/**
		 * @brief Result hook function called directly by the implementation.
		 */
		static void resultNotifyProc(void* data, id_type id);

		/**
		 * @brief Overridden from InformationServer.
		 */
		void clearValidations() override;
		/**
		 * @brief Holds the acquisition implementation.
		 */
		RsaInterface* _acquisition;
		/**
		 * @brief Variable for selecting an implementation.
		 */
		Variable _vImplementation;
		/**
		 * @brief Callback hook for variable events.
		 */
		TVariableHandler<RsaServer> _serverVariableHandler;
		/**
		 * @brief Event handler for variables.
		 */
		void serverVariableHandler(Variable::EEvent event, const Variable& caller, Variable& linker, bool same_inst);
		/**
		 * @brief Holds all created variables for the selected implementation.
		 */
		Variable::Vector _variableVector;
		/**
		 * @brief Callback hook for result events.
		 */
		TResultDataLinkHandler<RsaServer> _serverResultDataHandler;
		/**
		 * @brief Event handler for results.
		 */
		void serverResultDataHandler(ResultData::EEvent event,const ResultData& caller,ResultData& link,const Range& rng,bool same_inst);
		/**
		 * @brief Holds all created results for this instance.
		 */
		ResultData::Vector _resultVector;
		/**
		 * @brief Holds the device number passed at the constructor.
		 */
		long _deviceNumber;
		/**
		 * @brief Holds the VID of the implementation.
		 */
		long _implementationId;
		/**
		 * @brief Holds the compatibility state passed at the constructor.
		 */
		int _compatible;
		/**
		 * @brief Holds the device name for creating the variable names.
		 */
		std::string _serverName;
		/**
		 * @brief Variable indicating lock status.
		 */
		bool _lock;
		/**
		 * @brief ID of interface parameter being processed by the param hook.
		 */
		IdType _handledParamId;
};

}
