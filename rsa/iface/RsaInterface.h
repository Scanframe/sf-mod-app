#pragma once

#include <utility>

#include "misc/gen/gen_utils.h"
#include "misc/gen/Value.h"
#include "misc/gen/TVector.h"
#include "misc/gen/TClassRegistration.h"

#include "global.h"

namespace sf
{

/**
 * Structure to hold a single state consisting of a name and value.
 */
struct _RSA_CLASS ParamState
{
	ParamState() = default;

	ParamState(const ParamState& ps) = default;

	ParamState(std::string name, const Value& val)
	:name(std::move(name))
	,value(val)
	{}

	bool operator==(const ParamState& ps) const
	{
		return name == ps.name;
	}

	/**
	 * Holds the name of the state.
	 */
	std::string name;
	/**
	 * Holds the value of the state.
	 */
	Value value;
};

/**
 * Vector to specify states.
 */
typedef TVector<ParamState> ParamStates;

/**
 * Parameter flags for defining parameter behaviour.
 */
enum EParamFlag
{
	pfREADONLY = 1 << 0, // Parameter cannot be changed at all times.
	pfEFFECTPARAM = 1 << 1, // Parameter effects other parameters.
	pfEFFECTRESULT = 1 << 2, // Parameter effect result geometry.
	pfEFFECTDATA = 1 << 3, // Parameter effect result data generation.
	pfCHANNELSINGLE = 1 << 4, // Parameter has effect on all channels.
	pfGATE = 1 << 5, // Parameter is gate specific.
	pfMETHOD = 1 << 6, // Parameter is method specific.
	pfEXPORT = 1 << 7, // Parameter is exported when system flags has been set.
	pfSYSTEM = 1 << 8, // Parameter is a system setting and is not exported.
	pfWRITEABLE = 1 << 9, // Parameter is always writeable in any mode.
	pfALIAS = 1 << 10, // Alias of another parameter in a different form.
	// Parameters like these must not used to restore.
	pfARCHIVE = 1 << 11, // Parameter can be stored for restoring settings.
	pfWRITEATOFF = 1 << 12, // Parameter is only writeble at run mode off.
};
/**
 * Result flags for defining result behaviour.
 */
enum EResultFlag
{
	rfGATE = 1 << 0, // Result is generated by a gate.
	rfINDEX = 1 << 1, // Result contains synced i/o input.
	rfASYNC = 1 << 2, // Result data is not sync synchrone.
	rfASYNCINDEX = 1 << 3, // Result keeps track of sync counter values at the time async data was generated.
	rfHUGE = 1 << 4, // Result that generates a huge amount of data.
	rfGENDATA = 1 << 5, // Generates data to be stored.
};

/**
 * Structure to hold parameter information.
 */
struct _RSA_CLASS ParamInfo
{
	ParamInfo()
	{
		Init();
	}

	ParamInfo(const ParamInfo& pi)
	{
		Copy(pi);
	}

	ParamInfo& operator=(const ParamInfo& pi) {return Copy(pi);}

	void Init()
	{
		Id = 0;
		Flags = 0;
		Gate = 0;
		Index = 0;
		Channel = 0;
		Name = "";
		Unit = Name;
		Description = Name;
		Round.setType(Value::vitUndefined);
		Default.set(Round);
		Minimum.set(Round);
		Maximum.set(Round);
		States.clear();
	}

	ParamInfo& Copy(const ParamInfo& pi)
	{
		Id = pi.Id;
		Name = pi.Name;
		Description = pi.Description;
		Gate = pi.Gate;
		Channel = pi.Channel;
		Flags = pi.Flags;
		Index = pi.Index;
		Round = pi.Round;
		Default = pi.Default;
		Maximum = pi.Maximum;
		Minimum = pi.Minimum;
		States = pi.States;
		return *this;
	}

	// Holds an unique id for the current situation.
	int Id;
	// Holds the gate number this param belongs to.
	// When gate is UINT_MAX the param is not a gate related param.
	unsigned Gate;
	// Param index in the hierarchy of the gate or non gate params.
	unsigned Index;
	// Holds the channel number of this parameter.
	// When channel is UINT_MAX the param is not a gate related param.
	unsigned Channel;
	// Name of the parameter.
	std::string Name;
	// Unit of the parameter.
	std::string Unit;
	// Usage off the parameter of the parameter.
	std::string Description;
	// Round or step value of the parameter.
	Value Round;
	// Default or start value.
	Value Default;
	// Minimum allowed value.
	Value Minimum;
	// Maximum allowed value.
	Value Maximum;
	// Number of specified states for this.
	ParamStates States;
	// Parameter is readonly.
	int Flags;
};

/**
 * Structure to hold parameter information
 */
struct _RSA_CLASS ResultInfo
{
	ResultInfo() = default;

	void reset()
	{
		Id = 0;
		Flags = 0;
		Gate = 0;
		Index = 0;
		Channel = 0;
		Name.clear();
		Description.clear();
		Bits = 0;
		WordSize = 0;
		ArraySize = 0;
		Offset = 0;
	}

	ResultInfo(const ResultInfo& pi)
	{
		Copy(pi);
	}

	ResultInfo& operator=(const ResultInfo& pi)
	{
		Copy(pi);
		return *this;
	}

	ResultInfo& Copy(const ResultInfo& pi)
	{
		Id = pi.Id;
		Flags = pi.Flags;
		Name = pi.Name;
		Gate = pi.Gate;
		Index = pi.Index;
		Channel = pi.Channel;
		WordSize = pi.WordSize;
		ArraySize = pi.ArraySize;
		Bits = pi.Bits;
		Offset = pi.Offset;
		return *this;
	}

	/**
	 * Holds an unique id for the current situation.
	 */
	int Id{0};
	/**
	 * Flags
	 */
	int Flags{0};
	// Holds the gate number this gate belongs to.
	// When gate is UINT_MAX the param is not a gate related param.
	unsigned Gate{0};
	// Param index in the hierarchy of the gate or non gate params.
	unsigned Index{0};
	// Hold sthe channel number of this parameter.
	// When channel is UINT_MAX the param is not a gate related param.
	unsigned Channel{0};
	// Name of the parameter.
	std::string Name;
	// Usage off the parameter of the parameter.
	std::string Description;
	// Amount of Bits used to store the actual value.
	unsigned Bits{0};
	// Amount of bytes 1, 2 or 4 (later 8) used for storing a single value.
	unsigned WordSize{0};
	// Amount of values in an array for an arrayed value.
	unsigned ArraySize{0};
	// Level offset of the generated data.
	int Offset{0};
};

/**
 * Structure to hold data update information.
 */
struct BufferInfo
{
	// Initializing constructor.
	BufferInfo() = default;

	// Clears all members.
	void Clear()
	{
		Buffer = nullptr;
		Size = 0;
		BlockBufSize = 0;
		Remain = 0;
		Counter = 0;
	}

	// Holds the buffer pointer.
	void* Buffer{nullptr};
	// Holds the size of the buffer in data blocks.
	unsigned Size{0};
	// Holds the size of a single block in byte for debugging purposes.
	unsigned BlockBufSize{0};
	// Holds the remaining size to store in data blocks.
	unsigned Remain{0};
	// Possible sync counter value.
	unsigned Counter{0};
};

/**
 * Type of acquisition implementation.
 */
enum EAcquisitionType
{
	/** Ultra sonic type of implementation (Chan. Gates).*/
	atULTRASONIC,
	/** Eddy current type of implementation.*/
	atEDDYCURRENT
};
/**
 * Acquisition US Parameters.
 * List of id's which must allways be present at all times for each ultrasonic acquisition implementation.
 */
enum EDefaultParam
{
	apNONE = 0x0000,  // Used to indicate no parameter.
	// Interface parameters from here.
	// 0x0000 is used for the interface implementation parameter.
	// 0x0001 is used for the RUNMODE parameter.
	apCHANNELS = 0x0002,  // Amount of available channels.
	apERROR = 0x0003,  // Holds the error number of the implementation.
	// Restart is imminent when an error occurs less then zero.
	// Acquisition cannot continue when errors occur larger then zero.
	apERROR_MESSAGE = 0x0004,  // Error message.
	apAMPUNIT = 0x0005,  // Amplitude unit conversion factor to get the real
	// value from a result.
	apLAST = 0x000F,  // Free number from here.

	// Channel parameters from here.
	apCH_MASK = 0x4000,  // Mask to detect channeled parameter.
	apCH_REPRATE = 0x4001,  // Repetition rate of the interface.
	apCH_SYNCMODE = 0x4002,  // Sync mode internal = 0 or external = 1.
	apCH_GATES = 0x4003,  // Amount of gates for this channel.
	apCH_INPUTS = 0x4004,  // Inputs of this implementation.
	apCH_TIMEUNITS = 0x4005,  // Units in which results are generated in seconds.
	apCH_COPYDELAY = 0x4006,  // Delay of the ultrasonic gate for the
	// special copy result.
	apCH_COPYRANGE = 0x4007,  // Range of the ultrasonic gate for the
	// special copy result.
	apCH_SAMPLERATE = 0x4008,  // Sample rate of digitizer in Hz.
	apCH_COPYENABLE = 0x4009,  // Enables the generation data for the copy result.
	// 0 = disabled, 1 = enabled.
	apCH_POPMANUAL = 0x400A,  // Generates a POP index data entry manually.
	// 0 = Disabled, 1 = Ready, 2 = Trigger.
	// Value is reset to Enabled when trigger passed.
	apCH_BIDIRMODE = 0x400B,   // Enables Alternate mode for bidirectional
	// scanning in combination with popmanual.
	// Reversing every other scanline.
	apCH_LAST = 0x401F,  // Free number from here.

	// Gate parameters from here.
	apGATE_MASK = 0x8000,  // Mask to detect gated parameter.
	apGATE_NAME = 0x8001,  // Name of this gate.
	apGATE_DELAY = 0x8002,  // Delay of the ultrasonic gate in seconds.
	apGATE_RANGE = 0x8003,  // Range of the ultrasonic gate in seconds.
	apGATE_SLAVETO = 0x8004,  // Other gate the gat eis slaved to.
	apGATE_ENABLE = 0x8005,  // Enables the data generation of the gate.
	apGATE_METHOD = 0x8006,  // Method which is applied on the gate.
	apGATE_THRESHOLD = 0x8007,  // Enables the data generation of the gate.
	apGATE_AMP = 0x8008,  // Averaged gate amplitude.
	apGATE_TOF = 0x8009,  // Averaged gate time-of-flight.
	apGATE_LAST = 0x801F,  // Free gate parameters numbers from here.
};

/**
 * Acquisition US Result.
 * List of id's which must always be present at all times for each ultrasonic acquisition implementation.
 */
enum EDefaultResult
{
	arNONE = 0x0000,  // Used to indicate no result.
	//
	arLAST = 0x0001,  // Free number from here.

	// Channel results from here.
	arCH_MASK = 0x4000,  // Mask to detect channeled result.
	arCH_POPINDEX = 0x4001,  // Index result for position orientation puls.
	// Holds the the sync counter value at that stage.
	arCH_COPYDATA = 0x4002,  // Non synchronised result holding a copy of
	// the digitisers data.
	arCH_COPYINDEX = 0x4003,  // Sync counter value at the time from the copy.
	arCH_LAST = 0x401F,  // Free number from here.

	// Gate parameters from here.
	arGATE_MASK = 0x8000,  // Mask to detect gated result.
	arGATE_LAST = 0x801F   // Free gate results numbers from here.
};

/**
 * Enumerate for the pop manual function.
 */
enum EPopManual
{
	pmDISABLED = 0, // Disables manual pop triggering and uses external pop generation.
	pmREADY,        // POP manual is enabled or ready from last trigger.
	pmTRIGGER       // State to trigger a manual pop.
};
/**
 * Names used for method identification.
 */
#define US_METHOD_PEAK "Peak"
/**
 * Method for generating sample copy results in array's of Amplitude values.
 */
#define US_METHOD_COPY "Copy"

/**
 * Type for gathering parameter ID's.
 */
typedef TVector<int64_t> IdList;

/**
 * Function type for the function type which is called when a parameter
 * changes as a result of changes in the interface itself.
 */
typedef void (* TNotifyProc)(void* data, int id);

/**
 * Base class for the repetitive signal acquisition interface.
 */
class _RSA_CLASS RsaInterface
{
	public:

		struct Parameters
		{
			explicit Parameters(int mode) :_mode(mode) {}

			int _mode;
		};

		// Constructor for passing general structure for derived classes.
		explicit RsaInterface(const Parameters&);

		// Virtual destructor for derived classes.
		virtual ~RsaInterface();

		// Initializes the implementation for operation. Calls DoInitialize.
		bool Initialize();

		// Uninitializes the implementation. Calls DoInitialize.
		bool Uninitialize();

		// Returns the type of acquisition.
		// By default the type is ultrasonic.
		[[nodiscard]] EAcquisitionType GetType() const;

		// Writes the non exported parameters to profile.
		bool ReadWriteSettings(bool rd);

		// Returns the value of the error.
		int GetError() const;

		// Returns the amount of channels for this implementation.
		unsigned GetChannelCount();

		// Returns the amount of gates for this implementation.
		unsigned GetGateCount(unsigned channel);

		// Returns the name of the passed gate.
		std::string GetGateName(int gate, int channel = 0);

		// Sets the pop manual state for a channel.
		bool SetPopManual(int channel, EPopManual pm);

		// Gets the pop-manual state on a channel.
		EPopManual GetPopManual(int channel) const;

		// Retrieve information about the passed param id.
		bool GetParamInfo(int id, ParamInfo& info) const;

		// Returns the value of the specified parameter id.
		// When the ID does not exists it returns false.
		bool GetParam(int id, Value& value) const;

		bool SetParam(int id, const Value& value, bool skip_event);

		// Sets and immediately gets the same value again clipped or not.
		bool SetGetParam(int id, Value& value, bool skip_event);

		// This function must be overloaded to handle the interface parameters.
		// When info in non-null the parameter info must be filled in.
		// When the set or get value is non-null the value is set and/or retrieved.
		virtual bool HandleParam
			(
				int id,
				ParamInfo* info,
				const Value* setval,
				Value* getval
			) = 0;

		//
		bool SetParam(EDefaultParam param, unsigned gate, unsigned ch, const Value& value, bool skip_event);

		//
		bool GetParam(EDefaultParam param, unsigned gate, unsigned ch, Value& value) const;

		// Sets the implementation in hold or Run mode.
		// When clear is set all pending results are cleared.
		// and the index counter is reset.
		// When on is false the implementation goes in a non cpu time
		// consuming mode.
		virtual bool SetRunMode(bool on, bool clear) = 0;

		virtual bool GetRunMode() const = 0;

		// Returns the ID of the parameter for the specified gate.
		// UINT_MAX tels the implementation to ignore the parameter.
		virtual int GetParamId
			(
				EDefaultParam param,
				unsigned gate = UINT_MAX,
				unsigned channel = UINT_MAX
			) const = 0;

		// Enumerate interface parameters ids.
		virtual bool EnumParamIds(IdList& ids) = 0;

		// Sets a procedure hook for the interface implemetation to be called
		// when the value changes as a result of the implemntation itself.
		void SetParamHook(TNotifyProc proc, void* data);

		// Sets a procedure hook for the interface implemetation to be called
		// when there ias data result has data.
		void SetResultHook(TNotifyProc proc, void* data);

		// Returns the ID of the result for the specified gate.
		// UINT_MAX tels the implementation to ignore the result.
		virtual int GetResultId
			(
				EDefaultResult result,
				unsigned gate = UINT_MAX,
				unsigned channel = UINT_MAX
			) const = 0;

		// Enumerate interface results ids.
		virtual bool EnumResultIds(IdList& ids) = 0;

		// Retrieve inormation about the passed id.
		bool GetResultInfo(int id, ResultInfo& info);

		// Retrieve inormation about the passed id.
		virtual bool HandleResult(int id, ResultInfo* info, BufferInfo* bufinfo) = 0;

		// Gets the result buffer associated with the result ID
		// passed in the result hook at the time of the call.
		// Returns true on succes.
		bool GetResultBuffer(int id, BufferInfo& bufinfo);

		// For testing initialization by the implementation.
		bool IsInitialized() const {return FInitialized;}

		// Returns the path of the static configuration of the driver.
		std::string GetProfilePath() const;

		// Adds controller specific property pages to the passed sheet.
		virtual void AddPropertyPages(QWidget* sheet);

	protected:
		// Must be overloaded by a derived class.
		// Should check the hardware configuration.
		virtual bool DoInitialize(bool init) {return true;}

		// Calls the hooked function if it exists.
		// Passing the set data pointer and the ID of the effected parameter.
		void CallParamHook(int id);

		// Calls the hooked function if it exists.
		// Passing the set data pointer and the ID of the effected result.
		void CallResultHook(int id);

		// Sets the implementation type of the implementation used by the server side.
		// Only allowed before and during initialization.
		// By default the type is ultrasonic.
		bool SetType(EAcquisitionType at);

	private:
		// Holds the pointer to the parameter change procedure.
		TNotifyProc ParamNotifyProc;
		// Holds the pointer to the result change procedure.
		TNotifyProc ResultNotifyProc;
		// Holds the data set with the hook.
		void* ParamNotifyData;
		// Holds the data set with the hook.
		void* ResultNotifyData;
		// Holds flag of implementation initialization.
		bool FInitialized;
		// Holds the implementation type.
		EAcquisitionType FAcquisitionType;

		// Declarations of static functions and data members to be able to create registered RSA implementations.
	SF_DECL_IFACE(RsaInterface, RsaInterface::Parameters, Interface)

};

inline
EAcquisitionType RsaInterface::GetType() const
{
	return FAcquisitionType;
}

inline
void RsaInterface::SetParamHook(TNotifyProc proc, void* data)
{
	ParamNotifyProc = proc;
	ParamNotifyData = data;
}

inline
void RsaInterface::SetResultHook(TNotifyProc proc, void* data)
{
	ResultNotifyProc = proc;
	ResultNotifyData = data;
}

}
