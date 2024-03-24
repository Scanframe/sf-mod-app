#include "RsaInterface.h"
#include "RsaServer.h"
#include "MakeIds.h"

namespace sf
{

void RsaServer::paramNotifyProc(void* data, id_type id)
{
	((RsaServer*) data)->paramNotify(id);
}

void RsaServer::resultNotifyProc(void* data, id_type id)
{
	((RsaServer*) data)->resultNotify(id);
}

RsaServer::RsaServer(int compatible, id_type deviceNumber, const std::string& serverName)
	:InformationServer()
	 , _serverVariableHandler(this, &RsaServer::serverVariableHandler)
	 , _serverResultDataHandler(this, &RsaServer::serverResultDataHandler)
	 , _implementationId(0)
	 , _vImplementation()
	 , _deviceNumber(deviceNumber)
	 , _serverName(serverName.empty() ? "Acquisition" : serverName)
	 , _acquisition(nullptr)
	 , _lock(false)
	 , _handledParamId(0)
	 , _compatible(compatible)
{
	// Only for backward UT device compatibility.
	if (_compatible == 1 && _deviceNumber == DEVICENR_UT)
	{
		_implementationId = MAKE_VID(_deviceNumber, 0);
		// TODO: Some un wanted situation in the VID generation.
		if (_deviceNumber == DEVICENR_UT)
		{
			_implementationId = OLD_MAKE_UT_CHAN_VID(0, DEVICENR_UT, 0L, 0);
		}
		// Set up the implementation string using the names of the
		// names of possible implementations.
		std::string s = stringf
			(
				"0x%lX,%s|Implementation,,S,Implementation of acquisition server,"
				"INTEGER,,"
				"1,0,0,%i,None=0,",
				_implementationId,
				_serverName.c_str(),
				RsaInterface::Interface().size()
			);
		// Add the states generated from the registered names.
		for (size_t i = 0; i < RsaInterface::Interface().size(); i++)
		{
			s += RsaInterface::Interface().getName(i);
			s + "=" + itostr(i + 1);
		}
		// Set the variable up.
		_vImplementation.setup(s);
		// Link it to the handler.
		_vImplementation.setHandler(&_serverVariableHandler);
	}
}

RsaServer::~RsaServer()
{
	// Delete all variables and results for this implementation.
	destroyInterface();
	// If an implementation exists it is deleted.
	delete_null(_acquisition);
}

std::string RsaServer::getServerName() const
{
	return _serverName;
}

RsaServer::ExtraInfo* RsaServer::castExtraInfo(const Variable* var)
{
	if (var && var->getData())
	{
		return (ExtraInfo*) var->getData();
	}
	return nullptr;
}

RsaServer::ExtraInfo* RsaServer::castExtraInfo(const ResultData* var)
{
	if (var && var->getData())
	{
		return (ExtraInfo*) var->getData();
	}
	return nullptr;
}

std::string RsaServer::getImplementationName() const
{
	// Get the current name of the server.
	if (_acquisition)
	{
		return RsaInterface::Interface().getRegisterName(_acquisition);
	}
	return {};
}

bool RsaServer::createImplementation(const std::string& name)
{
	// If another implementation exists delete it first.
	if (_acquisition)
	{
		// Destroy all variables and result-data instances of this server.
		destroyInterface();
		_acquisition->uinitialize();
		delete _acquisition;
		_acquisition = nullptr;
	}
	if (!name.empty())
	{
		int mode = 0;
		// Create a new implementation of a registered class.
		_acquisition = RsaInterface::Interface().create(name, RsaInterface::Parameters(mode));
		if (_acquisition)
		{
			// Install function hooks.
			_acquisition->setParamHook(paramNotifyProc, this);
			_acquisition->setResultHook(resultNotifyProc, this);
			// Create or check all needed interface items.
			// Initialize the implementation.
			if (_acquisition->initialize())
			{
				createInterface();
				return true;
			}
		}
		return false;
	}
	return true;
}

bool RsaServer::createImplementation(int index)
{
	// If another implementation exists delete it first.
	if (_acquisition)
	{
		// Destroy all variables and result-data instances of this server.
		destroyInterface();
		// Uninitialize.
		_acquisition->uinitialize();
		// Delete the implementation.
		delete _acquisition;
		// Null the pointer.
		_acquisition = nullptr;
	}
	if (index >= 0)
	{
		// Get the number with which it was registered.
		int mode = 0;
		// Create a new implementation of a registered class indicated by index.
		_acquisition = RsaInterface::Interface().create(index, RsaInterface::Parameters(mode));
		// Return true if the implementation was created successfully.
		if (_acquisition)
		{
			// Initialize.
			_acquisition->initialize();
			// Install function hooks.
			_acquisition->setParamHook(paramNotifyProc, this);
			_acquisition->setResultHook(resultNotifyProc, this);
			// Create or check all needed interface items.
			createInterface();
			return true;
		}
	}
	return false;
}

std::string RsaServer::getNameOffset(const ParamInfo& info)
{
	// First one is the device name.
	std::string ofs = _serverName + "|";
	// Check if the parameter is a channel related parameter or
	// if it only a single channel parameter.
	if (info.Channel != UINT_MAX && !(info.Flags & pfChannelSingle))
	{
		// Change behaviour on the basis implementation type.
		switch (_acquisition->getType())
		{
			case atUltrasonic:
				// Add channel offset when there are more than one channels.
				if (_acquisition->getChannelCount() > 1)
				{
					ofs += stringf("Channel %i|", info.Channel + 1);
				}
				break;

			case atEddyCurrent:
				// Add numbered channel offset when there are more than one channels.
				if (_acquisition->getChannelCount() > 1)
				{
					ofs += stringf("Channel %i|", info.Channel + 1);
				}
				else
				{
					ofs += "Channel|";
				}
				break;
		}
		// Also check for gate/signal related.
		if (info.Gate != UINT_MAX)
		{
			// Add gate offset using the implementations name of the gate.
			ofs += _acquisition->getGateName(info.Gate, info.Channel) + "|";
		}
	}
	//
	return ofs;
}

std::string RsaServer::getDescription(const ParamInfo& info)
{
	// First one is the device name.
	std::string ofs = _serverName;
	// Check if the parameter is a channel related parameter.
	if (info.Channel != UINT_MAX && !(info.Flags & pfChannelSingle))
	{
		// Add channel offset when there are more than one channels.
		if (_acquisition->getChannelCount() > 1)
		{
			ofs += stringf(" Channel %i ", info.Channel + 1);
		}
		// Also check for gate related.
		if (info.Gate != UINT_MAX)
		{ // Add gate offset using the implementations name of the gate.
			ofs += " " + _acquisition->getGateName(info.Gate, info.Channel);
		}
	}
	auto pos = info.Description.find('%');
	// When not found.
	if (pos == std::string::npos)
	{
		//
		return ofs + " " + info.Description;
	}
	else
	{
		std::string s = info.Description;
		s.erase(pos, 1);
		s.insert(pos, ofs);
		return s;
	}
}

std::string RsaServer::getNameOffset(const ResultInfo& info)
{
	// First one is the device name.
	std::string ofs = _serverName + "|";
	// Check if the parameter is a channel related results.
	if (info.Channel != UINT_MAX)
	{
		// Add channel offset when there are more than one channels.
		if (_acquisition->getChannelCount() > 1)
		{
			ofs += stringf("Channel %i|", info.Channel + 1);
		}
		// Also check for gate related.
		if (info.Gate != UINT_MAX)
		{ // Add gate offset using the implementations name of the gate.
			ofs += _acquisition->getGateName(info.Gate, info.Channel) + "|";
		}
	}
	//
	return ofs;
}

std::string RsaServer::getDescription(const ResultInfo& info)
{
	// First one is the device name.
	std::string ofs = _serverName;
	// Check if the parameter is a channel related parameter.
	if (info.Channel != UINT_MAX)
	{ // Add channel offset when there are more than one channels.
		if (_acquisition->getChannelCount() > 1)
		{
			ofs += stringf(" Channel %i ", info.Channel + 1);
		}
		// Also check for gate related.
		if (info.Gate != UINT_MAX)
		{ // Add gate offset using the implementations name of the gate.
			ofs += " " + _acquisition->getGateName(info.Gate, info.Channel);
		}
	}
	auto pos = info.Description.find('%');
	// When not found.
	if (pos == std::string::npos)
	{
		//
		return ofs + " " + info.Description;
	}
	else
	{
		std::string s = info.Description;
		s.erase(pos, 1);
		s.insert(pos, ofs);
		return s;
	}
}

void RsaServer::createInterface()
{
	// The server state is always the non channel and non gate parameter.
	long vid_state = MAKE_VID(_deviceNumber, 0);
	// Check if backward compatibility is used.
	if (_compatible == 1 || _compatible == 2)
	{
		vid_state = OLD_MAKE_UT_CHAN_VID(0, _deviceNumber, 0, 1);
	}
	// Create the server state variable.
	setup("Server", _serverName, vid_state, MAKE_VID_DEVICE(_deviceNumber), MAKE_VID(0, 0));
	// Cannot change implementation during measurements.
	attachVariable(&_vImplementation, clC);
	// Call it for the first time.
	evaluateInterfaceParams();
	evaluateInterfaceResults();
}

std::string RsaServer::createSetupString(const ParamInfo& info, long vid)
{
	// Separator to separate fields.
	std::string sep = ",";
	//  vfId
	std::string rv = "0x" + itostr(vid, 16);
	rv += sep;
	//  vfName
	// Add the path for the parameter.
	rv += getNameOffset(info) + info.Name + sep;
	//  vfUnit
	rv += info.Unit + sep;
	//  vfFlags
	flags_type flags = Variable::flgShare;
	// Set archive flag and/or parameter flag.
	if (info.Flags & pfArchive)
	{
		flags |= Variable::flgArchive;
		// When not an Alias for another parameter or it is a system parameter it may be used to restore parameters.
		if (info.Flags & (pfAlias | pfSystem)) {}
		else
		{
			flags |= Variable::flgParameter;
		}
	}
	// Set read only if needed.
	if (info.Flags & pfReadonly)
	{
		flags |= Variable::flgReadonly;
		// When read only remove parameter flag.
		flags &= ~Variable::flgParameter;
	}
	//
	rv += Variable::getFlagsString(flags) + sep;
	//  vfDescription,
	rv += escape(getDescription(info), '"') + sep;
	//  vfType
	rv += Variable::getType(info.Default.getType()) + sep;
	//  vfConversionType,
	rv += "" + sep;
	//  vfRound,
	rv += info.Round.getString() + sep;
	//  vfDefault,
	rv += info.Default.getString() + sep;
	//  vfMinimum,
	rv += info.Minimum.getString() + sep;
	//  vfMaximum,
	rv += info.Maximum.getString() + sep;
	//  vfFirstState
	for (unsigned i = 0; i < info.States.count(); i++)
	{
		rv += info.States[i].name + "=" + info.States[i].value.getString() + sep;
	}
	// Return the setup string.
	return rv;
}

std::string RsaServer::createSetupString(const ResultInfo& info, long vid)
{
	// Separator to separate fields.
	std::string sep = ",";
	// rfeID
	std::string rv = "0x" + itostr(vid, 16);
	rv += sep;
	// rfName
	// Add the path for the parameter.
	rv += getNameOffset(info) + info.Name + sep;
	// rfFlags
	flags_type flags = ResultData::flgShare;
	// When not generating data the result is put in recycle mode.
	if (info.Flags & rfStored)
	{
		flags |= ResultData::flgArchive;
	}
	else
	{
		flags |= ResultData::flgRecycle;
	}
	rv += ResultData::getFlagsString(flags) + sep;
	// rfDescription
	rv += escape(getDescription(info), '"') + sep;
	// rfeTYPE
	ResultData::EType type = ResultData::rtInvalid;
	switch (info.WordSize)
	{
		case 1:
			type = ResultData::rtInt8;
			break;
		case 2:
			type = ResultData::rtInt16;
			break;
		case 4:
			type = ResultData::rtInt32;
			break;
		case 8:
			type = ResultData::rtInt64;
			break;
		default: SF_RTTI_NOTIFY(DO_CLOG, _serverName << " not correct implemented!");
			break;
	}
	// The array size cannot be zero.
	//
	rv += ResultData::getType(type) + sep;
	// rfBlockSize
	rv += stringf("%u", info.ArraySize) + sep;
	// rfSegmentSize
	unsigned segByteSize;
	// Prevent division by zero.
	if (info.ArraySize)
	{
		// Use 1 megabyte for small results.
		segByteSize = (1024 * 1024) / info.ArraySize;
		// Use 10 megabyte for large results.
		if (info.Flags & rfHugeData)
		{
			segByteSize = (10 * 1024 * 1024) / info.ArraySize;
		}
	}
	else
	{
		segByteSize = 1024 * 1024;
	}
	rv += itostr(segByteSize) + sep;
	// rfSigBits
	rv += itostr(info.Bits) + sep;
	// rfOffset
	rv += itostr(info.Offset);
	// Return the setup string.
	return rv;
}

Variable::PtrVector::size_type RsaServer::variableListFind(id_type id) const
{
	for (Variable::PtrVector::size_type i = 0; i < _variableVector.size(); i++)
	{
		ExtraInfo* ei = const_cast<RsaServer*>(this)->castExtraInfo(_variableVector[i]);
		if (ei && ei->_id == id)
		{
			return i;
		}
	}
	return Variable::PtrVector::npos;
}

ResultData::PtrVector::size_type RsaServer::resultListFind(id_type id) const
{
	for (ResultData::PtrVector::size_type i = 0; i < _resultVector.size(); i++)
	{
		ExtraInfo* ei = const_cast<RsaServer*>(this)->castExtraInfo(_resultVector[i]);
		if (ei && ei->_id == id)
		{
			return i;
		}
	}
	return Variable::PtrVector::npos;
}

/**
 *
 * TODO: This comparing of setup strings should be avoided. Use the VariableTypes::Definition structure instead and add compare method there.
 * Returns a bit map of fields that differ.
 */
unsigned CompareCsfFields(const std::string& s1, const std::string& s2)
{
	unsigned rv = 0;
	strings sl1, sl2;
	sl1.split(s1, ',', '"');
	sl2.split(s2, ',', '"');
	auto count = std::max(sl1.count(), sl2.count());
	for (size_t i = 0; i < count; i++)
	{
		if (sl1.at(i) != sl2.at(i))
		{
			// Set the bit of the field.
			rv += 1 << i;
		}
	}
	return rv;
}

bool RsaServer::createVariable(Variable*& var, ParamInfo& info, const std::string& setup)
{
	// Flag for indicating that this function created the variable instance.
	bool newed = false;
	// Test the variable pointer for an existing pointer.
	if (!var)
	{ // Assign a new variable.
		var = new Variable();
		newed = true;
	}
	else
	{
		//SF_RTTI_NOTIFY(DO_CLOG, "Recreating " << var->getName());
	}
	// Set the variable global if export flag has not been set and the system is.
	var->setGlobal(((info.Flags & (pfSystem | pfExport)) == pfSystem) ? false : true);
	// Set up the variable.
	var->setup(setup);
	// Check the creation of the variable
	if (var->getId())
	{
		// Get or create or get the extra info structure.
		ExtraInfo* ei = castExtraInfo(var);
		if (!ei)
		{
			ei = new ExtraInfo;
			// Assign the pointer to the variable.
			var->setData((uint64_t) ei);
		}
		// Set the setup string from the variable for later comparison.
		ei->_setupString = setup;
		// Set the ID.
		ei->_id = info.Id;
		// Set the channel.
		ei->_channel = info.Channel;
		// Copy the param flags.
		ei->_flags = info.Flags;
		// Add the variable to the list when it was newed in this function.
		if (newed)
		{
			_variableVector.append(var);
		}
		// Link the variable.
		var->setHandler(&_serverVariableHandler);
		// Initialize with class with no restrictions.
		EClass cls = clA;
		// Determine the class of the variable.
		if (ei->_flags & pfEffectsParameter)
		{
			cls = clB;
		}
		if (ei->_flags & (pfEffectsResult | pfWriteAtOff))
		{
			cls = clC;
		}
		// Add variable to the information servers control.
		attachVariable(var, cls);
		return true;
	}
	// If the variable was created in this function it should also be deleted
	// in case of an error.
	if (newed)
	{ // Delete the variable.
		delete var;
	}
	// Signal failure.
	return false;
}

bool RsaServer::createResultData(ResultData*& res, ResultInfo& info, const std::string& setup)
{
	// Flag for indicating that this function created the instance.
	bool newed = false;
	// Test the pointer for an existing pointer.
	if (!res)
	{ // Assign a new variable.
		res = new ResultData();
		newed = true;
	}
	else
	{
		//SF_RTTI_NOTIFY(DO_CLOG, "Recreating " << res->getName());
	}
	// Set up the result-data instance.
	res->setup(setup);
	// Check the creation of the result instance.
	if (res->getId())
	{
		// Create or get the extra info structure.
		ExtraInfo* ei = castExtraInfo(res);
		if (!ei)
		{
			ei = new ExtraInfo;
			// Assign the pointer to the variable.
			res->setData((uint64_t) ei);
		}
		// Set the setup string from the result for later comparison.
		ei->_setupString = setup;
		// Set the ID.
		ei->_id = info.Id;
		// Set the channel.
		ei->_channel = info.Channel;
		// Copy the param flags.
		ei->_flags = info.Flags;
		// Add the result to the list when it was newed in this function.
		if (newed)
		{
			// Add the variable to the list.
			_resultVector.append(res);
		}
		// Link the result.
		res->setHandler(&_serverResultDataHandler);
		// Add variable to the information servers control.
		attachResult(res);
		//
		return res;
	}
	// Delete the result.
	delete res;
	SF_RTTI_NOTIFY(DO_CLOG, "Creation of ResultData instance failed\n\t" << setup);
	// Signal failure.
	return false;
}

void RsaServer::destroyVariable(Variable* var)
{
	if (var)
	{
		// Detach the variable from the information servers control.
		detachVariable(var);
		// Unlink the variable event handler.
		var->setHandler(nullptr);
		// Remove the variable from the list.
		_variableVector.detach(var);
		// Cast the data of the variable;
		ExtraInfo* ei = castExtraInfo(var);
		//
		SF_COND_RTTI_NOTIFY(isDebug(), DO_CLOG, "Destroying Variable: " << var->getName());
		// Delete the structure if it exists.
		delete_null(ei);
		// Delete the variable.
		delete var;
	}
}

void RsaServer::destroyResultData(ResultData* res)
{
	if (res)
	{
		// Remove the variable from the list.
		if (!_resultVector.detach(res))
		{
			throw Exception("Design error '%s' is not attached!", res->getName().c_str());
		}
		// Detach the result data from the information servers control.
		detachResult(res);
		// Unlink the result data event handler.
		res->setHandler(nullptr);
		// Cast the data of the variable;
		ExtraInfo* ei = castExtraInfo(res);
		//
		SF_COND_RTTI_NOTIFY(isDebug(), DO_CLOG, "Destroying ResultData: " << res->getName());
		// Delete the structure if it exists.
		delete_null(ei);
		// Delete the variable.
		delete res;
	}
}

void RsaServer::evaluateInterfaceParams()
{
	// Vector to hold all parameter ids.
	IdList ids;
	// Get all the parameters.
	_acquisition->enumParamIds(ids);
	// Check if the list size is non-zero.
	if (!_variableVector.empty())
	{
		// Iterate backwards through the list of existing variables.
		for (auto it = _variableVector.rbegin(); it != _variableVector.rend(); it++)
		{
			// Get the extra info pointer of the variable.
			ExtraInfo* ei = castExtraInfo(*it);
			// If the info struct is present continue.
			if (ei)
			{ // Check if the ID is found in not wanted list of ID's.
				if (ids.find(ei->_id) == Variable::PtrVector::npos)
				{
					// Call the special function to delete a variable from the VarList.
					destroyVariable(*it);
				}
			}
		}
	}
	// Iterate through all parameter ids.
	for (auto id: ids)
	{
		// Declare an parameter info structure.
		ParamInfo info;
		// Get the parameter information of the id.
		if (!_acquisition->getParamInfo(id, info))
		{
			continue;
		}
		// Form the variable id.
		long vid = 0;
		// Change behaviour on the basis implementation type.
		switch (_acquisition->getType())
		{
			case atUltrasonic:
				if (_compatible == 1 || _compatible == 2)
				{
					if (info.Gate != UINT_MAX)
					{
						vid = OLD_MAKE_UT_GATE_VID(0, _deviceNumber, info.Channel + 1, info.Gate, info.Index);
					}
					else
					{
						vid = OLD_MAKE_UT_CHAN_VID(0, _deviceNumber, info.Channel + 1, info.Index);
					}
				}
				else
				{
					if (info.Gate != UINT_MAX)
					{
						vid = MAKE_UT_GATE_VID(0, _deviceNumber, info.Channel + 1, info.Gate + 1, info.Index);
					}
					else
					{ // Check if this is a non channel parameter.
						if (info.Channel != UINT_MAX)
						{
							vid = MAKE_UT_CHAN_VID(0, _deviceNumber, info.Channel + 1, info.Index);
						}
						else
						{
							// Create a non channel variable ID.
							vid = MAKE_UT_VID(0, _deviceNumber, info.Index);
						}
					}
				}
				break;

			case atEddyCurrent:
				if (info.Gate != UINT_MAX)
				{
					vid = MAKE_EC_SIGNAL_VID(0, _deviceNumber, info.Channel + 1, info.Gate + 1, info.Index);
				}
				else
				{
					// Check if this is a non channel parameter.
					if (info.Channel != UINT_MAX)
					{
						vid = MAKE_EC_CHAN_VID(0, _deviceNumber, info.Channel + 1, info.Index);
					}
					else
					{
						vid = MAKE_EC_VID(0, _deviceNumber, info.Index);
					}
				}
				break;
		}
		// Get the setup string for comparison.
		std::string setup = createSetupString(info, vid);
		// Lookup an existing entry in the variable list.
		auto index = variableListFind(id);
		// When the entry was found check if it has to be changed.
		if (index != Variable::PtrVector::npos)
		{
			ExtraInfo* ei = castExtraInfo(_variableVector[index]);
			// If the setup string does match, the variable does not have
			// to be recreated.
			if (setup != ei->_setupString || ei->_flags != info.Flags)
			{
				// Get the current value from the interface for setting later on.
				Value curval;
				_acquisition->getParam(ei->_id, curval);
				// Create a new one.
				Variable* var = _variableVector[index];
				// Check if the variable successfully was created.
				if (!createVariable(var, info, setup))
				{ // Remove the current variable.
					destroyVariable(var);
				}
				// Do not call our own event handler for this event.
				var->setCur(curval, true);
			}
		}
		else
		{ // Create a new one.
			Variable* var = nullptr;
			if (createVariable(var, info, setup))
			{
				// Synchronise the variables and interface parameters.
				// Get the current value of the implementation.
				Value curval;
				if (var && _acquisition->getParam(info.Id, curval))
				{
					// On success update the variable counter-part.
					// Do not call our own event handler for this event.
					var->setCur(curval, true);
				}
			}
		}
	}
}

void RsaServer::evaluateInterfaceResults()
{
	// Vector to hold all result ids.
	IdList ids;
	// Get all the results ids.
	_acquisition->enumResultIds(ids);
	// Check if the list size is non-zero.
	if (_resultVector.count())
	{
		// Iterate backwards through the list of existing results.
		for (auto it = _resultVector.rbegin(); it != _resultVector.rend(); it++)
		{
			// Get the extra info pointer of the result data instance.
			// If the info struct is present continue.
			if (auto ei = castExtraInfo(*it))
			{ // Check if the ID is found in not wanted list of ID's.
				if (ids.find(ei->_id) == IdList::npos)
				{
					// Call the special function to delete a variable from the VarList.
					destroyResultData(*it);
				}
			}
		}
	}
	// Iterate through all result ids.
	for (unsigned i = 0; i < ids.count(); i++)
	{
		// Declare a result info structure.
		ResultInfo info;
		// Get the result information of the id.
		if (!_acquisition->getResultInfo(ids[i], info))
		{
			continue;
		}
		// Form the result id.
		long rid = 0;
		// Change behaviour on the basis implementation type.
		switch (_acquisition->getType())
		{
			case atUltrasonic:
				// Check if old style naming is required.
				if (_compatible == 1 || _compatible == 2)
				{
					if (info.Gate != UINT_MAX)
					{
						rid = OLD_MAKE_UT_GATE_VID(0, _deviceNumber, info.Channel + 1, info.Gate, info.Index);
					}
					else
					{
						rid = OLD_MAKE_UT_CHAN_VID(0, _deviceNumber, info.Channel + 1, info.Index);
					}
				}
				else
				{
					if (info.Gate != UINT_MAX)
					{
						rid = MAKE_UT_GATE_VID(0, _deviceNumber, info.Channel + 1, info.Gate + 1, info.Index);
					}
					else
					{ // Check if this is a non channel parameter.
						if (info.Channel != UINT_MAX)
						{
							rid = MAKE_UT_CHAN_VID(0, _deviceNumber, info.Channel + 1, info.Index);
						}
						else
						{
							// Create a non channel variable ID.
							rid = MAKE_UT_VID(0, _deviceNumber, info.Index);
						}
					}
				}
				break;

			case atEddyCurrent:
				if (info.Gate != UINT_MAX)
				{
					rid = MAKE_EC_SIGNAL_VID(0, _deviceNumber, info.Channel + 1, info.Gate + 1, info.Index);
				}
				else
				{
					// Check if this is a non channel parameter.
					if (info.Channel != UINT_MAX)
					{
						rid = MAKE_EC_CHAN_VID(0, _deviceNumber, info.Channel + 1, info.Index);
					}
					else
					{
						rid = MAKE_EC_VID(0, _deviceNumber, info.Index);
					}
				}
				break;
		}
		// Get the setup string for comparison.
		std::string setup = createSetupString(info, rid);
		// Lookup an existing entry in the result list.
		auto index = resultListFind(ids[i]);
		// When the entry was found check if it has to be changed.
		if (index != ResultData::PtrVector::npos)
		{
			ResultData* res = _resultVector[index];
			ExtraInfo* ei = castExtraInfo(res);
			// If the setup string does match the result
			// does not have to be recreated.
			if (setup != ei->_setupString)
			{
				// Check if only the flags fields has changed.
				auto fields = CompareCsfFields(setup, ei->_setupString);
				if ((fields & ~(1L << ResultData::rfFlags)) == 0)
				{
					//SF_RTTI_NOTIFY(DO_CLOG, "Applying new Flags " << res->getName());
					// rfeFLAGS
					flags_type flags = ResultData::flgShare;
					// When not generating data the result is put in recycle mode.
					if (info.Flags & rfStored)
					{
						flags |= ResultData::flgArchive;
					}
					else
					{
						flags |= ResultData::flgRecycle;
					}
					// Update the flags on the result.
					if (res->updateFlags(flags, true))
					{
						// Assign the new setup string.
						ei->_setupString = setup;
					}
				}
				else
				{ // Check if the instance was successfully created.
					if (!createResultData(res, info, setup))
					{  // If not, destroy it.
						destroyResultData(res);
					}
				}
			}
		}
		else
		{
			ResultData* res = nullptr;
			// Create a new one.
			createResultData(res, info, setup);
		}
	}

}

void RsaServer::destroyInterface()
{
	// Stop the acquisition implementation.
	if (_acquisition)
	{
		_acquisition->setRunMode(false, true);
	}
	// Flush all results and variables from the information server.
	flush();
	// Delete all variables in reverse order.
	for (auto it = _variableVector.rbegin(); it != _variableVector.rend(); it++)
	{
		destroyVariable(*it);
		if (_variableVector.find(*it) != Variable::PtrVector::npos)
		{
			SF_RTTI_NOTIFY(DO_CERR, "Duplicate variable pointer found!");
		}
	}
	// Delete all results in reverse order.
	for (auto it = _resultVector.rbegin(); it != _resultVector.rend(); it++)
	{
		destroyResultData(*it);
		if (_resultVector.find(*it) != ResultData::PtrVector::npos)
		{
			SF_RTTI_NOTIFY(DO_CERR, "Duplicate result-data pointer found!");
		}
	}
	if (_variableVector.count() || _resultVector.count())
	{
		SF_RTTI_NOTIFY(DO_CERR, "Some how not all of the GII objects were destroyed!");
	}
}

void RsaServer::onStateChange(EState prevState, EState nextState)
{
	if (_acquisition)
	{
		switch (nextState)
		{
			default:
				break;

			case issOff: SF_RTTI_NOTIFY(DO_CLOG, "Run Mode set OFF and Data CLEARED");
				_acquisition->setRunMode(false, true);
				break;

			case issRun: SF_RTTI_NOTIFY(DO_CLOG, "Run Mode set ON and Data CLEARED");
				_acquisition->setRunMode(true, true);
				break;

			case issRecord: SF_RTTI_NOTIFY(DO_CLOG, "Run Mode set ON and Data CLEARED");
				// Clear can be prevented when coming from PAUSED state.
				if (prevState == issPause)
				{
					_acquisition->setRunMode(true, false);
				}
				else
				{
					_acquisition->setRunMode(true, true);
				}
				break;

			case issPause:
			case issStop: SF_RTTI_NOTIFY(DO_CLOG, "Run Mode set OFF");
				_acquisition->setRunMode(false, false);
				break;
		}
	}
}

void RsaServer::clearValidations()
{
	InformationServer::clearValidations();
}

void RsaServer::setLocked(bool lock)
{
	if (lock != _lock)
	{
		_lock = lock;
		checkReadOnly();
	}
}

void RsaServer::checkReadOnly()
{
	// Set up the mask needed checking.
	int mask = 0;
	// Iterate through the list fo variables.
	for (auto var: _variableVector)
	{
		int flags = castExtraInfo(var)->_flags;
		// When the parameter is readonly quit here.
		if (flags & pfReadonly)
		{
			continue;
		}
		//
		if ((flags & mask) || (_lock && !(flags & pfWriteable)))
		{
			var->setFlag(Variable::flgReadonly);
		}
		else
		{
			var->unsetFlag(Variable::flgReadonly);
		}
	}
}

void RsaServer::serverVariableHandler
	(
		Variable::EEvent event,
		const Variable& caller,
		Variable& link,
		bool sameInst
	)
{
	// get ID
	auto vid = caller.getId();
	switch (event)
	{
		default:
			break;

		case Variable::veValueChange:
			// Only when geting other id then zero.
			if (vid == _implementationId)
			{
				// Create a new implementation of type passed by the value.
				createImplementation(static_cast<int>(link.getCur().getInteger() - 1));
			}
			else
			{ // Get the flags now from the extra info because it could be that
				// this variable does not exist anymore after the SetGetParam() call.
				int flags = castExtraInfo(&link)->_flags;
				// Temporary storage of current run mode.
				bool runmode = _acquisition->getRunMode();
				// When param notify function was called prevent unneeded extra work.
				if (!sameInst || _handledParamId != castExtraInfo(&link)->_id)
				{
					// Create a temporary value for the current value and the return value.
					Value value(link.getCur());
					// Check if this parameter has effect on other the results geometry
					// and can be written to when run mode is true.
					// If so turn of run mode when run mode was ON.
					if (runmode && flags & (pfWriteAtOff | pfEffectsResult))
					{
						// Set the acquisition implementation to OFF.
						_acquisition->setRunMode(false, false);
						SF_COND_RTTI_NOTIFY(flags & pfWriteAtOff, DO_CLOG, "Run Mode set OFF before parameter change.");
					}
					// Set parameter value to the current variable value.
					// Skip the event because an unbreakable loop is eminent.
					_acquisition->paramSetGet(castExtraInfo(&link)->_id, value, true);
					// Check if the variable is still valid.
					// which means it means in this case it is not deleted by
					// the previous call to SetGetParam.
					if (link.isValid())
					{ // Set the current value because it might be changed.
						link.setCur(value, true);
					}
				}
				// Check if this parameter has effect on other parameter's geometry.
				if (flags & pfEffectsParameter)
				{
					// If so  reevaluate the parameters.
					evaluateInterfaceParams();
				}
				// Check if this parameter had an effect on other result's geometry.
				if (flags & pfEffectsResult)
				{ // Set the acquisition implementation to off and clear the current data.
					_acquisition->setRunMode(false, true);
					// If so  reevaluate the results.
					evaluateInterfaceResults();
					// Clear any residual results.
					clearValidations();
				}
				// TODO: Maybe this should be done in a sustain using a flag/timer.
				// Switch on run mode again when it was switched OFF.
				if (runmode && !_acquisition->getRunMode())
				{ // Enable run-mode again
					_acquisition->setRunMode(true, false);
					SF_COND_RTTI_NOTIFY(flags & pfWriteAtOff, DO_CLOG, "Run Mode set ON after parameter change.");
				}
			}
			break;

		case Variable::veIdChanged:
		case Variable::veRemove:    // variable is being removed
		case Variable::veGetOwner:  // this instance has lost ownership
		case Variable::veLostOwner: // this instance has become owner
		case Variable::veLinked:    // this instance has been linked
		case Variable::veUnlinked:  // this instance has lost link
		case Variable::veInvalid:   // variable reference is becoming invalid for this instance
			break;
	}//switch
}

void RsaServer::serverResultDataHandler(ResultData::EEvent, const ResultData&, ResultData&, const Range&, bool)
{
}

void RsaServer::paramNotify(id_type id)
{
	if (id)
	{
		Value value;
		// Get the current value from the interface.
		if (!_acquisition->getParam(id, value))
		{
			SF_RTTI_NOTIFY(DO_CLOG, stringf("Interface parameter [0x%lX] not found!", id));
		}
		else
		{
			// Set the kind of sentry when handling a parameter.
			_handledParamId = id;
			auto i = variableListFind(id);
			if (i != Variable::PtrVector::npos)
			{
				_variableVector[i]->setCur(value, false);
			}
			else
			{
				SF_RTTI_NOTIFY(DO_CLOG, stringf("Interface parameter [0x%lX] not found!", id));
			}
			// Clear kind of sentry.
			_handledParamId = 0;
		}
	}
	else
	{
		// Make sure that read only is read only.
		checkReadOnly();
	}
}

void RsaServer::resultNotify(InformationTypes::id_type id)
{
//  RTTI_NOTIFY(DO_DEFAULT, "ResultNotify("<< id << ")");
	// Looks up a result data id.
	auto idx = resultListFind(id);
	// When found process the available.
	if (idx != ResultData::PtrVector::npos)
	{ // Get a pointer to the result.
		ResultData* res = _resultVector[idx];
		unsigned blkByteSize = res->getBufferSize(1);
		// Declare buffer info structure to receive
		BufferInfo bufInfo;
		do
		{ // Reset the data members of the info structure.
			bufInfo.Clear();
			// Get the buffer information from the implementation.
			if (_acquisition->getResultBuffer(id, bufInfo) && bufInfo.BlockBufSize)
			{ // Write the data into the result.
				if (blkByteSize != bufInfo.BlockBufSize)
				{
					throw Exception("ResultNotify(): Block size '%s' is not of the expected size!", res->getName(2).c_str());
				}
				if (!res->blockWrite(-1L, bufInfo.Size, bufInfo.Buffer, true))
				{
					SF_RTTI_NOTIFY(DO_CLOG, "BlockWrite() Failed!");
					break;
				}
				// Commit all writes.
				res->commitValidations();
			}
		}
		while (bufInfo.Remain);
	}
}

}