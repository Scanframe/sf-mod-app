#pragma once


#include <misc/gen/Sustain.h>
#include <misc/gen/TDynamicBuffer.h>
#include <misc/gen/ElapseTimer.h>
#include <rsa/iface/RsaInterface.h>

namespace sf
{
/**
 * @brief Implements a repetitive signal acquisition emulator.
 */
class AcquisitionEmulator : public RsaInterface
{
	public:
		// Constructor.
		explicit AcquisitionEmulator(const Parameters&);
		// Virtual destructor.
		~AcquisitionEmulator() override;
		// Overridden from base class.
		bool setRunMode(bool on, bool clear) override;
		// Overridden from base class.
		[[nodiscard]] bool getRunMode() const override;
		// Overloaded from base class.
		[[nodiscard]] IdType getParamId(EDefaultParam param, unsigned gate, unsigned channel) const override;
		// Overridden from base class.
		bool handleParam(IdType id, ParamInfo* info, const Value* setval, Value* getval) override;
		// Overridden from base class.
		[[nodiscard]] IdType getResultId(EDefaultResult result, unsigned gate, unsigned channel) const override;
		// Overridden from base class.
		bool handleResult(IdType id, ResultInfo* info, BufferInfo* bufInfo) override;
		// Overridden from base class.
		bool enumParamIds(IdList& ids) override;
		// Overridden from base class.
		bool enumResultIds(IdList& ids) override;

		/**
		 * Polarity enumeration.
		 */
		enum EPolarity :int
		{
			/** Detect a negative peak. */
			plNegative = -1,
			/** Detect positive and negative peak. */
			plFull = 0,
			/** Detect a positive peak. */
			plPositive = 1,
		};

	private:
		// Overloaded from base class.
		bool doInitialize(bool init) override;
		// Sustain function.
		bool sustain(const timespec& t);
		// Hook for the sustain interface.
		TSustain<AcquisitionEmulator> SustainEntry;
		// Holds the run mode flag.
		bool RunMode{false};
		// Holds the flag for determining the fixed gate state or the
		// dynamic gates.
		bool FlagFixed{false};
		// Holds the amount of channels supported by this interface.
		unsigned ChannelCount{0};
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
				Polarity = plFull;
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
			// Divider used to generate the position orientation pulse result.
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
			double Sweep[3]{0, 0, 0};
		};
		//
		TChannelInfo* FChannelInfo{nullptr};
		//
		int ErrorValue{0};
		// Holds status of single shot generation
		ElapseTimer SingleShotTimer;
		//
		[[nodiscard]] long TimeUnits(const TChannelInfo& ci, const Value& value) const;
		/**
		 * TCG Table.
		 */
		struct
		{
			double Time{0};
			double Gain{0};
		} Tcg[20];
};

}
