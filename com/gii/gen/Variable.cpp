#include <cmath>
#include <cstdio>

#include <misc/gen/dbgutils.h>
#include <misc/gen/gen_utils.h>
#include <misc/gen/Value.h>
#include <misc/gen/csf.h>

#include "UnitConversion.h"
#include "Variable.h"
// Not part of the public interface.
#include "VariableStatic.h"
#include "VariableReference.h"

namespace sf
{

Variable::Variable()
{
	_global = true;
	attachRef(VariableStatic::zero()._reference);
}

Variable::Variable(void*, void*)
{
	if (!VariableStatic::_zero)
	{
		// Assign the zero variable.
		VariableStatic::_zero = this;
		_global = true;
		setup(std::string("0x0,n/a,n/a,R,n/a,n/a,n/a,,,,,?=?,"), 0);
	}
}

Variable::Variable(bool global)
{
	_global = global;
	attachRef(VariableStatic::zero()._reference);
}

bool Variable::isExported() const
{
	return _reference->_exported;
}

bool Variable::setExport(bool global)
{
	// Check out before continuing
	if (
		// Only an owner can be exported.
		!isOwner()
			// And it must be exportable.
			|| !isFlag(flgExport)
			// The reference pointer cannot be NULL.
			|| !_reference
			// The attached reference may not be the Zero one.
			|| _reference == VariableStatic::zero()._reference
			// To make an owner appear global it must be local.
			|| _global
			// The local variable ID cannot be zero when global.
			|| (global && !_reference->_id)
		)
	{
		//_RTTI_NOTIFY(DO_DEFAULT, "Cannot make this instance global!")
		// Signal failure to make it global.
		return false;
	}
	// Only continue when a change is made.
	if (_reference->_exported == global)
	{
		return true;
	}
	// Set the export flag accordingly.
	_reference->_exported = global;
	// See if the variable must be made global.
	if (global)
	{
		// Check if a global ID already exist, getReferenceById returns Zero if not found.
		VariableReference* ref = getReferenceById(_reference->_id);
		// Check if the current reference is already global.
		if (ref == _reference)
		{
			// If so signal success.
			return true;
		}
		// When the id was found.
		if (ref != VariableStatic::zero()._reference)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to export ID:\n"
				<< stringf("0x%llX,", _reference->_id) << _reference->_name << "\nover!\n" << ref->_name << '!')
			// If the ID already exist return false.
			return false;
		}
		// Notify all global linked users of Variable instances of new ID.
		emitGlobalEvent(veNewId, true);
		// Iterate through global variable instances and attach those having the desired ID just created.
		attachDesired();
	}
		// When variable must be made local again.
	else
	{
		// Create a temporary vector pointers to variables that must be attached because attachRef() modifies '_list'.
		Variable::Vector list;
		// Set the vector to reserve the maximum expected size.
		list.reserve(_reference->_list.size());
		// Iterate through all variables having the global flag Set.
		for (auto i: _reference->_list)
		{
			if (i->_global)
			{
				list.add(i);
			}
		}
		// Now iterated through the new list.
		for (auto j: list)
		{
			j->attachRef(VariableStatic::zero()._reference);
		}
	}
	// Signal success.
	return true;
}

bool Variable::setGlobal(bool global)
{
	// Only allowed when variable is not attached to other then Zero.
	if (_reference == VariableStatic::zero()._reference)
	{
		_global = global;
		return true;
	}
	return false;
}

void Variable::operator delete(void* p) // NOLINT(misc-new-delete-overloads)
{
	if (VariableStatic::_globalActive)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "Warning: Deleting instance during an event!")
		// Do not free allocated data before end of global event remove
		VariableStatic::_deleteWaitCache.add(p);
	}
	else
	{
		// Call real destructor.
		::operator delete(p);
		// If deletion is allowed again delete the wait cache too.
		TVector<void*>::size_type count = VariableStatic::_deleteWaitCache.size();
		if (count)
		{
			while (count--)
			{
				delete (char*) VariableStatic::_deleteWaitCache[count];
			}
			// Flush the entries from the list.
			VariableStatic::_deleteWaitCache.flush();
		}
	}
}

Variable::~Variable()
{
	// if a Link function exist notify removal of this instance.
	emitEvent(veRemove, *this);
	// Set link also signals the unlinking of the variable.
	setHandler(nullptr);
	// Remove this instance from the list by attach reference NULL.
	attachRef(nullptr);
	// If the temporary value is still in use do not forget to delete it.
	if (_temporary)
	{
		delete _temporary;
		_temporary = nullptr;
	}
	// When this is the zero variable being deleted null it because it is also used as a sentry.
	if (VariableStatic::_zero == this)
	{
		VariableStatic::_zero = nullptr;
	}
}

Variable::size_type Variable::getCount()
{
	ReferenceVector::size_type rv = 0;
	for (auto ref: VariableStatic::_references)
	{
		rv += (ref->_global || ref->_exported) ? 1 : 0;
	}
	// Subtract 1 for zero variable before returning.
	return --rv;
}

Variable::size_type Variable::getInstanceCount(bool global_only)
{
	ReferenceVector::size_type rv = 0;
	// Iterate through all variable references and count the usage count.
	for (auto ref: VariableStatic::_references)
	{
		if (!global_only || ref->_global)
		{
			rv += ref->_list.size();
		}
	}
	// Subtract 1 for zero variable before returning.
	return --rv;
}

Variable::Vector Variable::getList()
{
	// Return value.
	Variable::Vector rv;
	// Set the vector to reserve the maximum expected size.
	rv.reserve(VariableStatic::_references.size());
	// Iterate through the references.
	for (auto ref: VariableStatic::_references)
	{
		// Only globals and exclude zero variable which has id of zero.
		if ((ref->_global || ref->_exported) && ref->_id)
		{
			rv.insert(rv.begin(), ref->_list.at(0));
		}
	}
	return rv;
}

VariableReference* Variable::getReferenceById(Variable::id_type id)
{
	if (id)
	{
		for (auto ref: VariableStatic::_references)
		{
			if (ref->_id == id && ref->_global && ref != VariableStatic::zero()._reference)
			{
				return ref;
			}
		}
	}
	return VariableStatic::zero()._reference;
}

Variable& Variable::getInstanceById(Variable::id_type id, Variable::Vector& list)
{
	for (auto i: list)
	{
		if (i->getId() == id)
		{
			return *i;
		}
	}
	return VariableStatic::zero();
}

void Variable::makeOwner()
{
	// Do not change ownership if it is ZeroRef
	if (_reference == VariableStatic::zero()._reference)
	{
		return;
	}
	// Get the previous owner for signaling loosing of ownership.
	Variable* prev_owner = _reference->_list[0];
	// Do not change ownership if current instance is already owner
	if (prev_owner == this)
	{
		return;
	}
	// Put this pointer at the beginning of the list which makes it the owner
	_reference->_list.detach(this);
	_reference->_list.addAt(this, 0);
	// Notify previous owner of losing ownership
	prev_owner->emitEvent(veLostOwner, *this);
	// Notify this instance of getting ownership.
	emitEvent(veGetOwner, *this);
/*
  // Check
  if (_reference->_list[0] != this)
    RTTI_NOTIFY(DO_DEFAULT, "Variable is NOT Owner!");
*/
}

bool Variable::attachRef(VariableReference* ref)
{
	// If the new ref is the same as the current do nothing.
	if (_reference == ref)
	{
		return true;
	}
	// If the current instance is not global and the ref is global
	// do not attach the reference but copy the reference members.
	if ((ref != VariableStatic::zero()._reference) && ref && !_global && ref->_global)
	{
		// When the reference pointer valid continue.
		if (_reference)
		{
			// Special handling when zero ref is the current reference.
			if (_reference == VariableStatic::zero()._reference)
			{
				// Remove this variable from the Zero reference variable list.
				_reference->_list.detach(this);
				// Create a new local variable reference.
				_reference = new VariableReference(false);
				// Attach this variable to the new reference.
				_reference->_list.add(this);
			}
			// Copy the reference fields from the global reference to the current reference.
			_reference->copy(*ref);
			// Notify the user of this instance by telling it was attached.
			emitEvent(veIdChanged, *this);
			// Attachment succeeded.
			return true;
		}
	}
	// Initialize return value.
	bool rv = true;
	if (_reference)
	{
		// If this was the last one or the owner the referring reference will be deleted
		// _list[0] is the owner default the one who created the reference instance
		if (_reference->_list.empty() || this == _reference->_list[0])
		{
			// Notify all variables that this reference is becoming invalid.
			emitLocalEvent(veInvalid, false);
			// Make pointer to this variable NULL so it isn't attached again in
			// the reference destructor and detach it when the ref is deleted
			_reference->_list[0] = nullptr;
			// the destructor reattaches all variables that are left
			// in the list to the Zero reference except for the first
			// that is a NULL pointer and prevents this way the entry of this part
			// of this function because the first in the list is the owner.
			delete _reference;
		}
		else
		{ // Just remove variable from list if it is not the owner.
			if (!_reference->_list.detach(this))
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, __FUNCTION__ << ": Could not remove instance from list!")
				rv = false;
			}
		}
		// Member reference is assigned NULL to indicate no attachment
		_reference = nullptr;
	}
	//
	if (ref)
	{
		_reference = ref;
		// Add this instance to the the Variable Reference list
		_reference->_list.add(this);
		// Notify the user of this instance by telling it was attached.
		emitEvent(veIdChanged, *this);
	}
	return rv;
}

bool Variable::setId(Variable::id_type id, bool skip_self)
{
	// Only owner of local variable is allowed to do this.
	if (isOwner() && !_global)
	{
		// Check if the local instance is currently exported.
		if (!_reference->_exported)
		{
			// Check if the new ID is different and non zero.
			if (id && _reference->_id != id)
			{ // Assign the new ID.
				_reference->_id = id;
				// Notify all local referenced instances.
				emitLocalEvent(veIdChanged, skip_self);
				// Signal success.
				return true;
			}
		}
	}
	return false;
}

bool Variable::setup(const Definition& definition, Variable::id_type id_ofs)
{
	// Check for not owning and global variable.
	bool local_owner = !_global && isOwner() && getUsageCount() > 1;
	// If this is a not local owner attach it to the zero instance.
	if (!local_owner)
	{
		attachRef(VariableStatic::zero()._reference);
	}
	// Reset the desired ID data member.
	_desiredId = 0;
	// The default return value is the definitions '_valid' field.
	bool ret_val = definition._valid;
	// Check if variables other then the zero variable have an ID of zero.
	if (_reference != VariableStatic::zero()._reference && !local_owner && definition._id == 0)
	{
		ret_val = false;
	}
	// Don' bother to go on if an error occurred so far.
	if (ret_val)
	{
		// Check if the ID already exist with getReferenceById() returns the
		// zero reference when not found or if this instance is non global.
		VariableReference* ref = _global ? getReferenceById(definition._id + id_ofs) : VariableStatic::zero()._reference;
		if (ref && ref != VariableStatic::zero()._reference)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Tried to create duplicate ID: !\n"
				<< "(0x" << std::hex << (definition._id + id_ofs) << ") " << definition._name
				<< "\nover!\n(0x" << ref->_id << ") " << ref->_name << '!')
			// If ref already exist return false.
			return false;
		}
		else
		{ // Create new or use old one.
			if (local_owner)
			{
				// Reuse the current reference
				ref = _reference;
			}
			else
			{
				// Create new global or non global reference for this variable instance.
				ref = new VariableReference(_global);
			}
		}
		// InitializeBase reference members from here.
		ref->_valid = true;
		ref->_id = definition._id + id_ofs;
		ref->_name = definition._name;
		ref->_unit = definition._unit;
		ref->_convertOption = definition._convertOption;
		ref->_description = definition._description;
		ref->_flags = definition._flags;
		ref->_type = definition._type;
		// Use the original flags for the current ones.
		ref->_curFlags = ref->_flags;
		// Check for multi line string so the default value
		if (definition._type == Value::vitString && ref->_unit.find('M') != std::string::npos)
		{
			ref->_defValue.set(unescape(definition._defaultValue.getString()));
		}
		else
		{
			ref->_defValue = definition._defaultValue;
		}
		// Do not Set the current value when it is a local owner.
		if (!local_owner)
		{
			ref->_curValue.set(ref->_defValue);
		}
		ref->_rndValue = definition._roundValue;
		ref->_minValue = definition._minValue;
		ref->_maxValue = definition._maxValue;
		ref->_states = definition._states;
		// Get significant digits for the float type variable.
		if (Value::vitFloat == ref->_type)
		{
			// Get the significant digits from the round value.
			if (ref->_rndValue)
			{
				ref->_sigDigits = digits(ref->_rndValue.getFloat());
			}
			else
			{
				// Digits to maximum int because round is zero.
				ref->_sigDigits = std::numeric_limits<int>::max();
			}
		}
		else
		{
			ref->_sigDigits = 0;
		}
		// Skip type conversion if an error occurred so far.
		if (ret_val)
		{
			// Convert all Value reference data members to the wanted type
			// and check for errors during the conversion.
			ret_val &= ref->_curValue.setType(ref->_type);
			ret_val &= ref->_defValue.setType(ref->_type);
			ret_val &= ref->_maxValue.setType(ref->_type);
			ret_val &= ref->_minValue.setType(ref->_type);
			ret_val &= ref->_rndValue.setType(ref->_type);
			for (auto& s: ref->_states)
			{
				s._value.setType(ref->_type);
			}
		}
		// Attach ref to this variable if possible if everything went well so far.
		if (!ret_val || !attachRef(ref))
		{
			// Set 'ret_val' to false so an error message is generated
			ret_val = false;
			// When it is the local owner the reference is automatically
			// deleted further on.
			if (!local_owner)
			{
				// Delete the reference made with new because it failed to attach.
				delete ref;
			}
		}
		// Signal global variables of a new created variable ID.
		if (ret_val && _global)
		{
			// Notify all global linked users of Variable instances of new ID.
			emitGlobalEvent(veNewId, false);
			// Iterate through global variable instances and attach those having the desired ID just created.
			attachDesired();
		}
	}
	// When the reference was reused with all users attached.
	if (local_owner)
	{
		// When setup fails to setup this local owning instance.
		// Attach it to the Zero instance.
		if (ret_val)
		{
			// Notify the user of this instance of the redefined reference using an ID change event.
			emitLocalEvent(veIdChanged, false);
		}
		else
		{
			// This also signals the users.
			attachRef(VariableStatic::zero()._reference);
		}
	}
	// If the setup succeeded notify the handler of this event.
	if (ret_val)
	{
		emitEvent(veSetup, *this);
	}
	// In case of an error report to standard out.
	SF_COND_RTTI_NOTIFY(!ret_val, DO_DEFAULT, "Error in setup definition: "
		<< definition._name << " (0x" << std::hex << (definition._id) << ") offset (0x" << id_ofs << ")!")
	//
	return ret_val;
}

void Variable::setDesiredId(Variable::id_type id)
{
	if (_desiredId != id)
	{
		// Make the change.
		_desiredId = id;
		// Signal this event to this single instance.
		emitEvent(veDesiredId, *this);
	}
}

std::string Variable::getName(int levels) const
{
	// Return full name path at levels equals zero.
	std::string ret_val = _reference->_name;
	auto len = ret_val.length();
	// Check if levels is non zero.
	if (levels)
	{
		if (levels > 0)
		{
			int i = len;
			while (i--)
			{ // Is the character a separator character.
				if (ret_val[i] == '|')
				{
					ret_val[i] = ' ';
					if (!--levels)
					{
						break;
					}
				}
			}
			if (i && i < len - 1)
			{
				return ret_val.substr(i + 1);
			}
		}
		else
		{
			std::string::size_type p = 0;
			for (auto i = 0; i < ret_val.length(); i++)
			{
				// Is the character a separator character.
				if (ret_val[i] == '|')
				{ // Clear the separator character.
					ret_val[i] = ' ';
					//
					if (!++levels)
					{
						p = i + 1;
					}
				}
			}
			if (p < len)
			{
				return ret_val.substr(p, ret_val.length() - p);
			}
		}
	}
	return ret_val;
}

int Variable::getNameLevelCount() const
{
	int levels = 0;
	int i = _reference->_name.length();
	while (i--)
	{ // Is the character a separator character.
		if (_reference->_name[i] == '|')
		{
			levels++;
		}
	}
	return levels;
}

Variable::size_type Variable::getState(const Value& v) const
{
	for (auto i = _reference->_states.begin(); i != _reference->_states.end(); ++i)
	{
		if (i->_value == v)
		{
			return std::distance(_reference->_states.begin(), i);
		}
	}
	return std::numeric_limits<size_type>::max();
}

const Value& Variable::getStateValue(Variable::size_type state) const
{
	// Check index versus range
	if (state < _reference->_states.size())
	{
		return _reference->_states[state]._value;
	}
	// if out of range return State from ZeroRef
	return VariableStatic::zero()._reference->_states[0]._value;
}

std::string Variable::getStateName(Variable::size_type state) const
{ // check index versus range
	if (state < _reference->_states.size())
	{
		return _reference->_states[state]._name;
	}
	// if out of range return State from ZeroRef
	return VariableStatic::zero()._reference->_states[0]._name;
}

bool Variable::increase(int steps, bool skip_self)
{
	// Skip certain variables from this function.
	if (!isNumber() || _reference->_rndValue.isZero())
	{
		return false;
	}
	//
	if (_temporary)
	{
		Value& value(isConverted() ? _reference->_convertRndValue : _reference->_rndValue);
		return updateTempValue(*_temporary + (value * Value(steps)), skip_self);
	}
	else
	{
		return updateValue(_reference->_curValue + (_reference->_rndValue * Value(steps)), skip_self);
	}
}

void Variable::emitEvent(EEvent event, const Variable& caller)
{
	// If the caller is an other instance then this one when the value changes.
	// When one these events passes, Update the temporary value.
	if (_temporary)
	{
		if ((&caller != this && event == veValueChange)
			|| event == veIdChanged || event == veConverted)
		{
			// Skip self here because we're already handling an event.
			updateTemporary(true);
		}
	}
	// Check if a handler was linked and if so call it.
	if (_handler)
	{
		_handler->VariableEventHandler(event, caller, *this, &caller == this);
	}
}

Variable::size_type Variable::emitEvent(EEvent event, bool skip_self)
{ // check if the event must be global or not
	if (event < veFirstLocal)
	{
		return emitGlobalEvent(event, skip_self);
	}
	else
	{
		if (event > veFirstPrivate)
		{
			emitEvent(event, *this);
			return 1;
		}
		else
		{
			return emitLocalEvent(event, skip_self);
		}
	}
}

Variable::size_type Variable::emitLocalEvent(EEvent event, bool skip_self)
{
	// Disable deletion of local instances.
	VariableStatic::_globalActive++;
	// Temporary vector to allow changes to the queried vectors.
	Vector ev_list;
	// Set vector to reserve max expected size.
	ev_list.reserve(_reference->_list.size());
	// Iterate through list and generate the variables from which the event handler needs to be called.
	for (auto i: _reference->_list)
	{
		// Check for skip_self.
		if (i && (!skip_self || i != this))
		{
			ev_list.add(i);
		}
	}
	// Iterate through the generated list and call the event handler.
	// Any changes made by event handlers that could affect the list is avoided.
	for (auto j: ev_list)
	{
		// Call the event handler.
		j->emitEvent(event, *this);
	}
	// Enable deletion of local instances.
	VariableStatic::_globalActive--;
	// return the amount of variables that were effected by the call to this function.
	return ev_list.size();
}

Variable::size_type Variable::emitGlobalEvent(EEvent event, bool skip_self)
{
	// When the variable is not global send the event locally.
	if (!_global && !_reference->_exported)
	{
		return emitLocalEvent(event, skip_self);
	}
	// Disable deletion of instances.
	VariableStatic::_globalActive++;
	// Declare event list for instance pointers.
	Vector ev_list;
	// Iterate through all references.
	for (auto ref: VariableStatic::_references)
	{
		// Only global references are considered.
		if (ref->_global)
		{
			// Iterate through all variables of the reference.
			for (auto var: ref->_list)
			{
				// Check for skip_self and if the variable is global.
				// Only global variables get a global event.
				if (var && (!skip_self || var != this) && var->_global)
				{
					// Add variable to list.
					ev_list.add(var);
				}
			}
		}
	}
	// Iterate through the generated variable list and call the handler of them.
	// Any changes made by event handlers that could affect the list is avoided.
	for (auto var: ev_list)
	{
		var->emitEvent(event, *this);
	}
	// Enable deletion of instances.
	VariableStatic::_globalActive--;
	// Return  the lists size as the count of the events.
	return ev_list.size();
}

Variable::size_type Variable::attachDesired()
{
	// Disable deletion of instances.
	VariableStatic::_globalActive++;
	// Signal other event handler functions are called using a global predefined list.
	Vector ev_list;
	// Iterate through all references generating pointers to instances having a desired ID that must be attached.
	for (auto ref: VariableStatic::_references)
	{
		// Only applies for globals.
		if (ref->_global)
		{
			// Iterate through all variables of the reference.
			for (auto var: ref->_list)
			{
				// Check if the desired ID is non-zero which means it is enabled.
				// Also compare the desired and this ID to check if it has not be
				// linked to the reference. Also check if the variable is global
				// because global variables can only be attached by a desired ID.
				if (var && var->_global && var->_desiredId && var->_desiredId == _reference->_id)
				{
					// Add instance pointer the to list.
					ev_list.add(var);
				}
			}
		}
	}
	// Iterate through the generated variable list and attach them.
	// Any changes made by event handlers that could affect the list is avoided.
	for (auto var: ev_list)
	{
		// Compare them again to be sure after Attaching the others.
		if (var->_desiredId && var->_desiredId == _reference->_id)
		{
			// Attach the one having the correct desired ID.
			var->attachRef(_reference);
		}
	}
	// Enable deletion of instances.
	VariableStatic::_globalActive--;
	// Return  the lists size as the count of the events.
	return ev_list.size();
}

void Variable::setHandler(VariableHandler* handler)
{
	// only generate events if there is a change of link
	if (_handler != handler)
	{
		if (handler)
		{ // notify instance getting event link
			_handler = handler;
			emitEvent(veLinked, *this);
		}
		else
		{ // notify instance losing event link in the was already a link
			if (_handler)
			{
				emitEvent(veUnlinked, *this);
				_handler = nullptr;
			}
		}
	}
}

void Variable::removeHandler(VariableHandler* handler)
{
	for (auto ref: VariableStatic::_references)
	{
		// Iterate through the variables.
		for (auto v: ref->_list)
		{
			// Check if the 2 links are the same.
			if (v->_handler == handler)
			{
				// Set the link to NULL so the event function isn't called.
				v->_handler = nullptr;
			}
		}
	}
}

Variable::flags_type Variable::toFlags(const std::string& flags)
{
	flags_type rv{0};
	for (auto f: flags)
	{
		for (auto fl: VariableStatic::_flagLetters)
		{
			if (f == fl._letter)
			{
				rv |= fl._flag;
			}
		}
	}
	return rv;
}

std::string Variable::getFlagsString(Variable::flags_type flag)
{
	std::string rv;
	for (auto fl: VariableStatic::_flagLetters)
	{
		if (flag & fl._flag)
		{
			rv.append(1, fl._letter);
		}
	}
	return rv;
}

/**
 * Unsets a flag or multiple flags of the attached VariableReference.
 */
bool Variable::updateFlags(int flags, bool skip_self)
{
	if (isOwner())
	{
		auto cur_flags = _reference->_curFlags;
		_reference->_curFlags = flags;
		if (_reference->_curFlags != cur_flags)
		{ // skip self
			emitLocalEvent(veFlagsChange, skip_self);
			// Changes were made.
			return true;
		}
	}
	// Nothing changed.
	return false;
}

/**
 * Sets a flag or multiple flags on the reference
 */
bool Variable::setFlag(flags_type flag, bool skip_self)
{
	return updateFlags(_reference->_curFlags | flag, skip_self);
}

/**
 * Unsets a flag or multiple flags of the attached VariableReference.
 */
bool Variable::unsetFlag(flags_type flag, bool skip_self)
{
	return updateFlags(_reference->_curFlags & ~flag, skip_self);
}

const Value& Variable::getCur() const
{
	// When temporary is used return the temporary value.
	return _temporary ? (*_temporary) : (isConverted() ? _reference->_convertCurValue : _reference->_curValue);
}

const Value& Variable::getCur(bool converted) const
{
	// When temporary is used return the temporary value.
	return (converted && isConverted()) ? _reference->_convertCurValue : _reference->_curValue;
}

bool Variable::setCur(const Value& value, bool skip_self)
{
	// When the temporary value is used skip updating the real value.
	if (_temporary)
	{
		return updateTempValue(value, skip_self);
	}
	// If this instance uses the converted value it must be converted first to the real value.
	return updateValue(convert(value, true), skip_self);
}

bool Variable::isReadOnly() const
{
	// If it is the owner it is not readonly by default.
	// Only the owner can change a readonly instance.
	if (!isOwner())
	{
		// If the flag READONLY is Set the not owned variable is
		// read only by default.
		if (isFlag(flgReadonly))
		{
			return true;
		}
		// When the variable is a local exported one and this instance is
		// globally referencing it is also read only because only
		// local referencing variables may change the current value or flags.
		if (!const_cast<Variable*>(this)->getOwner()._global && _global)
		{
			if (isFlag(flgWriteable))
			{
				return false;
			}
			return true;
		}
	}
	return _reference == VariableStatic::zero()._reference;
}

bool Variable::loadCur(const Value& value) const
{
	// Only allowed by owners.
	if (isOwner())
	{
		// Check if this instance is not read-only and global because only globals are loadable.
		if (!isFlag(flgReadonly) && const_cast<Variable*>(this)->getOwner()._global)
		{  // Make a writable copy of the passed value.
			Value new_val(value);
			// Adjust the new_val type
			if (!new_val.setType(getType()))
			{
				return false;
			}
			// Clip when clipping is needed.
			if (isNumber() && _reference->_maxValue != _reference->_minValue)
			{ // Adjust new value to the allowed range.
				new_val = (new_val > _reference->_maxValue) ? _reference->_maxValue : new_val;
				new_val = (new_val < _reference->_minValue) ? _reference->_minValue : new_val;
			}
			// Signal when changed and do not skip the event for this instance.
			return const_cast<Variable*>(this)->updateValue(new_val, false);
		}
	}
	else
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Cannot load current value of non owning instance!")
	}
	// Signal failure or no change.
	return false;
}

bool Variable::updateValue(const Value& value, bool skip_self)
{
	// Check if this instance can change its value.
	if (isReadOnly())
	{
		return false;
	}
	// Flag that is evaluated at the end of the function for triggering an event.
	bool changed = false;
	// Make a difference between the several types.
	switch (getType())
	{
		case Value::vitInteger:
		case Value::vitFloat:
		{
			// If the new value is numerical adjust type to _curValue type
			// create non const Value type
			Value new_val = value;
			// Adjust the new_val type
			if (!new_val.setType(getType()))
			{
				// on failure return
				return false;
			}
			// Check if limits must be checked by comparing a difference and
			// checking the ownership.
			if (!isOwner() && _reference->_maxValue != _reference->_minValue)
			{
				// Adjust new value to the allowed range.
				new_val = (new_val > _reference->_maxValue) ? _reference->_maxValue : new_val;
				new_val = (new_val < _reference->_minValue) ? _reference->_minValue : new_val;
			}
			// Check if new value must be rounded
			if (!_reference->_rndValue.isZero())
			{
				new_val.round(_reference->_rndValue);
			}
			// Set local 'changed ' to trigger event.
			changed = _reference->_curValue != new_val;
			_reference->_curValue = new_val;
			break;
		}

		case Value::vitString:
		{
			// Create non const std::string that can be modified.
			std::string s;
			// Get the filter string from the type.
			switch (getStringType())
			{
				case stPath:
					s = filter(value.getString(), "/*?\"<>|,");
					// Strip also space from both ends.
					s = trim(s, " ");
					break;

				case stDirectory:
					s = filter(value.getString(), "/*?\"<>|,");
					// Strip also space from both ends.
					s = trim(s, " ");
					if (s.length() && s[s.length() - 1] != '\\')
					{
						s.append(1, '\\');
					}
					break;

				case stFilename:
					s = filter(value.getString(), "/\\:*?\"<>|,");
					// Strip also space from both ends.
					s = trim(s, " ");
					break;

				case stSubdirectory:
					s = filter(value.getString(), "/:*?\"<>|,");
					// Strip also space from both ends.
					s = trim(s, "\\");
					s = trim(s, " ");
					s = trim(s, "\\");
					s = trim(s, " ");
					// When not empty append the back slash.
					if (s.length())
					{
						s.append(1, '\\');
					}
					break;

				default:
					s = value.getString();
			}
			// Get the maximum allowed length of the round field.
			auto max_len = (size_t) _reference->_rndValue.getInteger();
			// adjust length if max_len is non-zero and length > std::string length
			if (max_len && (s.length() > max_len))
			{
				s = s.substr(0, max_len);
			}
			// create Value instance to compare and assign
			Value v(s);
			// Set local 'changed ' to trigger event
			changed = _reference->_curValue != v;
			// assign the value
			_reference->_curValue = v;
		}
			break;

		case Value::vitBinary:
		case Value::vitCustom:
		{
			// create non const Value type
			Value new_val = value;
			// adjust the new_val type
			if (!new_val.setType(getType()))
			{
				// on failure return
				return false;
			}
			// check for change of value
			if (_reference->_curValue != new_val)
			{
				_reference->_curValue = value;
				// Set changed local variable to trigger event
				changed = true;
			}
		}
			break;

		default:
			break;
	} // switch
	// Check if there was a change of the current value.
	if (changed)
	{
		// Update the Converted value if the unit is filled in.
		if (_reference->_convertUnit.length())
		{
			_reference->_convertCurValue = convert(_reference->_curValue, false);
		}
		// Notify all variables referencing this variable that the current value has changed.
		emitLocalEvent(veValueChange, skip_self);
	}
	return changed;
}

bool Variable::updateTempValue(const Value& value, bool skip_self)
{
	// Check if the instance can change its value.
	if (isReadOnly())
	{
		return false;
	}
	// Flag that is evaluated at the end of the function for triggering an event.
	bool changed = false;
	// Make a difference between the several types.
	switch (getType())
	{
		case Value::vitInteger:
		case Value::vitFloat:
		{
			// If the new value is numerical adjust type to _curValue type
			// create non const Value type
			Value new_val = value;
			// Adjust the new_val type
			if (!new_val.setType(getType()))
			{
				// on failure return
				return false;
			}
			// Clip and round the new value.
			clipRound(new_val);
			// Set local 'changed ' to trigger an event.
			changed = *_temporary != new_val;
			*_temporary = new_val;
			break;
		}

		case Value::vitString:
		{
			// create non const std::string to modify
			std::string s = value.getString();
			auto max_len = (size_t) _reference->_rndValue.getInteger();
			// adjust length if max_len is non-zero and length > std::string length
			if (max_len && (s.length() > max_len))
			{
				s = s.substr(0, max_len);
			}
			// Create Value instance to compare and assign.
			Value v(s);
			// Set local 'changed ' to trigger an event
			changed = *_temporary != v;
			// Assign the new value.
			*_temporary = v;
			break;
		}

		case Value::vitBinary:
		case Value::vitCustom:
		{ // Create non const Value type.
			Value new_val = value;
			// adjust the new_val type
			if (!new_val.setType(getType()))
			{
				// on failure return
				return false;
			}
			// check for change of value
			if (*_temporary != new_val)
			{
				*_temporary = value;
				// Set changed local variable to trigger event
				changed = true;
			}
			break;
		}

		default:
			break;
	}
	// Check if there was a change of the current value.
	if (changed)
	{
		// Notify only the current instance of the change of the value.
		if (!skip_self)
		{
			emitEvent(veValueChange, *this);
		}
	}
	return changed;
}

bool Variable::writeUpdate(std::ostream& os) const
{
	os << '(' << _reference->_id << ',' << _reference->_curValue << ',' << getFlagsString(_reference->_curFlags) << ')'
		<< '\n';
	// check if out stream is valid
	return !os.fail() && !os.bad();
}

// Function used for process communication.
bool Variable::readUpdate(std::istream& is, bool skip_self, Vector& list)
{
	Value value;
	id_type id = 0;
	std::string flags;
	char c;

	is >> c >> id >> c >> value >> c;
	getline(is, flags, ')');

	if (!is.fail() && !is.bad() && value.isValid())
	{ // GetReferenceById get reference to owner to be able to update readonly vars
		auto& var(not_ref_null(list) ? getInstanceById(id, list) : const_cast<Variable&>(getInstanceById(id)));
		var.updateValue(value, skip_self);
		var.updateFlags(toFlags(flags), skip_self);
		return true;
	}
	return false;
}

bool Variable::write(std::ostream& os) const
{
	os << '(' << _reference->_id << ',' << _reference->_curValue << ')';
	// check if out stream is valid
	return !os.fail() && !os.bad();
}

bool Variable::read(std::istream& is, bool skip_self, Vector& list)
{
	Value value;
	id_type id = 0;
	char c = 0;
	is >> c >> id >> c >> value >> c;
	SF_COND_NORM_NOTIFY(!value.isValid(), DO_DEFAULT, "Variable: Stream format Corrupt!")
	if (!is.fail() && !is.bad() && value.isValid())
	{
		// Check list for a NULL_REF and use
		auto& var(not_ref_null(list) ? getInstanceById(id, list) : const_cast<Variable&>(getInstanceById(id)));
		var.updateValue(value, skip_self);
		return true;
	}
	return false;
}

bool Variable::create(std::istream& is, Variable::Vector& list, bool global, int& err_line)
{
	bool ret_val = true;
	// Variable keeps track of the lines read.
	if (not_ref_null(err_line))
	{
		err_line = 1;
	}
	while (is.good() && ret_val)
	{
		std::string st;
		// Get err_line with max length of 1024
		getline(is, st);
		// Test ref pointer before increasing err_line count.
		if (not_ref_null(err_line))
		{
			err_line++;
		}
		// Check length and ignore lines starting with character ';'
		if (st.length() > 10 && st[0] != ';')
		{
			auto* var = new Variable();
			var->setGlobal(global);
			var->setup(st);
			if (var->isValid())
			{
				if (not_ref_null(list))
				{
					list.add(var);
				}
				else
				{
					delete var;
				}
			}
			else
			{
				delete var;
				ret_val = false;
			}
		}
	} // while
	return ret_val;
}

std::string Variable::getSetupString() const
{
	const char sep = ',';
	//  vfId,
	std::string rv = stringf("0x%llX", getId());
	rv += sep;
	//  vfName,
	rv += getName() + sep;
	//  vfUnit,
	rv += _reference->_unit + sep;
	//  vfFlags,
	rv += getFlagsString() + sep;
	//  vfDescription,
	rv += unescape(getDescription()) + sep;
	//  vfType,
	const char* tmp = getType(getType());
	rv += tmp;
	rv += sep;
	//  vfConversionType,
	rv += getConvertOption() + sep;
	//  vfRound,
	rv += _reference->_rndValue.getString() + sep;
	//  vfDefault,
	rv += _reference->_defValue.getString() + sep;
	//  vfMinimum,
	rv += _reference->_minValue.getString() + sep;
	//  vfMaximum,
	rv += _reference->_maxValue.getString();
	//  vfFirstState
	for (auto& i: _reference->_states)
	{
		rv += sep + i._name + '=' + i._value.getString();
	}
	return rv;
}

void Variable::clipRound(Value& value) const
{
	// Can only clip numeric values.
	if (isNumber())
	{
		// Check which values must be used.
		if (isConverted())
		{
			// Check if limits must be checked by comparing a difference.
			if (_reference->_convertMaxValue != _reference->_convertMinValue)
			{
				// Adjust new value to the allowed range.
				value = (value > _reference->_convertMaxValue) ? _reference->_convertMaxValue : value;
				value = (value < _reference->_convertMinValue) ? _reference->_convertMinValue : value;
			}
			// Check if new value must be rounded
			if (!_reference->_rndValue.isZero())
			{
				value.round(_reference->_rndValue);
			}
		}
		else
		{
			// Check if limits must be checked by comparing a difference.
			if (_reference->_maxValue != _reference->_minValue)
			{
				// Adjust new value to the allowed range.
				value = (value > _reference->_maxValue) ? _reference->_maxValue : value;
				value = (value < _reference->_minValue) ? _reference->_minValue : value;
			}
			// Check if new value must be rounded
			if (!_reference->_rndValue.isZero())
			{
				value.round(_reference->_convertRndValue);
			}
		}
	}
}

std::string Variable::getCurString(bool use_states) const
{
	switch (_reference->_type)
	{
		default:
		case Value::vitBinary:
		case Value::vitCustom:
		case Value::vitString:
		{
			if (_temporary)
			{
				return _temporary->getString();
			}
			else
			{
				return _reference->_curValue.getString();
			}
		}

		case Value::vitInteger:
		{
			// there are states return the state name if 'states' is true if not run in to default switch
			if (use_states && !_reference->_states.empty())
			{
				for (auto& state: _reference->_states)
				{
					if (state._value == (_temporary ? (*_temporary) : _reference->_curValue))
					{
						return state._name;
					}
				}
			}
			// run into next switch is state was not found
		}

		case Value::vitFloat:
		{
			// When a temporary is used then it is already converted or not.
			if (isConverted())
			{
				return (_temporary ? (*_temporary) : _reference->_convertCurValue).getString(_reference->_convertSigDigits);
			}
			else
			{
				return (_temporary ? (*_temporary) : _reference->_curValue).getString(_reference->_sigDigits);
			}
		}
	} // switch
}

void Variable::setConvertHandler(VariableHandler* handler)
{
	if (VariableStatic::_convertHandler != handler)
	{
		VariableStatic::_convertHandler = handler;
	}
}

bool Variable::setConvertValues
	(
		const std::string& unit,
		const Value& multiplier,
		const Value& offset,
		int digits
	)
{
	// Only owners are allowed to Set the conversion values.
	if (isOwner() && _reference->_type == Value::vitFloat)
	{
		// Check if something has changed.
		if (_reference->_convertMultiplier == multiplier && _reference->_convertOffset == offset &&
			_reference->_convertUnit == unit)
		{
			// Check if the significant digits are to be calculated here or passed here.
			if (!(digits != std::numeric_limits<int>::max() && _reference->_convertSigDigits != digits))
			{
				return true;
			}
		}
		// Correct the type of the passed values.
		_reference->_convertMultiplier = multiplier;
		_reference->_convertOffset = offset;
		// Correct the type of the passed values.
		_reference->_convertMultiplier.setType(_reference->_type);
		_reference->_convertOffset.setType(_reference->_type);
		// Set the static conversion values.
		_reference->_convertCurValue = _reference->_curValue * _reference->_convertMultiplier + _reference->_convertOffset;
		_reference->_convertDefValue = _reference->_defValue * _reference->_convertMultiplier + _reference->_convertOffset;
		_reference->_convertDefValue = _reference->_defValue * _reference->_convertMultiplier + _reference->_convertOffset;
		_reference->_convertMinValue = _reference->_minValue * _reference->_convertMultiplier + _reference->_convertOffset;
		_reference->_convertMaxValue = _reference->_maxValue * _reference->_convertMultiplier + _reference->_convertOffset;
		// Determine the conversion round value.
		_reference->_convertRndValue = _reference->_rndValue * _reference->_convertMultiplier;
		// Calculate significant digits here.
		if (digits == std::numeric_limits<int>::max())
		{
			// When the digits number is std::numeric_limits<int>::max() round was zero to start with.
			if (_reference->_sigDigits != std::numeric_limits<int>::max())
			{
				// Adjust the significant digits value with the magnitude of the multiplication factor.
				_reference->_convertSigDigits = _reference->_sigDigits + magnitude(_reference->_convertMultiplier.getFloat()) +
					1;
			}
			else
			{
				_reference->_convertSigDigits = std::numeric_limits<int>::max();
			}
		}
		else
		{
			_reference->_convertSigDigits = digits;
		}
		// If the unit length is non zero it always generates an event.
		// Only when both length values are zero there is no event generated.
		if (unit.length() || _reference->_convertUnit.length())
		{
			// Assigning the name will enable the conversion.
			_reference->_convertUnit = unit;
			// Signal the clients.
			emitLocalEvent(veConverted, false);
		}
		return true;
	}
	return false;
}

bool Variable::setConvertValues(bool convert)
{
	// Only owners are allowed to set the conversion values.
	// Check if the type is a floating point value that can be converted.
	if (isOwner() && _reference->_type == Value::vitFloat && _reference->_unit.length())
	{
		// Initialize.
		Value::flt_type multiplier = 1;
		Value::flt_type offset = 0;
		if (convert)
		{
			// If the global covert link has been set use it.
			if (VariableStatic::_convertHandler)
			{
				// Call the handler setting the linker to the zero variable to
				// indicate the global nature of the call.
				VariableStatic::_convertHandler->VariableEventHandler(veConvert, *this, VariableStatic::zero(), false);
			}
			else
			{
				int digits = _reference->_sigDigits;
				std::string new_unit = _reference->_unit;
				// Check if a conversion is found.
				if (getUnitConversion(_reference->_convertOption, _reference->_unit, _reference->_sigDigits,
					multiplier, offset, new_unit, digits))
				{
					// Set the convert values.
					return setConvertValues(new_unit, Value(multiplier), Value(offset), digits);
				}
			}
		}
		else
		{
			// Check if a conversion value must be disabled by checking the length of the conversion unit string as a sentry.
			if (_reference->_convertUnit.length())
			{
				// Set the convert values to the original.
				return setConvertValues("", Value(multiplier), Value(offset));
			}
			// Was al ready
			return true;
		}
	}
	// Signal failure.
	return false;
}

bool Variable::setConvert(bool enable)
{
	// Only when there is a change.
	if (_converted != enable)
	{
		// Make the change.
		_converted = enable;
		// Signal only if a conversion exists.
		if (_reference->_convertUnit.length())
		{
			// Signal this event to this single instance.
			emitEvent(veConverted, *this);
		}
	}
	return isConverted();
}

Value Variable::convert(const Value& value, bool to_org) const
{
	Value ret_val = value;
	// Check if a conversion must be performed.
	if (_reference->_convertUnit.length() && _reference->_type == Value::vitFloat)
	{
		// Abort the operation when conversion fails.
		if (!ret_val.setType(Value::vitFloat))
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Convert" << ret_val << " for passed value failed!")
			return value;
		}
		if (to_org)
		{
			ret_val -= _reference->_convertOffset;
			ret_val /= _reference->_convertMultiplier;
		}
		else
		{
			ret_val *= _reference->_convertMultiplier;
			ret_val += _reference->_convertOffset;
		}
	}
	return ret_val;
}

void Variable::setTemporary(bool on_off)
{
	// Check if there is a change.
	if (on_off != (_temporary != nullptr))
	{
		if (on_off)
		{
			// Create the temporary value and Set the on_off pointer as wel.
			_temporary = new Value();
			// Update the temporary value and signal this instance.
			updateTemporary(false);
		}
		else
		{
			// Only signal when there is a change between the temporary and the real value.
			if (*_temporary != (isConverted() ? _reference->_convertCurValue : _reference->_curValue))
			{
				// Notify the variable of the change.
				emitEvent(veValueChange, *this);
			}
			// Delete and disable the temporary value usage.
			if (_temporary)
			{
				delete _temporary;
				_temporary = nullptr;
			}
		}
	}
}

bool Variable::applyTemporary(bool skip_self)
{
	// Check if the temporary value is used.
	if (_temporary)
	{
		// Update using the straight value when not converted.
		return updateValue(convert(*_temporary, true), skip_self);
	}
	return false;
}

bool Variable::isTemporaryDifferent() const
{
	// Check if the temporary value is used.
	if (_temporary)
	{
		// Only when there is a change.
		Value* cur_val = isConverted() ? &_reference->_convertCurValue : &_reference->_curValue;
		bool rv = *_temporary != *cur_val;
		return rv;
	}
	return false;
}

bool Variable::updateTemporary(bool skip_self)
{
	// Check if the temporary value is used.
	if (_temporary)
	{
		// Only when there is a change.
		if (*_temporary != (isConverted() ? _reference->_convertCurValue : _reference->_curValue))
		{
			// Update the temporary value.
			*_temporary = (isConverted() ? _reference->_convertCurValue : _reference->_curValue);
			// Notify the variable when skip_self is false.
			if (!skip_self)
			{
				emitEvent(veValueChange, *this);
			}
			// Variable temporary value was changed.
			return true;
		}
	}
	// Variable temporary value was not changed.
	return false;
}

std::ostream& operator<<(std::ostream& os, const Variable& v)
{
	return os << v.getSetupString() << '\n';
}

std::istream& operator>>(std::istream& is, Variable& v)
{
	std::string st;
	// get one line
	getline(is, st);
	// check stream on errors
	auto bad = !is.fail() && !is.bad();
	// check stream state length and ignore lines starting with character ';'
	if (!bad && st.length() > 10 && st[0] != ';')
	{ // setup this variable with the read line from the input stream
		v.setup(st);
	}
	return is;
}

std::ostream& operator<<(std::ostream& os, const Variable::State::Vector& v)
{
	for (auto& i: v)
	{
		os << "(\"" << escape(i._value.getString(), '"') << "\"=\"" << escape(i._name, '"') << "\"), ";
	}
	return os;
}

/**
 * Implementation of undocumented debugging functions
 */
const char* Variable::getEventName(EEvent event)
{
	const char* rv = nullptr;
	switch (event)
	{
		case veNewId:
			rv = "NewId";
			break;

		case veConverted:
			rv = "Converted";
			break;

		case veFlagsChange:
			rv = "FlagsChange";
			break;

		case veIdChanged:
			rv = "IdChanged";
			break;

		case veDesiredId:
			rv = "DesiredId";
			break;

		case veRemove:
			rv = "Remove";
			break;

		case veGetOwner:
			rv = "getOwner";
			break;

		case veLostOwner:
			rv = "LostOwner";
			break;

		case veLinked:
			rv = "Linked";
			break;

		case veUnlinked:
			rv = "Unlinked";
			break;

		case veSetup:
			rv = "Setup";
			break;

		case veValueChange:
			rv = "ValueChange";
			break;

		case veInvalid:
			rv = "Invalid";
			break;

		default:
			// Check for global event.
			if (event < veFirstLocal)
			{
				rv = "UserGlobal";
				break;
			}
			else
			{
				if (event >= veFirstLocal && event < veFirstPrivate)
				{
					rv = "UserLocal";
				}
				else
				{
					if (event >= veUserPrivate)
					{
						rv = "UserPrivate";
					}
				}
			}
	}
	// When not Set the unknown value.
	if (!rv)
	{
		rv = "?Unknown";
	}
	return rv;
}

Variable::EStringType Variable::getStringType() const
{
	// Check for string type kind of variable
	if (_reference->_type == Value::vitString)
	{
		size_t pos = _reference->_unit.find_first_of("SNMPDF");
		if (pos != std::string::npos)
		{
			switch (_reference->_unit[pos])
			{
				case 'N':
					return stNormal;
				case 'M':
					return stMulti;
				case 'P':
					return stPath;
				case 'D':
					return stDirectory;
				case 'S':
					return stSubdirectory;
				case 'F':
					return stFilename;
			}
		}
	}
	// By default return the normal string type.
	return stNormal;
}

const char* Variable::getStringType(Variable::EStringType type)
{
	switch (type)
	{
		case stNormal:
			return "Normal";
		case stMulti:
			return "Multi";
		case stPath:
			return "Path";
		case stDirectory:
			return "Dir";
		case stSubdirectory:
			return "Subdir";
		case stFilename:
			return "File";
		default:
			break;
	}
	return "Unknown";
}

std::string Variable::getConvertOption() const
{
	// Only the float type can have a convert option.
	if (_reference->_type == Value::vitFloat)
	{
		return _reference->_convertOption;
	}
	return "";
}

std::string Variable::getUnit() const
{
	// Check for string type kind of variable
	if (_reference->_type == Value::vitString)
	{
		return getStringType(getStringType());
	}
	else
	{
		return isConverted() ? _reference->_convertUnit : _reference->_unit;
	}
}

std::string Variable::getUnit(bool converted) const
{
	// Check for string type kind of variable
	if (_reference->_type == Value::vitString)
	{
		return getStringType(getStringType());
	}
	else
	{
		return (converted && _reference->_convertUnit.length()) ? _reference->_convertUnit : _reference->_unit;
	}
}

VariableHandler* Variable::getConvertHandler() const
{
	return VariableStatic::_convertHandler;
}

void Variable::initialize()
{
	VariableStatic::initialize(true);
}

void Variable::deinitialize()
{
	VariableStatic::initialize(false);
}

void Variable::setDesiredId()
{
	setDesiredId(_reference->_id);
}

Variable& Variable::getOwner()
{
	return *_reference->_list[0];
}

bool Variable::isOwner() const
{
	return this == _reference->_list[0];
}

bool Variable::isValid() const
{
	return _reference && _reference->_valid;
}

bool Variable::isNumber() const
{
	return _reference->_type == Value::vitFloat || _reference->_type == Value::vitInteger;
}

bool Variable::isConverted() const
{
	return
		_converted &&
			_reference->_type == Value::vitFloat &&
			_reference->_convertUnit.length();
}

bool Variable::isTemporary()
{
	return _temporary != nullptr;
}

bool Variable::isFlag(int flag) const
{
	return (_reference->_curFlags & flag) == flag;
}

Variable::id_type Variable::getId() const
{
	return _reference->_id;
}

std::string Variable::getDescription() const
{
	return _reference->_description;
}

std::string Variable::getCurFlagsString() const
{
	return getFlagsString(_reference->_curFlags);
}

std::string Variable::getFlagsString() const
{
	return Variable::getFlagsString(_reference->_flags);
}

Variable::flags_type Variable::getFlags() const
{
	return _reference->_flags;
}

Variable::flags_type Variable::getCurFlags() const
{
	return _reference->_curFlags;
}

const Value& Variable::getDef() const
{
	return isConverted() ? _reference->_convertDefValue : _reference->_defValue;
}

const Value& Variable::getMin() const
{
	return isConverted() ? _reference->_convertMinValue : _reference->_minValue;
}

const Value& Variable::getMax() const
{
	return isConverted() ? _reference->_convertMaxValue : _reference->_maxValue;
}

const Value& Variable::getRnd() const
{
	return isConverted() ? _reference->_convertRndValue : _reference->_rndValue;
}

const Value& Variable::getDef(bool converted) const
{
	return (converted && _reference->_convertUnit.length()) ? _reference->_convertDefValue : _reference->_defValue;
}

int Variable::getSigDigits() const
{
	return isConverted() ? _reference->_convertSigDigits : _reference->_sigDigits;
}

const Value& Variable::getMin(bool converted) const
{
	return (converted && _reference->_convertUnit.length()) ? _reference->_convertMinValue : _reference->_minValue;
}

const Value& Variable::getMax(bool converted) const
{
	return (converted && _reference->_convertUnit.length()) ? _reference->_convertMaxValue : _reference->_maxValue;
}

const Value& Variable::getRnd(bool converted) const
{
	return (converted && _reference->_convertUnit.length()) ? _reference->_convertRndValue : _reference->_rndValue;
}

int Variable::getSigDigits(bool converted) const
{
	return (converted && _reference->_convertUnit.length()) ? _reference->_convertSigDigits : _reference->_sigDigits;
}

Value::EType Variable::getType() const
{
	return _reference->_type;
}

const Variable& Variable::getInstanceById(Variable::id_type id)
{
	return *(getReferenceById(id)->_list[0]);
}

Variable::size_type Variable::getUsageCount() const
{
	return _reference->_list.size();
}

Variable::size_type Variable::getStateCount() const
{
	return _reference->_states.size();
}

const Variable::State::Vector& Variable::getStates() const
{
	return _reference->_states;
}

Variable::Definition Variable::getDefinition(const std::string& str)
{
	Definition def;
	// Flag to determine if the conversion went well.
	bool ok = true;
	// Pointer that points to the place where the conversion of the ID went wrong.
	char* end_ptr = nullptr;
	// Get the result ID from the setup string.
	std::string tmp = GetCsfField(vfId, str);
	id_type id = std::strtoull(tmp.c_str(), &end_ptr, 0);
	// Return zero if an error occurred during conversion of the ID.
	if (end_ptr && *end_ptr != '\0')
	{
		ok = false;
	}
	// Read all values in as strings to convert them later to the actual form.
	def._id = id;
	def._name = GetCsfField(vfName, str);
	def._unit = GetCsfField(vfUnit, str);
	def._convertOption = GetCsfField(vfConversionType, str);
	def._description = unescape(GetCsfField(vfDescription, str));
	def._flags = toFlags(GetCsfField(vfFlags, str));
	// Check for multi line string so the default value
	Value::EType type = (Value::EType) Value::getType(GetCsfField(vfType, str).c_str());
	//
	if (type == Value::vitString && def._unit.find('M') != std::string::npos)
	{
		def._defaultValue.set(unescape(GetCsfField(vfDefault, str)));
	}
	else
	{
		def._defaultValue.set(GetCsfField(vfDefault, str));
	}
	def._minValue.set(GetCsfField(vfMinimum, str).c_str());
	def._maxValue.set(GetCsfField(vfMaximum, str).c_str());
	def._roundValue.set(GetCsfField(vfRound, str).c_str());
	// Get max state field count
	int state_count = 0;
	while (GetCsfField(vfFirstState + state_count, str).length())
	{
		state_count++;
	}
	def._states.resize(state_count);
	for (int i = 0; i < state_count; i++)
	{
		def._states[i]._name = GetCsfField(vfFirstState + i, str);
		if (def._states[i]._name.length())
		{
			size_t pos = def._states[i]._name.find_first_of('=');
			// If equal sign has been found add state value
			if (pos != std::string::npos)
			{
				def._states[i]._value.set(def._states[i]._name.substr(pos + 1).c_str());
				// Truncate name to position of the equal sign
				def._states[i]._name.resize(pos);
			}
		}
		else
		{
			break;
		}
	}
	// Skip type conversion if an error occurred so far.
	if (ok)
	{
		// Convert all Value reference data members to the wanted type
		// and check for errors during the conversion.
		ok &= def._maxValue.setType(type);
		ok &= def._minValue.setType(type);
		ok &= def._roundValue.setType(type);
		for (auto& s: def._states)
		{
			ok &= s._value.setType(type);
		}
		def._type = (Value::EType) type;
	}
	// Set the validation boolean flag.
	def._valid = ok;
	//
	return def;
}

}
