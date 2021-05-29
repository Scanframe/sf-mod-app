#pragma once


#include "../iface/RsaInterface.h"
#include <misc/gen/Sustain.h>
#include <misc/gen/TDynamicBuffer.h>
#include <misc/gen/ElapseTimer.h>

namespace sf
{
/**
 * @brief Implements a repetitive signal acquisition emulator.
 *
 *  TODO: Needs lots of renaming of methods and members and used types.
 */
class AcquisitionEmulator : public RsaInterface
{
	public:
		// Constructor.
		explicit AcquisitionEmulator(const Parameters&);
		// Channel constructor.
		explicit AcquisitionEmulator(AcquisitionEmulator* root);
		// Virtual destructor.
		~AcquisitionEmulator() override;

		// Sets the implementation in hold or Run mode.
		// When clear is set all pending results are cleared.
		// and the index counter is reset.
		// When on is false the implementation goes in a non cpu time
		// consuming mode.
		bool SetRunMode(bool on, bool clear) override;
		// Overloaded from base class.
		virtual bool GetRunMode() const;
		// Returns the ID of the parameter for the specified gate.
		// UINT_MAX tels the implementation to ignore this parameter.
		virtual int GetParamId
		(
			EDefaultParam param,
			unsigned gate = UINT_MAX,
			unsigned channel = UINT_MAX
		) const;
		// Overloaded from base class.
		virtual bool HandleParam
		(
			int id,
			ParamInfo* info,
			const Value* setval,
			Value* getval
		);
		// Overloaded from base class.
		virtual int GetResultId
		(
			EDefaultResult result,
			unsigned gate = UINT_MAX,
			unsigned channel = UINT_MAX
		) const;
		// Overloaded from base class.
		virtual bool HandleResult(int id, ResultInfo* info, BufferInfo* bufinfo);
		// Overloaded from base class.
		virtual bool EnumParamIds(IdList& ids);
		// Overloaded from base class.
		virtual bool EnumResultIds(IdList& ids);
		// Overloaded from base class.
		bool GetResult(int id, void* buffer, long bufsize, long &written);
		// Polarity enumeration.
		enum EPolarity
		{
			plNEGATIVE = -1, // Detect a negative peak
			plFULL = 0,      // Detect positive and negative peak
			plPOSITIVE = 1,  // Detect a positive peak
			plLASTENTRY = 2
		};

	private:
		// Overloaded from base class.
		virtual bool DoInitialize(bool init);
		// Hook for the sustain interface.
		TSustain<AcquisitionEmulator> SustainEntry;
		// Sustain function.
		bool Sustain(clock_t);
		// Holds the run mode flag.
		bool FRunMode{false};
		// Holds the flag for determining the fixed gate state or the
		// dynamic gates.
		bool FlagFixed{false};
		// Holds the amount of channels supported by this interface.
		unsigned FChannelCount{0};
		//
		struct TGateInfo
		{
			TGateInfo()
			{
				Name = "";
				MethodId = 0;
				Delay = 200;
				Range = 100;
				SlavedTo = -1;
				DataEnable = true;
				Threshold = 60;
				ThresholdValue = 70.0;
				PeakAmp = 0;
				PeakTof = 0;
				PeakFound = false;
				Polarity = plFULL;
			}
			// Holds the name of the gate.
			std::string Name;
			// Holds the id of the method selected for this gate.
			int MethodId;
			// Holds the delay of this gate in time units.
			long Delay;
			// Holds the range of this gate in time units.
			long Range;
			// Holds the gate to which this gate is slaved.
			long SlavedTo;
			// Holds the data generation enable status of this gate.
			bool DataEnable;
			// Hold the threshold level.
			unsigned Threshold;
			// Actual value in percentages.
			double ThresholdValue;
			// Copy method buffer.
			DynamicBuffer CopyBuf;
			// Holds the last peak value.
			unsigned PeakAmp;
			// Holds the last peak time of flight value.
			unsigned PeakTof;
			// Flag telling if a peak was found.
			bool PeakFound;
			// Gate polarity.
			EPolarity Polarity;
			// Calculate the threshold depending on the polarity.
			void CalculateThreshold();
		};
		//
		struct TChannelInfo
		{
			TChannelInfo() :GateInfo(new TGateInfo[8])
			{
				CopyDelay = 0;
				CopyRange = 0;
				GateCount = 0;
				RepRate = 100;
				TimeUnits = 1E6;
				Gain = 30;
				SyncMode = 0;
				PopDivider = 100;
				SyncCounter = 0;
				CopySyncIndex = 0;
				PopIndex = 0;
				CopyEnabled = false;
				TcgSlavedTo = -1;
				TcgRange = 0;
				TcgDelay = 0;
				TcgEnable = false;
				AscanRectify = 0;
				PopManual = 0;
				BiDirMode = false;
				IfPos = 0;
				// Offset for the indications.
				memset(&Sweep, 0, sizeof(Sweep));
			}
			~TChannelInfo() {delete[] GateInfo;}
			// Params
			long CopyDelay;
			long CopyRange;
			// Interface gate position for emulation.
			long IfPos;
			//
			unsigned GateCount;
			double RepRate;
			double TimeUnits;
			double Gain;
			long SyncMode;
			// Divider used to generate the position orientation puls result.
			long PopDivider;
			int PopManual;
			//
			TGateInfo* GateInfo;
			// Current sync counter value.
			uint32_t SyncCounter;
			// Buffer used for copy info transport.
			DynamicBuffer CopyBuf;
			// Sync Index table for the async copy result.
			uint32_t CopySyncIndex;
			// Position orientation pulse sync counter value.
			uint32_t PopIndex;
			//
			bool CopyEnabled;
			// TCG gate info.
			int TcgSlavedTo;
			long TcgRange;
			long TcgDelay;
			bool TcgEnable;
			// Bidirectional scanning.
			bool BiDirMode;
			//
			int AscanRectify;
			// Offset for the indications.
			double Sweep[3];
		};
		//
		TChannelInfo* FChannelInfo;
		//
		int FErrorValue{0};
		// Holds status of single shot generation
		ElapseTimer FSingleShotTimer;
		//
		[[nodiscard]] long TimeUnits(const TChannelInfo& ci, const Value& value) const;
		// TCG Table for all channels.
		struct
		{
			double Time{0};
			double Gain{0};
		} Tcg[20];
};

}
