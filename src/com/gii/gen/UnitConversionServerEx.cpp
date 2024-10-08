#include "UnitConversionServerEx.h"
#include "UnitConversion.h"
#include <misc/gen/ScriptEngine.h>

namespace sf
{

UnitConversionServerEx::Entry::Entry(UnitConversionServerEx* owner)
	: _owner(owner)
{
	// Add entry at the end.
	_owner->_entries.push_back(std::unique_ptr<Entry>(this));
	// Link the handler to the master.
	_vMaster.setHandler(this);
}

void UnitConversionServerEx::Entry::variableEventHandler(VariableTypes::EEvent event, const Variable& call_var, Variable& link_var, bool same_inst)
{
	// When the master variable changes all dependent variables needs to have their conversion values set.
	if (event == Variable::veValueChange || event == Variable::veIdChanged)
	{
		updateConversion();
	}
}

void UnitConversionServerEx::Entry::updateConversion()
{
	// Iterate through the ID's and set the conversion values.
	for (auto id: _ids)
	{
		const Variable& v(Variable::getInstanceById(id));
		// Check if the variable exists at all.
		if (v.getId())
		{
			if (!convertVariable(const_cast<Variable&>(v)))
			{
				_owner->convertVariable(const_cast<Variable&>(v), true);
			}
		}
	}
}

bool UnitConversionServerEx::Entry::convertVariable(Variable& var) const
{
	// When master is attached use the script and unit.
	if (_owner->_enabled && _vMaster.getId() && !_script.empty())
	{
		// Get the master value for calculation of the multiplication factor using the script.
		auto factor = calculator(_script, 1.0, _vMaster.getCur(false).getFloat());
		// Significant digits calculated using the factor and both variable rounding values.
		auto sig = digits(_vMaster.getRnd(false).getFloat() * factor * var.getRnd(false).getFloat());
		//		auto sig = 1;
		//		sig += magnitude(_vMaster.getRnd(false).getFloat());
		//		sig -= magnitude(factor);
		//		sig -= magnitude(var.getRnd(false).getFloat());
		//		qDebug() << "Sig = " << sig;
		// Declare multiplier and offset to be filled in.
		double multiplier = 1;
		double offset = 0;
		std::string new_unit;
		int new_sig;
		// Factor cannot be zero, so revert to regular conversion.
		if (std::abs(factor) > std::numeric_limits<double>::denorm_min())
		{
			// Check if a conversion is found.
			if (getUnitConversion(var.getConvertOption(), _unit, sig, multiplier, offset, new_unit, new_sig))
			{
				multiplier *= factor;
				// Check if not near zero.
				if (std::abs(multiplier) > std::numeric_limits<double>::denorm_min())
				{
					// Apply the values.
					var.setConvertValues(new_unit, Value(multiplier), Value(offset), new_sig);
					return true;
				}
				else
				{
					SF_RTTI_NOTIFY(DO_CERR, "Conversion multiplier is zero for unit and precision: " << _unit << "," << sig);
				}
			}
		}
	}
	// Signal conversion failed and must be done the regular way.
	return false;
}

UnitConversionServerEx::UnitConversionServerEx()
{
	// Set the handler to receive the global conversion events.
	Variable::setConvertHandler(this);
	// Set the handler to receive the NewId events.
	_vEnable.setHandler(this);
}

UnitConversionServerEx::~UnitConversionServerEx()
{
	// Reset the handlers.
	_vEnable.setHandler(nullptr);
	Variable::setConvertHandler(nullptr);
}

void UnitConversionServerEx::flush()
{
	_map.clear();
}

void UnitConversionServerEx::setEnable(bool enable)
{
	// When no change abort here.
	if (_enabled != enable)
	{
		// Assign the new value.
		_enabled = enable;
		//
		for (auto& e: _map)
		{
			e.second->updateConversion();
		}
	}
}

bool UnitConversionServerEx::load(std::istream& is)
{
	if (UnitConversionServer::load(is))
	{
		configReadWrite(false);
		return true;
	}
	return false;
}

bool UnitConversionServerEx::save(std::ostream& os)
{
	if (UnitConversionServer::save(os))
	{
		configReadWrite(true);
		return true;
	}
	return false;
}

void UnitConversionServerEx::configReadWrite(bool save)
{
	// Set or create the current section.
	_profile.setSection(getFollowersSectionName());
	if (!save)
	{
		// Delete all current entries.
		flush();
		// Temporary list to store all the key names.
		strings keys = _profile.getKeys();
		// Iterate through the list.
		for (auto& key: keys)
		{
			// Get the numeric id from the profile key.
			auto id = toNumberDefault<id_type>(key, 0);
			// Passing this class as the owner will attach it to the list.
			auto entry = new Entry(this);
			// Setup master variable using the id.
			entry->_vMaster.setup(id, true);
			// Get the value as a string list.
			strings sl;
			sl.split(_profile.getString(key), ',', '\'');
			// Check the size for script and unit.
			if (!sl.empty())
			{
				entry->_script = sl[0];
				if (sl.size() >= 2)
				{
					entry->_unit = sl[1];
					// Other entries in the list are the followers of the master.
					for (strings::size_type i = 2; i < sl.size(); i++)
					{
						// Convert the 64bit id decimal or hex.
						id = toNumberDefault<id_type>(sl[i], 0);
						// Only add when a non-zero id.
						if (id)
						{
							entry->_ids.push_back(id);
						}
					}
				}
			}
		}
	}
	else
	{
		for (auto& e: _entries)
		{
			//ce->updateConversion();
			auto key = "0x" + itostr(e->_vMaster.getDesiredId(), 16);
			auto value = "'" + e->_script + "'," + e->_unit;
			for (auto id: e->_ids)
			{
				value.append(",0x" + itostr(id, 16));
			}
			_profile.setString(key, value);
		}
	}
}

void UnitConversionServerEx::convertVariable(Variable& var, bool regular)
{
	// Flag for doing stuff th regular way.
	bool done = false;
	if (!regular)
	{
		// Are there any entries of passed ID in the list.
		auto it = _map.find(var.getId());
		// If so use the entry convert function.
		if (it != _map.end())
		{
			done = (*it).second->convertVariable(var.getOwner());
			//done = true;
		}
	}
	if (!done)
	{
		int sig = var.getSigDigits(false);
		std::string unit = var.getUnit(false);
		double multiplier = 1.0;
		double offset = 0.0;
		// Check if a conversion is found.
		// Since 'unit' is changed the from_unit needs to be a copy.
		if (getUnitConversion(var.getConvertOption(), std::string(unit), sig, multiplier, offset, unit, sig))
		{
			// Check if not near zero.
			if (std::abs(multiplier) > std::numeric_limits<double>::denorm_min())
			{
				// Apply the values.
				const_cast<Variable&>(var).getOwner().setConvertValues(unit, Value(multiplier), Value(offset), sig);
				// Return from function to prevent executing last part which disables the conversion.
				return;
			}
			else
			{
				SF_RTTI_NOTIFY(DO_CERR, "Conversion multiplier is zero for unit and precision: " << unit << "," << sig);
			}
		}
		// Disable the conversion on this variable.
		const_cast<Variable&>(var).getOwner().setConvertValues(false);
	}
}

void UnitConversionServerEx::variableEventHandler(VariableTypes::EEvent event, const Variable& call_var, Variable& link_var, bool same_inst)
{
	switch (event)
	{
		default:
			break;

		case veConvert: {
			// Lookup the variable if it should be converted in a dependency list.
			if (getUnitSystem() != usPassThrough)
			{
				convertVariable(const_cast<Variable&>(call_var), false);
			}
			break;
		}

		case veNewId: {
			// Let the variable resolve the unit conversion.
			const_cast<Variable&>(call_var).getOwner().setConvertValues(getUnitSystem() != usPassThrough);
			break;
		}

		// For variable controlling the enable-status.
		case veIdChanged:
		case veValueChange: {
			// Enable or disable conversion lists when there is an ID.
			if (link_var.getId())
			{
				setEnable(!link_var.getCur().isZero());
			}
			break;
		}
	}
}

void UnitConversionServerEx::reinitialize()
{
	// Delete all current entries.
	_map.clear();
	// Fill the map again.
	for (auto& entry: _entries)
	{
		for (auto id: entry->_ids)
		{
			_map[id] = entry.get();
		}
	}
	// Update the existing global variables.
	for (auto var: Variable::getList())
	{
		var->getOwner().setConvertValues(getUnitSystem() != usPassThrough);
	}
}

Variable::id_type UnitConversionServerEx::getEnableId() const
{
	return _vEnable.getDesiredId();
}

void UnitConversionServerEx::setEnableId(id_type id)
{
	_vEnable.setup(id, true);
}

void UnitConversionServerEx::setAskHandler(const UnitConversionServerClosure& closure)
{
	_askHandler.assign(closure);
}

bool UnitConversionServerEx::Handler(UnitConversionEvent& ev)
{
	// Try first to get a conversion.
	auto rv = UnitConversionServer::Handler(ev);
	// Check if the handler was successful and if not, if an ask handler was assigned.
	if (!rv && _askHandler && getUnitSystem() != usPassThrough)
	{
		// Ask for user or other input.
		if (_askHandler(ev))
		{
			// Update the conversion configuration.
			setConversion(ev);
			rv = true;
		}
	}
	return rv;
}

void UnitConversionServerEx::setUnitSystem(int us)
{
	// Detect a change is selection.
	if (us != getUnitSystem())
	{
		UnitConversionServer::setUnitSystem(us);
		// Reinitialize when a unit system change occurred.
		reinitialize();
	}
}

const char* UnitConversionServerEx::getFollowersSectionName() const
{
	return "Followers";
}

}// namespace sf