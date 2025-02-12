#include "GmiEmulator.h"
#include "misc/gen/IniProfile.h"
#include <gmi/iface/GmiUtils.h>

namespace sf
{

using gmi::Controller;
using gmi::EAxisLocation;
using gmi::EAxisMode;
using gmi::EMoveConCmd;
using gmi::EMovePosCmd;
using gmi::EMoveVelCmd;
using gmi::EParam;
using gmi::EResult;
using gmi::IdList;
using gmi::ParamInfo;
using gmi::ParamState;

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
GmiEmulator::TAxis::TAxis(GmiEmulator* me, EAxisLocation al)
	: TInherited(me, al)
	, _Controller(me)
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

GmiEmulator::TAxis::~TAxis() {}

GmiEmulator::GmiEmulator(const Parameters& params)
	: Controller(params)
	, _sustainEntry(this, &GmiEmulator::sustain)
	, _homingDelay(3000)
	, _error(0)
	, _chuckJaw(0)
	, _curMovePos(gmi::mpcCOMPLETE)
	, _curMoveVel(gmi::mvcCOMPLETE)
	, _curMoveCon(gmi::mccCOMPLETE)
	, _flagInitialized(false)
	, _joystickState(0)
	, _triggerAxis(gmi::alNA)
	, _triggerDensity(0.0)
	, _triggerFrequency(1.0)
	, _triggerEnable(true)
	, _triggerMode(0)
	, _popAxis(gmi::alNA)
	, _popEventCount(0)
	, _axisX(this, gmi::alX)
	, _axisY(this, gmi::alY)
	, _axisZ(this, gmi::alZ)
	, _axisA(this, gmi::alA)
	, _axisB(this, gmi::alB)
	, _axisC(this, gmi::alC)
	//,D_Axis(this, gmi::alD)
	, _axisE(this, gmi::alE)
{
	// Do not home immediately.
	_homingDelay.disable();
	// Configure axis movement limitations.
	_axisX._movements << gmi::amLINEAR << gmi::amLIMITED;
	_axisY._movements << gmi::amLINEAR << gmi::amLIMITED;
	_axisZ._movements << gmi::amLINEAR << gmi::amLIMITED;
	_axisA._movements << gmi::amRADIAL << gmi::amLIMITED;
	_axisB._movements << gmi::amRADIAL << gmi::amLIMITED;
	_axisC._movements << gmi::amRADIAL;
	//  D_Axis.FMovements << gmi::amRADIAL;
	_axisE._movements << gmi::amLINEAR << gmi::amLIMITED;
}

GmiEmulator::~GmiEmulator()
{
	// Signal the interface destruction of this instance.
	// Gives interface time to perform some actions.
	destroy();
}

bool GmiEmulator::doInitialize(bool init)
{
	if (init) SF_RTTI_NOTIFY(DO_DEFAULT, "Initialized Perfectly...")
	else
		SF_RTTI_NOTIFY(DO_DEFAULT, "Uninitialized Perfectly...")
	// Save of load last positions from profile.
	IniProfile profile("Last Positions", getProfilePath().c_str());
	for (unsigned i = 0; i < AxisList.count(); i++)
		if (init) AxisList[i]->CurPos = toNumber<double>(profile.getString(AxisList[i]->getName(), toString(0.0)));
		else
			profile.setString(AxisList[i]->getName(), toString(AxisList[i]->CurPos));
	return true;
}

bool GmiEmulator::doHomeAxes(bool skip)
{
	// When skip is true do not delay for homing.
	if (skip) setStatus(csREADY);
	else
	{
		// Set status for homing.
		setStatus(csHOMING);
		_homingDelay.reset();
	}
	//
	return true;
}

void GmiEmulator::addPropertyPages(PropertySheetDialog* sheet)
{
	TInherited::addPropertyPages(sheet);
}

bool GmiEmulator::handleParam(gmi::IdType id, ParamInfo* info, const Value* setval, Value* getval)
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
					if (getval) getval->set("XY Emulator 0.01");
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
					if (setval) _firmwareRevision = setval->getString();
					if (getval) getval->set(_firmwareRevision);
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
					if (setval) sendPopEvent();
					if (getval) getval->set(0);
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
						info->States.add(ParamState("POP Event", Value(0)));
						info->States.add(ParamState("POP Event", Value(1)));
						info->Flags &= ~gmi::pfARCHIVE;
						info->Flags |= gmi::pfEXPORT | gmi::pfWRITEABLE;
					}
					break;

				case PID_POP_AXIS:
					if (setval) _popAxis = (EAxisLocation) setval->getInteger();
					if (getval) getval->set(_popAxis);
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
							if (loc == gmi::alNA) name = "None";
							else
								name = getAxis(loc).getName() + std::string("-Axis");
							info->States.add(ParamState(name, Value(loc)));
						}
						// Cannot be enabled during movement.
						info->Flags |= gmi::pfEXPORT | gmi::pfMOVEVEL | gmi::pfMOVEPOS;
					}
					break;

				case PID_MOVEPOS:
					if (setval)
					{
						if (_debug) SF_RTTI_NOTIFY(DO_DEFAULT, "Debug capture event!")
						EMovePosCmd mpc = (EMovePosCmd) setval->getInteger();
						if (mpc != _curMovePos) doMovePos(mpc);
					}
					if (getval) getval->set(_curMovePos);
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
						info->States.add(ParamState("Complete", Value(gmi::mpcCOMPLETE)));
						info->States.add(ParamState("Abort", Value(gmi::mpcABORT)));
						info->States.add(ParamState("Stop", Value(gmi::mpcSTOP)));
						info->States.add(ParamState("Position", Value(gmi::mpcPOSITION)));
						info->States.add(ParamState("Curve", Value(gmi::mpcCURVE)));
						info->Flags |= gmi::pfEXPORT;
						info->Flags &= ~gmi::pfARCHIVE;
					}
					break;

				case PID_MOVEVEL:
					if (setval)
					{
						EMoveVelCmd mvc = (EMoveVelCmd) setval->getInteger();
						if (mvc != _curMoveVel) doMoveVel(mvc);
					}
					if (getval) getval->set(_curMoveVel);
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
						info->States.add(ParamState("Complete", Value(gmi::mvcCOMPLETE)));
						info->States.add(ParamState("Abort", Value(gmi::mvcABORT)));
						info->States.add(ParamState("Stop", Value(gmi::mvcSTOP)));
						info->States.add(ParamState("On", Value(gmi::mvcON)));
						info->Flags |= gmi::pfEXPORT;
						info->Flags &= ~gmi::pfARCHIVE;
					}
					break;

				case PID_MOVECON:
					if (setval)
					{
						EMoveConCmd mcc = (EMoveConCmd) setval->getInteger();
						if (mcc != _curMoveCon) doMoveCon(mcc);
					}
					if (getval) getval->set(_curMoveCon);
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
						info->States.add(ParamState("Complete", Value(gmi::mccCOMPLETE)));
						info->States.add(ParamState("Abort", Value(gmi::mccABORT)));
						info->States.add(ParamState("Stop", Value(gmi::mccSTOP)));
						info->States.add(ParamState("On", Value(gmi::mccON)));
						info->Flags |= gmi::pfEXPORT;
						info->Flags &= ~gmi::pfARCHIVE;
					}
					break;

				case PID_JOYSTICK:
					if (setval)
					{
						// Prevent looping
						if (_joystickState != setval->getInteger())
						{
							switch (setval->getInteger())
							{
								default:
									_joystickState = setval->getInteger();
									break;

								case gmi::jscSTOP:
									// When already Off do nothing.
									if (_joystickState != gmi::jscOFF) _joystickState = setval->getInteger();
									break;
							}
						}
					}
					if (getval) getval->set(_joystickState);
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
						info->States.add(ParamState("Inhibit", Value(gmi::jscINHIBIT)));
						info->States.add(ParamState("Off", Value(gmi::jscOFF)));
						info->States.add(ParamState("Stop", Value(gmi::jscSTOP)));
						info->States.add(ParamState("On", Value(gmi::jscON)));
						info->Flags |= gmi::pfEXPORT;
					}
					break;

				case PID_TRIGGER_AXIS:
					if (setval) _triggerAxis = setval->getInteger();
					if (getval) getval->set(_triggerAxis);
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
							std::string name = getAxis(loc).getName();
							if (loc == gmi::alNA) name = "None";
							info->States.add(ParamState(name, Value(loc)));
						}
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT | gmi::pfEFFECTPARAM;
					}
					break;

				case PID_TRIGGER_DENSITY:
					if (setval) _triggerDensity = setval->getFloat();
					if (getval) getval->set(_triggerDensity);
					if (info)
					{
						info->Id = id;
						info->Name = "Trigger|Density";
						gmi::AxisMovements mvs = getAxis(_triggerAxis).getMovements();
						if (mvs.has(gmi::amRADIAL)) info->Unit = "rad";
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
					if (setval) _triggerMode = setval->getInteger();
					if (getval) getval->set(_triggerMode);
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
						info->States.add(ParamState("Internal", Value(1)));
						info->States.add(ParamState("External", Value(0)));
						info->Flags |= gmi::pfEXPORT | gmi::pfEFFECTPARAM;
					}
					break;

				case PID_TRIGGER_ENABLE:
					if (setval) _triggerEnable = setval->getInteger();
					if (getval) getval->set(_triggerEnable);
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
						info->States.add(ParamState("Disabled", Value(false)));
						info->States.add(ParamState("Enabled", Value(true)));
						info->Flags |= gmi::pfEXPORT;
					}
					break;

				case PID_TRIGGER_FREQ:
				{
					// Check if the value must be set.
					if (setval)
					{
						_triggerFrequency = setval->getInteger();
						//CallParamHook(MAKE_ID(NO_AXIS, PID_TIMEUNIT));
					}
					// Check if the axis exists in the implementation.
					if (getval) getval->set(_triggerFrequency);
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
				}
				break;

				case PID_CHUCKJAW:
					if (setval) _chuckJaw = setval->getInteger();
					if (getval) getval->set(_chuckJaw);
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
						info->States.add(ParamState("Off", Value(gmi::cjOFF)));
						info->States.add(ParamState("Open", Value(gmi::cjOPEN)));
						info->States.add(ParamState("Close", Value(gmi::cjCLOSE)));
						info->Flags |= gmi::pfEXPORT;
					}
					break;

				case PID_ERROR:
					if (setval)
					{
						_error = setval->getInteger();
						// Make the client update the error message.
						callParamHook(MAKE_ID(NO_AXIS, PID_ERROR_MESSAGE));
					}
					if (getval) getval->set(_error);
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
						info->States.add(ParamState("Ok", Value(0)));
						info->States.add(ParamState("Error", Value(1)));
						info->Flags = gmi::pfEXPORT | gmi::pfWRITEABLE;
					}
					break;

				case PID_ERROR_MESSAGE:
					if (getval)
					{
						if (_error)
						{
							std::string errtxt;
							for (int i = 1; i < 10; i++) errtxt += stringf("Error message line %i\n", i);
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
						_debug = setval->getInteger();
						doMovePos(gmi::mpcABORT);
					}
					if (getval) getval->set(_debug);
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
						info->States.add(ParamState("Ready", Value(0)));
						info->States.add(ParamState("Debug", Value(1)));
						info->Flags = gmi::pfEXPORT | gmi::pfWRITEABLE;
					}
					break;
			}
		}
		// Check if the axis is implemented.
		else if (getAxis(GETAXIS(id)).getLocation() != gmi::alNA)
		{
			// Create temporary easy to use reference.
			TAxis& axis = dynamic_cast<TAxis&>(getAxis(GETAXIS(id)));
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
					SF_RTTI_NOTIFY(DO_DEFAULT, "Axis '" << axis.getName() << "' Param ID " << stringf("0x%lX", id) << " does not exists!")
					return false;

				case PID_AXIS_NAME:
					if (getval)
					{
						getval->set(axis.getName());
					}
					break;

				case PID_AXIS_OFS_POS:
				{
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (_curMovePos || _curMoveVel || _curMoveCon) retval = false;
						else
						{
							axis.OfsPos = setval->getFloat();
							callParamHook(MAKE_ID(GETAXIS(id), PID_AXIS_TRG_POS));
							callParamHook(MAKE_ID(GETAXIS(id), PID_AXIS_CUR_POS));
						}
					}
					if (getval) getval->set(axis.OfsPos);
					if (info)
					{
						info->Id = id;
						info->Name = "Offset";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Position offset of %s-Axis. It shifts all position parameters of this axis.", axis.getName());
						info->Default.set(0.0);
						info->Round.set(axis.Round);
						info->Minimum.set(-10.0);
						info->Maximum.set(10.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfEXPORT;
					}
				}
				break;

				case PID_AXIS_CUR_POS:
				{
					bool radial = axis.getMovements() == (gmi::AxisMovements() << gmi::amRADIAL);
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (_curMovePos || _curMoveVel || _curMoveCon || radial) retval = false;
						else
						{
							axis.OfsPos = setval->getFloat() - axis.CurPos;
							callParamHook(MAKE_ID(GETAXIS(id), PID_AXIS_TRG_POS));
							callParamHook(MAKE_ID(GETAXIS(id), PID_AXIS_OFS_POS));
						}
					}
					if (getval)
					{
						double val = axis.CurPos + axis.OfsPos;
						if (radial)
						{
							val = fmod(val, 2.0 * numbers::pi_v<double>);
							if (val < 0.0) val += 2.0 * numbers::pi_v<double>;
						}
						getval->set(val);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Current|Position";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Current position of %s-Axis.", axis.getName());
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
				}
				break;

				case PID_AXIS_TRG_POS:
				{
					bool radial = axis.getMovements() == (gmi::AxisMovements() << gmi::amRADIAL);
					if (setval)
					{
						if (radial)
						{
							// Get the angle position from the current axis position.
							double cur_angle = fmod(axis.CurPos - axis.OfsPos, 2.0 * numbers::pi_v<double>);
							if (cur_angle < 0.0) cur_angle += 2.0 * numbers::pi_v<double>;
							// Accept only positive values.
							axis.TrgPos = setval->getFloat();
							axis.TrgPos = fmod(axis.TrgPos, 2.0 * numbers::pi_v<double>);
							if (axis.TrgPos < 0.0) axis.TrgPos += 2.0 * numbers::pi_v<double>;
							// Determine difference between the angles.
							double dif_angle = fmod(axis.TrgPos - cur_angle, 2.0 * numbers::pi_v<double>);
							// Calculate the current position of the axis before the change.
							double trg_pos = axis.CurPos;
							// Do no accept changes less than the set accuracy.
							if (std::fabs(dif_angle) > axis.Accuracy) trg_pos += dif_angle;
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
						if (radial) getval->set(std::fmod(axis.TrgPos - 2.0 * numbers::pi_v<double> + axis.OfsPos, 2.0 * numbers::pi_v<double>));
						else
							getval->set(axis.TrgPos + axis.OfsPos);
					}
					if (info)
					{
						info->Id = id;
						info->Name = "Target|Position";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Target position of %s-Axis.", axis.getName());
						info->Default.set(0.0);
						info->Round.set(axis.Round);
						// Set limits if there are any.
						if (axis.getMovements().contains(gmi::amLIMITED))
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
				}
				break;

				case PID_AXIS_TRG_VEL:
					if (setval)
					{
						// IS the axis not disabled.
						if (axis.Mode != gmi::amDISABLED)
						{
							// Can only set the mode when controller is not moving.
							if (_curMovePos && axis.Mode == gmi::amPOSITION) retval = false;
						}
						//
						if (retval)
						{
							// Set the velocity.
							axis.TrgVel = setval->getFloat();
							// When in velocity mode make it accelerate or decelerate.
							if (_curMoveVel == gmi::mvcON) axis.DoMoveVel(gmi::mvcON);
							if (_curMoveCon == gmi::mccON) axis.DoMoveCon(gmi::mccON);
						}
					}
					if (getval) getval->set(axis.TrgVel);
					if (info)
					{
						info->Id = id;
						info->Name = "Target|Velocity";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m/s" : "rad/s";
						info->Description += stringf("Target velocity of %s-Axis.", axis.getName());
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
							if (_curMovePos && axis.Mode == gmi::amPOSITION) retval = false;
							else if (_curMoveVel && axis.Mode == gmi::amVELOCITY)
								retval = false;
						}
						//
						axis.TrgAcc = setval->getFloat();
					}
					if (getval) getval->set(axis.TrgAcc);
					if (info)
					{
						info->Id = id;
						info->Name = "Target|Acceleration";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m/s\xB2" : "rad/s\xB2";
						info->Description += stringf("Target acceleration of %s-Axis.", axis.getName());
						info->Default.set(0.0);
						info->Round.set(axis.Round);
						info->Minimum.set(0);
						info->Maximum.set(axis.MaxAcc);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT;
					}
					break;

				case PID_AXIS_CUR_VEL:
					if (setval) axis.CurVel = setval->getFloat();
					if (getval) getval->set(axis.CurVel);
					if (info)
					{
						info->Id = id;
						info->Name = "Current|Velocity";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m/s" : "rad/s";
						info->Description += stringf("Current velocity of %s-Axis.", axis.getName());
						info->Default.set(2.0);
						info->Round.set(axis.Round);
						info->Minimum.set(0.0);
						info->Maximum.set(axis.MaxVel);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEXPORT | gmi::pfREADONLY;
					}
					break;

				case PID_AXIS_MIN_POS:
				{
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (_curMovePos || _curMoveVel || _curMoveCon) retval = false;
						else
							axis.MinPos = setval->getFloat();
					}
					if (getval) getval->set(axis.MinPos);
					if (info)
					{
						info->Id = id;
						info->Name = "Minimum|Position";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Maximum position of %s-Axis.", axis.getName());
						info->Default.set(DEF_AXIS_MIN_POS);
						info->Round.set(axis.Round);
						info->Minimum.set(-10.0);
						info->Maximum.set(10.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
				}
				break;

				case PID_AXIS_MAX_POS:
				{
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (_curMovePos || _curMoveVel || _curMoveCon) retval = false;
						else
							axis.MaxPos = setval->getFloat();
					}
					if (getval) getval->set(axis.MaxPos);
					if (info)
					{
						info->Id = id;
						info->Name = "Maximum|Position";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Position of %s-Axis.", axis.getName());
						info->Default.set(DEF_AXIS_MAX_POS);
						info->Round.set(axis.Round);
						info->Minimum.set(-10.0);
						info->Maximum.set(10.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
				}
				break;

				case PID_AXIS_MAX_VEL:
					if (setval)
					{
						// Can only set the mode when controller is not moving.
						if (_curMovePos || _curMoveVel || _curMoveCon) retval = false;
						else
							axis.MaxVel = setval->getFloat();
					}
					if (getval) getval->set(axis.MaxVel);
					if (info)
					{
						info->Id = id;
						info->Name = "Maximum|Velocity";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m/s" : "rad/s";
						info->Description += stringf("Maximum velocity of %s-Axis.", axis.getName());
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
						if (_curMovePos || _curMoveVel || _curMoveCon) retval = false;
						else
							axis.MaxAcc = setval->getFloat();
					}
					if (getval) getval->set(axis.MaxAcc);
					if (info)
					{
						info->Id = id;
						info->Name = "Maximum|Acceleration";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m/s\xB2" : "rad/s\xB2";
						info->Description += stringf("Maximum acceleration of %s-Axis.", axis.getName());
						info->Default.set(DEF_AXIS_MAX_ACC);
						info->Round.set(axis.Round);
						info->Minimum.set(0);
						info->Maximum.set(5.0);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
					break;

				case PID_AXIS_ACCURACY:
					if (setval) axis.Accuracy = setval->getFloat();
					if (getval) getval->set(axis.Accuracy);
					if (info)
					{
						info->Id = id;
						info->Name = "Accuracy";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "m" : "rad";
						info->Description += stringf("Position and velocity accuracy of the axis.");
						info->Default.set(DEF_AXIS_ACCURACY);
						info->Round.set(1E-8);
						info->Minimum.set(1E-8);
						info->Maximum.set(1E-2);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfEFFECTPARAM | gmi::pfSYSTEM;
					}
					break;

				case PID_AXIS_RESOLUTION:
					if (setval) axis.Resolution = setval->getFloat();
					if (getval) getval->set(axis.Resolution);
					if (info)
					{
						info->Id = id;
						info->Name = "Resolution";
						info->Unit = axis._movements.contains(gmi::amLINEAR) ? "p/m" : "p/rad";
						info->Description += stringf("Resolution transformation factor for %s-Axis.", axis.getName());
						info->Default.set(DEF_AXIS_RES);
						info->Round.set(1E-8);
						info->Minimum.set(1E-8);
						info->Maximum.set(1E8);
						info->Flags |= gmi::pfMOVEVEL | gmi::pfMOVEPOS | gmi::pfSYSTEM;
					}
					break;

				case PID_AXIS_ROUND:
					if (setval) axis.Round = setval->getFloat();
					if (getval) getval->set(axis.Round);
					if (info)
					{
						info->Id = id;
						info->Name = "Rounding";
						info->Unit = "x";
						info->Description += stringf("Rounding for %s-Axis parameters.", axis.getName());
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
						if (!axis.canModeChange(axis.Mode, newmode, !_curMovePos, !_curMoveVel, !_curMoveCon)) retval = false;
						else
							axis.Mode = newmode;
					}
					if (getval) getval->set(axis.Mode);
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
						info->States.add(ParamState("Disabled", Value(gmi::amDISABLED)));
						info->States.add(ParamState("Position", Value(gmi::amPOSITION)));
						info->States.add(ParamState("Velocity", Value(gmi::amVELOCITY)));
						info->States.add(ParamState("Continue", Value(gmi::amCONTINUE)));
						info->States.add(ParamState("Home", Value(gmi::amHOME)));
						info->Flags |= gmi::pfMOVE | gmi::pfEXPORT;
					}
					break;
			}
		}
	}
	// Check if at least the id is filled in.
	if (info && info->Id == 0) return false;
	// Found it so return true.
	return retval;
}

bool GmiEmulator::enumParamIds(IdList& ids) const
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
		int loc = axis.getLocation();
		// Check if the axis has limited movement.
		ids.add(MAKE_ID(loc, PID_AXIS_ACCURACY));
		ids.add(MAKE_ID(loc, PID_AXIS_RESOLUTION));
		ids.add(MAKE_ID(loc, PID_AXIS_ROUND));
		if (AxisList[i]->_movements.contains(gmi::amLIMITED))
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

gmi::IdType GmiEmulator::getParamId(EParam param, int axis) const
{
	// Form the id on basis of the masks in the param.
	gmi::IdType id = MAKE_ID((param & gmi::mpAXIS_MASK) ? axis : NO_AXIS, param & ~(gmi::mpAXIS_MASK));
	return id;
}

gmi::IdType GmiEmulator::getResultId(EResult result, int axis) const
{
	return 0;
}

bool GmiEmulator::enumResultIds(IdList& ids) const
{
	return false;
}

bool GmiEmulator::handleResult(gmi::IdType id, gmi::ResultInfo* info, gmi::BufferInfo* buf_info)
{
	return false;
}

bool GmiEmulator::TAxis::IsMovePosComplete(timespec clk)
{
	// When the timer is disabled the move is complete.
	if (!MovePosTimer.isEnabled()) return true;
	else
	{// If timer elapsed.
		if (MovePosTimer(clk))
			// Disable the timer and so signal move completion.
			MovePosTimer.disable();
		// When stopping deceleration is in progress
		if (_Controller->_curMovePos == gmi::mpcSTOP)
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
		_Controller->callParamHook(MAKE_ID(getLocation(), PID_AXIS_CUR_POS));
		_Controller->callParamHook(MAKE_ID(getLocation(), PID_AXIS_CUR_VEL));
	}
	return false;
}

bool GmiEmulator::TAxis::IsMoveVelComplete(timespec clk)
{
	bool retval = true;
	// When the timer is disabled the move is complete.
	if (MoveVelTimer.isEnabled())
	{
		switch (_Controller->_curMoveVel)
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
					if (TrgVel - StartVel > 0) CurPos += (0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
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
					if (TrgVel - StartVel > 0) CurPos += (0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
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
		_Controller->callParamHook(MAKE_ID(getLocation(), PID_AXIS_CUR_POS));
		_Controller->callParamHook(MAKE_ID(getLocation(), PID_AXIS_CUR_VEL));
	}
	return retval;
}

bool GmiEmulator::TAxis::IsMoveConComplete(timespec clk)
{
	bool retval = true;
	// When the timer is disabled the move is complete.
	if (MoveConTimer.isEnabled())
	{
		switch (_Controller->_curMoveCon)
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
					if (TrgVel - StartVel > 0) CurPos += (0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
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
					if (TrgVel - StartVel > 0) CurPos += (0.5 * tmp * tmp * TrgAcc) + tmp * StartVel;
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
		_Controller->callParamHook(MAKE_ID(getLocation(), PID_AXIS_CUR_POS));
		_Controller->callParamHook(MAKE_ID(getLocation(), PID_AXIS_CUR_VEL));
	}
	return retval;
}

void GmiEmulator::TAxis::DoMovePos(EMovePosCmd mpc)
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
					SF_RTTI_NOTIFY(DO_DEFAULT, " Targeted Dist " << getName() << ": " << TrgDist << "  Vel: " << TrgVel << "  Acc: " << TrgAcc << "  Time: " << trgtm)
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
					if (CurPos < TrgPos) TrgDist += (0.5 * TrgAcc * tmp * tmp);
					else
						TrgDist -= (0.5 * TrgAcc * tmp * tmp);
					// Reset the time to the time to decelerate.
					MovePosTimer.set(tmp * CLOCKS_PER_SEC);
					//
					SF_RTTI_NOTIFY(DO_DEFAULT, " Decelerating " << getName() << "-Axis from " << CurPos << " to " << TrgDist << " [m] in " << tmp << " [sec]")
				}
				break;
			}
			// Run into next statement.

		case gmi::mpcABORT:
			MovePosTimer.disable();
			break;
	}
}

void GmiEmulator::TAxis::DoMoveVel(EMoveVelCmd mvc)
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
				if (TrgVel - StartVel > 0) TrgDist += (0.5 * acctime * acctime * TrgAcc) + acctime * StartVel;
				else
					TrgDist += (-0.5 * acctime * acctime * TrgAcc) + acctime * StartVel;
				//
				MoveVelTimer.set(acctime * CLOCKS_PER_SEC);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Ac/Decelerating " << getName() << "-Axis in " << acctime << " sec")
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
				if (StartVel > 0) TrgDist += (0.5 * acctime * acctime * TrgAcc);
				else
					TrgDist += (-0.5 * acctime * acctime * TrgAcc);
				//
				MoveVelTimer.set(acctime * CLOCKS_PER_SEC);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Decelerating " << getName() << "-Axis in " << acctime << " sec")
			}
			break;
	}
}

void GmiEmulator::TAxis::DoMoveCon(EMoveConCmd mcc)
{
	if (Mode != gmi::amCONTINUE) return;
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
				if (TrgVel - StartVel > 0) TrgDist += (0.5 * acctime * acctime * TrgAcc) + acctime * StartVel;
				else
					TrgDist += (-0.5 * acctime * acctime * TrgAcc) + acctime * StartVel;
				//
				MoveConTimer.set(acctime * CLOCKS_PER_SEC);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Ac/Decelerating " << getName() << "-Axis in " << acctime << " sec")
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
				if (StartVel > 0) TrgDist += (0.5 * acctime * acctime * TrgAcc);
				else
					TrgDist += (-0.5 * acctime * acctime * TrgAcc);
				//
				MoveConTimer.set(acctime * CLOCKS_PER_SEC);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Decelerating " << getName() << "-Axis in " << acctime << " sec")
			}
			break;
	}
}

void GmiEmulator::doMovePos(EMovePosCmd mpc)
{
	// Ignore movement command other than stop or abort
	// when moving.
	if (_curMovePos != gmi::mpcCOMPLETE && mpc != gmi::mpcABORT && mpc != gmi::mpcSTOP)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Ignored position move command!")
		return;
	}
	// Update the data member of the implementation.
	_curMovePos = mpc;
	// Iterate through the axes and signal the new command.
	for (unsigned i = 0; i < AxisList.count(); i++)
	{// Only for axes in position mode.
		if (AxisList[i]->Mode == gmi::amPOSITION) AxisList[i]->DoMovePos(mpc);
	}
	//
	if (mpc == gmi::mpcABORT)
	{
		doMoveVel(gmi::mvcABORT);
		doMoveCon(gmi::mccABORT);
		callParamHook(MAKE_ID(NO_AXIS, PID_MOVEPOS));
		callParamHook(MAKE_ID(NO_AXIS, PID_MOVEVEL));
		callParamHook(MAKE_ID(NO_AXIS, PID_MOVECON));
	}
}

void GmiEmulator::doMoveVel(EMoveVelCmd mvc)
{
	// Ignore movement command other than stop or abort.
	if (_curMoveVel != gmi::mvcCOMPLETE && mvc != gmi::mvcABORT && mvc != gmi::mvcSTOP)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Ignored velocity move command!")
		return;
	}
	//
	_curMoveVel = mvc;
	//
	for (unsigned i = 0; i < AxisList.count(); i++)
	{// Only for axes in velocity mode.
		if (AxisList[i]->Mode == gmi::amVELOCITY) AxisList[i]->DoMoveVel(mvc);
	}
}

void GmiEmulator::doMoveCon(EMoveConCmd mcc)
{
	// Ignore movement command other than stop or abort.
	if (_curMoveCon != gmi::mccCOMPLETE && mcc != gmi::mccABORT && mcc != gmi::mccSTOP)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Ignored continue move command!")
		return;
	}
	//
	_curMoveCon = mcc;
	//
	for (unsigned i = 0; i < AxisList.count(); i++)
	{
		// Only for axes in continuous mode.
		if (AxisList[i]->Mode == gmi::amCONTINUE) AxisList[i]->DoMoveCon(mcc);
	}
}

bool GmiEmulator::sustain(const timespec& t)
{
	// When delay elapsed set state to ready.
	if (_homingDelay(t))
	{
		setStatus(csREADY);
		// Disable the timer to prevent reentry.
		_homingDelay.disable();
	}
	// When moving check for completion.
	if (_curMoveVel != gmi::mvcCOMPLETE)
	{
		bool completed = true;
		for (unsigned i = 0; i < AxisList.count(); i++) completed &= AxisList[i]->IsMoveVelComplete(t);
		// When the axis are completed.
		if (completed && _curMoveVel != gmi::mvcABORT)
		{
			_curMoveVel = gmi::mvcCOMPLETE;
			SF_RTTI_NOTIFY(DO_DEFAULT, "Velocity Move completed..")
			// Notify parameters
			callParamHook(MAKE_ID(NO_AXIS, PID_MOVEVEL));
		}
	}
	// When moving check for completion.
	if (_curMoveCon != gmi::mccCOMPLETE)
	{
		bool completed = true;
		for (unsigned i = 0; i < AxisList.count(); i++) completed &= AxisList[i]->IsMoveConComplete(t);
		// When the axis are completed.
		if (completed && _curMoveCon != gmi::mccABORT)
		{
			_curMoveCon = gmi::mccCOMPLETE;
			SF_RTTI_NOTIFY(DO_DEFAULT, "Continue Move completed..")
			// Notify parameters
			callParamHook(MAKE_ID(NO_AXIS, PID_MOVECON));
		}
	}
	// When moving check for completion.
	if (_curMovePos != gmi::mpcCOMPLETE)
	{
		bool completed = true;
		for (unsigned i = 0; i < AxisList.count(); i++)
		{
			completed &= AxisList[i]->IsMovePosComplete(t);
		}
		// General purpose.
		gmi::AxesCoord ac("");
		// When the axis are completed.
		if (completed && _curMovePos != gmi::mpcABORT)
		{
			if (_curMovePos == gmi::mpcSTOP)
			{
				getCurrent(gmi::avtPOSITION, ac);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Position Move " << ac << " interrupted..")
			}
			else
			{
				getTarget(gmi::avtPOSITION, ac);
				SF_RTTI_NOTIFY(DO_DEFAULT, "Position Move " << ac << " completed..")
			}
			// Reset state to complete.
			_curMovePos = gmi::mpcCOMPLETE;
			// Notify parameters
			callParamHook(MAKE_ID(NO_AXIS, PID_MOVEPOS));
		}
	}
	// Check if a pop event needs to be generated.
	if (getPopAxis() == gmi::alC)
	{
		int count = std::floor(dynamic_cast<TAxis*>(AxisList[gmi::alC])->CurPos / (2.0 * numbers::pi_v<double>) );
		if (count != _popEventCount)
		{
			_popEventCount = count;
			sendPopEvent();
		}
	}
	//
	if (_joystickState == gmi::jscSTOP)
	{
		_joystickState = gmi::jscOFF;
	}
	//
	return true;
}

}// namespace sf
