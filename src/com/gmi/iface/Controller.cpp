#include "Controller.h"
#include "GmiUtils.h"
#include "misc/gen/ConfigLocation.h"
#include "misc/gen/IniProfile.h"

namespace sf::gmi
{

// Implementations of static functions and data members to be able to create registered stars implementations.
SF_IMPL_IFACE(TController, TController::Parameters, Interface)

//
// Motion controller class 'TController'
//
TController::TController(const Parameters&)
	: FNullAxis(this, alNA)
	, FStatus(csUNINIT)
	, FlagDestroying(false)
	, ParamNotifyProc(nullptr)
	, ResultNotifyProc(nullptr)
{
	// Initialize the axes map member using the NullAxis.
	for (int i = 0; i < std::size(FAxesMap); i++)
	{
		FAxesMap[i] = &FNullAxis;
	}
}

TController::~TController()
{
}

void TController::Attach(TAxis* axis)
{
	// Ignore the null axis.
	if (axis != &FNullAxis)
	{
		// Add the axis to the vector of axes.
		FAxes.add(axis);
		// Get the location.
		EAxisLocation al = axis->GetLocation();
		// Check if the current instance is not set yet.
		if (FAxesMap[al] != &FNullAxis)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Axis location '" << GetAxisName(al) << "' already in use!")
		}
		else
		{
			// Map the axis pointer in the array.
			FAxesMap[al] = axis;
		}
	}
}

void TController::Destroy()
{
	// Send the unhook signal.
	SendEvent(ceUNHOOKED);
	// Flush all the hooked events.
	FMotionEventList.flush();
	// Uninitialize first the controller.
	Uninitialize();
	// Set the destroy flag so no the TAxis implementations destruct errorless.
	FlagDestroying = true;
}

void TController::HookEventHandler(TControllerEvent handler)
{
	// Cannot attach the handlers when destroying or when it is NULL.
	if (!handler || FlagDestroying)
		return;
	// Check if the handler already was assigned.
	if (FMotionEventList.find(handler) == UINT_MAX)
	{
		FMotionEventList.add(handler);
		handler(this, ceHOOKED);
	}
	else
		SF_RTTI_NOTIFY(DO_DEFAULT, "Handler was already assigned!")
}

void TController::UnhookEventHandler(TControllerEvent handler)
{
	if (!FMotionEventList.detach(handler))
		SF_RTTI_NOTIFY(DO_DEFAULT, "Handler was not hooked!")
	else
		handler(this, ceUNHOOKED);
}

void TController::SendEvent(EControllerEvent ce)
{
	// Make a local copy of the event list so that handled events can not disturb content of the list.
	TEventList el = FMotionEventList;
	for (unsigned i = 0; i < el.count(); i++)
		el[i](this, ce);
}

void TController::CallParamHook(int id)
{
	if (ParamNotifyProc)
		ParamNotifyProc(ParamNotifyData, id);
	if (id == GetParamId(mpMOVEPOS) && IsMovePosCompleted())
		SendEvent(ceCOMPLETE);
}

void TController::CallResultHook(int id)
{
	if (ResultNotifyProc)
		ResultNotifyProc(ResultNotifyData, id);
}

struct
{
		const char* Name;
		const char* Description;
} ReferenceTable[] =
	{
		{"N/A", "Axis is not available at the driver impementation"},
		{"X", "Lineair and moves in [m]"},
		{"Y", "Lineair and moves in [m]"},
		{"Z", "Lineair and moves in [m]"},
		{"A", "Angular Gimble and moves [rad]"},
		{"B", "Angular Swiffel and moves [rad]"},
		{"C", "Angular Turntable and moves [rad and/or Hz]"},
		{"D", "Tool Angular and moves [rad]"},
		{"E", "Tool Lineair and moves in [m]"},
		{"AUX1", "Auxiiary axis 1 has no location in the constallation of axes"},
		{"AUX2", "Auxiiary axis 2 has no location in the constallation of axes"},
		{"AUX3", "Auxiiary axis 3 has no location in the constallation of axes"},
		{"AUX4", "Auxiiary axis 4 has no location in the constallation of axes"},
};

const char* TController::GetAxisName(int axis_loc)
{
	size_t index = axis_loc + 1;
	if (index >= std::size(ReferenceTable))
	{
		throw std::range_error(SF_RTTI_NAME(TController) + "::" + __FUNCTION__ + "() invalid axis location!");
	}
	return ReferenceTable[index].Name;
}

const char* TController::GetAxisDescription(int axis_loc)
{
	size_t index = axis_loc + 1;
	if (index >= std::size(ReferenceTable))
	{
		throw std::range_error(SF_RTTI_NAME(TController) + "::" + __FUNCTION__ + "() invalid axis location!");
	}
	return ReferenceTable[index].Name;
}

const char* TController::GetStatusName(EStatus status)
{
	switch (status)
	{
		case csERROR:
			return "ERROR";
		case csUNINIT:
			return "UNINIT";
		case csUNHOMED:
			return "UNHOMED";
		case csINIT:
			return "INIT";
		case csREADY:
			return "READY";
		default:
			return "Unknown";
	}
}

void TController::AddPropertyPages(PropertySheetDialog* sheet)
{
	auto pp = new PropertyPage(sheet);
	sheet->addPage(pp);
}

bool TController::Uninitialize()
{
	// Uninitialize the hardware.
	if (DoInitialize(false))
	{
		// Can only go back to the status not initialized when
		// the status is larger than the state initialized.
		if (FStatus >= csINIT)
			SetStatus(csUNINIT);
		return true;
	}
	return false;
}

bool TController::Initialize()
{// Prevent initialize from being called twice.
	if (FStatus == csUNINIT)
	{
		// Try to initialize the hardware driver.
		if (DoInitialize(true))
		{
			// Update the status to initialize.
			SetStatus(csINIT);
			// Read the implementation driver settings from profile.
			if (ReadWriteSettings(true))
			{
				// Signal succes of initialization.
				return true;
			}
		}
	}
	return false;
}

void TController::SetStatus(EStatus status)
{
	if (FStatus != status)
	{
		FStatus = status;
		// Send the status change event to all hooked handlers.
		SendEvent(ceSTATUS);
	}
	// Make the attached interface check changes of the state.
	CallParamHook(0);
}

bool TController::HomeAxes(bool skip)
{// Check if with the current status the axes can be homed.
	if (FStatus >= csINIT)
	{
		// Call the implemented home function which should set the status to ready.
		// or at a later stage.
		if (DoHomeAxes(skip))
			// Return success of starting homing function.
			return true;

		// When homing failed check the current status.
		if (FStatus == csERROR)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Homing function resulted in error!")
		}
		else
			SetStatus(csUNHOMED);
		// Signal failure.
		return false;
	}
	SF_RTTI_NOTIFY(DO_DEFAULT, "Axes homing failed because of controller status!")
	// Signal failure to home axes.
	return false;
}

bool TController::DoHomeAxes(bool)
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "Homing procedure not implemented!")
	return false;
}

bool TController::SetParam(int id, const Value& value, bool skip_event)
{
	// When skip event is true simple handling.
	bool rv = HandleParam(id, nullptr, &value, nullptr);
	// Call the hook interface handler when the parameter was set successfully.
	if (!skip_event && rv)
		CallParamHook(id);
	//
	return rv;
}

bool TController::SetParam(
	EParam param,
	unsigned axis,
	const Value& value,
	bool skip_event
)
{
	// Get the local ID of the default parameter.
	int id = GetParamId(param, axis);
	// Check the ID for non-existence.
	if (id)
		return SetParam(id, value, skip_event);
	// Signal ID failure.
	SF_RTTI_NOTIFY(DO_DEFAULT, "SetParam: param" << param << " of axis: " << GetAxisName(axis) << " is not present!")
	return false;
}

bool TController::GetParam(EParam param, unsigned axis, Value& value) const
{
	// Get the local ID of the default parameter.
	int id = GetParamId(param, axis);
	// Check the ID for non-existence.
	if (id)
		return GetParam(id, value);
	SF_RTTI_NOTIFY(DO_DEFAULT, "GetParam: param" << param << " of axis: " << GetAxisName(axis) << " is not present!")
	return false;
}

bool TController::SetGetParam(int id, Value& value, bool skip_event)
{
	// When skip event is true simple handling.
	if (HandleParam(id, nullptr, &value, &value))
	{
		// Call the hook interface handler when the parameter was set successfully.
		if (!skip_event)
			CallParamHook(id);
		// Signal succes.
		return true;
	}
	SF_RTTI_NOTIFY(DO_DEFAULT, "SetGetParam: Setting the param " << id << " failed!")
	// On failure to set the parameter retrieve the current value.
	HandleParam(id, NULL, NULL, &value);
	// Signal failure.
	return false;
}

bool TController::GetCurrent(EAxisValueType avt, TAxesCoord& coord) const
{
	// Clear all values ins the passed coordinate.
	coord.Clear();
	// Iterate through the implemented axes.
	for (size_t i = 0; i < FAxes.count(); i++)
		coord << TAxisValue(FAxes[i]->GetLocation(), FAxes[i]->GetCurrent(avt));
	// Signal success.
	return true;
}

bool TController::SetCurrent(EAxisValueType avt, const TAxesCoord& coord)
{
	// Initialize return value.
	bool rv = true;
	// Iterate through the implemented axes.
	for (size_t i = 0; i < FAxes.count(); i++)
	{// Get the axis location.
		int loc = FAxes[i]->FLocation;
		// Ignore unset locations.
		if (coord.IsSet(loc))
		{// Set the axis value.
			if (!FAxes[i]->SetTarget(avt, coord[loc].value))
				rv = false;
		}
	}
	// Signal success or failure.
	return rv;
}

bool TController::GetAccuracy(TAxesCoord& accuracy) const
{
	accuracy.Clear();
	// Initialize return value.
	bool rv = true;
	// Temp value for storage.
	Value value;
	// Iterate through the implemented axes.
	for (size_t i = 0; i < FAxes.count(); i++)
	{
		// Get the axis location.
		EAxisLocation loc = FAxes[i]->FLocation;
		// Check if the parameter is available.
		if (GetParam(mpAXIS_ACCURACY, loc, value))
		{
			// Set the axis value.
			accuracy.Set(loc, value.getFloat());
		}
		else
		{
			// Failed to get this parameter.
			rv = false;
		}
	}
	// Signal success or failure.
	return rv;
}

bool TController::GetResolution(TAxesCoord& resolution) const
{
	resolution.Clear();
	// Initialize return value.
	bool rv = true;
	// Temp value for storage.
	Value value;
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < FAxes.count(); i++)
	{
		// Get the axis location.
		unsigned int loc = FAxes[i]->FLocation;
		// Check if the parameter is available.
		if (GetParam(mpAXIS_ACCURACY, loc, value))
			// Set the axis value.
			resolution.Set(loc, value.getFloat());
		else
			// Failed to get this parameter.
			rv = false;
	}
	// Signal success or failure.
	return rv;
}

bool TController::GetTarget(EAxisValueType avt, TAxesCoord& coord) const
{
	// Clear all values ins the passed coordinate.
	coord.Clear();
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < FAxes.count(); i++)
		coord << TAxisValue(FAxes[i]->GetLocation(), FAxes[i]->GetTarget(avt));
	// Signal success.
	return true;
}

bool TController::GetMinMax(EAxisMinMax amm, TAxesCoord& coord) const
{
	// Clear all values ins the passed coordinate.
	coord.Clear();
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < FAxes.count(); i++)
		coord << TAxisValue(FAxes[i]->GetLocation(), FAxes[i]->GetMinMax(amm));
	// Signal success.
	return true;
}

bool TController::SetTarget(EAxisValueType avt, const TAxesCoord& coord)
{
	// Initialize return value.
	bool rv = true;
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < FAxes.count(); i++)
	{// Get the axis location.
		int loc = FAxes[i]->FLocation;
		// Ignore unset locations.
		if (coord.IsSet(loc))
		{// Set the axis value.
			if (!FAxes[i]->SetTarget(avt, coord[loc].value))
				rv = false;
		}
	}
	// Signal success or failure.
	return rv;
}

bool TController::SetTarget(
	const TAxesCoord& pos,
	const TAxesCoord& vel,
	const TAxesCoord& acc,
	bool linear
)
{
	bool rv = true;
	if (linear)
	{
		double trgtime = 0;
		TAxesCoord dist, trgvel, trgacc;
		// Get current position for linear calculations.
		rv &= GetCurrent(avtPOSITION, dist);
		// Check if getting the current position was successful.
		if (rv)
		{
			// Allow only axes specified in pos to be used in calculation.
			dist.SetMap(pos.GetMap());
			// Subtract the passed position to get the distance for calculation.
			dist -= pos;
			// Disable axis on basis of the axis accuracy to prevent no proper calculations.
			// Also on the velocity must be non-zero.
			for (int i = alFirst; i < alLAST_ENTRY; i++)
			{
				if (std::fabs(vel.Value(i)) < std::numeric_limits<double>::min())
					dist.Unset(i);
				if (dist.IsSet(i) && fabs(dist.Value(i)) < GetAxis(i).GetAccuracy())
					dist.Unset(i);
			}
			// Get values for a linear profile.
			if (getLinearValues(dist, vel, acc, trgvel, trgacc, trgtime))
			{
				// For all axis that do not move set the passed velocity and acceleration.
				// This to prevent the controller to stop at all.
				for (int i = alFirst; i < alLAST_ENTRY; i++)
					if (pos.IsSet(i) && !dist.IsSet(i))
					{
						trgvel << vel[i];
						trgacc << acc[i];
					}
				// Set the targets for linear movement.
				rv &= SetTarget(avtVELOCITY, trgvel);
				rv &= SetTarget(avtACCELERATION, trgacc);
				rv &= SetTarget(avtPOSITION, pos);
				return rv;
			}
		}
		SF_RTTI_NOTIFY(DO_DEFAULT, "Linear calculation failed switching to regular!")
	}
	// On failure to calculate linear positions or by default.
	rv &= SetTarget(avtVELOCITY, vel);
	rv &= SetTarget(avtACCELERATION, acc);
	rv &= SetTarget(avtPOSITION, pos);
	// Position is the same for both cases.
	return rv;
}

bool TController::SetOffset(const TAxesCoord& coord)
{
	// Initialize return value.
	bool rv = true;
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < FAxes.count(); i++)
	{
		// Get the axis location.
		int loc = FAxes[i]->FLocation;
		// Ignore unset locations.
		if (coord.IsSet(loc))
		{
			// Set the axis value.
			if (!FAxes[i]->SetOffset(coord[loc].value))
				rv = false;
		}
	}
	// Signal success or failure.
	return rv;
}

bool TController::GetOffset(TAxesCoord& coord) const
{
	// Clear all values ins the passed coordinate.
	coord.Clear();
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < FAxes.count(); i++)
		coord << TAxisValue(FAxes[i]->GetLocation(), FAxes[i]->GetOffset());
	// Signal success.
	return true;
}

bool TController::SetMode(EAxisMode am, const TAxesCoord& ac, EAxisMode amdef)
{
	bool rv = true;
	for (size_t i = 0; i < FAxes.count(); i++)
	{
		// When an implemented axis does not exist in the coord it set to the default.
		if (!FAxes[i]->SetMode(ac.IsSet(FAxes[i]->FLocation) ? am : amdef))
		{
			// On failure set the return value to false.
			rv = false;
		}
	}
	// Return succes or failure.
	return rv;
}

bool TController::SetMovePos(EMovePosCmd mpc)
{
	// Cannot complete by user of interface.
	if (mpc == mpcCOMPLETE || (!IsReady() && mpc != mpcABORT))
	{
		return false;
	}
	// Get the motion parameter id.
	auto id = GetParamId(mpMOVEPOS);
	Value value(mpc);
	// Check if the parameter is available.
	if (!SetGetParam(id, value, false))
		return false;
	else
	{// It could be that the controller is already on
		// its target and the gotten value is equal to Complete.
		if (value.getInteger() != mpc && value.getInteger() != mpcCOMPLETE)
		{
			return false;
		}
	}
	//
	return true;
}

bool TController::SetMoveVel(EMoveVelCmd mvc)
{
	// Cannot switch to off by user of interface.
	if (mvc == mvcCOMPLETE || !IsReady())
	{
		return false;
	}
	// Get the motion parameter id.
	int id = GetParamId(mpMOVEVEL);
	Value value(mvc);
	// Check if the parameter is available.
	if (!SetGetParam(id, value, false))
	{
		return false;
	}
	else if (value.getInteger() != mvc)
	{
		return false;
	}
	return true;
}

bool TController::SetMoveCon(EMoveConCmd mcc)
{
	// Cannot switch to off by user of interface.
	if (mcc == mccCOMPLETE || !IsReady())
	{
		return false;
	}
	// Get the motion parameter id.
	int id = GetParamId(mpMOVECON);
	Value value(mcc);
	// Check if the parameter is available.
	if (!SetGetParam(id, value, false))
	{
		return false;
	}
	else if (value.getInteger() != mcc)
	{
		return false;
	}
	return true;
}

EMovePosCmd TController::GetMovePos() const
{
	Value value;
	// Check if the parameter is available.
	if (!GetParam(mpMOVEPOS, UINT_MAX, value))
	{
		return mpcABORT;
	}
	return (EMovePosCmd) value.getInteger();
}

EMoveVelCmd TController::GetMoveVel() const
{
	Value value;
	// Check if the parameter is available.
	if (!GetParam(mpMOVEVEL, UINT_MAX, value))
	{
		return mvcABORT;
	}
	return (EMoveVelCmd) value.getInteger();
}

EMoveConCmd TController::GetMoveCon() const
{
	Value value;
	// Check if the parameter is available.
	if (!GetParam(mpMOVECON, UINT_MAX, value))
	{
		return mccABORT;
	}
	return (EMoveConCmd) value.getInteger();
}

bool TController::SetJoystick(EJoystickCmd jsc)
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(jsc);
		if (SetParam(mpJOYSTICK, UINT_MAX, value, false))
		{
			if (GetParam(mpJOYSTICK, UINT_MAX, value))
				return jsc == (EJoystickCmd) value.getInteger();
		}
	}
	return false;
}

bool TController::SetJoystickAxis(EAxisLocation al)
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(al);
		if (SetParam(mpJOYSTICK_AXIS, UINT_MAX, value, false))
		{
			if (GetParam(mpJOYSTICK_AXIS, UINT_MAX, value))
				return value.getInteger() == al;
		}
	}
	return false;
}

bool TController::SetPopAxis(EAxisLocation al)
{// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(al);
		if (SetParam(mpPOP_AXIS, UINT_MAX, value, false))
		{
			if (GetParam(mpPOP_AXIS, UINT_MAX, value))
			{
				return value.getInteger() == al;
			}
		}
	}
	return false;
}

bool TController::SetTriggerEnable(bool enable)
{// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(enable);
		if (SetParam(mpTRIGGER_ENABLE, UINT_MAX, value, false))
		{
			if (GetParam(mpTRIGGER_ENABLE, UINT_MAX, value))
			{
				return value.getInteger() == enable;
			}
		}
	}
	return false;
}

double TController::SetTriggerFreq(double freq)
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(freq);
		if (SetParam(mpTRIGGER_FREQ, UINT_MAX, value, false))
		{
			if (GetParam(mpTRIGGER_FREQ, UINT_MAX, value))
			{
				return value.getFloat();
			}
		}
	}
	return -1.0;
}

bool TController::SetTriggerAxis(EAxisLocation al)
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(al);
		if (SetParam(mpTRIGGER_AXIS, UINT_MAX, value, false))
		{
			if (GetParam(mpTRIGGER_AXIS, UINT_MAX, value))
			{
				return (value.getInteger() == al);
			}
		}
	}
	return false;
}

EAxisLocation TController::GetTriggerAxis() const
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value;
		if (GetParam(mpTRIGGER_AXIS, UINT_MAX, value))
		{
			return (EAxisLocation) value.getInteger();
		}
	}
	return alNA;
}

bool TController::SetTriggerDensity(double td)
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(td);
		if (SetParam(mpTRIGGER_DENSITY, UINT_MAX, value, false))
		{
			return true;
		}
	}
	return false;
}

double TController::GetTriggerDensity() const
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value;
		if (GetParam(mpTRIGGER_DENSITY, UINT_MAX, value))
		{
			return value.getFloat();
		}
	}
	return 0.0;
}

bool TController::SetTriggerMode(bool intern)
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(intern);
		if (SetParam(mpTRIGGER_MODE, UINT_MAX, value, false))
		{
			return true;
		}
	}
	return false;
}

bool TController::GetTriggerMode() const
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value;
		if (GetParam(mpTRIGGER_MODE, UINT_MAX, value))
		{
			return value.getInteger();
		}
	}
	return false;
}

bool TController::SetChuckJaw(EChuckJaw cj)
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value(cj);
		if (SetParam(mpCHUCKJAW, UINT_MAX, value, false))
		{
			return true;
		}
	}
	return false;
}

EChuckJaw TController::GetChuckJaw() const
{
	// Any status above un init is okay.
	if (FStatus > csUNINIT)
	{
		Value value;
		if (GetParam(mpCHUCKJAW, UINT_MAX, value))
		{
			return (EChuckJaw) value.getInteger();
		}
	}
	return cjOFF;
}

EJoystickCmd TController::GetJoystick() const
{
	Value value(jscOFF);
	if (GetParam(mpJOYSTICK, UINT_MAX, value))
	{
		return (EJoystickCmd) value.getInteger();
	}
	return jscOFF;
}

EAxisLocation TController::GetJoystickAxis() const
{
	Value value(-1);
	if (GetParam(mpJOYSTICK_AXIS, UINT_MAX, value))
	{
		return (EAxisLocation) value.getInteger();
	}
	return alNA;
}

EAxisLocation TController::GetPopAxis() const
{
	Value value(-1);
	if (GetParam(mpPOP_AXIS, UINT_MAX, value))
	{
		return (EAxisLocation) value.getInteger();
	}
	return alNA;
}

bool TController::GetTriggerEnable() const
{
	Value value(-1);
	if (GetParam(mpTRIGGER_ENABLE, UINT_MAX, value))
	{
		return value.getInteger();
	}
	return true;
}

double TController::GetTriggerFreq() const
{
	Value value(-1.0);
	if (GetParam(mpTRIGGER_FREQ, UINT_MAX, value))
	{
		return value.getFloat();
	}
	return -1.0;
}

std::string TController::GetProfilePath() const
{
	return getConfigLocation({}, true) + "gmi-iface.cfg";
}

bool TController::SetPosition(EAxisLocation al, double value)
{
	// Any status above uninitialized is okay.
	if (FStatus > csUNINIT && al != alNA)
	{
		if (DoSetPosition(al, value))
		{
			// Write the new home offset along with all other system settings.
			if (!ReadWriteSettings(false))
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, "Could not save home offset!")
			}
			return true;
		}
	}
	return false;
}

bool TController::SetPosition(const TAxesCoord& ac)
{
	// Initialize return value.
	bool rv = true;
	if (FStatus > csUNINIT)
	{
		// Iterate through the implemented axes.
		for (unsigned i = 0; i < FAxes.count(); i++)
		{
			// Get the axis location.
			int loc = FAxes[i]->FLocation;
			// Ignore unset locations.
			if (ac.IsSet(loc))
			{// Set the axis value.
				if (!FAxes[i]->SetPosition(ac[loc].value))
					rv = false;
			}
		}
	}
	// Only save on success.
	if (rv)
	{
		// Write the new home offset along with all other system settings.
		if (!ReadWriteSettings(false))
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Could not save home offset!")
		}
	}
	// Signal success or failure.
	return rv;
}

bool TController::DoSetPosition(EAxisLocation, double)
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "DoSetPosition is not implemented!")
	return false;
}

TAxesCoord& TController::Normalize(TAxesCoord& pos) const
{
	// For all set axis normalize the passed position.
	for (int i = alFirst; i < alLAST_ENTRY; i++)
		if (pos.IsSet(i))
			pos.Value(i) = GetAxis(i).Normalized(pos.Value(i));
	return pos;
}

AxisLocations TController::GetRadialUnlimted() const
{
	AxisLocations als;
	AxisMovements am;
	am << amRADIAL;
	for (int i = alFirst; i < alLAST_ENTRY; i++)
		if (GetAxis(i).FMovements == am)
			als << static_cast<EAxisLocation>(i);
	return als;
}

bool TController::ReadWriteSettings(bool rd)
{
	IniProfile profile("", GetProfilePath().c_str());
	TIdList ids;
	// Get the list of valid ID's.
	bool retval = EnumParamIds(ids);
	// Iterate through the list twice when reading parameters.
	// This is to be shure the order of parameters being set is not a real problem.1
	//  for (unsigned n = 0; n < (rd ? 2 : 1); n++)
	for (unsigned i = 0; i < ids.count(); i++)
	{
		TParamInfo info;
		// Get the parameter info.
		if (GetParamInfo(ids[i], info))
		{
			// Check if the variable is a system setting parameter.
			if (info.Flags & pfSYSTEM)
			{
				// Set the section based on the axis.
				if (info.Flags & pfAXIS)
					profile.setSection(stringf("%s-Axis", GetAxisName(info.Axis)).c_str());
				else
					profile.setSection("General");
				// If no name exists use the ID.
				if (!info.Name.length())
					info.Name = stringf("0x%X", info.Id);
				if (rd)
				{
					// Read the value info value class.
					Value val(profile.getString(info.Name.c_str(), info.Default.getString().c_str()));
					// Write the parameter to the interface.
					if (!SetParam(ids[i], val, false))
						retval = false;
				}
				else
				{
					// Temporary value for storage.
					Value val;
					// Get the parameter.
					if (GetParam(ids[i], val))
						// Write it to the parameter value to the profile.
						profile.setString(info.Name.c_str(), val.getString().c_str());
					else
						retval = false;
				}
			}
		}
	}
	// Signal if something went wrong.
	return retval;
}

//
// Controller::Axis methods.
//

TController::TAxis::TAxis(TController* mc, EAxisLocation al)
	: FController(*mc)
	, FLocation(al)
{
	// Register the axis at the controller.
	mc->Attach(this);
}

TController::TAxis::~TAxis()
{
	if (std::uncaught_exceptions() == 0)
	{
		// Major error. Cannot destruct an TAxis instance out side the controller.
		assert(FController.FlagDestroying);
	}
}

double TController::TAxis::GetCurrent(EAxisValueType avt) const
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		static EParam mps[] = {mpNONE, mpAXIS_CUR_POS, mpAXIS_CUR_VEL};
		// When out of range return 0.0;
		if (avt < std::size(mps))
		{// Temp value to store it in.
			Value value;
			// Check if the parameter is available.
			if (FController.GetParam(mps[avt], FLocation, value))
			{
				return value.getFloat();
			}
		}
	}
	return 0.0;
}

bool TController::TAxis::SetCurrent(EAxisValueType avt, double val)
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		static EParam mps[] = {mpNONE, mpAXIS_CUR_POS, mpAXIS_CUR_VEL};
		// When out of range return 0.0;
		if (avt < std::size(mps))
		{
			// Check if the parameter is available.
			if (FController.SetParam(mps[avt], FLocation, Value(val), false))
			{
				// Return the retrieved value.
				return true;
			}
		}
	}
	return false;
}

double TController::TAxis::GetMinMax(EAxisMinMax amm) const
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		static EParam mps[] =
			{
				mpAXIS_MIN_POS,
				mpAXIS_MAX_POS,
				mpAXIS_MAX_VEL,
				mpAXIS_MAX_ACC,
			};
		// When out of range return 0.0;
		if (amm < std::size(mps))
		{
			// Temp value for storage.
			Value value;
			// Check if the parameter is available.
			if (FController.GetParam(mps[amm], FLocation, value))
				return value.getFloat();
		}
	}
	return 0.0;
}

double TController::TAxis::GetTarget(EAxisValueType avt) const
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		static EParam mps[] = {mpNONE, mpAXIS_TRG_POS, mpAXIS_TRG_VEL, mpAXIS_TRG_ACC};
		// When out of range return 0.0;
		if (avt < std::size(mps))
		{
			// Temp value for storage.
			Value value;
			// Check if the parameter is available.
			if (FController.GetParam(mps[avt], FLocation, value))
				return value.getFloat();
		}
	}
	return 0.0;
}

bool TController::TAxis::SetTarget(EAxisValueType avt, double val)
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		static EParam mps[] = {mpNONE, mpAXIS_TRG_POS, mpAXIS_TRG_VEL, mpAXIS_TRG_ACC};
		// When out of range return 0.0;
		if (avt < std::size(mps))
		{
			// Set the parameter.
			return FController.SetParam(mps[avt], FLocation, Value(val), false);
		}
	}
	return false;
}

bool TController::TAxis::SetOffset(double ofs)
{
	// Set the parameter.
	return FController.SetParam(mpAXIS_OFS_POS, FLocation, Value(ofs), false);
}

double TController::TAxis::GetOffset() const
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		// Temp value for storage.
		Value value;
		// Check if the parameter is available.
		if (FController.GetParam(mpAXIS_OFS_POS, FLocation, value))
			return value.getFloat();
	}
	return 0.0;
}

double TController::TAxis::GetAccuracy() const
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		// Temp value for storage.
		Value value;
		// Check if the parameter is available.
		if (FController.GetParam(mpAXIS_ACCURACY, FLocation, value))
			return value.getFloat();
	}
	return 0.0;
}

double TController::TAxis::GetResolution() const
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		// Temp value for storage.
		Value value;
		// Check if the parameter is available.
		if (FController.GetParam(mpAXIS_RESOLUTION, FLocation, value))
			return value.getFloat();
	}
	return 1.0;
}

bool TController::TAxis::CanModeChange(EAxisMode curmode, EAxisMode mode, bool pos_cmplt, bool vel_cmplt, bool con_cmplt)
{
	// Check what the new mode will be and if is allowed to change.
	switch (mode)
	{
		default:
			return false;

		case amHOME:
			// All axis must be not moving before a mode can be changed.
			if (!pos_cmplt || !vel_cmplt || !con_cmplt)
				return false;
			return true;

		case amDISABLED:
			switch (curmode)
			{
				default:
					return false;
				case amPOSITION:
				case amHOME:
					if (!pos_cmplt)
						return false;
					break;
				case amVELOCITY:
					// Only when velocity mode is complete.
					if (!vel_cmplt)
						return false;
					break;
				case amCONTINUE:
					// Only when velocity mode is complete.
					if (!con_cmplt)
						return false;
					break;
			}
			break;

		case amPOSITION:
			switch (curmode)
			{
				default:
					return false;
				case amDISABLED:
				case amHOME:
					// Only when position move is complete.
					if (!pos_cmplt)
						return false;
					break;
				case amVELOCITY:
					// Only when not moving at all can be switched from
					// velocity to position mode.
					if (!pos_cmplt || !vel_cmplt)
						return false;
					break;
				case amCONTINUE:
					// Only when velocity mode is complete.
					if (!pos_cmplt || !con_cmplt)
						return false;
					break;
			}
			break;

		case amVELOCITY:
			switch (curmode)
			{
				default:
					return false;
				case amDISABLED:
					// Only when not moving in velocity mode.
					if (!vel_cmplt)
						return false;
					break;
				case amPOSITION:
				case amHOME:
					// Only when not moving at all.
					if (!pos_cmplt || !vel_cmplt)
						return false;
					break;
				case amCONTINUE:
					// Only when velocity mode is complete.
					if (!con_cmplt || !vel_cmplt)
						return false;
					break;
			}
			break;

		case amCONTINUE:
			switch (curmode)
			{
				default:
					return false;
				case amDISABLED:
					// Only when not moving in velocity mode.
					if (!con_cmplt)
						return false;
					break;
				case amPOSITION:
				case amHOME:
					// Only when not moving at all.
					if (!pos_cmplt || !con_cmplt)
						return false;
					break;
				case amVELOCITY:
					// Only when velocity mode is complete.
					if (!vel_cmplt || !con_cmplt)
						return false;
					break;
			}
			break;
	}
	return true;
}

bool TController::TAxis::SetMode(EAxisMode mode)
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		Value value(0);
		// Get the parameter to see if it needs changing.
		if (FController.GetParam(mpAXIS_MODE, FLocation, value))
		{
			// Get the current mode.
			EAxisMode cur_mode = static_cast<EAxisMode>(value.getInteger());
			// Compare current mode for a change.
			if (cur_mode != mode)
			{
				// Check if a change is allowed.
				bool ok = CanModeChange(
					cur_mode, mode,
					FController.IsMovePosCompleted(),
					FController.IsMoveVelCompleted(),
					FController.IsMoveConCompleted()
				);
				//
				if (!ok)
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to change axis " << GetName() << " mode to [" << mode << "] during move!")
					return false;
				}
				// Assign the mode to the value so it can be assigned.
				value.set(mode);
				// Check if the parameter is available.
				return FController.SetParam(mpAXIS_MODE, FLocation, value, false);
			}
			// Signal success because there is no need for changing.
			return true;
		}
	}
	// Signal failure.
	return false;
}

EAxisMode TController::TAxis::GetMode() const
{
	// Test if the axis is a valid one.
	if (FLocation != alNA)
	{
		Value value(0);
		// Check if the parameter is available.
		FController.GetParam(mpAXIS_MODE, FLocation, value);
		//
		return static_cast<EAxisMode>(value.getInteger());
	}
	// By default, the axis is disabled.
	return amDISABLED;
}

bool TController::TAxis::SetPosition(double value)
{
	// Pass to controller.
	return FController.SetPosition(FLocation, value);
}

double TController::TAxis::Normalized(double pos) const
{
	// Is the axis radial and unlimited.
	if (FMovements == (AxisMovements() << amRADIAL))
	{// Bring the angle in range of 0..360 degree.
		pos = fmod(pos, sf::numbers::pi_v<Value::flt_type> * 2.0);
		// All angle near zero are zeroed.
		if (fabs(pos) >= sf::numbers::pi_v<Value::flt_type> * 2.0 - GetAccuracy())
			pos = 0.0;
		// Make the angle positive.
		if (pos < 0.0)
			pos += sf::numbers::pi_v<Value::flt_type> * 2.0;
	}
	return pos;
}

const char* TController::TAxis::GetName() const
{
	return FController.GetAxisName(FLocation);
}

const char* TController::TAxis::GetDescription() const
{
	return FController.GetAxisDescription(FLocation);
}

bool TController::IsMoving() const
{
	EMovePosCmd mpc = GetMovePos();
	EMoveVelCmd mvc = GetMoveVel();
	EMoveConCmd mcc = GetMoveCon();
	// When one axis has the abort state motion has stopped for shure.
	if (mpc == mpcABORT || mvc == mvcABORT || mcc == mccABORT)
		return GetJoystick() > jscOFF;
	// All axes have to be completed before no movement is assumed.
	if (mpc == mpcCOMPLETE && mvc == mvcCOMPLETE && mcc == mccCOMPLETE)
		return GetJoystick() > jscOFF;
	return true;
}

TController::TAxis& TController::GetAxis(int axis_loc)
{
	// Check if location index is in range.
	if (axis_loc >= alFirst && axis_loc < alLAST_ENTRY)
	{
		return *FAxesMap[axis_loc];
	}
	// Return default the null axis.
	return FNullAxis;
}

const TController::TAxis& TController::GetAxis(int axis_loc) const
{
	// Check if location index is in range.
	if (axis_loc >= alFirst && axis_loc < alLAST_ENTRY)
	{
		return *FAxesMap[axis_loc];
	}
	// Return default the null axis.
	return FNullAxis;
}

TController::TAxis& TController::GetPhysAxis(int axis_num)
{
	// Check if location index is in range.
	if (axis_num >= 0 && axis_num < static_cast<int>(FAxes.count()))
	{
		return *FAxes[axis_num];
	}
	// Return default the null axis.
	return FNullAxis;
}

}// namespace sf::gmi
