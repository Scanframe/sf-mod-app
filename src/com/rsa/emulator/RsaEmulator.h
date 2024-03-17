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
		bool setRunMode(bool runMode, bool clear) override;
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
		// Holds the time the run mode was started.
		double SyncTimeStart{0.0};
		// Holds the flag for determining the fixed gate state or the dynamic gates.
		bool FlagFixed{false};
		// Holds the digitiser´s amplitude unit.
		double AmplitudeUnit{0.01};
		// Holds the amount of channels supported by this interface.
		unsigned ChannelCount{0};
		//
		struct TGateInfo
		{
			// Holds the name of the gate.
			std::string Name;
			// Holds the id of the method selected for this gate.
			int MethodId{0};
			// Holds the delay of this gate in time units.
			long Delay{200};
			// Holds the range of this gate in time units.
			long Range{100};
			// Holds the gate to which this gate is slaved.
			long SlavedTo{-1};
			// Holds the data generation enable status of this gate.
			bool DataEnable{true};
			// Hold the threshold level.
			unsigned Threshold{60};
			// Actual value in percentages.
			double ThresholdValue{70.0};
			// Copy method buffer.
			DynamicBuffer CopyBuf;
			// Holds the last peak value.
			unsigned PeakAmp{0};
			// Holds the last peak time of flight value.
			unsigned PeakTof{0};
			// Flag telling if a peak was found.
			bool PeakFound{false};
			// Gate polarity.
			EPolarity Polarity{plFull};
			// Calculate the threshold depending on the polarity.
			void CalculateThreshold();
		};
		//
		struct TChannelInfo
		{
			TChannelInfo() :GateInfo(new TGateInfo[8]){}
			~TChannelInfo() {delete[] GateInfo;}

			// Holds the time offset when the RepRate was changed on the fly.
			double SyncTimeOffset{0.0};

			// Params
			long CopyDelay{0};
			long CopyRange{0};
			// Interface gate position for emulation.
			long IfPos{0};
			//
			unsigned GateCount{0};
			double RepRate{100};
			double TimeUnits{1e6};
			double Gain{30};
			long SyncMode{0};
			// Divider used to generate the position orientation pulse result.
			long PopDivider{100};
			int PopManual{0};
			//
			TGateInfo* GateInfo;
			// Current sync counter value.
			uint32_t SyncCounter{0};
			// Buffer used for copy info transport.
			DynamicBuffer CopyBuf;
			// Sync Index table for the async copy result.
			uint32_t CopySyncIndex{0};
			// Position orientation pulse sync counter value.
			uint32_t PopIndex{0};
			//
			bool CopyEnabled{false};
			// TCG gate info.
			int TcgSlavedTo{-1};
			long TcgRange{0};
			long TcgDelay{0};
			bool TcgEnable{false};
			// Bidirectional scanning.
			bool BiDirMode{false};
			//
			int AscanRectify{0};
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
