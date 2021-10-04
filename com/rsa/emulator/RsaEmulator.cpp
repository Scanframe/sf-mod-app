#include <misc/gen/gen_utils.h>

#include "RsaEmulator.h"

namespace sf
{

#define EMU_CHANNELCOUNT 8
#define EMU_MAXGATES 8
#define EMU_INPUTCOUNT 2
#define EMU_MAX_TCG_POINTS 16
//
// Parameter ID manipulation parameters.
//
#define MAKE_ID(ch, gate, param) (((ch) << 24) | ((gate) << 16) | (param))
#define GETINDEX(id) ((id) & 0xFFFF)
// Strips the gate index number from the ID.
#define GETGATE(id) (((id) & 0xFF0000) >> 16)
// Strips the channel index number from the ID.
#define GETCHANNEL(id) (((id) & 0xFF000000) >> 24)
// Define for a no gate number.
#define NO_GATE 0xFF
// Define for a no channel number.
#define NO_CHANNEL 0xFF

// Macro's for mapping parameter enumerates PID numbers.
#define PID_MAP(n)         (n)
#define PID_CHANNEL_MAP(n) ((n) & ~apChannel_Mask)
#define PID_GATE_MAP(n)    ((n) & ~apGate_Mask)
// Macro's for added parameter enumerates PID numbers.
#define PID_ADDED(n)         (apUserFirst + (n))
#define PID_CHANNEL_ADDED(n) ((apChannel_UserFirst + (n)) & ~apChannel_Mask)
#define PID_GATE_ADDED(n)    ((apGate_UserFirst + (n)) & ~apGate_Mask)
// Macro's for mapping result enumerates PID numbers.
#define RID_MAP(n)         (n)
#define RID_CHANNEL_MAP(n) ((n) & ~arChannel_Mask)
#define RID_GATE_MAP(n)    ((n) & ~arGATE_Mask)
// Macro's for added result enumerates PID numbers.
#define RID_ADDED(n)         (arLAST + (n))
#define RID_CHANNEL_ADDED(n) ((arChannel_UserFirst + (n)) & ~arChannel_Mask)
#define RID_GATE_ADDED(n)    ((arGATE_UserFirst + (n)) & ~arGate_Mask)

// Map default parameters to our own ID's.
#define PID_CHANNELS       PID_MAP(apChannels)
#define PID_AMPUNITS       PID_MAP(apAmplitudeUnit)
#define PID_ERROR          PID_MAP(apError)
// TCG is a non channel parameter now because of limited number
// of params per channel.
#define PID_TCG            PID_ADDED(16)
// TCG Record member offsets
#define PID_TCG_TIME(n)    (PID_TCG + (n))
#define PID_TCG_GAIN(n)    (PID_TCG + (n) + 16)
//
#define PID_ONESHOTDELAY   PID_ADDED(12)
#define PID_ONESHOTSTATE   PID_ADDED(13)
// Map default channel parameters to our own ID's.
#define PID_REPRATE         PID_CHANNEL_MAP(apChannel_RepRate)
#define PID_SYNCMODE        PID_CHANNEL_MAP(apChannel_SyncMode)
#define PID_GATES           PID_CHANNEL_MAP(apChannel_Gates)
#define PID_INPUTS          PID_CHANNEL_MAP(apChannel_Inputs)
#define PID_TIMEUNITS       PID_CHANNEL_MAP(apChannel_TimeUnits)
#define PID_COPY_DELAY      PID_CHANNEL_MAP(apChannel_CopyDelay)
#define PID_COPY_RANGE      PID_CHANNEL_MAP(apChannel_CopyRange)
#define PID_COPY_ENABLE     PID_CHANNEL_MAP(apChannel_CopyEnable)
#define PID_POPMANUAL       PID_CHANNEL_MAP(apChannel_PopManual)
#define PID_BIDIRMODE       PID_CHANNEL_MAP(apChannel_BiDirMode)
// Emulator added parameters
#define PID_GAIN            PID_CHANNEL_ADDED(1)
#define PID_POPDIV          PID_CHANNEL_ADDED(2)
#define PID_IF_POSITION     PID_CHANNEL_ADDED(3)
// Map default gate params to our own ID's.
#define PID_GATE_NAME       PID_GATE_MAP(apGate_Name)
#define PID_GATE_DELAY      PID_GATE_MAP(apGate_Delay)
#define PID_GATE_RANGE      PID_GATE_MAP(apGate_Range)
#define PID_GATE_SLAVETO    PID_GATE_MAP(apGate_SlaveTo)
#define PID_GATE_ENABLE     PID_GATE_MAP(apGate_Enable)
#define PID_GATE_METHOD     PID_GATE_MAP(apGate_Method)
#define PID_GATE_THRESHOLD  PID_GATE_MAP(apGate_Threshold)
#define PID_GATE_AMP        PID_GATE_MAP(apGate_Amplitude)
#define PID_GATE_TOF        PID_GATE_MAP(apGate_TimeOfFlight)
//
#define PID_GATE_POLARITY   PID_GATE_ADDED(2)
//
#define PID_GAIN                PID_CHANNEL_ADDED(1)
#define PID_SEC_GAIN            PID_CHANNEL_ADDED(2)
#define PID_TESTMODE            PID_CHANNEL_ADDED(3)
#define PID_PULSEWIDTH          PID_CHANNEL_ADDED(4)
#define PID_VOLTAGE             PID_CHANNEL_ADDED(5)
#define PID_TCG_ENABLE          PID_CHANNEL_ADDED(6)
#define PID_RFFILTER            PID_CHANNEL_ADDED(7)
#define PID_REPPERIOD           PID_CHANNEL_ADDED(8)
#define PID_ASCAN_RECTIFY       PID_CHANNEL_ADDED(10)
#define PID_ASCAN_DISPLAY_MODE  PID_CHANNEL_ADDED(11)
#define PID_ASCAN_REJECT        PID_CHANNEL_ADDED(12)
#define PID_ATTN_ENABLE         PID_CHANNEL_ADDED(13)
#define PID_SEC_ATTN_ENABLE     PID_CHANNEL_ADDED(14)
#define PID_PULS_FREQUENCY      PID_CHANNEL_ADDED(15)
#define PID_TCG_SLAVETO         PID_CHANNEL_ADDED(16)
#define PID_TCG_DELAY           PID_CHANNEL_ADDED(17)
#define PID_TCG_RANGE           PID_CHANNEL_ADDED(18)
//
#define PID_GATE_MODE           PID_GATE_ADDED(1)
#define PID_GATE_DETECTION      PID_GATE_ADDED(2)
#define PID_GATE_PHASE          PID_GATE_ADDED(3)

//
// Map default results to our own ID's.
//
#define RID_POPINDEX        RID_CHANNEL_MAP(arChannel_PopIndex)
#define RID_COPYDATA        RID_CHANNEL_MAP(arChannel_CopyData)
#define RID_COPYINDEX       RID_CHANNEL_MAP(arChannel_CopyIndex)
//
#define RID_PEAK_AMP        RID_CHANNEL_ADDED(1)
#define RID_PEAK_TOF        RID_CHANNEL_ADDED(2)
#define RID_COPY            RID_CHANNEL_ADDED(3)

// Offset of zero level for TOF values.
#define TOF_OFFSET      0x7FFFFF

// Map method names to our ID's.
#define MID_NONE 0x0000
#define MID_PEAK 0x0001
#define MID_COPY 0x0002

/**
 * Forward definitions.
 */
void FillDataBuffer(
	DynamicBuffer buf,
	unsigned width,
	unsigned wordSize,
	unsigned bits,
	double gain,
	int delay1,
	int delay2,
	int delay3
);

/**
 *
 * @param polarity  Value <0 = neg, 0= full, >0 = pos.
 * @param threshold The threshold value to determine a peak (value > threshold).
 * @param count Number of samples within the gate.
 * @param values Array of all values for each sample within the gate.
 * @param peakidx Sample number of the peak value.
 * @param peakval Value of the peak.
 * @return ??
 */
bool PeakNormal(
	int polarity,
	unsigned threshold,
	unsigned count,
	const unsigned char* values,
	unsigned& peakidx,
	unsigned& peakval
);

AcquisitionEmulator::AcquisitionEmulator(const Parameters& parameters)
	:RsaInterface(parameters)
	 , FChannelInfo(new TChannelInfo[EMU_CHANNELCOUNT])
	 , SustainEntry(this, &AcquisitionEmulator::sustain, SustainBase::spDefault)
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "Constructor of " << parameters._mode);
	// Disable the single shot timer.
	SingleShotTimer.disable();
	// Set the sustain local timer to limit the frequency to 20 (0.05s) when SustainBase::spTimer is set.
	SustainEntry.setInterval(TimeSpec(0.05));
	// Set the fixed flag when USPC21000 is selected.
	if (parameters._mode == 1)
	{
		FlagFixed = true;
	}
	// Initialize channel structures.
	for (int i = 0; i < EMU_CHANNELCOUNT; i++)
	{
		FChannelInfo[i].GateCount = (FlagFixed) ? EMU_MAXGATES : 0;
		FChannelInfo[i].TimeUnits = 1e-7;
		FChannelInfo[i].Gain = 30;
		FChannelInfo[i].RepRate = 1000;
		FChannelInfo[i].PopDivider = 1;
		FChannelInfo[i].CopyEnabled = true;
		FChannelInfo[i].CopyRange = 100;

		for (int j = 0; j < EMU_MAXGATES; j++)
		{
			FChannelInfo[i].GateInfo[j].MethodId = MID_COPY;
			FChannelInfo[i].GateInfo[j].Delay = 100 + j * 100;
			if (j == 0)
			{
				FChannelInfo[i].GateInfo[j].Name = "IF Gate";
			}
			else
			{
				FChannelInfo[i].GateInfo[j].Name = stringf("Gate %u", j);
			}
		}
	}
	//*
	// Dik hout zagen van planken methode.
	// Get the current ids
	IdList ids;
	if (enumParamIds(ids))
	{
		for (unsigned i = 0; i < ids.count(); i++)
		{
			ParamInfo info;
			// Get the default value by getting the info struct.
			if (handleParam(ids[i], &info, nullptr, nullptr))
			{ // Set the value using the info structs default.
				handleParam(ids[i], nullptr, &info.Default, nullptr);
			}
		}
	}
	//*/
}

AcquisitionEmulator::~AcquisitionEmulator()
{
	delete[] FChannelInfo;
}

bool AcquisitionEmulator::doInitialize(bool init)
{
	return true;
}

bool AcquisitionEmulator::getRunMode() const
{
	return RunMode;
}

bool AcquisitionEmulator::setRunMode(bool runMode, bool clear)
{
	if (RunMode != runMode)
	{
		RunMode = runMode;
	}
	// When clear is set
	if (clear)
	{
		SyncTimeStart = TimeSpec(getTime()).toDouble();
		for (unsigned i = 0; i < ChannelCount; i++)
		{
			FChannelInfo[i].SyncCounter = 0;
			FChannelInfo[i].SyncTimeOffset = 0.0;
		}
	}
	return true;
}

long AcquisitionEmulator::TimeUnits(const TChannelInfo& ci, const Value& value) const
{
	return static_cast<long>(round<Value::flt_type>(value.getFloat() / ci.TimeUnits, 1.0));
}

RsaTypes::IdType AcquisitionEmulator::getParamId
	(
		EDefaultParam param,
		unsigned gate,
		unsigned channel
	) const
{
	// Form the id on basis of the masks in the param.
	int id = MAKE_ID
	(
		((param & apChannel_Mask) | (param & apGate_Mask)) ? channel : NO_CHANNEL,
		(param & apGate_Mask) ? gate : NO_GATE,
		param & ~(apChannel_Mask | apGate_Mask)
	);
	return id;
}

RsaTypes::IdType AcquisitionEmulator::getResultId
	(
		EDefaultResult result,
		unsigned gate,
		unsigned channel
	) const
{
	// Form the id on basis of the masks in the param.
	IdType id = MAKE_ID
	(
		((result & arChannel_Mask) | (result & arGate_Mask)) ? channel : NO_CHANNEL,
		(result & arGate_Mask) ? gate : NO_GATE,
		result & ~(arChannel_Mask | apGate_Mask)
	);
	return id;
}

void AcquisitionEmulator::TGateInfo::CalculateThreshold()
{
	auto val = ThresholdValue; //calc_offset(ThresholdValue, 0.0, 100.0, 128, true);
	val *= Polarity ? Polarity : 1;
	val += Polarity ? 128 : 0;
	Threshold = (unsigned) val;
	SF_RTTI_NOTIFY(DO_DEFAULT, "Threshold Level set to : " << val);
}

bool AcquisitionEmulator::handleParam
	(
		IdType id,
		ParamInfo* info,
		const Value* setval,
		Value* getval
	)
{
	// Check if info must be filled in.
	if (info)
	{
		// Initialize the info structure.
		info->Init();
		// Archive flag is default.
		info->Flags = pfArchive;
		// Default for all for this implementation.
		info->Index = GETINDEX(id);
		// Retrieve the gate number form the param id.
		info->Gate = (GETGATE(id) == NO_GATE) ? UINT_MAX : GETGATE(id);
		// Retrieve the channel number form the param id.
		info->Channel = (GETCHANNEL(id) == NO_CHANNEL) ? UINT_MAX : GETCHANNEL(id);
	}
	// Switch between channel non channel parameter ID's.
	if (GETCHANNEL(id) == NO_CHANNEL)
	{
		int pid = GETINDEX(id);
		switch (pid)
		{
			case PID_ERROR:
				if (setval)
				{ // Allow only when the current error is recoverable.
					// and the passed value is zero.
					if (ErrorValue > 0 && setval->getInteger())
					{
						ErrorValue = 0;
					}
				}
				if (getval)
				{
					getval->set(ErrorValue);
				}
				if (info)
				{
					info->Id = id;
					info->Name = "Error";
					info->Unit = "!";
					info->Description += "Error value.";
					info->Default.set(0);
					info->Round.set(1);
					info->Minimum.set(-1);
					info->Maximum.set(1);
					info->States.add(ParamState("Irrecoverable", Value(-1)));
					info->States.add(ParamState("Okay", Value(0)));
					info->States.add(ParamState("Recoverable", Value(1)));
				}
				break;

			case PID_CHANNELS:
				if (setval)
				{
					ChannelCount = setval->getInteger();
				}
				if (getval)
				{
					getval->set(ChannelCount);
				}
				if (info)
				{
					info->Id = id;
					info->Name = "Channels";
					info->Unit = "!";
					info->Description += "Amount of available channels.";
					info->Default.set(1);
					info->Round.set(1);
					info->Minimum.set(1);
					info->Maximum.set(4);
					info->Flags = pfSystem | pfEffectsParameter | pfEffectsResult;
				}
				break;

			case PID_AMPUNITS:
				if (setval)
				{
					AmplitudeUnit = setval->getFloat();
				}
				if (getval)
				{
					getval->set(AmplitudeUnit);
				}
				if (info)
				{
					info->Id = id;
					info->Name = "Amplitude Unit";
					info->Unit = "V";
					info->Description += "Amplitude units of the digitiser single step.";
					info->Default.set(0.01);
					info->Round.set(0.001);
					info->Minimum.set(0.001);
					info->Maximum.set(0.05);
					//info->Flags = pfSystem;
				}
				break;

			case PID_ONESHOTDELAY:
				if (setval)
				{
					SingleShotTimer.set(setval->getInteger());
					SingleShotTimer.disable();
				}
				if (getval)
				{
					getval->set(SingleShotTimer.getElapseTime().toDouble());
				}
				if (info)
				{
					info->Id = id;
					info->Name = "One Shot|Delay";
					info->Unit = "ms";
					info->Description += "Delay to wait on a single shot.";
					info->Default.set(500);
					info->Round.set(1);
					info->Minimum.set(0);
					info->Maximum.set(5000);
				}
				break;

			case PID_ONESHOTSTATE:
				if (setval)
				{
					if (setval->getInteger())
					{
						SingleShotTimer.reset();
					}
					else
					{
						SingleShotTimer.disable();
					}
				}
				if (getval)
				{
					getval->set(SingleShotTimer.isEnabled());
				}
				if (info)
				{
					info->Id = id;
					info->Name = "One Shot|State";
					info->Unit = "!";
					info->Description += "Enables single shot.";
					info->Default.set(0);
					info->Round.set(1);
					info->Minimum.set(0);
					info->Maximum.set(1);
					info->States.add(ParamState("Ready", Value(0)));
					info->States.add(ParamState("Running", Value(1)));
					info->Flags = 0;
				}
				break;

			case PID_TCG_TIME(0):
			case PID_TCG_TIME(1):
			case PID_TCG_TIME(2):
			case PID_TCG_TIME(3):
			case PID_TCG_TIME(4):
			case PID_TCG_TIME(5):
			case PID_TCG_TIME(6):
			case PID_TCG_TIME(7):
			case PID_TCG_TIME(8):
			case PID_TCG_TIME(9):
			case PID_TCG_TIME(10):
			case PID_TCG_TIME(11):
			case PID_TCG_TIME(12):
			case PID_TCG_TIME(13):
			case PID_TCG_TIME(14):
			case PID_TCG_TIME(15):
			{
				// Calculate the point number in the tcg list.
				int index = pid - PID_TCG_TIME(0);
				if (setval)
				{
					Tcg[index].Time = setval->getFloat();
				}
				if (getval)
				{
					getval->set(Tcg[index].Time);
				}
				if (info)
				{
					info->Id = id;
					info->Name += stringf("Receiver|TCG|Point%3i|Time", index + 1);
					info->Unit = "s";
					info->Description += stringf("TCG time point number. %i", index + 1);
					info->Default.set(0.0);
					info->Round.set(0.01E-6);
					info->Minimum.set(0.0);
					info->Maximum.set(326E-6);
				}
			}
				break;

				// Receiver tcg point Gain slope in 0.1dB steps
			case PID_TCG_GAIN(0):
			case PID_TCG_GAIN(1):
			case PID_TCG_GAIN(2):
			case PID_TCG_GAIN(3):
			case PID_TCG_GAIN(4):
			case PID_TCG_GAIN(5):
			case PID_TCG_GAIN(6):
			case PID_TCG_GAIN(7):
			case PID_TCG_GAIN(8):
			case PID_TCG_GAIN(9):
			case PID_TCG_GAIN(10):
			case PID_TCG_GAIN(11):
			case PID_TCG_GAIN(12):
			case PID_TCG_GAIN(13):
			case PID_TCG_GAIN(14):
			case PID_TCG_GAIN(15):
			{
				// Calculate the point number in the tcg list.
				int index = pid - PID_TCG_GAIN(0);
				if (setval)
				{
					Tcg[index].Gain = setval->getFloat();
				}
				if (getval)
				{
					getval->set(Tcg[index].Gain);
				}
				if (info)
				{
					info->Id = id;
					info->Name += stringf("Receiver|TCG|Point%3i|Gain", index + 1);
					info->Unit = "dB";
					info->Description += "Receiver TCG point gain.";
					info->Default.set(0.0);
					info->Round.set(0.1);
					info->Minimum.set(0.0);
					info->Maximum.set(80.0);
				}
			}
				break;

			default:
				// Report the id was not found.
			SF_RTTI_NOTIFY(DO_DEFAULT, "Param ID " << stringf("0x%lX", id) << " does not exist!");
				return false;
		}
	}
	else
	{
		// Create temporary easy to use reference.
		TChannelInfo& ci(FChannelInfo[GETCHANNEL(id)]);
		// Switch between gate and non gate parameter ID's.
		if (GETGATE(id) == NO_GATE)
		{
			switch (GETINDEX(id))
			{
				default:
					// Report the id was not found.
				SF_RTTI_NOTIFY(DO_DEFAULT, "Channel Param ID 0x" << itostr(id, 16) << " does not exist!");
					return false;

				case PID_REPRATE:
					if (setval)
					{
						ci.RepRate = setval->getFloat();
						//ci.SyncCounter = (time - SyncTimeStart + ci.SyncTimeOffset) * ci.RepRate;
						ci.SyncTimeOffset = ((double) ci.SyncCounter / ci.RepRate) + SyncTimeStart - TimeSpec(getTime()).toDouble();
					}
					if (getval)
					{
						getval->set(ci.RepRate);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Rep.Rate";
						info->Unit = "Hz";
						info->Description += "Repetition rate when sync is set to internal.";
						info->Default.set(5.0);
						info->Round.set(1.0);
						info->Minimum.set(1.0);
						info->Maximum.set(1000.0);
					}
					break;

				case PID_SYNCMODE:
					if (setval)
					{
						ci.SyncMode = setval->getInteger();
					}
					if (getval)
					{
						getval->set(ci.SyncMode);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Sync Mode";
						info->Unit = "!";
						info->Description += "Sync signal source selection.";
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->States.add(ParamState("Internal", Value(0)));
						info->States.add(ParamState("External", Value(1)));
					}
					break;

				case PID_GATES:
					if (setval)
					{
						ci.GateCount = setval->getInteger();
					}
					if (getval)
					{
						getval->set(ci.GateCount);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Gates";
						info->Unit = "!";
						info->Description += "Amount of gates selection.";
						info->Default.set(FlagFixed ? ci.GateCount : 0);
						info->Round.set(1);
						info->Minimum.set(FlagFixed ? ci.GateCount : 0);
						info->Maximum.set(FlagFixed ? ci.GateCount : 3);
						info->Flags |= pfEffectsParameter | pfEffectsResult;
						if (FlagFixed)
						{
							info->Flags |= pfReadonly;
						}
						info->States.add(ParamState("None", Value(0)));
						info->States.add(ParamState("IF Gate Only", Value(1)));
						info->States.add(ParamState("1 Signal Gate", Value(2)));
						info->States.add(ParamState("2 Signal Gates", Value(3)));
					}
					break;

				case PID_INPUTS:
					if (getval)
					{
						getval->set(1);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Inputs";
						info->Unit = "!";
						info->Description += "Amount of available synchronised inputs.";
						info->Default.set(1);
						info->Round.set(1);
						info->Minimum.set(1);
						info->Maximum.set(1);
						info->Flags |= pfReadonly;
					}
					break;

				case PID_POPMANUAL:
					if (setval)
					{
						ci.PopManual = setval->getInteger();
						// When the
						if (ci.PopManual > 1)
						{
							ci.PopManual = 1;
							SF_RTTI_NOTIFY(DO_DEFAULT, "Triggering Manual POP.");
						}
					}
					if (getval)
					{
						getval->set(ci.PopManual);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Pop Manual";
						info->Unit = "!";
						info->Description += "Enables and generates manual POP index in the table. Disables hardware generation of POP indices.";
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(2);
						info->Flags &= ~(pfWriteAtOff | pfArchive);
						info->States.add(ParamState("Disabled", Value(0)));
						info->States.add(ParamState("Ready", Value(1)));
						info->States.add(ParamState("Trigger", Value(2)));
					}
					break;

				case PID_BIDIRMODE:
					if (setval)
					{
						ci.BiDirMode = setval->getInteger();
					}
					if (getval)
					{
						getval->set(ci.BiDirMode);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Bidirectional Mode";
						info->Unit = "!";
						info->Description += "Reverses every other scanline. When manual POP is used.";
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->Flags = pfChannelSingle;
						info->States.add(ParamState("Disabled", Value(0)));
						info->States.add(ParamState("Enabled", Value(1)));
					}
					break;

				case PID_TIMEUNITS:
					if (getval)
					{
						getval->set(ci.TimeUnits);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Time Unit";
						info->Unit = "s";
						info->Description += "Time in seconds that is used for the time dependant results.";
						info->Default.set(1e-6);
						info->Round.set(1e-8);
						info->Minimum.set(1e-8);
						info->Maximum.set(1e-6);
						// info->States
						info->Flags |= pfReadonly;
					}
					break;

				case PID_COPY_ENABLE:
					if (setval)
					{
						ci.CopyEnabled = setval->getInteger();
					}
					if (getval)
					{
						getval->set(ci.CopyEnabled);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Enable";
						info->Unit = "!";
						info->Description += "Enables generation of data for presenting an a-scan.";
						info->Default.set(1);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->States.add(ParamState("Off", Value(0)));
						info->States.add(ParamState("On", Value(1)));
					}
					break;

				case PID_COPY_DELAY:
					if (setval)
					{
						ci.CopyDelay = TimeUnits(ci, *setval);
					}
					if (getval)
					{
						getval->set(ci.TimeUnits * ci.CopyDelay);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "A-scan|Delay";
						info->Unit = "s";
						info->Description += "Delay for the A-scan result of this channel.";
						info->Default.set(ci.TimeUnits * 0);
						info->Round.set(ci.TimeUnits);
						info->Minimum.set(ci.TimeUnits * -5000);
						info->Maximum.set(ci.TimeUnits * 5000);
					}
					break;

				case PID_COPY_RANGE:
					if (setval)
					{
						ci.CopyRange = TimeUnits(ci, *setval);
					}
					if (getval)
					{
						getval->set(ci.TimeUnits * ci.CopyRange);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "A-scan|Range";
						info->Unit = "s";
						info->Description += "Range for the A-scan result of this channel.";
						info->Default.set(ci.TimeUnits * 400);
						info->Round.set(ci.TimeUnits);
						info->Minimum.set(ci.TimeUnits * 10);
						info->Maximum.set(ci.TimeUnits * 5000);
						info->Flags |= pfEffectsResult;
					}
					break;

				case PID_GAIN:
					if (setval)
					{
						ci.Gain = setval->getFloat();
					}
					if (getval)
					{
						getval->set(ci.Gain);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Receiver|Gain";
						info->Unit = "dB";
						info->Description += "Receiver gain of this channel.";
						info->Default.set(35.0);
						info->Round.set(0.1);
						info->Minimum.set(-20.0);
						info->Maximum.set(+80.0);
					}
					break;

				case PID_POPDIV:
					if (setval)
					{
						ci.PopDivider = setval->getInteger();
					}
					if (getval)
					{
						getval->set(ci.PopDivider);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "POP Divider";
						info->Unit = "n";
						info->Description += "Position orientation sync pulse divider.";
						info->Default.set(10);
						info->Round.set(1);
						info->Minimum.set(1);
						info->Maximum.set(1 << 12);
					}
					break;

				case PID_IF_POSITION:
					if (setval)
					{
						ci.IfPos = TimeUnits(ci, *setval);
					}
					if (getval)
					{
						getval->set(ci.TimeUnits * ci.IfPos);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "IF Position";
						info->Unit = "s";
						info->Description += "Interface position.";
						info->Default.set(203E-6);
						info->Round.set(ci.TimeUnits);
						info->Minimum.set(ci.TimeUnits * -5000.0);
						info->Maximum.set(ci.TimeUnits * 5000.0);
					}
					break;

				case PID_ASCAN_RECTIFY:
					if (setval)
					{
						ci.AscanRectify = setval->getInteger();
					}
					if (getval)
					{
						getval->set(ci.AscanRectify);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "A-scan|Rectify";
						info->Unit = "";
						info->Description += "A-scan rectification mode.";
						info->Default.set(0);
						info->Minimum.set(0);
						info->Maximum.set(3);
						info->States.add(ParamState("RF", Value(0)));
						info->States.add(ParamState("Half Wave +", Value(1)));
						info->States.add(ParamState("Half Wave -", Value(2)));
						info->States.add(ParamState("Full Wave", Value(3)));
						info->Flags |= pfEffectsResult;
					}
					break;

				case PID_TCG_ENABLE:
					if (setval)
					{
						ci.TcgEnable = setval->getInteger();
					}
					if (getval)
					{
						getval->set(ci.TcgEnable);
					}
					if (info)
					{
						info->Id = id;
						info->Name += "Receiver|TCG|Enable";
						info->Unit = "!";
						info->Description += "Receiver TCG Enable of this channel.";
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->States.add(ParamState("Off", Value(0)));
						info->States.add(ParamState("On", Value(1)));
					}
					break;

				case PID_TCG_SLAVETO:
					// Value has an offset so that the IF gate has value zero.
					if (setval)
					{
						ci.TcgSlavedTo = setval->getInteger();
					}
					if (getval)
					{
						getval->set(ci.TcgSlavedTo);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Receiver|TCG|Slaved";
						info->Unit = "!";
						info->Description += "";
						info->Default.set(-1);
						info->Round.set(1);
						info->Minimum.set(-2);
						info->Maximum.set(0);
						info->States.add(ParamState("Artificial", Value(-2)));
						info->States.add(ParamState("Initial Pulse", Value(-1)));
						info->States.add(ParamState("IF Gate", Value(0)));
					}
					break;

				case PID_TCG_DELAY:
					if (setval)
					{
						ci.TcgDelay = TimeUnits(ci, *setval);
					}
					if (getval)
					{
						getval->set(ci.TimeUnits * ci.TcgDelay);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Receiver|TCG|Delay";
						info->Unit = "s";
						info->Description += "Delay for the receiver time controlled gain gate.";
						info->Default.set(200E-6);
						info->Default.set(16E-6);
						info->Round.set(20E-9);
						info->Minimum.set(0.0);
						info->Maximum.set(655E-6);
					}
					break;

				case PID_TCG_RANGE:
					if (setval)
					{
						ci.TcgRange = TimeUnits(ci, *setval);
					}
					if (getval)
					{
						getval->set(ci.TimeUnits * ci.TcgRange);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Receiver|TCG|Range";
						info->Unit = "s";
						info->Description += "Range for the receiver time controlled gain gate.";
						info->Default.set(200E-6);
						info->Default.set(16E-6);
						info->Round.set(20E-9);
						info->Minimum.set(0.0);
						info->Maximum.set(655E-6);
					}
					break;
			}
		}
		else
		{
			int gate = GETGATE(id);
			// Create temporary easy to use reference.
			TGateInfo& gi(ci.GateInfo[gate]);
			// Preset the gate related flag
			if (info)
			{
				info->Flags |= pfGate;
			}
			switch (GETINDEX(id))
			{
				default:
					// Report the id was not found.
				SF_RTTI_NOTIFY(DO_DEFAULT,"Gate " << info->Gate << " Param ID " << stringf("0x%lX", id) << " does not exist!");
					return false;

				case PID_GATE_NAME:
					if (getval)
					{
						getval->set(gi.Name);
					}
					break;

				case PID_GATE_SLAVETO:
					if (setval)
					{
						gi.SlavedTo = setval->getInteger();
					}
					if (getval)
					{
						getval->set(gi.SlavedTo);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Slaved";
						info->Unit = "!";
						info->Description += "Slaves to previous gate or not.";
						info->Default.set(-1);
						info->Round.set(1);
						info->Minimum.set(-2);
						info->Maximum.set(ci.GateCount);
						info->States.add(ParamState("Artificial", Value(-2)));
						info->States.add(ParamState("Independent", Value(-1)));
						// Only add gates to slave to if this is not the IF gate
						if (gate != 0)
						{ // Add all the gates that come before this gate
							for (int i = 0; i < gate; i++)
							{
								// When it has peak type of method
								if (ci.GateInfo[i].MethodId != MID_COPY)
								{
									// and when it is not slaved
									if (ci.GateInfo[i].SlavedTo < 0)
									{
										info->States.add(ParamState(ci.GateInfo[i].Name, Value(i)));
									}
								}
							}
						}
						// ???
						info->Flags |= pfEffectsParameter;
					}
					break;

				case PID_GATE_ENABLE:
					if (setval)
					{
						gi.DataEnable = setval->getInteger();
					}
					if (getval)
					{
						getval->set(gi.DataEnable);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Enable";
						info->Unit = "!";
						info->Description += "Enables storing of % data.";
						info->Default.set(1);
						info->Round.set(1);
						info->States.add(ParamState("Off", Value(0)));
						info->States.add(ParamState("On", Value(1)));
						info->Flags |= pfEffectsResult;
					}
					break;

				case PID_GATE_METHOD:
					if (setval)
					{
						// Get the method id from the result.
						if (setval->getString() == US_METHOD_PEAK)
						{
							gi.MethodId = MID_PEAK;
						}
						else if (setval->getString() == US_METHOD_COPY)
						{
							gi.MethodId = MID_COPY;
						}
						else
						{
							gi.MethodId = MID_NONE;
						}
					}
					if (getval)
					{
						const char* s[] =
							{
								"None",
								US_METHOD_PEAK,
								US_METHOD_COPY
							};
						getval->set(s[gi.MethodId]);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Method";
						info->Unit = "!";
						info->Description += "Method selector.";
						info->Default.set(US_METHOD_PEAK);
						info->States.add(ParamState("Peak", Value(US_METHOD_PEAK)));
						info->States.add(ParamState("Copy", Value(US_METHOD_COPY)));
						// This parameter effects the results and parameters geometry.
						info->Flags |= pfEffectsParameter | pfEffectsResult;
						if (FlagFixed)
						{
							info->Flags |= pfReadonly;
						}
					}
					break;

				case PID_GATE_DELAY:
					if (setval)
					{
						gi.Delay = TimeUnits(ci, *setval);
					}
					if (getval)
					{
						getval->set(ci.TimeUnits * gi.Delay);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Delay";
						info->Unit = "s";
						info->Description += "Delay.";
						info->Default.set(ci.TimeUnits * 200);
						info->Round.set(ci.TimeUnits);
						info->Minimum.set(-ci.TimeUnits * 3000);
						info->Maximum.set(ci.TimeUnits * 3000);
					}
					break;

				case PID_GATE_THRESHOLD:
					if (setval)
					{
						gi.ThresholdValue = setval->getFloat();
						// Calculate new threshold level depending on polarity.
						gi.CalculateThreshold();
					}
					if (getval)
					{
						getval->set(gi.ThresholdValue);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Threshold";
						info->Unit = "%";
						info->Description += "Peak threshold level.";
						info->Default.set(30.0);
						info->Round.set(0.5);
						info->Minimum.set(0.0);
						info->Maximum.set(128.0);
					}
					break;

				case PID_GATE_AMP:
				{
					if (getval)
					{ // Check if a peak was found at all.
						if (gi.PeakFound)
						{
							getval->set(abs((int) gi.PeakAmp - 127));
						}
						else
						{
							getval->set(0);
						}
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Amplitude";
						info->Unit = "%";
						info->Description += "Holds the current averaged amplitude.";
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(127);
						info->Flags |= pfReadonly;
						info->Flags &= ~pfArchive;
					}
				}
					break;

				case PID_GATE_TOF:
				{
					if (getval)
					{
						getval->set(ci.TimeUnits * (gi.PeakTof - TOF_OFFSET));
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Time Of Flight";
						info->Unit = "s";
						info->Description += "Holds the current averaged time-of-flight.";
						info->Default.set(0.0);
						info->Round.set(ci.TimeUnits);
						info->Minimum.set(0.0);
						info->Maximum.set(ci.TimeUnits * 0xFFFFFFF);
						info->Flags |= pfReadonly;
						info->Flags &= ~pfArchive;
					}
				}
					break;

				case PID_GATE_POLARITY:
					if (setval)
					{
						gi.Polarity = (EPolarity) setval->getInteger();
						gi.CalculateThreshold();
					}
					if (getval)
					{
						getval->set(gi.Polarity);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Polarity";
						info->Unit = "!";
						info->Description += "Negative or positive peak detection.";
						info->Default.set(plPositive);
						info->Round.set(1);
						info->Minimum.set(-1);
						info->Maximum.set(1);
						info->States.add(ParamState("Negative", Value(plNegative)));
						info->States.add(ParamState("Positive", Value(plPositive)));
						info->States.add(ParamState("Full", Value(plFull)));
					}
					break;

				case PID_GATE_RANGE:
					if (setval)
					{
						gi.Range = TimeUnits(ci, *setval);
					}
					if (getval)
					{
						getval->set(ci.TimeUnits * gi.Range);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Range";
						info->Unit = "s";
						info->Description += "Time range.";
						info->Default.set(ci.TimeUnits * 400);
						info->Round.set(ci.TimeUnits);
						info->Minimum.set(ci.TimeUnits * 10);
						info->Maximum.set(ci.TimeUnits * 5000);
						// Only effects results when method is copy.
						if (gi.MethodId == MID_COPY)
						{
							info->Flags |= pfEffectsResult;
						}
					}
					break;

			}
		}
	}
	// Check if at least the id is filled in.
	if (info && info->Id == 0)
	{
		return false;
	}
	// Found it so return true.
	return true;
}

bool AcquisitionEmulator::enumParamIds(IdList& ids)
{
	// Add interface parameters.
	ids.add(MAKE_ID(NO_CHANNEL, NO_GATE, PID_CHANNELS));
	ids.add(MAKE_ID(NO_CHANNEL, NO_GATE, PID_ERROR));
	ids.add(MAKE_ID(NO_CHANNEL, NO_GATE, PID_ONESHOTDELAY));
	ids.add(MAKE_ID(NO_CHANNEL, NO_GATE, PID_ONESHOTSTATE));
	ids.add(MAKE_ID(NO_CHANNEL, NO_GATE, PID_AMPUNITS));
	// Add the TCG ID's
	for (int i = 0; i < EMU_MAX_TCG_POINTS; i++)
	{
		ids.add(MAKE_ID(NO_CHANNEL, NO_GATE, PID_TCG_TIME(i)));
		ids.add(MAKE_ID(NO_CHANNEL, NO_GATE, PID_TCG_GAIN(i)));
	}
	// Add channel parameters
	for (unsigned channel = 0; channel < ChannelCount; channel++)
	{ // Fast local reference.
		TChannelInfo& ci(FChannelInfo[channel]);
		//
		ids.add(MAKE_ID(channel, NO_GATE, PID_REPRATE));
		ids.add(MAKE_ID(channel, NO_GATE, PID_SYNCMODE));
		ids.add(MAKE_ID(channel, NO_GATE, PID_GATES));
		ids.add(MAKE_ID(channel, NO_GATE, PID_INPUTS));
		ids.add(MAKE_ID(channel, NO_GATE, PID_TIMEUNITS));
		ids.add(MAKE_ID(channel, NO_GATE, PID_COPY_DELAY));
		ids.add(MAKE_ID(channel, NO_GATE, PID_COPY_RANGE));
		// Only for channel one.
		if (!channel)
		{
			ids.add(MAKE_ID(channel, NO_GATE, PID_POPMANUAL));
			ids.add(MAKE_ID(channel, NO_GATE, PID_BIDIRMODE));
		}
		// Emulation added
		ids.add(MAKE_ID(channel, NO_GATE, PID_GAIN));
		ids.add(MAKE_ID(channel, NO_GATE, PID_ASCAN_RECTIFY));
		ids.add(MAKE_ID(channel, NO_GATE, PID_POPDIV));
		ids.add(MAKE_ID(channel, NO_GATE, PID_IF_POSITION));
		// Add gate parameters
		unsigned gates = FChannelInfo[channel].GateCount;
		for (unsigned gate = 0; gate < gates; gate++)
		{ // Fast local reference.
			TGateInfo& gi(ci.GateInfo[gate]);
			//
			ids.add(MAKE_ID(channel, gate, PID_GATE_DELAY));
			ids.add(MAKE_ID(channel, gate, PID_GATE_RANGE));
			ids.add(MAKE_ID(channel, gate, PID_GATE_SLAVETO));
			ids.add(MAKE_ID(channel, gate, PID_GATE_ENABLE));
			ids.add(MAKE_ID(channel, gate, PID_GATE_METHOD));
			ids.add(MAKE_ID(channel, gate, PID_GATE_SLAVETO));
			// check for non-copy gate
			if (gi.MethodId != MID_COPY)
			{
				ids.add(MAKE_ID(channel, gate, PID_GATE_THRESHOLD));
				ids.add(MAKE_ID(channel, gate, PID_GATE_POLARITY));
				if (gate)
				{
					ids.add(MAKE_ID(channel, gate, PID_GATE_AMP));
					ids.add(MAKE_ID(channel, gate, PID_GATE_TOF));
				}
			}
		}
		ids.add(MAKE_ID(channel, NO_GATE, PID_TCG_ENABLE));
		ids.add(MAKE_ID(channel, NO_GATE, PID_TCG_SLAVETO));
		ids.add(MAKE_ID(channel, NO_GATE, PID_TCG_DELAY));
		ids.add(MAKE_ID(channel, NO_GATE, PID_TCG_RANGE));
	}
	return true;
}

bool AcquisitionEmulator::handleResult(IdType id, ResultInfo* info, BufferInfo* bufInfo)
{
	// Check if info must be filled in.
	if (info)
	{
		// Initialize the info structure.
		info->reset();
		// Default for all for this implementation.
		info->Index = GETINDEX(id);
		// Retrieve the gate number form the param id.
		info->Gate = (GETGATE(id) == NO_GATE) ? UINT_MAX : GETGATE(id);
		// Retrieve the channel number form the param id.
		info->Channel = (GETCHANNEL(id) == NO_CHANNEL) ? UINT_MAX : GETCHANNEL(id);
		//
		info->Flags = 0;
	}
	// Check if the pointer is valid and clear the structure.
	if (bufInfo)
	{
		bufInfo->Clear();
	}
	// Switch between channel non channel parameter ID's.
	if (GETCHANNEL(id) == NO_CHANNEL)
	{
		switch (GETINDEX(id))
		{
			default:
				// Report the id was not found.
			SF_RTTI_NOTIFY(DO_DEFAULT, "Result ID " << stringf("0x%lX", id) << " does not exist!");
				return false;
		}
	}
	else
	{
		// Create temporary easy to use reference.
		TChannelInfo& ci(FChannelInfo[GETCHANNEL(id)]);
		// Switch between gate and non gate parameter ID's.
		if (GETGATE(id) == NO_GATE)
		{
			switch (GETINDEX(id))
			{
				default:
					// Report the id was not found.
				SF_RTTI_NOTIFY(DO_DEFAULT, "Channel Result ID " << stringf("0x%lX", id) << " does not exist!");
					return false;

				case RID_POPINDEX:
					if (info)
					{
						info->Id = id;
						info->Name = "Position Index";
						info->Description += "";
						info->Flags |= rfAsync;
						// Check every gate if data storage is enabled.
						for (unsigned i = 0; i < ci.GateCount; i++)
						{
							if (ci.GateInfo[i].DataEnable)
							{
								// If so make this result be stored as well.
								info->Flags |= rfStored;
							}
						}
						info->Bits = 24;
						info->WordSize = sizeof(uint32_t);
						info->ArraySize = 1;
					}
					if (bufInfo)
					{
						bufInfo->Buffer = &ci.PopIndex;
						bufInfo->Counter = ci.SyncCounter;
						bufInfo->Size = 1;
						bufInfo->Remain = 0;
						bufInfo->BlockBufSize = sizeof(uint32_t);
					}
					break;

				case RID_COPYINDEX:
					if (info)
					{
						info->Id = id;
						info->Name = "Copy|Index";
						info->Description += "";
						info->Flags |= rfAsyncIndex;
						info->Bits = 24;
						info->WordSize = sizeof(uint32_t);
						info->ArraySize = 1;
					}
					if (bufInfo)
					{
						bufInfo->Buffer = &ci.CopySyncIndex;
						bufInfo->Counter = ci.SyncCounter;
						bufInfo->Size = 1;
						bufInfo->Remain = 0;
						bufInfo->BlockBufSize = sizeof(uint32_t);
					}
					break;

				case RID_COPYDATA:
					if (info)
					{
						info->Id = id;
						info->Name = "Copy|Data";
						info->Description += "";
						info->Flags |= rfAsync;
						info->Bits = ci.AscanRectify ? 7 : 8;
						info->Offset = ci.AscanRectify ? 0 : 127;
						info->WordSize = sizeof(uint8_t);
						// The size of the array is dependant of the range of the gate.
						info->ArraySize = ci.CopyRange;
					}
					if (bufInfo)
					{
						bufInfo->Buffer = ci.CopyBuf.data();
						bufInfo->Counter = ci.SyncCounter;
						bufInfo->Size = 1;
						bufInfo->Remain = 0;
						bufInfo->BlockBufSize = ci.CopyRange * sizeof(uint8_t);
					}
					break;
			}
		}
		else
		{
			// Create temporary easy to use reference.
			TGateInfo& gi(ci.GateInfo[GETGATE(id)]);
			// Preset the gate related flag
			if (info)
			{
				info->Flags |= rfGate;
			}
			switch (GETINDEX(id))
			{
				default:
					// Report the id was not found.
				SF_RTTI_NOTIFY(DO_DEFAULT, "Gate " << info->Gate << " Param ID " << stringf("0x%lX", id) << " does not exist!");
					return false;

				case RID_COPY:
					if (info)
					{
						info->Id = id;
						info->Name = "Copy|Amplitude Array";
						info->Description += "Amplitude array with length of the range.";
						info->Bits = 8;
						info->Offset = 127;
						info->WordSize = sizeof(uint8_t);
						info->ArraySize = gi.Range;
						if (gi.DataEnable)
						{
							info->Flags |= rfStored;
						}
					}
					if (bufInfo)
					{
						bufInfo->Buffer = gi.CopyBuf.data();
						bufInfo->Counter = ci.SyncCounter;
						bufInfo->Size = 1;
						bufInfo->Remain = 0;
						bufInfo->BlockBufSize = gi.Range * sizeof(uint8_t);
					}
					break;

				case RID_PEAK_AMP:
					if (info)
					{
						info->Id = id;
						info->Name = "Peak|Amplitude";
						info->Description += "Amplitude of found peak.";
						info->Bits = 8;
						info->WordSize = 1;
						info->Offset = 127;
						info->ArraySize = 1;
						if (gi.DataEnable)
						{
							info->Flags |= rfStored;
						}
					}
					if (bufInfo)
					{
						bufInfo->Buffer = &gi.PeakAmp;
						bufInfo->Counter = ci.SyncCounter;
						bufInfo->Size = 1;
						bufInfo->Remain = 0;
						bufInfo->BlockBufSize = sizeof(uint8_t);
					}
					break;

				case RID_PEAK_TOF:
					if (info)
					{
						info->Id = id;
						info->Name = "Peak|Time Of Flight";
						info->Description += "Time Of Flight (TOF) of found peak.";
						info->Bits = 24;
						info->WordSize = 4;
						info->Offset = TOF_OFFSET;
						info->ArraySize = 1;
						if (gi.DataEnable)
						{
							info->Flags |= rfStored;
						}
					}
					if (bufInfo)
					{
						bufInfo->Buffer = &gi.PeakTof;
						bufInfo->Counter = ci.SyncCounter;
						bufInfo->Size = 1;
						bufInfo->Remain = 0;
						bufInfo->BlockBufSize = sizeof(uint32_t);
					}
					break;
			}
		}
	}
	// Check if at least the id is filled in.
	if (info && info->Id == 0)
	{
		return false;
	}
	// Found it so return true.
	return true;
}

bool AcquisitionEmulator::enumResultIds(IdList& ids)
{
	// Add channel results
	for (unsigned channel = 0; channel < ChannelCount; channel++)
	{
		ids.add(MAKE_ID(channel, NO_GATE, RID_POPINDEX));
		ids.add(MAKE_ID(channel, NO_GATE, RID_COPYDATA));
		ids.add(MAKE_ID(channel, NO_GATE, RID_COPYINDEX));
		// Add gate method results
		unsigned gates = FChannelInfo[channel].GateCount;
		for (unsigned gate = 0; gate < gates; gate++)
		{
			auto mid = FChannelInfo[channel].GateInfo[gate].MethodId;
			switch (mid)
			{
				default:
				case MID_PEAK:
					ids.add(MAKE_ID(channel, gate, RID_PEAK_AMP));
					ids.add(MAKE_ID(channel, gate, RID_PEAK_TOF));
					break;

				case MID_COPY:
					ids.add(MAKE_ID(channel, gate, RID_COPY));
					break;
			}
		}
	}
	return true;
}

bool AcquisitionEmulator::sustain(const timespec& t)
{
	// Sentry declaration and initialisation.
	static bool sentry = false;
	// When reentering return here.
	if (sentry)
	{
		return true;
	}
	// Do not allow reentry by the sustain loop.
	sentry = true;
	// Check the current mode the emulator is now.
	if (RunMode)
	{
		// Amount of time (seconds) since the start of the run mode.
		auto runTime = TimeSpec(t).toDouble() - SyncTimeStart;
		//
		for (int channel = 0; channel < ChannelCount; channel++)
		{
			auto& ci(FChannelInfo[channel]);
			// When the sync mode is internal generate data.
			if (!ci.SyncMode)
			{
				// Sync counter expected value.
				uint32_t syncCount = std::floor((runTime + ci.SyncTimeOffset) * ci.RepRate);
				// Generate the amount syncs which is needed so far.
				while (ci.SyncCounter <= syncCount)
				{
					// Generate a result at each n-th sync.
					if (ci.SyncCounter && ci.SyncCounter % ci.PopDivider == 0)
					{
						ci.PopIndex = ci.SyncCounter;
						callResultHook(MAKE_ID(channel, NO_GATE, RID_POPINDEX));
					}
					// Set the data for retrieval.
					ci.CopySyncIndex = ci.SyncCounter;
					// Signal the owner that data is available.
					callResultHook(MAKE_ID(channel, NO_GATE, RID_COPYINDEX));
					// Iterate through the gates.
					for (double& i: ci.Sweep)
					{
						i = random(0, 3);
					}
					//
					FillDataBuffer(
						ci.CopyBuf,
						ci.CopyRange,
						1,
						8,
						pow(10, ci.Gain / 20.0),
						ci.Sweep[0] - ci.CopyDelay + ci.IfPos + 100.0,
						ci.Sweep[1] - ci.CopyDelay + ci.IfPos + 250.0,
						ci.Sweep[2] - ci.CopyDelay + ci.IfPos + 350.0
					);
					//
					switch (ci.AscanRectify)
					{
						default:
							break;
						case 1:
							for (int i = 0; i < ci.CopyRange; i++)
							{
								auto amp = ci.CopyBuf[i];
								if (amp > 0)
								{
									amp = 0;
								}
								else
								{
									amp = 127 - clip(std::abs(amp), 0, 127);
								}
								ci.CopyBuf[i] = static_cast<uint8_t>(amp);
							}
							break;

						case 2:
							for (int i = 0; i < ci.CopyRange; i++)
							{
								int amp = ci.CopyBuf[i];
								if (amp < 0)
								{
									amp = 0;
								}
								else
								{
									amp = 127 - clip(abs(amp), 0, 127);
								}
								ci.CopyBuf[i] = static_cast<uint8_t>(amp);
							}
							break;

						case 3:
							for (int i = 0; i < ci.CopyRange; i++)
							{
								auto amp = ci.CopyBuf[i];
								amp = 127 - clip(abs(amp), 0, 127);
								ci.CopyBuf[i] = static_cast<uint8_t>(amp);
							}
							break;
					}
					// Signal the owner that data is available.
					callResultHook(MAKE_ID(channel, NO_GATE, RID_COPYDATA));
					// Iterate through the gates.
					for (unsigned gate = 0; gate < ci.GateCount; gate++)
					{
						TGateInfo& gi(ci.GateInfo[gate]);
						//
						double delay = gi.Delay;
						// Calculate the delay for the slave mode.
						if (gi.SlavedTo >= 0)
						{
							delay += ci.GateInfo[gi.SlavedTo].PeakTof - TOF_OFFSET;
						}
						else if (gi.SlavedTo == -2)
						{
							delay += ci.GateInfo[0].Delay;
						}
						//
						FillDataBuffer
							(
								gi.CopyBuf,
								gi.Range,
								1,
								8,
								pow(10, ci.Gain / 20.0),
								ci.Sweep[0] - delay + ci.IfPos + 100.0,
								ci.Sweep[1] - delay + ci.IfPos + 250.0,
								ci.Sweep[2] - delay + ci.IfPos + 350.0
							);
						//
						switch (gi.MethodId)
						{
							case MID_PEAK:
							{
								gi.PeakFound = PeakNormal(gi.Polarity, gi.Threshold, gi.Range, (uint8_t*) gi.CopyBuf.data(), gi.PeakTof,gi.PeakAmp);
								//SF_RTTI_NOTIFY(DO_CLOG, "Gate: " << gate << " Tof: " << gi.PeakTof << " Amp: " << gi.PeakAmp)
								// Correct found peak with delay and result offset.
								gi.PeakTof += gi.Delay + TOF_OFFSET;
/*
							//
							// List of frequencies and amplitude combinations.
							struct
							{
								double Amp;
								double Freq;
							}
							static comp[] =
							{
								{1.0, 1.0},
				//        {0.4, 2},
				//        {1.0, 10},
				//        {0.2, 20},
				//        {0.1, 50}
							};
							//
							long ti = ci.SyncCounter % ci.PopDivider;
							int count = sizeof(comp) / sizeof(comp[0]);
							double val = 0.0;
							for (int k = 0; k < count; k++)
								val += 0.5 * comp[k].Amp *
									sin(calc_offset(ti, 0L, ci.PopDivider, 2.0 * M_PI * comp[k].Freq, true));
							// Apply the gain on the amplitude.
							val *= pow(10, (ci.Gain - 40.0) / 20.0);
							//
							gi.PeakAmp = calc_offset(val, -0.5, 0.5, 255, true);
*/
								/*
								if ((ci.SyncCounter) % ci.PopDivider == 0)
									gi.PeakTof = gi.Range + gi.Delay;
								*/
								/*
								if (ci.SyncCounter)
								{
									if ((ci.SyncCounter) % (ci.PopDivider*2) == 0)
										gi.PeakAmp = 200;
									else
										gi.PeakAmp = 127;
									//
									gi.PeakAmp += (ci.SyncCounter % ci.PopDivider) * 4;
								}
								else
									gi.PeakAmp = 255;
								*/
								//
								callResultHook(MAKE_ID(channel, gate, RID_PEAK_AMP));
								callResultHook(MAKE_ID(channel, gate, RID_PEAK_TOF));
								// Update the gate amplitude value.
								if (gate)
								{
									callParamHook(MAKE_ID(channel, gate, PID_GATE_AMP));
									callParamHook(MAKE_ID(channel, gate, PID_GATE_TOF));
								}
								break;
							}

							case MID_COPY:
								callResultHook(MAKE_ID(channel, gate, RID_COPY));
								break;
						}
					}
					// Increment the sync counter.
					FChannelInfo[channel].SyncCounter++;
				}
			}
			// Check if the single shot has finished.
			if (SingleShotTimer)
			{
				// Prevent reentry.
				SingleShotTimer.disable();
				// Update the client's parameter.
				callParamHook(MAKE_ID(NO_CHANNEL, NO_GATE, PID_ONESHOTSTATE));
			}
		}
	}
	// Allow reentry again.
	sentry = false;
	//
	return true;
}

double FormWave
	(
		double x,     // x-position
		double delta, // tooth width
		double slope, // steepness of the form
		double freq   // amount of waves in the delta.
	)
{
	double value = 0.0;
	double xd2 = delta / 2;
	if (x <= xd2 && x >= -xd2)
	{
		value = 0.0;
		//--------------------------------
		// Saw tooth wave.
		//value = (fabs(-x / xd2) - 1.0);
		//--------------------------------
		// exponential wave.
		//const shift = 0.4;
		//value = exp(slope * ((-fabs(x/xd2) + shift) - shift));
		// Remove starting offset.
		//value -= exp(slope * (-1.0 + shift));
		//--------------------------------
		const int shift = 1;
		value = pow(-fabs(x / xd2) + shift, slope);
		//--------------------------------
		// Super seed cosin wave form on top of other form.
		value *= -cos(2 * M_PI * freq * (x / delta));
		//--------------------------------
	}
	return value * -1;
}

void FillDataBuffer
	(
		DynamicBuffer buf,
		unsigned width,
		unsigned wordSize,
		unsigned bits,
		double gain,
		int delay1,
		int delay2,
		int delay3
	)
{
	uint32_t amplitude = (1 << bits) - 1;
	buf.resize(wordSize * width);
	//
	double delta = 75;
	double slope = 2.9;
	double freq = 4;
	// Give the gain an of set of -40 dB.
	gain /= 100.0;
	//
	for (int i = 0; i < (int) width; i++)
	{
		double value = FormWave(i - delay1, delta, slope, freq);
		value -= FormWave(i - delay2, delta, slope - 0.3, freq);
		value -= FormWave(i - delay3, delta, slope - 0.6, freq);
		// Add some noise dependent on the gain starting with 2%.
		value += 0.02 * double(rand()) / double(RAND_MAX);//  (gain*gain/10.0)
		value *= gain;
		// Clip after multiplying.
		uint32_t amp = calculateOffset(clip(value, -1.0, 1.0), -1.0, 1.0, amplitude, true);
		//
		if (sizeof(uint32_t) == wordSize)
		{
			*(uint32_t*) &buf[i * wordSize] = amp;
		}
		else if (sizeof(uint16_t) == wordSize)
		{
			*(uint16_t*) &buf[i * wordSize] = (uint16_t) amp;
		}
		else if (sizeof(uint8_t) == wordSize)
		{
			*(uint8_t*) &buf[i * wordSize] = (uint8_t) amp;
		}
	}
}

//
// Simple method implementation:
//
bool PeakNormal
	(
		int polarity, // <0 = neg, 0= full, >0 = pos
		unsigned threshold,
		unsigned count,
		const unsigned char* values,
		unsigned& _peakidx,
		unsigned& _peakval
	)
{
	// Set impossible value to detect if a peak was found at the end.
	unsigned peakidx = UINT_MAX;
	unsigned peakval = 0;
	unsigned peaksame = 0;
	//We are looking for a negative peak
	if (polarity < 0)
	{
		peakval = 255;
		for (unsigned i = 0; i < count; i++)
		{
			volatile unsigned val = values[i];
			// Discard everything above threshold value or greater than current peak
			if ((val <= threshold) && (val <= peakval))
			{
				// Count the amount of peak which are the same.
				if (val == peakval)
				{
					peaksame++;
				}
				else
				{
					peaksame = 1;
				}
				//
				peakval = val;
				peakidx = i;
			}
		}
	}
	else if (polarity > 0)
	{
		peakval = 0;
		for (unsigned i = 0; i < count; i++)
		{
			volatile unsigned val = values[i];
			// Discard everything below threshold value or smaller than current peak
			if ((val >= threshold) && (val >= peakval))
			{
				// Count the amount of peak which are the same.
				if (val == peakval)
				{
					peaksame++;
				}
				else
				{
					peaksame = 1;
				}
				// Assign index and value.
				peakval = val;
				peakidx = i;
			}
		}
	}
	else // (polarity == 0)
	{
		peakval = 0;
		for (unsigned i = 0; i < count; i++)
		{
			// Rectify samples before comparison.
			volatile int val = values[i];
			val = abs(val - 127);
			//
			if ((val >= threshold) && (val >= peakval))
			{ // Count the amount of peak which are the same.
				if (val == peakval)
				{
					peaksame++;
				}
				else
				{
					peaksame = 1;
				}
				// Assign index and value.
				peakval = val;
				peakidx = i;
			}
		}
	}
	// Check if the index was set which indicates that a peak was found.
	if (peakidx != UINT_MAX)
	{
		_peakidx = peakidx - peaksame / 2;
		_peakval = values[peakidx];
		return true;
	}
	else
	{
		_peakidx = 0;
		if (polarity < 0)
		{
			_peakval = -128;
		}
		else if (polarity > 0)
		{
			_peakval = 127;
		}
		else
		{
			_peakval = 127;
		}
		return false;
	}
}

}
