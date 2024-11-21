#include "GmiEmulator.h"
#include "misc/gen/IniProfile.h"
#include <gmi/iface/GmiUtils.h>

namespace sf
{

using gmi::EAxisLocation;
using gmi::EAxisMode;
using gmi::EMoveConCmd;
using gmi::EMovePosCmd;
using gmi::EMoveVelCmd;
using gmi::EParam;
using gmi::EResult;
using gmi::TBufferInfo;
using gmi::TController;
using gmi::TIdList;
using gmi::TParamInfo;
using gmi::TParamState;
using gmi::TResultInfo;

//
// Parameter ID manipulation macro's
//
#define MAKE_ID(axis, param) (((axis) << 16) | (param))
// Strips the Axis index number from the ID.
#define GETAXIS(id) (((id) & 0xFF0000) >> 16)
// Strips the parameter index number from the ID.
#define GETINDEX(id) ((id) & 0xFFFF)
// Define for a no axis number.
#define NO_AXIS 0xFF
// Macro's for mapping parameter enumerates PID numbers.
#define PID_MAP(n) (n)
#define PID_AXIS_MAP(n) ((n) & ~gmi::mpAXIS_MASK)
// Macro's for added PID number generation
#define PID_ADDED(n) (gmi::mpLAST + (n))
#define PID_AXIS_ADDED(n) ((gmi::mpAXIS_LAST + (n)) & ~gmi::mpAXIS_MASK)
//
// Map default base parameters to our own ID's.
//
#define PID_VERSION PID_MAP(gmi::mpVERSION)
#define PID_MOVEPOS PID_MAP(gmi::mpMOVEPOS)
#define PID_MOVEVEL PID_MAP(gmi::mpMOVEVEL)
#define PID_MOVECON PID_MAP(gmi::mpMOVECON)
#define PID_JOYSTICK PID_MAP(gmi::mpJOYSTICK)
#define PID_JOYSTICK_AXIS PID_MAP(gmi::mpJOYSTICK_AXIS)
#define PID_POP_AXIS PID_MAP(gmi::mpPOP_AXIS)
#define PID_TRIGGER_AXIS PID_MAP(gmi::mpTRIGGER_AXIS)
#define PID_TRIGGER_DENSITY PID_MAP(gmi::mpTRIGGER_DENSITY)
#define PID_TRIGGER_MODE PID_MAP(gmi::mpTRIGGER_MODE)
#define PID_TRIGGER_FREQ PID_MAP(gmi::mpTRIGGER_FREQ)
#define PID_TRIGGER_ENABLE PID_MAP(gmi::mpTRIGGER_ENABLE)
#define PID_CHUCKJAW PID_MAP(gmi::mpCHUCKJAW)
#define PID_ERROR PID_MAP(gmi::mpERROR)
#define PID_ERROR_MESSAGE PID_MAP(gmi::mpERROR_MESSAGE)
//
#define PID_AXIS_NAME PID_AXIS_MAP(gmi::mpAXIS_NAME)
#define PID_AXIS_MODE PID_AXIS_MAP(gmi::mpAXIS_MODE)
#define PID_AXIS_ACCURACY PID_AXIS_MAP(gmi::mpAXIS_ACCURACY)
#define PID_AXIS_RESOLUTION PID_AXIS_MAP(gmi::mpAXIS_RESOLUTION)
#define PID_AXIS_MIN_POS PID_AXIS_MAP(gmi::mpAXIS_MIN_POS)
#define PID_AXIS_MAX_POS PID_AXIS_MAP(gmi::mpAXIS_MAX_POS)
#define PID_AXIS_MAX_VEL PID_AXIS_MAP(gmi::mpAXIS_MAX_VEL)
#define PID_AXIS_MAX_ACC PID_AXIS_MAP(gmi::mpAXIS_MAX_ACC)
#define PID_AXIS_OFS_POS PID_AXIS_MAP(gmi::mpAXIS_OFS_POS)
#define PID_AXIS_CUR_POS PID_AXIS_MAP(gmi::mpAXIS_CUR_POS)
#define PID_AXIS_CUR_VEL PID_AXIS_MAP(gmi::mpAXIS_CUR_VEL)
#define PID_AXIS_TRG_POS PID_AXIS_MAP(gmi::mpAXIS_TRG_POS)
#define PID_AXIS_TRG_VEL PID_AXIS_MAP(gmi::mpAXIS_TRG_VEL)
#define PID_AXIS_TRG_ACC PID_AXIS_MAP(gmi::mpAXIS_TRG_ACC)

#define PID_DEBUG PID_AXIS_ADDED(0)
#define PID_POPEVENT PID_AXIS_ADDED(1)
#define PID_FW_REVISION PID_ADDED(2)
#define PID_AXIS_ROUND PID_AXIS_ADDED(1)

// Default values for parameters.
#define DEF_AXIS_ACCURACY 1.0E-4
#define DEF_AXIS_RES 1E5
#define DEF_AXIS_MAX_VEL 0.1
#define DEF_AXIS_MAX_ACC 0.5
#define DEF_AXIS_MIN_POS -1.0
#define DEF_AXIS_MAX_POS 6.0

//
// Limitations on a real Nipscan
//  X =  -22 to  +40 inch =   -56 to  +101 cm
//  Y =  -22 to  +22 inch =   -56 to   +56 cm
//  Z =    0 to  +40 inch =     0 to  +101 cm
//  A = -200 to +200 Deg  =  -3.5 to  +3.5 rad
//  B = -135 to +135 Deg  = -2.35 to -2.35 rad
//
//
MotionEmulator::TAxis::TAxis(MotionEmulator* me, EAxisLocation al)
	: TInherited(me, al)
	, Controller(me)
	, Mode(gmi::amDISABLED)
{
	// Disable move timers.
	MovePosTimer.disable();
	MoveVelTimer.disable();
	MoveConTimer.disable();
	// Add the axes to the axis list.
	me->AxisList.add(this);
	TrgPos = 0.0;
	TrgVel = 0.0;
	TrgAcc = 0.0;
	CurPos = 0.0;
	CurVel = 0.0;
	OfsPos = 0.0;
	StartVel = 0.0;
	Accuracy = DEF_AXIS_ACCURACY;
	Round = DEF_AXIS_ACCURACY;
	Resolution = DEF_AXIS_RES;
	//
	MinPos = DEF_AXIS_MIN_POS;
	MaxPos = DEF_AXIS_MAX_POS;
	MaxVel = DEF_AXIS_MAX_VEL;
	MaxAcc = DEF_AXIS_MAX_ACC;
}

MotionEmulator::TAxis::~TAxis()
{
}

MotionEmulator::MotionEmulator(const Parameters& params)
	: TController(params)
	, FSustainEntry(this, &MotionEmulator::Sustain)
	, FHomingDelay(3000)
	, FError(0)
	, FChuckJaw(0)
	, FCurMovePos(gmi::mpcCOMPLETE)
	, FCurMoveVel(gmi::mvcCOMPLETE)
	, FCurMoveCon(gmi::mccCOMPLETE)
	, FlagInitialized(false)
	, FJoystickState(0)
	, FTriggerAxis(gmi::alNA)
	, FTriggerDensity(0.0)
	, FTriggerFreq(1.0)
	, FTriggerEnable(true)
	, FTriggerMode(0)
	, FPopAxis(gmi::alNA)
	, FPopEventCount(0)
	, X_Axis(this, gmi::alX)
	, Y_Axis(this, gmi::alY)
	, Z_Axis(this, gmi::alZ)
	, A_Axis(this, gmi::alA)
	, B_Axis(this, gmi::alB)
	, C_Axis(this, gmi::alC)
	//,D_Axis(this, gmi::alD)
	, E_Axis(this, gmi::alE)
{
	// Do not home immediately.
	FHomingDelay.disable();
	// Configure axis movement limitations.
	X_Axis.FMovements << gmi::amLINEAR << gmi::amLIMITED;
	Y_Axis.FMovements << gmi::amLINEAR << gmi::amLIMITED;
	Z_Axis.FMovements << gmi::amLINEAR << gmi::amLIMITED;
	A_Axis.FMovements << gmi::amRADIAL << gmi::amLIMITED;
	B_Axis.FMovements << gmi::amRADIAL << gmi::amLIMITED;
	C_Axis.FMovements << gmi::amRADIAL;
	//  D_Axis.FMovements << gmi::amRADIAL;
	E_Axis.FMovements << gmi::amLINEAR << gmi::amLIMITED;
}

MotionEmulator::~MotionEmulator()
{
	// Signal the interface destruction of this instance.
	// Gives interface time to perform some actions.
	Destroy();
}

bool MotionEmulator::DoInitialize(bool init)
{
	if (init)
		SF_RTTI_NOTIFY(DO_DEFAULT, "Initialized Perfectly...")
	else
		SF_RTTI_NOTIFY(DO_DEFAULT, "Uninitialized Perfectly...")
	// Save of load last positions from profile.
	IniProfile profile("Last Positions", GetProfilePath().c_str());
	for (unsigned i = 0; i < AxisList.count(); i++)
		if (init)
			AxisList[i]->CurPos = toNumber<double>(profile.getString(AxisList[i]->GetName(), toString(0.0)));
		else
			profile.setString(AxisList[i]->GetName(), toString(AxisList[i]->CurPos));
	return true;
}

bool MotionEmulator::DoHomeAxes(bool skip)
{
	// When skip is true do not delay for homing.
	if (skip)
		SetStatus(csREADY);
	else
	{
		// Set status for homing.
		SetStatus(csHOMING);
		FHomingDelay.reset();
	}
	//
	return true;
}

void MotionEmulator::AddPropertyPages(PropertySheetDialog* sheet)
{
	TInherited::AddPropertyPages(sheet);
}

bool MotionEmulator::HandleParam(
	int id,
	TParamInfo* info,
	const Value* setval,
	Value* getval
)
{
	// Return value for when setting a value.
	// To prevent return statements which skip getval values from being set.
	bool retval = true;
	// Check if info must be filled in.
	if (info)
	{
		// Initialize the info structure.
		info->Init();
		// Archive flag is default.
		info->Flags = gmi::pfARCHIVE;
		// Default for all for this implementation.
		info->Index = GETINDEX(id);
		// Retrieve the gate number form the param id.
		info->Axis = (GETAXIS(id) == NO_AXIS) ? UINT_MAX : GETAXIS(id);
	}
	// ??? Do not yet ???
	//if (??)
	{
		// Switch bwtween axis and non axis parameter ID's.
		if (GETAXIS(id) == NO_AXIS)
		{
			switch (GETINDEX(id))
			{
				default:
					// Report the id was not found.
					SF_RTTI_NOTIFY(DO_DEFAULT, "Param ID " << sf::stringf("0x%lX", id) << " does not exists!")
					return false;

				case PID_VERSION:
					if (getval)
						getval->set("XY Emulator 0.01");
					if (info)
					{
						info->Id = id;
						info->Name = "Version";
						info->Description += "Version of software hardware and driver";
						info->Default.set("");
						info->Round.set(40);
						info->Flags |= /*pfREADONLY |*/ gmi::pfEXPORT | gmi::pfWRITEABLE;
					}
					break;

				case PID_FW_REVISION:
					if (setval)
						FFirmwareRevision = setval->getString();
					if (getval)
						getval->set(FFirmwareRevision);
					if (info)
					{
						info->Id = id;
						info->Name = "Firmware Revision";
						info->Description += "Identification of revision of embedded application";
						info->Default.set("");
						info->Round.set(60);
						info->Flags |= gmi::pfREADONLY | gmi::pfEXPORT;
					}
					break;

				case PID_POPEVENT:
					if (setval)
						SendPopEvent();
					if (getval)
						getval->set(0);
					if (info)
					{
						info->Id = id;
						info->Name = "Pop Event";
						info->Unit = "!";
						info->Description += stringf("Triggers a pop event on the interface manually.");
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->States.add(TParamState("POP Event", Value(0)));
						info->States.add(TParamState("POP Event", Value(1)));
						info->Flags &= ~gmi::pfARCHIVE;
						info->Flags |= gmi::pfEXPORT | gmi::pfWRITEABLE;
					}
					break;

				case PID_POP_AXIS:
					if (setval)
						FPopAxis = (EAxisLocation) setval->getInteger();
					if (getval)
						getval->set(FPopAxis);
					if (info)
					{
						info->Id = id;
						info->Name += "Pop Axis";
						info->Unit = "!";
						info->Description += "Axis selection for the position orientation pulse.";
						info->Default.set(gmi::alC);
						info->Round.set(1);
						info->Minimum.set(gmi::alNA);
						info->Maximum.set(gmi::alLAST_ENTRY - 1);
						for (int loc = gmi::alNA; loc < gmi::alLAST_ENTRY; loc++)
						{
							std::string name;
							if (loc == gmi::alNA)
								name = "None";
							else
								name = GetAxis(loc).GetName() + std::string("-Axis");
							info->States.add(TParamState(name, Value(loc)));
						}
						// Cannot be enabled during movement.
						info->Flags |= gmi::pfEXPORT | gmi::pfMOVEVEL | gmi::pfMOVEPOS;
					}
					break;

				case PID_MOVEPOS:
					if (setval)
					{
						if (FDebug)
							SF_RTTI_NOTIFY(DO_DEFAULT, "Debug capture event!")
						EMovePosCmd mpc = (EMovePosCmd) setval->getInteger();
						if (mpc != FCurMovePos)
							DoMovePos(mpc);
					}
					if (getval)
						getval->set(FCurMovePos);
					if (info)
					{
						info->Id = id;
						info->Name = "Move|Position";
						info->Unit = "!";
						info->Description += stringf("Starts movement for all axes in position mode.");
						info->Default.set(gmi::mpcCOMPLETE);
						info->Round.set(1);
						info->Minimum.set(gmi::mpcABORT);
						info->Maximum.set(gmi::mpcCURVE);
						info->States.add(TParamState("Complete", Value(gmi::mpcCOMPLETE)));
						info->States.add(TParamState("Abort", Value(gmi::mpcABORT)));
						info->States.add(TParamState("Stop", Value(gmi::mpcSTOP)));
						info->States.add(TParamState("Position", Value(gmi::mpcPOSITION)));
						info->States.add(TParamState("Curve", Value(gmi::mpcCURVE)));
						info->Flags |= gmi::pfEXPORT;
						info->Flags &= ~gmi::pfARCHIVE;
					}
					break;

				case PID_MOVEVEL:
					if (setval)
					{
						EMoveVelCmd mvc = (EMoveVelCmd) setval->getInteger();
						if (mvc != FCurMoveVel)
							DoMoveVel(mvc);
					}
					if (getval)
						getval->set(FCurMoveVel);
					if (info)
					{
						info->Id = id;
						info->Name = "Move|Velocity";
						info->Unit = "!";
						info->Description += stringf("Starts movement for all axes in velocity mode.");
						info->Default.set(gmi::mvcCOMPLETE);
						info->Round.set(1);
						info->Minimum.set(gmi::mvcABORT);
						info->Maximum.set(gmi::mvcON);
						info->States.add(TParamState("Complete", Value(gmi::mvcCOMPLETE)));
						info->States.add(TParamState("Abort", Value(gmi::mvcABORT)));
						info->States.add(TParamState("Stop", Value(gmi::mvcSTOP)));
						info->States.add(TParamState("On", Value(gmi::mvcON)));
						info->Flags |= gmi::pfEXPORT;
						info->Flags &= ~gmi::pfARCHIVE;
					}
					break;

				case PID_MOVECON:
					if (setval)
					{
						EMoveConCmd mcc = (EMoveConCmd) setval->getInteger();
						if (mcc != FCurMoveCon)
							DoMoveCon(mcc);
					}
					if (getval)
						getval->set(FCurMoveCon);
					if (info)
					{
						info->Id = id;
						info->Name = "Move|Continue";
						info->Unit = "!";
						info->Description += "Starts movement for all axes in continueuos mode.";
						info->Default.set(gmi::mccCOMPLETE);
						info->Round.set(1);
						info->Minimum.set(gmi::mccABORT);
						info->Maximum.set(gmi::mccON);
						info->States.add(TParamState("Complete", Value(gmi::mccCOMPLETE)));
						info->States.add(TParamState("Abort", Value(gmi::mccABORT)));
						info->States.add(TParamState("Stop", Value(gmi::mccSTOP)));
						info->States.add(TParamState("On", Value(gmi::mccON)));
						info->Flags |= gmi::pfEXPORT;
						info->Flags &= ~gmi::pfARCHIVE;
					}
					break;

				case PID_JOYSTICK:
					if (setval)
					{
						// Prevent looping
						if (FJoystickState != setval->getInteger())
						{
							switch (setval->getInteger())
							{
								default:
									FJoystickState = setval->getInteger();
									break;

								case gmi::jscSTOP:
									// When already Off do nothing.
									if (FJoystickState != gmi::jscOFF)
										FJoystickState = setval->getInteger();
									break;
							}
						}
					}
					if (getval)
						getval->set(FJoystickState);
					if (info)
					{
						info->Id = id;
						info->Name = "Remote|State";
						info->Unit = "!";
						info->Description += "Activates or deactivates the remote. Off is status only.";
						info->Default.set(gmi::jscOFF);
						info->Round.set(1);
						info->Minimum.set(gmi::jscINHIBIT);
						info->Maximum.set(gmi::jscON);
						info->States.add(TParamState("Inhibit", Value(gmi::jscINHIBIT)));
						info->States.add(TParamState("Off", Value(gmi::jscOFF)));
						info->States.add(TParamState("Stop", Value(gmi::jscSTOP)));
						info->States.add(TParamState("On", Value(gmi::jscON)));
						info->Flags |= gmi::pfEXPORT;
					}
					break;

				case PID_TRIGGER_AXIS:
					if (setval)
						FTriggerAxis = setval->getInteger();
					if (getval)
						getval->set(FTriggerAxis);
					if (info)
					{
						info->Id = id;
						info->Name = "Trigger|Axis";
						info->Unit = "!";
						info->Description += "Selection of the axis responsible for the trigger of the measurement device.";
						info->Default.set(gmi::alNA);
						info->Round.set(1);
						info->Minimum.set(gmi::alNA);
						info->Maximum.set(gmi::alLAST_ENTRY - 1);
						for (int loc = gmi::alNA; loc < gmi::alLAST_ENTRY; loc++)
						{
							std::string name = GetAxis(loc).GetName();
							if (loc == gmi::alNA)
								name = "None";
							info->States.add(TParamState(name, Value(loc)));
						}
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT | gmi::pfEFFECTPARAM;
					}
					break;

				case PID_TRIGGER_DENSITY:
					if (setval)
						FTriggerDensity = setval->getFloat();
					if (getval)
						getval->set(FTriggerDensity);
					if (info)
					{
						info->Id = id;
						info->Name = "Trigger|Density";
						gmi::AxisMovements mvs = GetAxis(FTriggerAxis).GetMovements();
						if (mvs.has(gmi::amRADIAL))
							info->Unit = "rad";
						else if (mvs.has(gmi::amLINEAR))
							info->Unit = "m";
						else
							info->Unit = "?";
						info->Description += "Density of the trigger axis.";
						info->Round.set(1E-5);
						info->Default.set(info->Round);
						info->Minimum.set(info->Round);
						info->Maximum.set(info->Round.getFloat() * (1 << 16));
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT;
					}
					break;

				case PID_TRIGGER_MODE:
					if (setval)
						FTriggerMode = setval->getInteger();
					if (getval)
						getval->set(FTriggerMode);
					if (info)
					{
						info->Id = id;
						info->Name = "Trigger|Mode";
						info->Unit = "!";
						info->Description += "Trigger or Sync signal internal/external selection.";
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->States.add(TParamState("Internal", Value(1)));
						info->States.add(TParamState("External", Value(0)));
						info->Flags |= gmi::pfEXPORT | gmi::pfEFFECTPARAM;
					}
					break;

				case PID_TRIGGER_ENABLE:
					if (setval)
						FTriggerEnable = setval->getInteger();
					if (getval)
						getval->set(FTriggerEnable);
					if (info)
					{
						info->Id = id;
						info->Name += "Trigger|Enable";
						info->Unit = "!";
						info->Description += "Enables trigger or sync signal.";
						info->Default.set(1);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->States.add(TParamState("Disabled", Value(false)));
						info->States.add(TParamState("Enabled", Value(true)));
						info->Flags |= gmi::pfEXPORT;
					}
					break;

				case PID_TRIGGER_FREQ: {
					// Check if the value must be set.
					if (setval)
					{
						FTriggerFreq = setval->getInteger();
						//CallParamHook(MAKE_ID(NO_AXIS, PID_TIMEUNIT));
					}
					// Check if the axis exists in the implementation.
					if (getval)
						getval->set(FTriggerFreq);
					if (info)
					{
						info->Id = id;
						info->Name += "Trigger|RepRate";
						info->Unit = "Hz";
						info->Description += "Repetition Rate for the internal trigger.";
						info->Round.set(1.0);
						info->Default.set(200.0);
						info->Minimum.set(1);
						info->Maximum.set(10E3);
						info->Flags |= gmi::pfEXPORT | gmi::pfWRITEABLE;
					}
				} break;

				case PID_CHUCKJAW:
					if (setval)
						FChuckJaw = setval->getInteger();
					if (getval)
						getval->set(FChuckJaw);
					if (info)
					{
						info->Id = id;
						info->Name = "ChuckJaw";
						info->Unit = "!";
						info->Description += "Open and close the chuckjaw.";
						info->Default.set(gmi::cjOFF);
						info->Round.set(1);
						info->Minimum.set(gmi::cjOFF);
						info->Maximum.set(gmi::cjCLOSE);
						info->States.add(TParamState("Off", Value(gmi::cjOFF)));
						info->States.add(TParamState("Open", Value(gmi::cjOPEN)));
						info->States.add(TParamState("Close", Value(gmi::cjCLOSE)));
						info->Flags |= gmi::pfEXPORT;
					}
					break;

				case PID_ERROR:
					if (setval)
					{
						FError = setval->getInteger();
						// Make the client update the error message.
						CallParamHook(MAKE_ID(NO_AXIS, PID_ERROR_MESSAGE));
					}
					if (getval)
						getval->set(FError);
					if (info)
					{
						info->Id = id;
						info->Name = "Error";
						info->Unit = "!";
						info->Description += "Reports errors.";
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->States.add(TParamState("Ok", Value(0)));
						info->States.add(TParamState("Error", Value(1)));
						info->Flags = gmi::pfEXPORT | gmi::pfWRITEABLE;
					}
					break;

				case PID_ERROR_MESSAGE:
					if (getval)
					{
						if (FError)
						{
							std::string errtxt;
							for (int i = 1; i < 10; i++)
								errtxt += stringf("Error message line %i\n", i);
							getval->set(errtxt);
						}
						else
							getval->set("");
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Error Message";
						info->Unit = "!";
						info->Description += "Reports error messages.";
						info->Default.set("");
						info->Round.set(255);
						info->Flags |= gmi::pfREADONLY | gmi::pfEXPORT;
					}
					break;

				case PID_DEBUG:
					if (setval)
					{
						FDebug = setval->getInteger();
						DoMovePos(gmi::mpcABORT);
					}
					if (getval)
						getval->set(FDebug);
					if (info)
					{
						info->Id = id;
						info->Name = "Debug";
						info->Unit = "!";
						info->Description += stringf("Triggers a debug event.");
						info->Default.set(0);
						info->Round.set(1);
						info->Minimum.set(0);
						info->Maximum.set(1);
						info->States.add(TParamState("Ready", Value(0)));
						info->States.add(TParamState("Debug", Value(1)));
						info->Flags = gmi::pfEXPORT | gmi::pfWRITEABLE;
					}
					break;
			}
		}
		// Check if the axis is implemented.
		else if (GetAxis(GETAXIS(id)).GetLocation() != gmi::alNA)
		{
			// Create temporary easy to use reference.
			TAxis& axis = dynamic_cast<TAxis&>(GetAxis(GETAXIS(id)));
			// Preset the axis related flag
			if (info)
			{
				info->Flags |= gmi::pfAXIS;
			}
			int index = GETINDEX(id);
			switch (index)
			{
				default:
					// Report the id was not found.
					SF_RTTI_NOTIFY(DO_DEFAULT, "Axis '" << axis.GetName() << "' Param ID " << stringf("0x%lX", id) << " does not exists!")
					return false;

				case PID_AXIS_NAME:
					if (getval)
					{
						getval->set(axis.GetName());
					}
					break;

				case PID_AXIS_OFS_POS: {
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (FCurMovePos || FCurMoveVel || FCurMoveCon)
							retval = false;
						else
						{
							axis.OfsPos = setval->getFloat();
							CallParamHook(MAKE_ID(GETAXIS(id), PID_AXIS_TRG_POS));
							CallParamHook(MAKE_ID(GETAXIS(id), PID_AXIS_CUR_POS));
						}
					}
					if (getval)
						getval->set(axis.OfsPos);
					if (info)
					{
						info->Id = id;
						info->Name = "Offset";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Position offset of %s-Axis. It shifts all position parameters of this axis.", axis.GetName());
						info->Default.set(0.0);
						info->Round.set(axis.Round);
						info->Minimum.set(-10.0);
						info->Maximum.set(10.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfEXPORT;
					}
				} break;

				case PID_AXIS_CUR_POS: {
					bool radial = axis.GetMovements() == (gmi::AxisMovements() << gmi::amRADIAL);
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (FCurMovePos || FCurMoveVel || FCurMoveCon || radial)
							retval = false;
						else
						{
							axis.OfsPos = setval->getFloat() - axis.CurPos;
							CallParamHook(MAKE_ID(GETAXIS(id), PID_AXIS_TRG_POS));
							CallParamHook(MAKE_ID(GETAXIS(id), PID_AXIS_OFS_POS));
						}
					}
					if (getval)
					{
						double val = axis.CurPos + axis.OfsPos;
						if (radial)
						{
							val = fmod(val, 2.0 * numbers::pi_v<double>);
							if (val < 0.0)
								val += 2.0 * numbers::pi_v<double>;
						}
						getval->set(val);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Current|Position";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Current position of %s-Axis.", axis.GetName());
						info->Default.set(0.0);
						info->Round.set(axis.Round);
						// When an axis is radial the values are 0 to 2 PI radians.
						if (radial)
						{
							info->Minimum.set(0.0);
							info->Maximum.set(2.0 * numbers::pi_v<double>);
							info->Flags |= gmi::pfREADONLY;
						}
						else
						{
							info->Minimum.set(axis.MinPos + axis.OfsPos);
							info->Maximum.set(axis.MaxPos + axis.OfsPos);
						}
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT | gmi::pfALIAS;
					}
				} break;

				case PID_AXIS_TRG_POS: {
					bool radial = axis.GetMovements() == (gmi::AxisMovements() << gmi::amRADIAL);
					if (setval)
					{
						if (radial)
						{
							// Get the angle position from the current axis position.
							double cur_angle = fmod(axis.CurPos - axis.OfsPos, 2.0 * numbers::pi_v<double>);
							if (cur_angle < 0.0)
								cur_angle += 2.0 * numbers::pi_v<double>;
							// Accept only positive values.
							axis.TrgPos = setval->getFloat();
							axis.TrgPos = fmod(axis.TrgPos, 2.0 * numbers::pi_v<double>);
							if (axis.TrgPos < 0.0)
								axis.TrgPos += 2.0 * numbers::pi_v<double>;
							// Determine difference between the angles.
							double dif_angle = fmod(axis.TrgPos - cur_angle, 2.0 * numbers::pi_v<double>);
							// Calculate the current position of the axis before the change.
							double trg_pos = axis.CurPos;
							// Do no accept changes less than the set accuracy.
							if (std::fabs(dif_angle) > axis.Accuracy)
								trg_pos += dif_angle;
							//
							axis.TrgPos = trg_pos;
						}
						else
						{
							axis.TrgPos = setval->getFloat() - axis.OfsPos;
						}
					}
					if (getval)
					{
						if (radial)
							getval->set(std::fmod(axis.TrgPos - 2.0 * numbers::pi_v<double> + axis.OfsPos, 2.0 * numbers::pi_v<double>));
						else
							getval->set(axis.TrgPos + axis.OfsPos);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Target|Position";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Target position of %s-Axis.", axis.GetName());
						info->Default.set(0.0);
						info->Round.set(axis.Round);
						// Set limits if there are any.
						if (axis.GetMovements().contains(gmi::amLIMITED))
						{
							info->Minimum.set(axis.MinPos + axis.OfsPos);
							info->Maximum.set(axis.MaxPos + axis.OfsPos);
						}
						else
						{
							if (radial)
							{
								info->Minimum.set(0.0);
								info->Maximum.set(2.0 * numbers::pi_v<double>);
							}
							else
							{
								info->Minimum.set(0.0);
								info->Maximum.set(0.0);
							}
						}
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT;
					}
				} break;

				case PID_AXIS_TRG_VEL:
					if (setval)
					{
						// IS the axis not disabled.
						if (axis.Mode != gmi::amDISABLED)
						{
							// Can only set the mode when controller is not moving.
							if (FCurMovePos && axis.Mode == gmi::amPOSITION)
								retval = false;
						}
						//
						if (retval)
						{
							// Set the velocity.
							axis.TrgVel = setval->getFloat();
							// When in velocity mode make it accelerate or decelerate.
							if (FCurMoveVel == gmi::mvcON)
								axis.DoMoveVel(gmi::mvcON);
							if (FCurMoveCon == gmi::mccON)
								axis.DoMoveCon(gmi::mccON);
						}
					}
					if (getval)
						getval->set(axis.TrgVel);
					if (info)
					{
						info->Id = id;
						info->Name = "Target|Velocity";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m/s" : "rad/s";
						info->Description += stringf("Target velocity of %s-Axis.", axis.GetName());
						info->Default.set(0.0);
						info->Round.set(axis.Round);
						info->Minimum.set(-axis.MaxVel);
						info->Maximum.set(axis.MaxVel);
						info->Flags |= gmi::pfMOVEPOS | gmi::pfEXPORT;
					}
					break;

				case PID_AXIS_TRG_ACC:
					if (setval)
					{
						// IS the axis not disabled.
						if (axis.Mode != gmi::amDISABLED)
						{
							// Can only set the mode when controller is not moving.
							if (FCurMovePos && axis.Mode == gmi::amPOSITION)
								retval = false;
							else if (FCurMoveVel && axis.Mode == gmi::amVELOCITY)
								retval = false;
						}
						//
						axis.TrgAcc = setval->getFloat();
					}
					if (getval)
						getval->set(axis.TrgAcc);
					if (info)
					{
						info->Id = id;
						info->Name = "Target|Acceleration";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m/s\xB2" : "rad/s\xB2";
						info->Description += stringf("Target acceleration of %s-Axis.", axis.GetName());
						info->Default.set(0.0);
						info->Round.set(axis.Round);
						info->Minimum.set(0);
						info->Maximum.set(axis.MaxAcc);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT;
					}
					break;

				case PID_AXIS_CUR_VEL:
					if (setval)
						axis.CurVel = setval->getFloat();
					if (getval)
						getval->set(axis.CurVel);
					if (info)
					{
						info->Id = id;
						info->Name = "Current|Velocity";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m/s" : "rad/s";
						info->Description += stringf("Current velocity of %s-Axis.", axis.GetName());
						info->Default.set(2.0);
						info->Round.set(axis.Round);
						info->Minimum.set(0.0);
						info->Maximum.set(axis.MaxVel);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT | gmi::pfREADONLY;
					}
					break;

				case PID_AXIS_MIN_POS: {
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (FCurMovePos || FCurMoveVel || FCurMoveCon)
							retval = false;
						else
							axis.MinPos = setval->getFloat();
					}
					if (getval)
						getval->set(axis.MinPos);
					if (info)
					{
						info->Id = id;
						info->Name = "Minimum|Position";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Maximum position of %s-Axis.", axis.GetName());
						info->Default.set(DEF_AXIS_MIN_POS);
						info->Round.set(axis.Round);
						info->Minimum.set(-10.0);
						info->Maximum.set(10.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
				} break;

				case PID_AXIS_MAX_POS: {
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (FCurMovePos || FCurMoveVel || FCurMoveCon)
							retval = false;
						else
							axis.MaxPos = setval->getFloat();
					}
					if (getval)
						getval->set(axis.MaxPos);
					if (info)
					{
						info->Id = id;
						info->Name = "Maximum|Position";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Position of %s-Axis.", axis.GetName());
						info->Default.set(DEF_AXIS_MAX_POS);
						info->Round.set(axis.Round);
						info->Minimum.set(-10.0);
						info->Maximum.set(10.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
				} break;

				case PID_AXIS_MAX_VEL:
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (FCurMovePos || FCurMoveVel || FCurMoveCon)
							retval = false;
						else
							axis.MaxVel = setval->getFloat();
					}
					if (getval)
						getval->set(axis.MaxVel);
					if (info)
					{
						info->Id = id;
						info->Name = "Maximum|Velocity";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m/s" : "rad/s";
						info->Description += stringf("Maximum velocity of %s-Axis.", axis.GetName());
						info->Default.set(DEF_AXIS_MAX_VEL);
						info->Round.set(axis.Round);
						info->Minimum.set(0.0);
						info->Maximum.set(2.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
					break;

				case PID_AXIS_MAX_ACC:
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (FCurMovePos || FCurMoveVel || FCurMoveCon)
							retval = false;
						else
							axis.MaxAcc = setval->getFloat();
					}
					if (getval)
						getval->set(axis.MaxAcc);
					if (info)
					{
						info->Id = id;
						info->Name = "Maximum|Acceleration";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m/s\xB2" : "rad/s\xB2";
						info->Description += stringf("Maximum acceleration of %s-Axis.", axis.GetName());
						info->Default.set(DEF_AXIS_MAX_ACC);
						info->Round.set(axis.Round);
						info->Minimum.set(0);
						info->Maximum.set(5.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
					break;

				case PID_AXIS_ACCURACY:
					if (setval)
						axis.Accuracy = setval->getFloat();
					if (getval)
						getval->set(axis.Accuracy);
					if (info)
					{
						info->Id = id;
						info->Name = "Accuracy";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Position and velocity accuracy of the axis.");
						info->Default.set(DEF_AXIS_ACCURACY);
						info->Round.set(1E-8);
						info->Minimum.set(1E-8);
						info->Maximum.set(1E-2);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
					break;

				case PID_AXIS_RESOLUTION:
					if (setval)
						axis.Resolution = setval->getFloat();
					if (getval)
						getval->set(axis.Resolution);
					if (info)
					{
						info->Id = id;
						info->Name = "Resolution";
						info->Unit = axis.FMovements.contains(gmi::amLINEAR) ? "p/m" : "p/rad";
						info->Description += stringf("Resolution transformation factor for %s-Axis.", axis.GetName());
						info->Default.set(DEF_AXIS_RES);
						info->Round.set(1E-8);
						info->Minimum.set(1E-8);
						info->Maximum.set(1E8);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfSYSTEM;
					}
					break;

				case PID_AXIS_ROUND:
					if (setval)
						axis.Round = setval->getFloat();
					if (getval)
						getval->set(axis.Round);
					if (info)
					{
						info->Id = id;
						info->Name = "Rounding";
						info->Unit = "x";
						info->Description += stringf("Rounding for %s-Axis parameters.", axis.GetName());
						info->Default.set(DEF_AXIS_ACCURACY);
						info->Round.set(1E-8);
						info->Minimum.set(1E-8);
						info->Maximum.set(1.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
					break;

				case PID_AXIS_MODE:
					if (setval)
					{
						EAxisMode newmode = (EAxisMode) setval->getInteger();
						// Can only set the mode according to some rules.
						if (!axis.CanModeChange(axis.Mode, newmode, !FCurMovePos, !FCurMoveVel, !FCurMoveCon))
							retval = false;
						else
							axis.Mode = newmode;
					}
					if (getval)
						getval->set(axis.Mode);
					if (info)
					{
						info->Id = id;
						info->Name = "Mode";
						info->Unit = "!";
						info->Description += stringf("Start movement using velocity position contineuous or home mode.");
						info->Default.set(gmi::amDISABLED);
						info->Round.set(1);
						info->Minimum.set(gmi::amDISABLED);
						info->Maximum.set(gmi::amCONTINUE);
						info->States.add(TParamState("Disabled", Value(gmi::amDISABLED)));
						info->States.add(TParamState("Position", Value(gmi::amPOSITION)));
						info->States.add(TParamState("Velocity", Value(gmi::amVELOCITY)));
						info->States.add(TParamState("Continue", Value(gmi::amCONTINUE)));
						info->States.add(TParamState("Home", Value(gmi::amHOME)));
						info->Flags |= gmi::pfMOVE | gmi::pfEXPORT;
					}
					break;
			}
		}
	}
	// Check if at least the id is filled in.
	if (info && info->Id == 0)
		return false;
	// Found it so return true.
	return retval;
}

bool MotionEmulator::EnumParamIds(TIdList& ids) const
{
	// Add interface parameters.
	ids.add(MAKE_ID(NO_AXIS, PID_DEBUG));
	ids.add(MAKE_ID(NO_AXIS, PID_VERSION));
	ids.add(MAKE_ID(NO_AXIS, PID_FW_REVISION));
	ids.add(MAKE_ID(NO_AXIS, PID_MOVEPOS));
	ids.add(MAKE_ID(NO_AXIS, PID_MOVEVEL));
	ids.add(MAKE_ID(NO_AXIS, PID_MOVECON));
	ids.add(MAKE_ID(NO_AXIS, PID_JOYSTICK));
	ids.add(MAKE_ID(NO_AXIS, PID_TRIGGER_AXIS));
	ids.add(MAKE_ID(NO_AXIS, PID_TRIGGER_DENSITY));
	ids.add(MAKE_ID(NO_AXIS, PID_TRIGGER_MODE));
	ids.add(MAKE_ID(NO_AXIS, PID_TRIGGER_MODE));
	ids.add(MAKE_ID(NO_AXIS, PID_TRIGGER_FREQ));
	ids.add(MAKE_ID(NO_AXIS, PID_TRIGGER_ENABLE));
	ids.add(MAKE_ID(NO_AXIS, PID_POP_AXIS));
	ids.add(MAKE_ID(NO_AXIS, PID_POPEVENT));
	ids.add(MAKE_ID(NO_AXIS, PID_CHUCKJAW));
	ids.add(MAKE_ID(NO_AXIS, PID_ERROR));
	ids.add(MAKE_ID(NO_AXIS, PID_ERROR_MESSAGE));
	// Add channel parameters
	unsigned count = AxisList.count();
	for (unsigned i = 0; i < count; i++)
	{
		TAxis& axis(*AxisList[i]);
		int loc = axis.GetLocation();
		// Check if the axis has limited movement.
		ids.add(MAKE_ID(loc, PID_AXIS_ACCURACY));
		ids.add(MAKE_ID(loc, PID_AXIS_RESOLUTION));
		ids.add(MAKE_ID(loc, PID_AXIS_ROUND));
		if (AxisList[i]->FMovements.contains(gmi::amLIMITED))
		{
			ids.add(MAKE_ID(loc, PID_AXIS_MIN_POS));
			ids.add(MAKE_ID(loc, PID_AXIS_MAX_POS));
		}
		ids.add(MAKE_ID(loc, PID_AXIS_NAME));
		ids.add(MAKE_ID(loc, PID_AXIS_MODE));
		//    ids.Add(MAKE_ID(loc, PID_AXIS_IDX_POS));
		ids.add(MAKE_ID(loc, PID_AXIS_MAX_VEL));
		ids.add(MAKE_ID(loc, PID_AXIS_MAX_ACC));
		ids.add(MAKE_ID(loc, PID_AXIS_OFS_POS));
		ids.add(MAKE_ID(loc, PID_AXIS_CUR_POS));
		ids.add(MAKE_ID(loc, PID_AXIS_OFS_POS));
		ids.add(MAKE_ID(loc, PID_AXIS_CUR_VEL));
		ids.add(MAKE_ID(loc, PID_AXIS_TRG_POS));
		ids.add(MAKE_ID(loc, PID_AXIS_TRG_VEL));
		ids.add(MAKE_ID(loc, PID_AXIS_TRG_ACC));
	}
	//
	return true;
}

int MotionEmulator::GetParamId(EParam param, unsigned axis) const
{
	// Form the id on basis of the masks in the param.
	int id = MAKE_ID((param & gmi::mpAXIS_MASK) ? axis : NO_AXIS, param & ~(gmi::mpAXIS_MASK));
	return id;
}

int MotionEmulator::GetResultId(EResult result, unsigned axis) const
{
	return 0;
}

bool MotionEmulator::EnumResultIds(TIdList& ids) const
{
	return false;
}

bool MotionEmulator::HandleResult(int id, TResultInfo* info, TBufferInfo* bufinfo)
{
	return false;
}

bool MotionEmulator::TAxis::IsMovePosComplete(timespec clk)
{
	// When the timer is disabled the move is complete.
	if (!MovePosTimer.isEnabled())
		return true;
	else
	{// If timer elapsed.
		if (MovePosTimer(clk))
			// Disable the timer and so signal move completion.
			MovePosTimer.disable();
		// When stopping deceleration is in progress
		if (Controller->FCurMovePos == gmi::mpcSTOP)
		{// Calculate the time elapsed from the start.
			double tmp = MovePosTimer.getTimeLeft(clk).toDouble();
			// What is the direction.
			if (CurPos < TrgPos)
				// Calculate the new current position.
				CurPos = TrgDist - (0.5 * TrgAcc * tmp * tmp);
			else
				// Calculate the new current position.
				CurPos = TrgDist + (0.5 * TrgAcc * tmp * tmp);
			// Calculate new current velocity.
			CurVel = tmp * TrgAcc;
		}
		else
		{
			// Calculate the time elapsed from the start.
			double tmp = (MovePosTimer.getElapseTime() - MovePosTimer.getTimeLeft(clk)).toDouble();
			// On calculation failure.
			if (gmi::getPositionVelocity(TrgDist, TrgVel, TrgAcc, tmp, tmp, CurVel))
			{
				CurPos = (TrgPos - TrgDist) + tmp;
			}
		}
		// Notify parameters
		Controller->CallParamHook(MAKE_ID(GetLocation(), PID_AXIS_CUR_POS));
		Controller->CallParamHook(MAKE_ID(GetLocation(), PID_AXIS_CUR_VEL));
	}
	return false;
}

bool MotionEmulator::TAxis::IsMoveVelComplete(timespec clk)
{
	bool retval = true;
	// When the timer is disabled the move is complete.
	if (MoveVelTimer.isEnabled())
	{
		switch (Controller->FCurMoveVel)
		{
			default:
				break;

			case gmi::mvcON:
				// When timer elapsed acceleration has finished.
				if (MoveVelTimer(clk))
				{
					// Assign the starting position to start with.
					CurPos = TrgDist;
					// Get the time after acceleration.
					double tmp = MoveVelTimer.getTimeOver(clk).toDouble();
					// Add the distance traveled in that time.
					CurPos += TrgVel * std::fabs(tmp);
					// At this stage the current velocity is the targeted velocity.
					CurVel = std::fabs(TrgVel);
				}
				else
				{
					// Get the time left for acceleration or deceleration.
					double left = MoveVelTimer.getTimeLeft(clk).toDouble();
					double elapse = MoveVelTimer.getElapseTime().toDouble();
					double tmp = elapse - left;
					// Subtract the distances from the distance reached after acceleration.
					CurPos = TrgDist2;
					// Subtract the distances from the distance reached after acceleration.
					if (TrgVel - StartVel > 0)
						CurPos += (0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
					else
						CurPos += (-0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
					// Calculate the speed for the acceleration time from start velocity.
					CurVel = std::fabs(StartVel + TrgAcc * (elapse - left));
				}
				retval = false;
				break;

			case gmi::mvcSTOP:
				// When timer elapsed acceleration has finished.
				if (MoveVelTimer(clk))
				{// Disable the timer because to prevent reentry of this function.
					MoveVelTimer.disable();
					// No velocity anymore.
					CurVel = StartVel = 0.0;
				}
				else
				{// Get the time left for acceleration or deceleration.
					double left = MoveVelTimer.getTimeLeft(clk).toDouble();
					double elapse = MoveVelTimer.getElapseTime().toDouble();
					double tmp = elapse - left;
					// Subtract the distances from the distance reached after acceleration.
					CurPos = TrgDist2;
					// Subtract the distances from the distance reached after acceleration.
					if (TrgVel - StartVel > 0)
						CurPos += (0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
					else
						CurPos += (-0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
					// Calculate the speed for the acceleration time from start velocity.
					CurVel = std::fabs(StartVel + TrgAcc * (elapse - left));
					//
					retval = false;
				}
				break;
		}
		// Notify parameters
		Controller->CallParamHook(MAKE_ID(GetLocation(), PID_AXIS_CUR_POS));
		Controller->CallParamHook(MAKE_ID(GetLocation(), PID_AXIS_CUR_VEL));
	}
	return retval;
}

bool MotionEmulator::TAxis::IsMoveConComplete(timespec clk)
{
	bool retval = true;
	// When the timer is disabled the move is complete.
	if (MoveConTimer.isEnabled())
	{
		switch (Controller->FCurMoveCon)
		{
			default:
				break;

			case gmi::mccON:
				// When timer elapsed acceleration has finished.
				if (MoveConTimer(clk))
				{
					// Assign the starting position to start with.
					CurPos = TrgDist;
					// Get the time after acceleration.
					double tmp = MoveConTimer.getTimeOver(clk).toDouble();
					// Add the distance traveled in that time.
					CurPos += TrgVel * std::fabs(tmp);
					// At this stage the current velocity is the targeted velocity.
					CurVel = std::fabs(TrgVel);
				}
				else
				{
					// Get the time left for acceleration or deceleration.
					double left = MoveConTimer.getTimeLeft(clk).toDouble();
					double elapse = MoveConTimer.getElapseTime().toDouble();
					double tmp = elapse - left;
					// Subtract the distances from the dinstance reached after acceleration.
					CurPos = TrgDist2;
					// Subtract the distances from the dinstance reached after acceleration.
					if (TrgVel - StartVel > 0)
						CurPos += (0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
					else
						CurPos += (-0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
					// Calculate the speed for the acceleration time from start velocity.
					CurVel = std::fabs(StartVel + TrgAcc * (elapse - left));
				}
				retval = false;
				break;

			case gmi::mccSTOP:
				// When timer elapsed acceleration has finished.
				if (MoveConTimer(clk))
				{// Disable the timer because to prevent reentry of this function.
					MoveConTimer.disable();
					// No velocity anymore.
					CurVel = StartVel = 0.0;
				}
				else
				{// Get the time left for acceleration or deceleration.
					double left = MoveConTimer.getTimeLeft(clk).toDouble();
					double elapse = MoveConTimer.getElapseTime().toDouble();
					double tmp = elapse - left;
					// Subtract the distances from the dinstance reached after acceleration.
					CurPos = TrgDist2;
					// Subtract the distances from the dinstance reached after acceleration.
					if (TrgVel - StartVel > 0)
						CurPos += (0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
					else
						CurPos += (-0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
					// Calculate the speed for the acceleration time from start velocity.
					CurVel = std::fabs(StartVel + TrgAcc * (elapse - left));
					//
					retval = false;
				}
				break;
		}
		// Notify parameters
		Controller->CallParamHook(MAKE_ID(GetLocation(), PID_AXIS_CUR_POS));
		Controller->CallParamHook(MAKE_ID(GetLocation(), PID_AXIS_CUR_VEL));
	}
	return retval;
}

void MotionEmulator::TAxis::DoMovePos(EMovePosCmd mpc)
{
	switch (mpc)
	{
		default:
			break;

		case gmi::mpcPOSITION:
			// Set the targeted distance.
			TrgDist = TrgPos - CurPos;
			// When target distance and acceleration have been set.
			if (!isZero(TrgDist) && !isZero(TrgAcc))
			{
				double trgtm = 0.0;
				// Calculate the time to travel.
				if (gmi::getTargetTime(TrgDist, TrgVel, TrgAcc, trgtm))
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, " Targeted Dist " << GetName() << ": " << TrgDist << "  Vel: " << TrgVel << "  Acc: " << TrgAcc << "  Time: " << trgtm)
					// Set the axis elapse timer.
					MovePosTimer.set(trgtm * CLOCKS_PER_SEC);
				}
			}
			break;

		case gmi::mpcSTOP:
			if (!isZero(TrgAcc) && MovePosTimer.isEnabled())
			{
				// Calculate the time elapsed from the start.
				double tmp = (MovePosTimer.getElapseTime() - MovePosTimer.getTimeLeft()).toDouble();
				// Calculate the current velocity.
				if (gmi::getPositionVelocity(TrgDist, TrgVel, TrgAcc, tmp, tmp, CurVel))
				{
					CurPos = (TrgPos - TrgDist) + tmp;
					// Calculate the time to stop using the target acceleration.
					tmp = CurVel / TrgAcc;
					// Set a new target position for the deceleration.
					TrgDist = CurPos;
					if (CurPos < TrgPos)
						TrgDist += (0.5 * TrgAcc * tmp * tmp);
					else
						TrgDist -= (0.5 * TrgAcc * tmp * tmp);
					// Reset the time to the time to decelerate.
					MovePosTimer.set(tmp * CLOCKS_PER_SEC);
					//
					SF_RTTI_NOTIFY(DO_DEFAULT, " Decelerating " << GetName() << "-Axis from " << CurPos << " to " << TrgDist << " [m] in " << tmp << " [sec]")
				}
				break;
			}
			// Run into next statement.

		case gmi::mpcABORT:
			MovePosTimer.disable();
			break;
	}
}

void MotionEmulator::TAxis::DoMoveVel(EMoveVelCmd mvc)
{
	if (Mode != gmi::amVELOCITY)
	{
		return;
	}
	switch (mvc)
	{
		default:
			break;

		case gmi::mvcABORT:
			MoveVelTimer.disable();
			CurVel = 0.0;
			break;

		case gmi::mvcON:
			if (!isZero(TrgAcc))
			{
				// Hold the current position for reference.
				TrgDist = CurPos;
				TrgDist2 = CurPos;
				// There is a new velocity to start from.
				StartVel = CurVel;
				// Set the time to decelerate.
				double acctime = std::fabs(TrgVel - StartVel) / TrgAcc;
				// Add the distance from accelerating to the target velocity.
				if (TrgVel - StartVel > 0)
					TrgDist += (0.5 * acctime * acctime * TrgAcc) + acctime * StartVel;
				else
					TrgDist += (-0.5 * acctime * acctime * TrgAcc) + acctime * StartVel;
				//
				MoveVelTimer.set(acctime * CLOCKS_PER_SEC);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Ac/Decelerating " << GetName() << "-Axis in " << acctime << " sec")
			}
			break;

		case gmi::mvcSTOP:
			if (!isZero(TrgAcc) && !isZero(CurVel))
			{
				// Hold the current position for reference.
				TrgDist = CurPos;
				TrgDist2 = CurPos;
				// There is a new velocity to start from.
				StartVel = CurVel;
				// Set the time to decelerate.
				double acctime = std::fabs(StartVel) / TrgAcc;
				// Add the distance from accelerating to the target velocity.
				if (StartVel > 0)
					TrgDist += (0.5 * acctime * acctime * TrgAcc);
				else
					TrgDist += (-0.5 * acctime * acctime * TrgAcc);
				//
				MoveVelTimer.set(acctime * CLOCKS_PER_SEC);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Decelerating " << GetName() << "-Axis in " << acctime << " sec")
			}
			break;
	}
}

void MotionEmulator::TAxis::DoMoveCon(EMoveConCmd mcc)
{
	if (Mode != gmi::amCONTINUE)
		return;
	//
	switch (mcc)
	{
		default:
			break;

		case gmi::mccABORT:
			MoveConTimer.disable();
			CurVel = 0.0;
			break;

		case gmi::mccON:
			if (!isZero(TrgAcc))
			{
				// Hold the current position for reference.
				TrgDist = CurPos;
				TrgDist2 = CurPos;
				// There is a new velocity to start from.
				StartVel = CurVel;
				// Set the time to decelerate.
				double acctime = std::fabs(TrgVel - StartVel) / TrgAcc;
				// Add the distance from accelerating to the target velocity.
				if (TrgVel - StartVel > 0)
					TrgDist += (0.5 * acctime * acctime * TrgAcc) + acctime * StartVel;
				else
					TrgDist += (-0.5 * acctime * acctime * TrgAcc) + acctime * StartVel;
				//
				MoveConTimer.set(acctime * CLOCKS_PER_SEC);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Ac/Decelerating " << GetName() << "-Axis in " << acctime << " sec")
			}
			break;

		case gmi::mccSTOP:
			if (!isZero(TrgAcc) && !isZero(CurVel))
			{
				// Hold the current position for reference.
				TrgDist = CurPos;
				TrgDist2 = CurPos;
				// There is a new velocity to start from.
				StartVel = CurVel;
				// Set the time to decelerate.
				double acctime = std::fabs(StartVel) / TrgAcc;
				// Add the distance from accelerating to the target velocity.
				if (StartVel > 0)
					TrgDist += (0.5 * acctime * acctime * TrgAcc);
				else
					TrgDist += (-0.5 * acctime * acctime * TrgAcc);
				//
				MoveConTimer.set(acctime * CLOCKS_PER_SEC);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Decelerating " << GetName() << "-Axis in " << acctime << " sec")
			}
			break;
	}
}

void MotionEmulator::DoMovePos(EMovePosCmd mpc)
{
	// Ignore movement command other than stop or abort
	// when moving.
	if (FCurMovePos != gmi::mpcCOMPLETE && mpc != gmi::mpcABORT && mpc != gmi::mpcSTOP)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Ignored position move command!")
		return;
	}
	// Update the data member of the implementation.
	FCurMovePos = mpc;
	// Iterate through the axes and signal the new command.
	for (unsigned i = 0; i < AxisList.count(); i++)
	{// Only for axes in position mode.
		if (AxisList[i]->Mode == gmi::amPOSITION)
			AxisList[i]->DoMovePos(mpc);
	}
	//
	if (mpc == gmi::mpcABORT)
	{
		DoMoveVel(gmi::mvcABORT);
		DoMoveCon(gmi::mccABORT);
		CallParamHook(MAKE_ID(NO_AXIS, PID_MOVEPOS));
		CallParamHook(MAKE_ID(NO_AXIS, PID_MOVEVEL));
		CallParamHook(MAKE_ID(NO_AXIS, PID_MOVECON));
	}
}

void MotionEmulator::DoMoveVel(EMoveVelCmd mvc)
{
	// Ignore movement command other than stop or abort.
	if (FCurMoveVel != gmi::mvcCOMPLETE && mvc != gmi::mvcABORT && mvc != gmi::mvcSTOP)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Ignored velocity move command!")
		return;
	}
	//
	FCurMoveVel = mvc;
	//
	for (unsigned i = 0; i < AxisList.count(); i++)
	{// Only for axes in velocity mode.
		if (AxisList[i]->Mode == gmi::amVELOCITY)
			AxisList[i]->DoMoveVel(mvc);
	}
}

void MotionEmulator::DoMoveCon(EMoveConCmd mcc)
{
	// Ignore movement command other than stop or abort.
	if (FCurMoveCon != gmi::mccCOMPLETE && mcc != gmi::mccABORT && mcc != gmi::mccSTOP)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Ignored continue move command!")
		return;
	}
	//
	FCurMoveCon = mcc;
	//
	for (unsigned i = 0; i < AxisList.count(); i++)
	{
		// Only for axes in continuous mode.
		if (AxisList[i]->Mode == gmi::amCONTINUE)
			AxisList[i]->DoMoveCon(mcc);
	}
}

bool MotionEmulator::Sustain(const timespec& clk)
{
	// When delay elapsed set state to ready.
	if (FHomingDelay(clk))
	{
		SetStatus(csREADY);
		// Disable the timer to prevent reentry.
		FHomingDelay.disable();
	}
	// When moving check for completion.
	if (FCurMoveVel != gmi::mvcCOMPLETE)
	{
		bool completed = true;
		for (unsigned i = 0; i < AxisList.count(); i++)
			completed &= AxisList[i]->IsMoveVelComplete(clk);
		// When the axis are completed.
		if (completed && FCurMoveVel != gmi::mvcABORT)
		{
			FCurMoveVel = gmi::mvcCOMPLETE;
			SF_RTTI_NOTIFY(DO_DEFAULT, "Velocity Move completed..")
			// Notify parameters
			CallParamHook(MAKE_ID(NO_AXIS, PID_MOVEVEL));
		}
	}
	// When moving check for completion.
	if (FCurMoveCon != gmi::mccCOMPLETE)
	{
		bool completed = true;
		for (unsigned i = 0; i < AxisList.count(); i++)
			completed &= AxisList[i]->IsMoveConComplete(clk);
		// When the axis are completed.
		if (completed && FCurMoveCon != gmi::mccABORT)
		{
			FCurMoveCon = gmi::mccCOMPLETE;
			SF_RTTI_NOTIFY(DO_DEFAULT, "Continue Move completed..")
			// Notify parameters
			CallParamHook(MAKE_ID(NO_AXIS, PID_MOVECON));
		}
	}
	// When moving check for completion.
	if (FCurMovePos != gmi::mpcCOMPLETE)
	{
		bool completed = true;
		for (unsigned i = 0; i < AxisList.count(); i++)
		{
			completed &= AxisList[i]->IsMovePosComplete(clk);
		}
		// General purpose.
		gmi::TAxesCoord ac("");
		// When the axis are completed.
		if (completed && FCurMovePos != gmi::mpcABORT)
		{
			if (FCurMovePos == gmi::mpcSTOP)
			{
				GetCurrent(gmi::avtPOSITION, ac);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Position Move " << ac << " interrupted..")
			}
			else
			{
				GetTarget(gmi::avtPOSITION, ac);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Position Move " << ac << " completed..")
			}
			// Reset state to complete.
			FCurMovePos = gmi::mpcCOMPLETE;
			// Notify parameters
			CallParamHook(MAKE_ID(NO_AXIS, PID_MOVEPOS));
		}
	}
	// Check if a pop event needs to be generated.
	if (GetPopAxis() == gmi::alC)
	{
		int count = std::floor(dynamic_cast<TAxis*>(AxisList[gmi::alC])->CurPos / (2.0 * numbers::pi_v<double>) );
		if (count != FPopEventCount)
		{
			FPopEventCount = count;
			SendPopEvent();
		}
	}
	//
	if (FJoystickState == gmi::jscSTOP)
	{
		FJoystickState = gmi::jscOFF;
	}
	//
	return true;
}

}// namespace sf
