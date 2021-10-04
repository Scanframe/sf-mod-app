#include <misc/gen/dbgutils.h>
#include "InformationServer.h"

namespace sf
{

InformationServer::InformationServer()
	:_variableHandler(this, &InformationServer::variableEventHandler)
	 , _serverDeviceMask(0)
	 , _serverDeviceId(0)
	 , _prevState(issOff)
	 , _curState(issOff)
{
	// Initialize jump table
	for (int from = 0; from < issMaxState; from++)
	{
		for (int to = 0; to < issMaxState; to++)
		{
			if (from == to)
			{
				_jumpTable[from][to] = &InformationServer::stateChangeUnknown;
			}
			else
			{
				_jumpTable[from][to] = &InformationServer::stateChangeIllegal;
			}
		}
	}
	//
	for (auto& from: _jumpTable)
	{
		from[issOff] = &InformationServer::stateChangeAnyToOff;
	}
	//
	_jumpTable[issOff][issRun] = &InformationServer::stateChangeOffToRun;
	_jumpTable[issStop][issRun] = &InformationServer::stateChangeStopToRun;
	_jumpTable[issOff][issRecord] = &InformationServer::stateChangeRunToRecord;
	_jumpTable[issStop][issRecord] = &InformationServer::stateChangeRunToRecord;
	_jumpTable[issRecord][issStop] = &InformationServer::stateChangePauseToStop;
	_jumpTable[issRecord][issRun] = &InformationServer::stateChangeStopToRun;
	_jumpTable[issRun][issRecord] = &InformationServer::stateChangeRunToRecord;
	_jumpTable[issRun][issStop] = &InformationServer::stateChangeRunToStop;
	_jumpTable[issRecord][issPause] = &InformationServer::stateChangeRecordToPause;
	_jumpTable[issPause][issStop] = &InformationServer::stateChangePauseToStop;
	_jumpTable[issPause][issRecord] = &InformationServer::stateChangeRecordToPause;
}

void InformationServer::flush()
{
	// Assign the zero variable to delete the state variable.
	_vState.setup(0);
	// Clear the lists.
	for (int i = clA; i < clMaxClass; i++)
	{
		_variableList[i].flush();
	}
	_resultList.flush();
	// Initialize states.
	_prevState = issOff;
	_curState = issOff;
	_serverDeviceMask = 0;
	_serverDeviceId = 0;
}

void InformationServer::setup
	(
		const std::string& name,
		const std::string& namePrefix,
		long vid,
		long deviceMask,
		long serverMask
	)
{
	// Clear all lists and variables.
	flush();
	//
	_serverDeviceMask = deviceMask | serverMask;
	_serverDeviceId = vid & (deviceMask | serverMask);
	// Create the setup string for creating the control information control variable.
	std::string setupString = stringf("0x%lX,%s|State,,S,Generic information server state %s,", vid, namePrefix.c_str(), name.c_str());
	// Add the range part.
	setupString += stringf("INTEGER,,1,0,%i,%i,", issOff, issMaxState - 1);
	// Add the states.
	for (int i = 0; i < issMaxState; i++)
	{
		setupString += stringf("%s=%i,", getStateName(i), i);
	}
	// Create and link variable for controlling the information server.
	_vState.setup(setupString);
	_vState.setHandler(&_variableHandler);
}

InformationServer::~InformationServer()
{
	(void) this;
}

const char* InformationServer::getStateName(int state) const
{
	static const char* names[] =
		{
			"OFF",
			"RUN",
			"RECORD",
			"PAUSE",
			"STOP",
			"<unknown>"
		};
	return names[state >= issMaxState ? issMaxState : state];
}

void InformationServer::attachVariable(Variable* var, EClass c)
{
	// Remove this instance from any list first.
	detachVariable(var);
	// Add it to the var list of the passed class.
	_variableList[c].append(var);
	// When initial flags has not the ARCHIVE flag set the archive
	// state is not changed here.
	if (var->getFlags() & Variable::flgArchive)
	{
		if (_curState != issOff)
		{
			var->setFlag(Variable::flgArchive, false);
		}
		else
		{
			var->unsetFlag(Variable::flgArchive, false);
		}
	}
}

void InformationServer::detachVariable(Variable* var)
{
	for (auto& c: _variableList)
	{
		c.detach(var);
	}
}

void InformationServer::attachResult(ResultData* res)
{
	auto i = _resultList.find(res);
	if (i == ResultData::PtrVector::npos)
	{
		_resultList.append(res);
		// When initial flags has not the ARCHIVE flag set the archive
		// state is not changed here.
		if (res->getFlags() & ResultData::flgArchive)
		{
			if (_curState != issOff)
			{
				res->setFlag(ResultData::flgArchive, false);
			}
			else
			{
				res->unsetFlag(ResultData::flgArchive, false);
			}
		}
	}
	else
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Cannot add result pointer more than once!");
	}
}

void InformationServer::detachResult(ResultData* res)
{
	_resultList.detach(res);
}

void InformationServer::clearValidations()
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "Clear Validations of " << _resultList.size() << " results.");
	for (auto r: _resultList)
	{
		r->clearValidations();
	}
}

void InformationServer::setRecycleFlag(bool recycle_enable)
{
	// Iterator through the result list.
	for (auto r: _resultList)
	{
		// Only change recycle mode on archived results. Testing the initial flags.
		if (r->getFlags() & ResultData::flgArchive)
		{
			if (recycle_enable)
			{
				r->setFlag(ResultData::flgRecycle, false);
			}
			else
			{
				r->unsetFlag(ResultData::flgRecycle, false);
			}
		}
	}
}

void InformationServer::setWriteFlagForClass(EClass c, bool write_enable)
{
	// Create iterator from variables vector.
	for (auto var: _variableList[c])
	{
		// When initial flags has READONLY flag readonly state cannot change.
		if (!(var->getFlags() & Variable::flgReadonly))
		{
			if (write_enable)
			{
				var->unsetFlag(Variable::flgReadonly, false);
			}
			else
			{
				var->setFlag(Variable::flgReadonly, false);
			}
		}
	}
	// generate debugging information.
	SF_RTTI_NOTIFY(DO_DEFAULT, _variableList[c].size() << " variables of class " << std::string("ABC")[c]
		<< " were made " << (write_enable ? " write enabled" : " read-only"));
}

void InformationServer::setArchiveFlag(bool archive)
{
	for (int c = 0; c <= clC; c++)
	{
		// Create iterator from variables vector.
		for (auto var: _variableList[c])
		{
			// When initial flags has not the ARCHIVE flag set the archive
			// state is not changed here.
			if (var->getFlags() & Variable::flgArchive)
			{
				if (archive)
				{
					var->setFlag(Variable::flgArchive, false);
				}
				else
				{
					var->unsetFlag(Variable::flgArchive, false);
				}
			}
		}
	}
	// Iterator through the result list.
	for (auto res: _resultList)
	{
		// Only change recycle mode on archived results. Testing the initial flags.
		if (res->getFlags() & ResultData::flgArchive)
		{
			if (archive)
			{
				res->setFlag(ResultData::flgArchive, false);
			}
			else
			{
				res->unsetFlag(ResultData::flgArchive, false);
			}
		}
	}
	// Generate debugging information.
	SF_RTTI_NOTIFY(DO_DEFAULT, (archive ? "Setting" : "Resetting") << " the archive flag.");
}

void InformationServer::stateChangeUnknown()
{
	SF_RTTI_NOTIFY(DO_DEFAULT, "Unknown state change encountered!");
}

void InformationServer::stateChangeIllegal()
{
	// Assuming that the mode became OFF
	SF_RTTI_NOTIFY(DO_DEFAULT, "Illegal state change encountered!");
	// Write-enable class B
	setWriteFlagForClass(clB, true);
	// Write-enable class C
	setWriteFlagForClass(clC, true);
}

void InformationServer::stateChangeOffToRun()
{
	// Flush current acquired data.
	clearValidations();
	// All results instances should recycle in run mode.
	setRecycleFlag(true);
}

void InformationServer::stateChangeStopToRun()
{
	// Write-enable class B
	setWriteFlagForClass(clB, true);
	// Write-enable class C
	setWriteFlagForClass(clC, true);
	// Flush current acquired data.
	clearValidations();
	// All results instances should recycle in run mode.
	setRecycleFlag(true);
}

void InformationServer::stateChangeRunToStop()
{
	// Do nothing.
}

void InformationServer::stateChangeRunToRecord()
{
	// Make class B readonly
	setWriteFlagForClass(clB, false);
	// Make class C readonly
	setWriteFlagForClass(clC, false);
	// Flush current acquired data.
	clearValidations();
	// All results not having the recycle flag should store.
	setRecycleFlag(false);
}

void InformationServer::stateChangeRecordToPause()
{
	// Do nothing
}

void InformationServer::stateChangePauseToStop()
{
	// Write-enable class B
	setWriteFlagForClass(clB, true);
}

void InformationServer::stateChangeAnyToOff()
{
	// Write-enable class B
	setWriteFlagForClass(clB, true);
	// Write-enable class C
	setWriteFlagForClass(clC, true);
	// Flush current acquired data.
	clearValidations();
}

bool InformationServer::isGeneratingResults()
{
	return _curState == issRun || _curState == issRecord;
}

void InformationServer::setState(EState state)
{
	// Prevent reentry.
	if (_curState != state)
	{
		// Keep copy of previous state.
		EState prevState = _curState;
		// Assign the different state to the member.
		_curState = state;
		// Get the member function belonging to this state transition.
		VoidFunction func = _jumpTable[_prevState][_curState];
		// When the state change is illegal go to the OFF state.
		if (func == &InformationServer::stateChangeIllegal)
		{
			// Move to the OFF state by default in case of an error.
			func = _jumpTable[_curState][issOff];
			// Set the current state to off.
			_curState = issOff;
		}
		// Set the current to what it is and skip the event for myself.
		_vState.setCur(Value(_curState), true);
		// Clear all archive flags when state is off.
		setArchiveFlag(_curState != issOff);
		// Call the member function that can be overloaded in derived classes.
		onStateChange(prevState, _curState);
		// Call the member function.
		(this->*func)();
		// Set the previous state data member also to the current state.
		_prevState = _curState;
	}
}

void InformationServer::variableEventHandler(Variable::EEvent event, const Variable& call, Variable& link, bool sameInst)
{
	(void) sameInst;
	// Avoid reentry of own variable.
	if (&link == &_vState)
	{
		switch (event)
		{
			// Value changed on linked variable.
			case Variable::veValueChange:
				// Get the wanted current state of the variable.
				setState(static_cast<EState>(link.getCur().getInteger()));
				break;

				// New variable was introduced in the process.
			case Variable::veNewId:
			{
				// Check if the new variable id could be part of this server.
				if (isServerId(call.getId()))
				{
					// _RTTI_NOTIFY(DO_DEFAULT, "This variable could be added automatically...");
				}
				break;
			}
			default:
				break;
		}
	}
	// We did not handle event,
	// however, we do not have a parent class to pass it on to.
	// Therefore, the event is now lost. Who cares.
}

InformationServer::EState InformationServer::getState() const
{
	return _curState;
}

bool InformationServer::isServerId(id_type id) const
{
	return (_serverDeviceMask) && (id & _serverDeviceMask) == _serverDeviceId;
}

}
