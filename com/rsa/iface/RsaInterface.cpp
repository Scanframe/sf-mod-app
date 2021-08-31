#include <misc/gen/dbgutils.h>
#include <misc/gen/Exception.h>
#include <misc/gen/Value.h>
#include <misc/gen/TVector.h>
#include <misc/gen/ConfigLocation.h>
#include <misc/gen/IniProfile.h>

#include "RsaInterface.h"

namespace sf
{


// Implementations of static functions and data members to be able to create registered stars implementations.
SF_IMPL_IFACE(RsaInterface, RsaInterface::Parameters, Interface)

RsaInterface::RsaInterface(const RsaInterface::Parameters&)
	:_paramNotifyProcedure(nullptr)
	 , _resultNotifyProcedure(nullptr)
	 , _paramNotifyData(nullptr)
	 , _resultNotifyData(nullptr)
	 , _initialized(false)
	 , _acquisitionType(atUltrasonic)
{
}

RsaInterface::~RsaInterface()
{
	// Should actually be done in the derived class's destructor.
	uinitialize();
}

RsaTypes::EAcquisitionType RsaInterface::getType() const
{
	return _acquisitionType;
}

void RsaInterface::setParamHook(NotifyProc proc, void* data)
{
	_paramNotifyProcedure = proc;
	_paramNotifyData = data;
}

void RsaInterface::setResultHook(NotifyProc proc, void* data)
{
	_resultNotifyProcedure = proc;
	_resultNotifyData = data;
}

void RsaInterface::addPropertyPages(PropertySheetDialog* sheet)
{
	// Deliberate not implemented.
}

bool RsaInterface::uinitialize()
{
	// Uninitialize the hardware.
	if (doInitialize(false))
	{
		return true;
	}
	return false;
}

bool RsaInterface::initialize()
{
	bool rv = false;
	// Prevent initialize from being called twice or when an error occurred.
	if (!_initialized && getError() == 0)
	{ // Try to initialize the hardware driver.
		if (doInitialize(true))
		{
			// Read the implementation driver settings from profile.
			rv = settingsReadWrite(true);
			// Set the sentry for initializing after setting the ReadWrite function.
			// This because IsInitialized is used by the implementation.
			_initialized = true;
		}
	}
	return rv;
}

bool RsaInterface::setType(EAcquisitionType at)
{
	// Not allowed after initialization.
	if (_initialized)
	{
		return false;
	}
	// Assign the data member.
	_acquisitionType = at;
	return true;
}

bool RsaInterface::getResultInfo(IdType id, ResultInfo& info)
{
	return handleResult(id, &info, nullptr);
}

bool RsaInterface::getResultBuffer(IdType id, BufferInfo& bufInfo)
{
	return handleResult(id, nullptr, &bufInfo);
}

bool RsaInterface::getParamInfo(IdType id, ParamInfo& info) const
{
	return const_cast<RsaInterface*>(this)->handleParam(id, &info, nullptr, nullptr);
}

bool RsaInterface::getParam(IdType id, Value& value) const
{
	return const_cast<RsaInterface*>(this)->handleParam(id, nullptr, nullptr, &value);
}

bool RsaInterface::setParam(IdType id, const Value& value, bool skip_event)
{
	// When skip event is true simple handling.
	bool rv = handleParam(id, nullptr, &value, nullptr);
	// Call the hook interface handler when the parameter was set successfully.
	if (!skip_event && rv)
	{
		callParamHook(id);
	}
	//
	return rv;
}

bool RsaInterface::paramSetGet(IdType id, Value& value, bool skip_event)
{
	// When skip event is true simple handling.
	if (handleParam(id, nullptr, &value, &value))
	{
		// Call the hook interface handler when the parameter was set successfully.
		if (!skip_event)
		{
			callParamHook(id);
		}
		// Signal success.
		return true;
	}
	SF_RTTI_NOTIFY(DO_DEFAULT, "SetGetParam: Setting the param " << id << " failed!");
	// On failure to set the parameter retrieve the current value.
	handleParam(id, nullptr, nullptr, &value);
	// Signal failure.
	return false;
}

void RsaInterface::callParamHook(IdType id)
{
	if (_paramNotifyProcedure && _initialized)
	{
		_paramNotifyProcedure(_paramNotifyData, id);
	}
}

void RsaInterface::callResultHook(IdType id)
{
	if (_resultNotifyProcedure)
	{
		_resultNotifyProcedure(_resultNotifyData, id);
	}
}

bool RsaInterface::getParam
	(
		EDefaultParam param,
		unsigned gate,
		unsigned ch,
		Value& value
	) const
{
	// Get the local ID of the default parameter.
	auto id = getParamId(param, gate, ch);
	// Check the ID for non-existence.
	if (id)
	{
		return getParam(id, value);
	}
	//
	SF_RTTI_NOTIFY(DO_DEFAULT,
		"GetParam: param" << param << " of channel, gate: " << ch << ", " << gate << " is not present!");
	return false;
}

bool RsaInterface::setParam
	(
		EDefaultParam param,
		unsigned gate,
		unsigned ch,
		const Value& value,
		bool skip_event
	)
{
	// Get the local ID of the default parameter.
	auto id = getParamId(param, gate, ch);
	// Check the ID for non-existence.
	if (id)
	{
		return setParam(id, value, skip_event);
	}
	// Signal ID failure.
	SF_RTTI_NOTIFY(DO_DEFAULT, "GetParam: param" << param << " of channel, gate: "
		<< ch << ", " << gate << " is not present!");
	return false;
}

int RsaInterface::getError() const
{
	Value rv;
	if (!getParam(getParamId(apError), rv))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Interface parameter apERROR is not implemented!");
		// Set a severe error.
		//rv.set(-1);
	}
	return static_cast<int>(rv.getInteger());
}

unsigned RsaInterface::getGateCount(unsigned channel)
{
	Value value;
	if (!getParam(getParamId(apChannel_Gates, 0, channel), value))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Gate count parameter is not implemented!");
	}
	return (unsigned) value.getInteger();
}

unsigned RsaInterface::getChannelCount()
{
	Value value;
	if (!getParam(getParamId(apChannels, 0, 0), value))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Channel count parameter is not implemented!");
	}
	return (unsigned) value.getInteger();
}

std::string RsaInterface::getGateName(unsigned int gate, unsigned int channel)
{
	Value rv;
	if (!getParam(getParamId(apGate_Name, gate, channel), rv))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Gate name parameter is not implemented!");
	}
	return rv.getString();
}

bool RsaInterface::setPopManual(int channel, EPopManual pm)
{
	// Any status above un init is okay.
	if (_initialized)
	{
		Value value(pm);
		if (setParam(apChannel_PopManual, UINT_MAX, channel, value, false))
		{
			if (getParam(apChannel_PopManual, UINT_MAX, channel, value))
			{
				return pm == value.getInteger();
			}
		}
	}
	return false;
}

RsaTypes::EPopManual RsaInterface::getPopManual(int channel) const
{
	if (_initialized)
	{
		Value value(0);
		if (getParam(apChannel_PopManual, UINT_MAX, channel, value))
		{
			return (EPopManual) value.getInteger();
		}
	}
	return (EPopManual) 0;
}

std::string RsaInterface::getProfilePath() const
{
	return getConfigLocation({}, true) + "rsa-iface.cfg";
}

bool RsaInterface::settingsReadWrite(bool rd)
{
	try
	{
		IniProfile profile("", getProfilePath());
		IdList ids;
		// Get the list of valid ID's.
		bool rv = enumParamIds(ids);
		// Iterate through the list twice when reading parameters.
		// This is to be sure the order of parameters being set is not a real problem.1
		//  for (unsigned n = 0; n < (rd?2:1); n++)
		for (unsigned i = 0; i < ids.count(); i++)
		{
			ParamInfo info;
			// Get the parameter info.
			if (getParamInfo(ids[i], info))
			{
				// Check if the variable is a system setting parameter.
				if (info.Flags & pfSystem)
				{
					// Set the section based on the Gate.
					if (info.Flags & pfGate)
					{
						profile.setSection(stringf("Gate %i", info.Gate));
					}
					else
					{
						profile.setSection("General");
					}
					// If no name exists use the ID.
					if (info.Name.length())
					{
						// Only add channel when not a general param.
						if (info.Channel != UINT_MAX)
						{
							info.Name.insert(0, stringf("Channel %i|", info.Channel + 1));
						}
					}
					else
					{
						info.Name = stringf("0x%X", info.Id);
					}

					if (rd)
					{
						// Read the value info value class.
						Value value(profile.getString(info.Name, info.Default.getString()));
						// Write the parameter to the interface.
						if (!setParam(ids[i], value, false))
						{
							rv = false;
						}
					}
					else
					{ // Temporary value for storage.
						Value value;
						// Get the parameter.
						if (getParam(ids[i], value))
						{
							// Write it to the parameter value to the profile.
							profile.setString(info.Name, value.getString());
						}
						else
						{
							rv = false;
						}
					}
				}
			}
		}
		// Signal if something went wrong.
		return rv;
	} // try
	catch (std::exception& ex)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "ReadWriteSettings() Exception: " << ex.what() << "!");
		return false;
	}
}

}
