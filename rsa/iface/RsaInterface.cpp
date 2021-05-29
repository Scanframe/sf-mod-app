#include <misc/gen/dbgutils.h>
#include <misc/gen/Exception.h>
#include <misc/gen/Value.h>
#include <misc/gen/gen_utils.h>
#include <misc/gen/TVector.h>

#include "RsaInterface.h"

namespace sf
{

// Implementations of static functions and data members to be able to create registered stars implementations.
SF_IMPL_IFACE(RsaInterface, RsaInterface::Parameters, Interface)

RsaInterface::RsaInterface(const RsaInterface::Parameters&)
	:ParamNotifyProc(nullptr)
	 , ResultNotifyProc(nullptr)
	 , ParamNotifyData(nullptr)
	 , ResultNotifyData(nullptr)
	 , FInitialized(false)
	 , FAcquisitionType(atULTRASONIC)
{
}

RsaInterface::~RsaInterface()
{
	// Should actually be done in the derived classes destructor.
	Uninitialize();
}

void RsaInterface::AddPropertyPages(QWidget*)
{
	// Deliberate not implemented.
}

bool RsaInterface::Uninitialize()
{
	// Uninitialize the hardware.
	if (DoInitialize(false))
	{
		return true;
	}
	return false;
}

bool RsaInterface::Initialize()
{
	bool retval = false;
	// Prevent initialize from being called twice or when an error occurred.
	if (!FInitialized && GetError() == 0)
	{ // Try to initialize the hardware driver.
		if (DoInitialize(true))
		{
			// Read the implementation driver settings from profile.
			retval = ReadWriteSettings(true);
			// Set the sentry for initializing after setting the ReadWrite function.
			// This because IsInitialized is used by the implementation.
			FInitialized = true;
		}
	}
	return retval;
}

bool RsaInterface::SetType(EAcquisitionType at)
{
	// Not allowed after initialization.
	if (FInitialized)
	{
		return false;
	}
	// Assign the data member.
	FAcquisitionType = at;
	return true;
}

bool RsaInterface::GetResultInfo(int id, ResultInfo& info)
{
	return HandleResult(id, &info, nullptr);
}

bool RsaInterface::GetResultBuffer(int id, BufferInfo& bufinfo)
{
	return HandleResult(id, nullptr, &bufinfo);
}

bool RsaInterface::GetParamInfo(int id, ParamInfo& info) const
{
	return const_cast<RsaInterface*>(this)->HandleParam(id, &info, nullptr, nullptr);
}

bool RsaInterface::GetParam(int id, Value& value) const
{
	return const_cast<RsaInterface*>(this)->HandleParam(id, nullptr, nullptr, &value);
}

bool RsaInterface::SetParam(int id, const Value& value, bool skip_event)
{
	// When skip event is true simple handling.
	bool retval = HandleParam(id, nullptr, &value, nullptr);
	// Call the hook interface handler when the parameter was set successfully.
	if (!skip_event && retval)
	{
		CallParamHook(id);
	}
	//
	return retval;
}

bool RsaInterface::SetGetParam(int id, Value& value, bool skip_event)
{
	// When skip event is true simple handling.
	if (HandleParam(id, nullptr, &value, &value))
	{
		// Call the hook interface handler when the parameter was set successfully.
		if (!skip_event)
		{
			CallParamHook(id);
		}
		// Signal success.
		return true;
	}
	SF_RTTI_NOTIFY(DO_DEFAULT, "SetGetParam: Setting the param " << id << " failed!");
	// On failure to set the parameter retrieve the current value.
	HandleParam(id, nullptr, nullptr, &value);
	// Signal failure.
	return false;
}

void RsaInterface::CallParamHook(int id)
{
	if (ParamNotifyProc && FInitialized)
	{
		ParamNotifyProc(ParamNotifyData, id);
	}
}

void RsaInterface::CallResultHook(int id)
{
	if (ResultNotifyProc)
	{
		ResultNotifyProc(ResultNotifyData, id);
	}
}

bool RsaInterface::GetParam
	(
		EDefaultParam param,
		unsigned gate,
		unsigned ch,
		Value& value
	) const
{
	// Get the local ID of the default parameter.
	int id = GetParamId(param, gate, ch);
	// Check the ID for non existence.
	if (id)
	{
		return GetParam(id, value);
	}
	//
	SF_RTTI_NOTIFY(DO_DEFAULT,
		"GetParam: param" << param << " of channel, gate: " << ch << ", " << gate << " is not present!");
	return false;
}

bool RsaInterface::SetParam
	(
		EDefaultParam param,
		unsigned gate,
		unsigned ch,
		const Value& value,
		bool skip_event
	)
{
	// Get the local ID of the default parameter.
	int id = GetParamId(param, gate, ch);
	// Check the ID for non existence.
	if (id)
	{
		return SetParam(id, value, skip_event);
	}
	// Signal ID failure.
	SF_RTTI_NOTIFY(DO_DEFAULT, "GetParam: param" << param << " of channel, gate: "
		<< ch << ", " << gate << " is not present!");
	return false;
}

int RsaInterface::GetError() const
{
	Value retval;
	if (!GetParam(GetParamId(apERROR), retval))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Interface parameter apERROR is not implemented!");
		// Set a severe error.
		retval.set(-1);
	}
	//
	return retval.getInteger();
}

unsigned RsaInterface::GetGateCount(unsigned channel)
{
	Value value;
	if (!GetParam(GetParamId(apCH_GATES, 0, channel), value))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Gate count parameter is not implemented!");
	}
	return (unsigned) value.getInteger();
}

unsigned RsaInterface::GetChannelCount()
{
	Value value;
	if (!GetParam(GetParamId(apCHANNELS, 0, 0), value))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Channel count parameter is not implemented!");
	}
	return (unsigned) value.getInteger();
}

std::string RsaInterface::GetGateName(int gate, int channel)
{
	Value retval;
	if (!GetParam(GetParamId(apGATE_NAME, gate, channel), retval))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Gate name parameter is not implemented!");
	}
	//
	return retval.getString();
}

bool RsaInterface::SetPopManual(int channel, EPopManual pm)
{
	// Any status above un init is okay.
	if (FInitialized)
	{
		Value value(pm);
		if (SetParam(apCH_POPMANUAL, UINT_MAX, channel, value, false))
		{
			if (GetParam(apCH_POPMANUAL, UINT_MAX, channel, value))
			{
				return pm == value.getInteger();
			}
		}
	}
	return false;
}

EPopManual RsaInterface::GetPopManual(int channel) const
{
	if (FInitialized)
	{
		Value value(0);
		if (GetParam(apCH_POPMANUAL, UINT_MAX, channel, value))
		{
			return (EPopManual) value.getInteger();
		}
	}
	return (EPopManual) 0;
}

std::string RsaInterface::GetProfilePath() const
{
	std::string path;
	// TODO: Make this work?
	/*
	path = GetFileDirAndDrive() + GetRegisterName() + ".cfg";
	path GetRedirectedProfilePath(path);
	 */
	return path;
}

bool RsaInterface::ReadWriteSettings(bool rd)
{
#if IS_RPK_TARGET
	try
	{
	try
	{
		TExtProfile profile("", GetProfilePath().c_str());
		TIdList ids;
		// Get the list of valid ID's.
		bool retval = EnumParamIds(ids);
		// Iterate through the list twice when reading parameters.
		// This is to be shure the order of parameters being set is not a real problem.1
	//  for (unsigned n = 0; n < (rd?2:1); n++)
		for (unsigned i=0; i<ids.Count(); i++)
		{
			TParamInfo info;
			// Get the parameter info.
			if (GetParamInfo(ids[i], info))
			{
				// Check if the variable is a system setting parameter.
				if (info.Flags & pfSYSTEM)
				{
					// Set the section based on the Gate.
					if (info.Flags & pfGATE)
						profile.SetSection(stringf("Gate %i", info.Gate).c_str());
					else
						profile.SetSection("General");
					// If no name exists use the ID.
					if (info.Name.length())
					{
						// Only add channel when not a general param.
						if (info.Channel != UINT_MAX)
							info.Name.prepend(stringf("Channel %i|", info.Channel + 1));
					}
					else
						info.Name = stringf("0x%X", info.Id);

					if (rd)
					{
						// Read the value info value class.
						TValue value = profile.GetString(info.Name.c_str(), info.Default.GetString().c_str());
						// Write the parameter to the interface.
						if (!SetParam(ids[i], value, false))
							retval = false;
					}
					else
					{ // Temporary value for storage.
						TValue value;
						// Get the parameter.
						if (GetParam(ids[i], value))
							// Write it to the parameter value to the profile.
							profile.WriteString(info.Name.c_str(), value.GetString().c_str());
						else
							retval = false;
					}
				}
			}
		}
		// Signal if something went wrong.
		return retval;
	} // try
	__except(true)
	{
		RTTI_NOTIFY(DO_DEFAULT, "ReadWriteSettings() intercepted exception!");
		return false;
	}
	} // try
	catch(exception& ex)
	{
		RTTI_NOTIFY(DO_DEFAULT, "ReadWriteSettings() Exception" << ex.what() << "!");
		return false;
	}
#else
	return true;
#endif // IS_RPK_TARGET
}

}
