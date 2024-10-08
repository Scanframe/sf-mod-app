#pragma once

#include "RsaTypes.h"
#include "global.h"
#include <misc/gen/TClassRegistration.h>
#include <misc/gen/TVector.h>
#include <misc/gen/Value.h>
#include <utility>

namespace sf
{

// Forward definition.
class PropertySheetDialog;

/**
 * @brief Base class for the repetitive signal acquisition interface.
 */
class _RSA_CLASS RsaInterface : public RsaTypes
{
	public:
		struct Parameters
		{
				explicit Parameters(int mode)
					: _mode(mode)
				{}

				int _mode;
		};

		/**
		 * @brief Constructor for passing general structure for derived classes.
		 */
		explicit RsaInterface(const Parameters&);

		/**
		 * @brief Virtual destructor for derived classes.
		 */
		virtual ~RsaInterface();

		/**
		 * @brief Initializes the implementation for operation. Calls DoInitialize.
		 * @return True on success.
		 */
		bool initialize();

		/**
		 * @brief Un-initializes the implementation. Calls DoInitialize.
		 * @return True on success.
		 */
		bool uninitialize();

		/**
		 * Gets the type of acquisition.
		 * @return By default, the type returned ultrasonic.
		 */
		[[nodiscard]] EAcquisitionType getType() const;

		/**
		 * @brief Writes the non exported parameters to profile.
		 * @param rd When true settings are read and with false written.
		 * @return True on success.
		 */
		bool settingsReadWrite(bool rd);

		/**
		 * @brief Gets the error value.
		 */
		[[nodiscard]] int getError() const;

		/**
		 * @brief Gets the amount of channels for this implementation.
		 */
		unsigned getChannelCount();

		/**
		 * @brief Gets the amount of gates for this channels' implementation.
		 *
		 * @param channel Channel number.
		 * @return Channel count
		 */
		unsigned getGateCount(unsigned channel);

		/**
		 * @brief Gets the name of the passed gate and channel.
		 *
		 * @param gate Gate number
		 * @param channel Channel number.
		 * @return Name of the gate.
		 */
		std::string getGateName(unsigned int gate, unsigned int channel = 0);

		/**
		 * @brief Sets the pop manual state for a channel.
		 *
		 * @param channel Channel number.
		 * @param pm Manual pop mode.
		 * @return True on success.
		 */
		bool setPopManual(int channel, EPopManual pm);

		/**
		 * @brief Gets the pop-manual state on a channel.
		 * @param channel Channel number.
		 * @return Mode of pop manual.
		 */
		[[nodiscard]] EPopManual getPopManual(int channel) const;

		/**
		 * Retrieve information about the passed param id.
		 * @param id id Parameter identifier.
		 * @param info Return value.
		 * @return True on success.
		 */
		bool getParamInfo(IdType id, ParamInfo& info) const;

		/**
		 * @brief Gets the value of the specified parameter id.
		 * When the ID does not exist it returns false.
		 *
		 * @param id Parameter identifier.
		 * @param value Returned value.
		 * @return True on success.
		 */
		bool getParam(IdType id, Value& value) const;

		/**
		 * @brief Sets the value of the specified parameter id.
		 * When the ID does not exist it returns false.
		 *
		 * @param id Parameter identifier.
		 * @param value Set value.
		 * @param skip_event
		 * @return True on success.
		 */
		bool setParam(IdType id, const Value& value, bool skip_event);

		/**
		 * Sets and immediately gets the same value again clipped or not.
		 * @param id Parameter identifier.
		 * @param value Set, get value.
		 * @param skip_event
		 * @return True on success.
		 */
		bool paramSetGet(IdType id, Value& value, bool skip_event);

		/**
		 * @brief Pure virtual method to be overridden in a derived class.
		 * This method handles the interface parameters.
		 * When info in non-null the parameter info must be filled in.
		 * When the set or get value is non-null the value is set and/or retrieved.
		 *
		 * @param id
		 * @param info
		 * @param setval
		 * @param getval
		 * @return
		 */
		virtual bool handleParam(IdType id, ParamInfo* info, const Value* setval, Value* getval) = 0;

		/**
		 * @brief Same as setParam() but the ID is formed by getParamId()
		 */
		bool setParam(EDefaultParam param, unsigned gate, unsigned ch, const Value& value, bool skip_event);

		/**
		 * @brief Same as getParam() but the ID is formed by getParamId()
		 */
		bool getParam(EDefaultParam param, unsigned gate, unsigned ch, Value& value) const;

		/**
		 * @brief Sets the implementation in hold or Run mode.
		 * @param on When false the implementation goes in a non cpu time-consuming mode.
		 * @param clear When true all pending results are cleared and the index counter is reset.
		 * @return True on success.
		 */
		virtual bool setRunMode(bool on, bool clear) = 0;

		/**
		 * @brief Gets the current run mode of a derived class.
		 */
		[[nodiscard]] virtual bool getRunMode() const = 0;

		/**
		 * @brief Gets the ID of the parameter for the specified gate.
		 *
		 * @param param Type of default parameter.
		 * @param gate Gate number and when UINT_MAX  it is ignored.
		 * @param channel Channel number and when UINT_MAX  it is ignored.
		 * @return True on success.
		 */
		[[nodiscard]] virtual IdType
		getParamId(EDefaultParam param, unsigned gate = UINT_MAX, unsigned channel = UINT_MAX) const = 0;// NOLINT(google-default-arguments)

		/**
		 * @brief Gets all interface parameters ids.
		 * @param ids The return value.
		 * @return True on success.
		 */
		virtual bool enumParamIds(IdList& ids) = 0;

		/**
		 * @brief Sets a procedure hook for the interface implementation to be called
		 * when the value changes as a result of the implementation itself.
		 */
		void setParamHook(NotifyProc proc, void* data);

		/**
		 * @brief Sets a procedure hook for the interface implementation to be called	when there ias data result has data.
		 */
		void setResultHook(NotifyProc proc, void* data);

		/**
		 * @brief Returns the ID of the result for the specified gate.
		 *
		 * @param result Default result type.
		 * @param gate Gate number and when UINT_MAX tels the argument is ignored.
		 * @param channel Channel number and when UINT_MAX tels the argument is ignored.
		 * @return True on success.
		 */
		[[nodiscard]] virtual IdType
		getResultId(EDefaultResult result, unsigned gate = UINT_MAX, unsigned channel = UINT_MAX) const = 0;// NOLINT(google-default-arguments)

		/**
		 * @brief Gets interface results ids.
		 *
		 * @param ids List of identifiers
		 * @return True on success.
		 */
		virtual bool enumResultIds(IdList& ids) = 0;

		/**
		 * @brief Retrieve information about the passed id.
		 */
		bool getResultInfo(IdType id, ResultInfo& info);

		/**
		 * @brief Handles results.
		 */
		virtual bool handleResult(IdType id, ResultInfo* info, BufferInfo* bufInfo) = 0;

		/**
		 * @brief Gets the result buffer associated with the result ID passed in the result hook at the time of the call.
		 * @return True on success.
		 */
		bool getResultBuffer(IdType id, BufferInfo& bufInfo);

		/**
		 * @brief For testing initialization by the implementation.
		 * @return True on success.
		 */
		[[nodiscard]] bool isInitialized() const { return _initialized; }

		/**
		 * @brief Returns the path of the static configuration of the driver.
		 * @return True on success.
		 */
		[[nodiscard]] std::string getProfilePath() const;

		/**
		 * Adds controller specific property pages to the passed sheet.
		 * @param sheet
		 */
		virtual void addPropertyPages(PropertySheetDialog* sheet);

	protected:
		/**
		 * @brief Must be overridden by a derived class.
		 * Should check the hardware configuration.
		 *
		 * @param init True when initializing.
		 * @return True on success.
		 */
		virtual bool doInitialize(bool init) { return true; }

		/**
		 * @brief Calls the hooked function when it exists.
		 *
		 * @param id Parameter identifier.
		 */
		void callParamHook(IdType id);

		/**
		 * @brief Calls the hooked function when it exists.
		 *
		 * @param id Result identifier.
		 */
		void callResultHook(IdType id);

		/**
		 * @brief Sets the implementation type of the implementation used by the server side.
		 * Only allowed before and during initialization.
		 * By default, the type is ultrasonic.
		 * @param at
		 * @return True on success.
		 */
		bool setType(EAcquisitionType at);

	private:
		// Holds the pointer to the parameter change procedure.
		NotifyProc _paramNotifyProcedure;
		// Holds the pointer to the result change procedure.
		NotifyProc _resultNotifyProcedure;
		// Holds the data set with the hook.
		void* _paramNotifyData;
		// Holds the data set with the hook.
		void* _resultNotifyData;
		// Holds flag of implementation initialization.
		bool _initialized;
		// Holds the implementation type.
		EAcquisitionType _acquisitionType;

		// Declarations of static functions and data members to be able to create registered RSA implementations.
		SF_DECL_IFACE(RsaInterface, RsaInterface::Parameters, Interface)
};

}// namespace sf
