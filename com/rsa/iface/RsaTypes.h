#pragma once

#include <misc/gen/gen_utils.h>
#include <misc/gen/Value.h>
#include "global.h"

namespace sf
{

/**
 * Names used for method identification.
 */
#define US_METHOD_PEAK "Peak"

/**
 * Method for generating sample copy results in array's of Amplitude values.
 */
#define US_METHOD_COPY "Copy"

/**
 * @brief Type holding all types for the RsaInterface in a single namespaced structure.
 */
struct _RSA_CLASS RsaTypes
{
	/**
	 * @brief Type for gathering parameter ID's.
	 */
	typedef unsigned long long IdType;

	/**
	 * @brief Type for gathering parameter ID's.
	 */
	typedef TVector <IdType> IdList;

	/**
	 * @brief Function type for the function type which is called when a parameter changes as a result of changes in the interface itself.
	 */
	typedef void (* NotifyProc)(void* data, IdType id);

	/**
	 * @brief Structure to hold a single state consisting of a name and value.
	 */
	struct _RSA_CLASS ParamState
	{
		ParamState() = default;

		ParamState(const ParamState& ps) = default;

		ParamState(std::string name, Value val)
			:name(std::move(name))
			 , value(std::move(val)) {}

		bool operator==(const ParamState& ps) const
		{
			return name == ps.name;
		}

		/**
		 * @brief Holds the name of the state.
		 */
		std::string name;
		/**
		 * @brief Holds the value of the state.
		 */
		Value value;
	};

	/**
	 * @brief Vector to specify states.
	 */
	typedef TVector <ParamState> ParamStates;

	/**
	 * @brief Parameter flags for defining parameter behaviour.
	 */
	enum EParamFlag
	{
		/** Parameter cannot be changed at all times. */
		pfReadonly = 1 << 0,
		/** Parameter effects other parameters. */
		pfEffectsParameter = 1 << 1,
		/** Parameter effect result geometry. */
		pfEffectsResult = 1 << 2,
		/** Parameter effect result data generation. */
		pfEffectsData = 1 << 3,
		/** Parameter has effect on all channels. */
		pfChannelSingle = 1 << 4,
		/** Parameter is gate specific. */
		pfGate = 1 << 5,
		/** Parameter is method specific. */
		pfMethod = 1 << 6,
		/** Parameter is exported when system flags has been set. */
		pfExport = 1 << 7,
		/** Parameter is a system setting and is not exported. */
		pfSystem = 1 << 8,
		/** Parameter is always writeable in any mode. */
		pfWriteable = 1 << 9,
		/** Alias of another parameter in a different form. */
		pfAlias = 1 << 10,
		/** Parameter can be stored for restoring settings and must not used to restore. */
		pfArchive = 1 << 11,
		/** Parameter is only writable at run mode off and must not used to restore. */
		pfWriteAtOff = 1 << 12,
	};
	/**
	 * @brief Result flags for defining result behaviour.
	 */
	enum EResultFlag
	{
		rfGate = 1 << 0, // Result is generated by a gate.
		rfIndex = 1 << 1, // Result contains synced i/o input.
		rfAsync = 1 << 2, // Result data is not sync synchronous.
		rfAsyncIndex = 1 << 3, // Result keeps track of sync counter values at the time async data was generated.
		rfHugeData = 1 << 4, // Result that generates a huge amount of data.
		rfStored = 1 << 5, // Generates data to be stored.
	};

	/**
	 * @brief Structure to hold parameter information.
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

		ParamInfo& operator=(const ParamInfo& pi)
		{
			Copy(pi);
			return *this;
		}

		void Init()
		{
			Id = 0;
			Flags = 0;
			Gate = 0;
			Index = 0;
			Channel = 0;
			Name.clear();
			Unit.clear();
			Description.clear();
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

		/**
		 * Holds an unique id for the current situation.
		 */
		IdType Id{0};
		/**
		 * Holds the gate number this param belongs to.
		 * When gate is UINT_MAX the param is not a gate related param.
		 */
		unsigned Gate{0};
		/**
		 * Param index in the hierarchy of the gate or non gate params.
		 */
		unsigned Index{0};
		/**
		 * Holds the channel number of this parameter.
		 * When channel is UINT_MAX the param is not a gate related param.
		 */
		unsigned Channel{0};
		/**
		 * Name of the parameter.
		 */
		std::string Name;
		/**
		 * Unit of the parameter.
		 */
		std::string Unit;
		/**
		 * Usage off the parameter of the parameter.
		 */
		std::string Description;
		/**
		 * Round or step value of the parameter.
		 */
		Value Round;
		/**
		 * Default or start value.
		 */
		Value Default;
		/**
		 * Minimum allowed value.
		 */
		Value Minimum;
		/**
		 * Maximum allowed value.
		 */
		Value Maximum;
		/**
		 * Number of specified states for this.
		 */
		ParamStates States;
		/**
		 * Parameter is readonly.
		 */
		int Flags{0};
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
		/**
		 * Holds the gate number this gate belongs to.
		 * When gate is UINT_MAX the param is not a gate related param.
		 */
		unsigned Gate{0};
		/**
		 * Param index in the hierarchy of the gate or non gate params.
		 */
		unsigned Index{0};
		/**
		 * Holds the channel number of this parameter.
		 * When channel is UINT_MAX the param is not a gate related param.
		 */
		unsigned Channel{0};
		/**
		 * Name of the parameter.
		 */
		std::string Name;
		/**
		 * Usage off the parameter of the parameter.
		 */
		std::string Description;
		/**
		 * Amount of Bits used to store the actual value.
		 */
		unsigned Bits{0};
		/**
		 * Amount of bytes 1, 2 or 4 (later 8) used for storing a single value.
		 */
		unsigned WordSize{0};
		/**
		 * Amount of values in an array for an arrayed value.
		 */
		unsigned ArraySize{0};
		/**
		 * Level offset of the generated data.
		 */
		int Offset{0};
	};

	/**
	 * Structure to hold data update information.
	 */
	struct BufferInfo
	{
		/**
		 * Initializing constructor.
		 */
		BufferInfo() = default;

		/**
		 * Clears all members.
		 */
		void Clear()
		{
			Buffer = nullptr;
			Size = 0;
			BlockBufSize = 0;
			Remain = 0;
			Counter = 0;
		}

		/**
		 * Holds the buffer pointer.
		 */
		void* Buffer{nullptr};
		/**
		 * Holds the size of the buffer in data blocks.
		 */
		size_t Size{0};
		/**
		 * Holds the size of a single block in byte for debugging purposes.
		 */
		size_t BlockBufSize{0};
		/**
		 * Holds the remaining size to store in data blocks.
		 */
		size_t Remain{0};
		/**
		 * Possible sync counter value.
		 */
		size_t Counter{0};
	};

	/**
	 * Type of acquisition implementation.
	 */
	enum EAcquisitionType
	{
		/** Ultra sonic type of implementation (Chan. Gates).*/
		atUltrasonic,
		/** Eddy current type of implementation.*/
		atEddyCurrent
	};

	/**
	 * Acquisition US Parameters.
	 * List of id's which must always be present at all times for each ultrasonic acquisition implementation.
	 */
	enum EDefaultParam
	{
		/** Used to indicate no parameter. */
		apNone = 0x0000,
		// Interface parameters from here.
		// 0x0000 is used for the interface implementation parameter.
		// 0x0001 is used for the RunMode parameter.
		/** Amount of available channels. */
		apChannels = 0x0002,
		/** Holds the error number of the implementation. */
		apError = 0x0003,
		// Restart is imminent when an error occurs less than zero.
		// Acquisition cannot continue when errors occur larger than zero.
		/** Error message. */
		apErrorMessage = 0x0004,
		/** Amplitude unit conversion factor to get the real value from a result. */
		apAmplitudeUnit = 0x0005,
		/** First user number from here. */
		apUserFirst = 0x000F,
		/*
		 * Channel parameters from here.
		 */
		/** Mask to detect channeled parameter. */
		apChannel_Mask = 0x4000,
		/** Repetition rate of the interface. */
		apChannel_RepRate = 0x4001,
		/** Sync mode internal = 0 or external = 1. */
		apChannel_SyncMode = 0x4002,
		/** Amount of gates for this channel. */
		apChannel_Gates = 0x4003,
		/** Inputs of this implementation. */
		apChannel_Inputs = 0x4004,
		/** Units in which results are generated in seconds. */
		apChannel_TimeUnits = 0x4005,
		/** Delay of the ultrasonic gate for the special copy result. */
		apChannel_CopyDelay = 0x4006,
		/** Range of the ultrasonic gate for the special copy result. */
		apChannel_CopyRange = 0x4007,
		/** Sample rate of digitizer in Hz. */
		apChannel_SampleRate = 0x4008,
		/** Enables the generation data for the copy result. 0 = disabled, 1 = enabled. */
		apChannel_CopyEnable = 0x4009,
		/**
		 * Generates a POP index data entry manually. 0 = Disabled, 1 = Ready, 2 = Trigger.
		 * Value is reset to Enabled when trigger passed.
		 */
		apChannel_PopManual = 0x400A,
		/** Enables Alternate mode for bidirectional scanning in combination with pop manual.
		 * Reversing every other scanline.
		 */
		apChannel_BiDirMode = 0x400B,
		/** First user channel number from here. */
		apChannel_UserFirst = 0x401F,
		/*
		 * Gate parameters from here.
		 */
		/** Mask to detect gated parameter. */
		apGate_Mask = 0x8000,
		/** Name of this gate. */
		apGate_Name = 0x8001,
		/** Delay of the ultrasonic gate in seconds. */
		apGate_Delay = 0x8002,
		/** Range of the ultrasonic gate in seconds. */
		apGate_Range = 0x8003,
		/** Other gate the gat eis slaved to. */
		apGate_SlaveTo = 0x8004,
		/** Enables the data generation of the gate. */
		apGate_Enable = 0x8005,
		/** Method which is applied on the gate. */
		apGate_Method = 0x8006,
		/** Enables the data generation of the gate. */
		apGate_Threshold = 0x8007,
		/** Averaged gate amplitude. */
		apGate_Amplitude = 0x8008,
		/** Averaged gate time-of-flight. */
		apGate_TimeOfFlight = 0x8009,
		/** Free gate parameters numbers from here. */
		apGate_UserFirst = 0x801F,
	};

	/**
	 * Acquisition Ultrasonic Results.
	 * List of id's which must always be present at all times for each ultrasonic acquisition implementation.
	 */
	enum EDefaultResult
	{
		/** Used to indicate no result. */
		arNone = 0x0000,
		/** First user number from here. */
		arUserFirst = 0x0001,
		/*
		 * Channel results from here.
		 */
		/** Mask to detect channeled result. */
		arChannel_Mask = 0x4000,
		/** Index result for position orientation pulse. Holds the sync counter value at that stage. */
		arChannel_PopIndex = 0x4001,
		/** Non synchronised result holding a copy of the digitisers data. */
		arChannel_CopyData = 0x4002,
		/** Sync counter value at the time from the copy. */
		arChannel_CopyIndex = 0x4003,
		/** Free number from here. */
		arChannel_UserFirst = 0x401F,
		/**
		 * Gate results from here.
		 */
		/** Mask to detect gated result. */
		arGate_Mask = 0x8000,
		/** Free gate results numbers from here. */
		arGate_UserFirst = 0x801F
	};

	/**
	 * @brief Enumerate for the pop manual function.
	 */
	enum EPopManual
	{
		/** Disables manual pop triggering and uses external pop generation. */
		pmDisabled = 0,
		/** POP manual is enabled or ready from last trigger. */
		pmReady,
		/** State to trigger a manual pop. */
		pmTrigger
	};
};

}
