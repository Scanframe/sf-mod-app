#include "Controller.h"
#include "GmiUtils.h"
#include "misc/gen/ConfigLocation.h"
#include "misc/gen/IniProfile.h"

namespace sf::gmi
{

// Implementations of static functions and data members to be able to create registered stars implementations.
SF_IMPL_IFACE(Controller, Controller::Parameters, Interface)

//
// Motion controller class 'TController'
//
Controller::Controller(const Parameters&)
	: _nullAxis(this, alNA)
	, _status(csUNINIT)
	, _flagDestroying(false)
	, _paramNotifyProc(nullptr)
	, _resultNotifyProc(nullptr)
{
	// Initialize the axes map member using the NullAxis.
	for (int i = 0; i < std::size(_axesMap); i++)
	{
		_axesMap[i] = &_nullAxis;
	}
}

Controller::~Controller() {}

void Controller::attach(Axis* axis)
{
	// Ignore the null axis.
	if (axis != &_nullAxis)
	{
		// Add the axis to the vector of axes.
		_axes.add(axis);
		// Get the location.
		EAxisLocation al = axis->getLocation();
		// Check if the current instance is not set yet.
		if (_axesMap[al] != &_nullAxis)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Axis location '" << getAxisName(al) << "' already in use!")
		}
		else
		{
			// Map the axis pointer in the array.
			_axesMap[al] = axis;
		}
	}
}

void Controller::destroy()
{
	// Send the unhook signal.
	sendEvent(ceUNHOOKED);
	// Flush all the hooked events.
	_motionEventList.flush();
	// Uninitialize first the controller.
	uninitialize();
	// Set the destroy flag so no the TAxis implementations destruct errorless.
	_flagDestroying = true;
}

void Controller::hookEventHandler(ControllerEvent handler)
{
	// Cannot attach the handlers when destroying or when it is NULL.
	if (!handler || _flagDestroying) return;
	// Check if the handler already was assigned.
	if (_motionEventList.find(handler) == UINT_MAX)
	{
		_motionEventList.add(handler);
		handler(this, ceHOOKED);
	}
	else
		SF_RTTI_NOTIFY(DO_DEFAULT, "Handler was already assigned!")
}

void Controller::unhookEventHandler(ControllerEvent handler)
{
	if (!_motionEventList.detach(handler)) SF_RTTI_NOTIFY(DO_DEFAULT, "Handler was not hooked!")
	else
		handler(this, ceUNHOOKED);
}

void Controller::sendEvent(EControllerEvent ce)
{
	// Make a local copy of the event list so that handled events can not disturb content of the list.
	EventList el = _motionEventList;
	for (unsigned i = 0; i < el.count(); i++) el[i](this, ce);
}

void Controller::callParamHook(IdType id)
{
	if (_paramNotifyProc) _paramNotifyProc(_paramNotifyData, id);
	if (id == getParamId(mpMOVEPOS) && isMovePosCompleted()) sendEvent(ceCOMPLETE);
}

void Controller::callResultHook(IdType id)
{
	if (_resultNotifyProc) _resultNotifyProc(_resultNotifyData, id);
}

struct
{
		const char* Name;
		const char* Description;
} ReferenceTable[] = {
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

const char* Controller::getAxisName(int axis_loc)
{
	size_t index = axis_loc + 1;
	if (index >= std::size(ReferenceTable))
	{
		throw std::range_error(SF_RTTI_NAME(Controller) + "::" + __FUNCTION__ + "() invalid axis location!");
	}
	return ReferenceTable[index].Name;
}

const char* Controller::getAxisDescription(int axis_loc)
{
	size_t index = axis_loc + 1;
	if (index >= std::size(ReferenceTable))
	{
		throw std::range_error(SF_RTTI_NAME(Controller) + "::" + __FUNCTION__ + "() invalid axis location!");
	}
	return ReferenceTable[index].Name;
}

const char* Controller::getStatusName(EStatus status)
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

void Controller::addPropertyPages(PropertySheetDialog* sheet)
{
#if IS_QT
	auto pp = new PropertyPage(sheet);
	sheet->addPage(pp);
#endif
}

bool Controller::uninitialize()
{
	// Uninitialize the hardware.
	if (doInitialize(false))
	{
		// Can only go back to the status not initialized when
		// the status is larger than the state initialized.
		if (_status >= csINIT) setStatus(csUNINIT);
		return true;
	}
	return false;
}

bool Controller::initialize()
{// Prevent initialize from being called twice.
	if (_status == csUNINIT)
	{
		// Try to initialize the hardware driver.
		if (doInitialize(true))
		{
			// Update the status to initialize.
			setStatus(csINIT);
			// Read the implementation driver settings from profile.
			if (settingsReadWrite(true))
			{
				// Signal succes of initialization.
				return true;
			}
		}
	}
	return false;
}

void Controller::setStatus(EStatus status)
{
	if (_status != status)
	{
		_status = status;
		// Send the status change event to all hooked handlers.
		sendEvent(ceSTATUS);
	}
	// Make the attached interface check changes of the state.
	callParamHook(0);
}

bool Controller::homeAxes(bool skip)
{// Check if with the current status the axes can be homed.
	if (_status >= csINIT)
	{
		// Call the implemented home function which should set the status to ready.
		// or at a later stage.
		if (doHomeAxes(skip))
			// Return success of starting homing function.
			return true;

		// When homing failed check the current status.
		if (_status == csERROR)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Homing function resulted in error!")
		}
		else
			setStatus(csUNHOMED);
		// Signal failure.
		return false;
	}
	SF_RTTI_NOTIFY(DO_DEFAULT, "Axes homing failed because of controller status!")
	// Signal failure to home axes.
	return false;
}

bool Controller::doHomeAxes(bool)
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "Homing procedure not implemented!")
	return false;
}

bool Controller::setParam(IdType id, const Value& value, bool skip_event)
{
	// When skip event is true simple handling.
	bool rv = handleParam(id, nullptr, &value, nullptr);
	// Call the hook interface handler when the parameter was set successfully.
	if (!skip_event && rv) callParamHook(id);
	//
	return rv;
}

bool Controller::setParam(EParam param, int axis, const Value& value, bool skip_event)
{
	// Get the local ID of the default parameter.
	IdType id = getParamId(param, axis);
	// Check the ID for non-existence.
	if (id) return setParam(id, value, skip_event);
	// Signal ID failure.
	SF_RTTI_NOTIFY(DO_DEFAULT, "setParam: param" << param << " of axis: " << getAxisName(axis) << " is not present!")
	return false;
}

bool Controller::getParam(EParam param, int axis, Value& value) const
{
	// Get the local ID of the default parameter.
	IdType id = getParamId(param, axis);
	// Check the ID for non-existence.
	if (id) return getParam(id, value);
	SF_RTTI_NOTIFY(DO_DEFAULT, "getParam: param" << param << " of axis: " << getAxisName(axis) << " is not present!")
	return false;
}

bool Controller::setGetParam(IdType id, Value& value, bool skip_event)
{
	// When skip event is true simple handling.
	if (handleParam(id, nullptr, &value, &value))
	{
		// Call the hook interface handler when the parameter was set successfully.
		if (!skip_event) callParamHook(id);
		// Signal succes.
		return true;
	}
	SF_RTTI_NOTIFY(DO_DEFAULT, "setGetParam: Setting the param " << id << " failed!")
	// On failure to set the parameter retrieve the current value.
	handleParam(id, NULL, NULL, &value);
	// Signal failure.
	return false;
}

bool Controller::getCurrent(EAxisValueType avt, AxesCoord& ac) const
{
	// Clear all values ins the passed coordinate.
	ac.clear();
	// Iterate through the implemented axes.
	for (size_t i = 0; i < _axes.count(); i++) ac << AxisValue(_axes[i]->getLocation(), _axes[i]->getCurrent(avt));
	// Signal success.
	return true;
}

bool Controller::setCurrent(EAxisValueType avt, const AxesCoord& coord)
{
	// Initialize return value.
	bool rv = true;
	// Iterate through the implemented axes.
	for (size_t i = 0; i < _axes.count(); i++)
	{// Get the axis location.
		int loc = _axes[i]->_location;
		// Ignore unset locations.
		if (coord.isSet(loc))
		{// Set the axis value.
			if (!_axes[i]->setTarget(avt, coord[loc]._value)) rv = false;
		}
	}
	// Signal success or failure.
	return rv;
}

bool Controller::getAccuracy(AxesCoord& accuracy) const
{
	accuracy.clear();
	// Initialize return value.
	bool rv = true;
	// Temp value for storage.
	Value value;
	// Iterate through the implemented axes.
	for (size_t i = 0; i < _axes.count(); i++)
	{
		// Get the axis location.
		EAxisLocation loc = _axes[i]->_location;
		// Check if the parameter is available.
		if (getParam(mpAXIS_ACCURACY, loc, value))
		{
			// Set the axis value.
			accuracy.set(loc, value.getFloat());
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

bool Controller::getResolution(AxesCoord& resolution) const
{
	resolution.clear();
	// Initialize return value.
	bool rv = true;
	// Temp value for storage.
	Value value;
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < _axes.count(); i++)
	{
		// Get the axis location.
		unsigned int loc = _axes[i]->_location;
		// Check if the parameter is available.
		if (getParam(mpAXIS_ACCURACY, loc, value))
			// Set the axis value.
			resolution.set(loc, value.getFloat());
		else
			// Failed to get this parameter.
			rv = false;
	}
	// Signal success or failure.
	return rv;
}

bool Controller::getTarget(EAxisValueType avt, AxesCoord& coord) const
{
	// Clear all values ins the passed coordinate.
	coord.clear();
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < _axes.count(); i++) coord << AxisValue(_axes[i]->getLocation(), _axes[i]->getTarget(avt));
	// Signal success.
	return true;
}

bool Controller::getMinMax(EAxisMinMax amm, AxesCoord& coord) const
{
	// Clear all values ins the passed coordinate.
	coord.clear();
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < _axes.count(); i++) coord << AxisValue(_axes[i]->getLocation(), _axes[i]->getMinMax(amm));
	// Signal success.
	return true;
}

bool Controller::setTarget(EAxisValueType avt, const AxesCoord& coord)
{
	// Initialize return value.
	bool rv = true;
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < _axes.count(); i++)
	{// Get the axis location.
		int loc = _axes[i]->_location;
		// Ignore unset locations.
		if (coord.isSet(loc))
		{// Set the axis value.
			if (!_axes[i]->setTarget(avt, coord[loc]._value)) rv = false;
		}
	}
	// Signal success or failure.
	return rv;
}

bool Controller::setTarget(const AxesCoord& pos, const AxesCoord& vel, const AxesCoord& acc, bool linear)
{
	bool rv = true;
	if (linear)
	{
		double trgtime = 0;
		AxesCoord dist, trgvel, trgacc;
		// Get current position for linear calculations.
		rv &= getCurrent(avtPOSITION, dist);
		// Check if getting the current position was successful.
		if (rv)
		{
			// Allow only axes specified in pos to be used in calculation.
			dist.setMap(pos.getMap());
			// Subtract the passed position to get the distance for calculation.
			dist -= pos;
			// Disable axis on basis of the axis accuracy to prevent no proper calculations.
			// Also on the velocity must be non-zero.
			for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
			{
				if (std::fabs(vel.getValue(i)) < std::numeric_limits<double>::min()) dist.unset(i);
				if (dist.isSet(i) && fabs(dist.getValue(i)) < getAxis(i).getAccuracy()) dist.unset(i);
			}
			// Get values for a linear profile.
			if (getLinearValues(dist, vel, acc, trgvel, trgacc, trgtime))
			{
				// For all axis that do not move set the passed velocity and acceleration.
				// This to prevent the controller to stop at all.
				for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
					if (pos.isSet(i) && !dist.isSet(i))
					{
						trgvel << vel[i];
						trgacc << acc[i];
					}
				// Set the targets for linear movement.
				rv &= setTarget(avtVELOCITY, trgvel);
				rv &= setTarget(avtACCELERATION, trgacc);
				rv &= setTarget(avtPOSITION, pos);
				return rv;
			}
		}
		SF_RTTI_NOTIFY(DO_DEFAULT, "Linear calculation failed switching to regular!")
	}
	// On failure to calculate linear positions or by default.
	rv &= setTarget(avtVELOCITY, vel);
	rv &= setTarget(avtACCELERATION, acc);
	rv &= setTarget(avtPOSITION, pos);
	// Position is the same for both cases.
	return rv;
}

bool Controller::setOffset(const AxesCoord& coord)
{
	// Initialize return value.
	bool rv = true;
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < _axes.count(); i++)
	{
		// Get the axis location.
		int loc = _axes[i]->_location;
		// Ignore unset locations.
		if (coord.isSet(loc))
		{
			// Set the axis value.
			if (!_axes[i]->setOffset(coord[loc]._value)) rv = false;
		}
	}
	// Signal success or failure.
	return rv;
}

bool Controller::getOffset(AxesCoord& coord) const
{
	// Clear all values ins the passed coordinate.
	coord.clear();
	// Iterate through the implemented axes.
	for (unsigned i = 0; i < _axes.count(); i++) coord << AxisValue(_axes[i]->getLocation(), _axes[i]->setOffset());
	// Signal success.
	return true;
}

bool Controller::setMode(EAxisMode am, const AxesCoord& ac, EAxisMode amdef)
{
	bool rv = true;
	for (size_t i = 0; i < _axes.count(); i++)
	{
		// When an implemented axis does not exist in the coord it set to the default.
		if (!_axes[i]->setMode(ac.isSet(_axes[i]->_location) ? am : amdef))
		{
			// On failure set the return value to false.
			rv = false;
		}
	}
	// Return succes or failure.
	return rv;
}

bool Controller::setMovePos(EMovePosCmd mpc)
{
	// Cannot complete by user of interface.
	if (mpc == mpcCOMPLETE || (!isReady() && mpc != mpcABORT))
	{
		return false;
	}
	// Get the motion parameter id.
	auto id = getParamId(mpMOVEPOS);
	Value value(mpc);
	// Check if the parameter is available.
	if (!setGetParam(id, value, false)) return false;
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

bool Controller::setMoveVel(EMoveVelCmd mvc)
{
	// Cannot switch to off by user of interface.
	if (mvc == mvcCOMPLETE || !isReady())
	{
		return false;
	}
	// Get the motion parameter id.
	IdType id = getParamId(mpMOVEVEL);
	Value value(mvc);
	// Check if the parameter is available.
	if (!setGetParam(id, value, false))
	{
		return false;
	}
	else if (value.getInteger() != mvc)
	{
		return false;
	}
	return true;
}

bool Controller::setMoveCon(EMoveConCmd mcc)
{
	// Cannot switch to off by user of interface.
	if (mcc == mccCOMPLETE || !isReady())
	{
		return false;
	}
	// Get the motion parameter id.
	IdType id = getParamId(mpMOVECON);
	Value value(mcc);
	// Check if the parameter is available.
	if (!setGetParam(id, value, false))
	{
		return false;
	}
	else if (value.getInteger() != mcc)
	{
		return false;
	}
	return true;
}

EMovePosCmd Controller::getMovePos() const
{
	Value value;
	// Check if the parameter is available.
	if (!getParam(mpMOVEPOS, UINT_MAX, value))
	{
		return mpcABORT;
	}
	return (EMovePosCmd) value.getInteger();
}

EMoveVelCmd Controller::getMoveVel() const
{
	Value value;
	// Check if the parameter is available.
	if (!getParam(mpMOVEVEL, UINT_MAX, value))
	{
		return mvcABORT;
	}
	return (EMoveVelCmd) value.getInteger();
}

EMoveConCmd Controller::getMoveCon() const
{
	Value value;
	// Check if the parameter is available.
	if (!getParam(mpMOVECON, UINT_MAX, value))
	{
		return mccABORT;
	}
	return (EMoveConCmd) value.getInteger();
}

bool Controller::setJoystick(EJoystickCmd jsc)
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(jsc);
		if (setParam(mpJOYSTICK, UINT_MAX, value, false))
		{
			if (getParam(mpJOYSTICK, UINT_MAX, value)) return jsc == (EJoystickCmd) value.getInteger();
		}
	}
	return false;
}

bool Controller::setJoystickAxis(EAxisLocation axis_loc)
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(axis_loc);
		if (setParam(mpJOYSTICK_AXIS, UINT_MAX, value, false))
		{
			if (getParam(mpJOYSTICK_AXIS, UINT_MAX, value)) return value.getInteger() == axis_loc;
		}
	}
	return false;
}

bool Controller::setPopAxis(EAxisLocation al)
{// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(al);
		if (setParam(mpPOP_AXIS, UINT_MAX, value, false))
		{
			if (getParam(mpPOP_AXIS, UINT_MAX, value))
			{
				return value.getInteger() == al;
			}
		}
	}
	return false;
}

bool Controller::setTriggerEnable(bool enable)
{// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(enable);
		if (setParam(mpTRIGGER_ENABLE, UINT_MAX, value, false))
		{
			if (getParam(mpTRIGGER_ENABLE, UINT_MAX, value))
			{
				return value.getInteger() == enable;
			}
		}
	}
	return false;
}

double Controller::setTriggerFreq(double freq)
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(freq);
		if (setParam(mpTRIGGER_FREQ, UINT_MAX, value, false))
		{
			if (getParam(mpTRIGGER_FREQ, UINT_MAX, value))
			{
				return value.getFloat();
			}
		}
	}
	return -1.0;
}

bool Controller::setTriggerAxis(EAxisLocation al)
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(al);
		if (setParam(mpTRIGGER_AXIS, UINT_MAX, value, false))
		{
			if (getParam(mpTRIGGER_AXIS, UINT_MAX, value))
			{
				return (value.getInteger() == al);
			}
		}
	}
	return false;
}

EAxisLocation Controller::getTriggerAxis() const
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value;
		if (getParam(mpTRIGGER_AXIS, UINT_MAX, value))
		{
			return (EAxisLocation) value.getInteger();
		}
	}
	return alNA;
}

bool Controller::setTriggerDensity(double td)
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(td);
		if (setParam(mpTRIGGER_DENSITY, UINT_MAX, value, false))
		{
			return true;
		}
	}
	return false;
}

double Controller::getTriggerDensity() const
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value;
		if (getParam(mpTRIGGER_DENSITY, UINT_MAX, value))
		{
			return value.getFloat();
		}
	}
	return 0.0;
}

bool Controller::setTriggerMode(bool intern)
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(intern);
		if (setParam(mpTRIGGER_MODE, UINT_MAX, value, false))
		{
			return true;
		}
	}
	return false;
}

bool Controller::getTriggerMode() const
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value;
		if (getParam(mpTRIGGER_MODE, UINT_MAX, value))
		{
			return value.getInteger();
		}
	}
	return false;
}

bool Controller::setChuckJaw(EChuckJaw cj)
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value(cj);
		if (setParam(mpCHUCKJAW, UINT_MAX, value, false))
		{
			return true;
		}
	}
	return false;
}

EChuckJaw Controller::getChuckJaw() const
{
	// Any status above un init is okay.
	if (_status > csUNINIT)
	{
		Value value;
		if (getParam(mpCHUCKJAW, UINT_MAX, value))
		{
			return (EChuckJaw) value.getInteger();
		}
	}
	return cjOFF;
}

EJoystickCmd Controller::getJoystick() const
{
	Value value(jscOFF);
	if (getParam(mpJOYSTICK, UINT_MAX, value))
	{
		return (EJoystickCmd) value.getInteger();
	}
	return jscOFF;
}

EAxisLocation Controller::getJoystickAxis() const
{
	Value value(-1);
	if (getParam(mpJOYSTICK_AXIS, UINT_MAX, value))
	{
		return (EAxisLocation) value.getInteger();
	}
	return alNA;
}

EAxisLocation Controller::getPopAxis() const
{
	Value value(-1);
	if (getParam(mpPOP_AXIS, UINT_MAX, value))
	{
		return (EAxisLocation) value.getInteger();
	}
	return alNA;
}

bool Controller::getTriggerEnable() const
{
	Value value(-1);
	if (getParam(mpTRIGGER_ENABLE, UINT_MAX, value))
	{
		return value.getInteger();
	}
	return true;
}

double Controller::getTriggerFreq() const
{
	Value value(-1.0);
	if (getParam(mpTRIGGER_FREQ, UINT_MAX, value))
	{
		return value.getFloat();
	}
	return -1.0;
}

std::string Controller::getProfilePath() const
{
	return getConfigLocation({}, true) + "gmi-iface.cfg";
}

bool Controller::setPosition(EAxisLocation al, double value)
{
	// Any status above uninitialized is okay.
	if (_status > csUNINIT && al != alNA)
	{
		if (doSetPosition(al, value))
		{
			// Write the new home offset along with all other system settings.
			if (!settingsReadWrite(false))
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, "Could not save home offset!")
			}
			return true;
		}
	}
	return false;
}

bool Controller::setPosition(const AxesCoord& ac)
{
	// Initialize return value.
	bool rv = true;
	if (_status > csUNINIT)
	{
		// Iterate through the implemented axes.
		for (unsigned i = 0; i < _axes.count(); i++)
		{
			// Get the axis location.
			int loc = _axes[i]->_location;
			// Ignore unset locations.
			if (ac.isSet(loc))
			{// Set the axis value.
				if (!_axes[i]->setPosition(ac[loc]._value)) rv = false;
			}
		}
	}
	// Only save on success.
	if (rv)
	{
		// Write the new home offset along with all other system settings.
		if (!settingsReadWrite(false))
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Could not save home offset!")
		}
	}
	// Signal success or failure.
	return rv;
}

bool Controller::doSetPosition(EAxisLocation al, double)
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "doSetPosition is not implemented!")
	return false;
}

AxesCoord& Controller::normalize(AxesCoord& pos) const
{
	// For all set axis normalize the passed position.
	for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
		if (pos.isSet(i)) pos.getValue(i) = getAxis(i).normalized(pos.getValue(i));
	return pos;
}

AxisLocations Controller::getRadialUnlimited() const
{
	AxisLocations als;
	AxisMovements am;
	am << amRADIAL;
	for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
		if (getAxis(i)._movements == am) als << static_cast<EAxisLocation>(i);
	return als;
}

bool Controller::settingsReadWrite(bool rd)
{
	IniProfile profile("", getProfilePath().c_str());
	IdList ids;
	// Get the list of valid ID's.
	bool retval = enumParamIds(ids);
	// Iterate through the list twice when reading parameters.
	// This is to be shure the order of parameters being set is not a real problem.1
	//  for (unsigned n = 0; n < (rd ? 2 : 1); n++)
	for (unsigned i = 0; i < ids.count(); i++)
	{
		ParamInfo info;
		// Get the parameter info.
		if (getParamInfo(ids[i], info))
		{
			// Check if the variable is a system setting parameter.
			if (info.Flags & pfSYSTEM)
			{
				// Set the section based on the axis.
				if (info.Flags & pfAXIS) profile.setSection(stringf("%s-Axis", getAxisName(info.Axis)).c_str());
				else
					profile.setSection("General");
				// If no name exists use the ID.
				if (!info.Name.length()) info.Name = stringf("0x%X", info.Id);
				if (rd)
				{
					// Read the value info value class.
					Value val(profile.getString(info.Name.c_str(), info.Default.getString().c_str()));
					// Write the parameter to the interface.
					if (!setParam(ids[i], val, false)) retval = false;
				}
				else
				{
					// Temporary value for storage.
					Value val;
					// Get the parameter.
					if (getParam(ids[i], val))
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

Controller::Axis::Axis(Controller* mc, EAxisLocation al)
	: _controller(*mc)
	, _location(al)
{
	// Register the axis at the controller.
	mc->attach(this);
}

Controller::Axis::~Axis()
{
	if (std::uncaught_exceptions() == 0)
	{
		// Major error. Cannot destruct an TAxis instance out side the controller.
		assert(_controller._flagDestroying);
	}
}

double Controller::Axis::getCurrent(EAxisValueType avt) const
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		static EParam mps[] = {mpNONE, mpAXIS_CUR_POS, mpAXIS_CUR_VEL};
		// When out of range return 0.0;
		if (avt < std::size(mps))
		{// Temp value to store it in.
			Value value;
			// Check if the parameter is available.
			if (_controller.getParam(mps[avt], _location, value))
			{
				return value.getFloat();
			}
		}
	}
	return 0.0;
}

bool Controller::Axis::setCurrent(EAxisValueType avt, double val)
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		static EParam mps[] = {mpNONE, mpAXIS_CUR_POS, mpAXIS_CUR_VEL};
		// When out of range return 0.0;
		if (avt < std::size(mps))
		{
			// Check if the parameter is available.
			if (_controller.setParam(mps[avt], _location, Value(val), false))
			{
				// Return the retrieved value.
				return true;
			}
		}
	}
	return false;
}

double Controller::Axis::getMinMax(EAxisMinMax amm) const
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		static EParam mps[] = {
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
			if (_controller.getParam(mps[amm], _location, value)) return value.getFloat();
		}
	}
	return 0.0;
}

double Controller::Axis::getTarget(EAxisValueType avt) const
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		static EParam mps[] = {mpNONE, mpAXIS_TRG_POS, mpAXIS_TRG_VEL, mpAXIS_TRG_ACC};
		// When out of range return 0.0;
		if (avt < std::size(mps))
		{
			// Temp value for storage.
			Value value;
			// Check if the parameter is available.
			if (_controller.getParam(mps[avt], _location, value)) return value.getFloat();
		}
	}
	return 0.0;
}

bool Controller::Axis::setTarget(EAxisValueType avt, double val)
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		static EParam mps[] = {mpNONE, mpAXIS_TRG_POS, mpAXIS_TRG_VEL, mpAXIS_TRG_ACC};
		// When out of range return 0.0;
		if (avt < std::size(mps))
		{
			// Set the parameter.
			return _controller.setParam(mps[avt], _location, Value(val), false);
		}
	}
	return false;
}

bool Controller::Axis::setOffset(double ofs)
{
	// Set the parameter.
	return _controller.setParam(mpAXIS_OFS_POS, _location, Value(ofs), false);
}

double Controller::Axis::setOffset() const
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		// Temp value for storage.
		Value value;
		// Check if the parameter is available.
		if (_controller.getParam(mpAXIS_OFS_POS, _location, value)) return value.getFloat();
	}
	return 0.0;
}

double Controller::Axis::getAccuracy() const
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		// Temp value for storage.
		Value value;
		// Check if the parameter is available.
		if (_controller.getParam(mpAXIS_ACCURACY, _location, value)) return value.getFloat();
	}
	return 0.0;
}

double Controller::Axis::getResolution() const
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		// Temp value for storage.
		Value value;
		// Check if the parameter is available.
		if (_controller.getParam(mpAXIS_RESOLUTION, _location, value)) return value.getFloat();
	}
	return 1.0;
}

bool Controller::Axis::canModeChange(EAxisMode cur_mode, EAxisMode new_mode, bool pos_cmplt, bool vel_cmplt, bool con_cmplt)
{
	// Check what the new mode will be and if is allowed to change.
	switch (new_mode)
	{
		default:
			return false;

		case amHOME:
			// All axis must be not moving before a mode can be changed.
			if (!pos_cmplt || !vel_cmplt || !con_cmplt) return false;
			return true;

		case amDISABLED:
			switch (cur_mode)
			{
				default:
					return false;
				case amPOSITION:
				case amHOME:
					if (!pos_cmplt) return false;
					break;
				case amVELOCITY:
					// Only when velocity mode is complete.
					if (!vel_cmplt) return false;
					break;
				case amCONTINUE:
					// Only when velocity mode is complete.
					if (!con_cmplt) return false;
					break;
			}
			break;

		case amPOSITION:
			switch (cur_mode)
			{
				default:
					return false;
				case amDISABLED:
				case amHOME:
					// Only when position move is complete.
					if (!pos_cmplt) return false;
					break;
				case amVELOCITY:
					// Only when not moving at all can be switched from
					// velocity to position mode.
					if (!pos_cmplt || !vel_cmplt) return false;
					break;
				case amCONTINUE:
					// Only when velocity mode is complete.
					if (!pos_cmplt || !con_cmplt) return false;
					break;
			}
			break;

		case amVELOCITY:
			switch (cur_mode)
			{
				default:
					return false;
				case amDISABLED:
					// Only when not moving in velocity mode.
					if (!vel_cmplt) return false;
					break;
				case amPOSITION:
				case amHOME:
					// Only when not moving at all.
					if (!pos_cmplt || !vel_cmplt) return false;
					break;
				case amCONTINUE:
					// Only when velocity mode is complete.
					if (!con_cmplt || !vel_cmplt) return false;
					break;
			}
			break;

		case amCONTINUE:
			switch (cur_mode)
			{
				default:
					return false;
				case amDISABLED:
					// Only when not moving in velocity mode.
					if (!con_cmplt) return false;
					break;
				case amPOSITION:
				case amHOME:
					// Only when not moving at all.
					if (!pos_cmplt || !con_cmplt) return false;
					break;
				case amVELOCITY:
					// Only when velocity mode is complete.
					if (!vel_cmplt || !con_cmplt) return false;
					break;
			}
			break;
	}
	return true;
}

bool Controller::Axis::setMode(EAxisMode mode)
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		Value value(0);
		// Get the parameter to see if it needs changing.
		if (_controller.getParam(mpAXIS_MODE, _location, value))
		{
			// Get the current mode.
			EAxisMode cur_mode = static_cast<EAxisMode>(value.getInteger());
			// Compare current mode for a change.
			if (cur_mode != mode)
			{
				// Check if a change is allowed.
				bool ok = canModeChange(cur_mode, mode, _controller.isMovePosCompleted(), _controller.isMoveVelCompleted(), _controller.isMoveConCompleted());
				//
				if (!ok)
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to change axis " << getName() << " mode to [" << mode << "] during move!")
					return false;
				}
				// Assign the mode to the value so it can be assigned.
				value.set(mode);
				// Check if the parameter is available.
				return _controller.setParam(mpAXIS_MODE, _location, value, false);
			}
			// Signal success because there is no need for changing.
			return true;
		}
	}
	// Signal failure.
	return false;
}

EAxisMode Controller::Axis::getMode() const
{
	// Test if the axis is a valid one.
	if (_location != alNA)
	{
		Value value(0);
		// Check if the parameter is available.
		_controller.getParam(mpAXIS_MODE, _location, value);
		//
		return static_cast<EAxisMode>(value.getInteger());
	}
	// By default, the axis is disabled.
	return amDISABLED;
}

bool Controller::Axis::setPosition(double value)
{
	// Pass to controller.
	return _controller.setPosition(_location, value);
}

double Controller::Axis::normalized(double pos) const
{
	// Is the axis radial and unlimited.
	if (_movements == (AxisMovements() << amRADIAL))
	{// Bring the angle in range of 0..360 degree.
		pos = fmod(pos, sf::numbers::pi_v<Value::flt_type> * 2.0);
		// All angle near zero are zeroed.
		if (fabs(pos) >= sf::numbers::pi_v<Value::flt_type> * 2.0 - getAccuracy()) pos = 0.0;
		// Make the angle positive.
		if (pos < 0.0) pos += sf::numbers::pi_v<Value::flt_type> * 2.0;
	}
	return pos;
}

const char* Controller::Axis::getName() const
{
	return _controller.getAxisName(_location);
}

const char* Controller::Axis::getDescription() const
{
	return _controller.getAxisDescription(_location);
}

bool Controller::isMoving() const
{
	EMovePosCmd mpc = getMovePos();
	EMoveVelCmd mvc = getMoveVel();
	EMoveConCmd mcc = getMoveCon();
	// When one axis has the abort state motion has stopped for shure.
	if (mpc == mpcABORT || mvc == mvcABORT || mcc == mccABORT) return getJoystick() > jscOFF;
	// All axes have to be completed before no movement is assumed.
	if (mpc == mpcCOMPLETE && mvc == mvcCOMPLETE && mcc == mccCOMPLETE) return getJoystick() > jscOFF;
	return true;
}

Controller::Axis& Controller::getAxis(int axis_loc)
{
	// Check if location index is in range.
	if (axis_loc >= alFIRST_ENTRY && axis_loc < alLAST_ENTRY)
	{
		return *_axesMap[axis_loc];
	}
	// Return default the null axis.
	return _nullAxis;
}

const Controller::Axis& Controller::getAxis(int axis_loc) const
{
	// Check if location index is in range.
	if (axis_loc >= alFIRST_ENTRY && axis_loc < alLAST_ENTRY)
	{
		return *_axesMap[axis_loc];
	}
	// Return default the null axis.
	return _nullAxis;
}

Controller::Axis& Controller::getPhysicalAxis(int axis_num)
{
	// Check if location index is in range.
	if (axis_num >= 0 && axis_num < static_cast<int>(_axes.count()))
	{
		return *_axes[axis_num];
	}
	// Return default the null axis.
	return _nullAxis;
}

}// namespace sf::gmi
