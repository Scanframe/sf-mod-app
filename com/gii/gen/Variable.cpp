#include <cmath>
#include <cstdio>

#include <misc/gen/dbgutils.h>
#include <misc/gen/genutils.h>
#include <misc/gen/Value.h>
#include <misc/gen/csf.h>

#include "UnitConversion.h"
#include "Variable.h"
// Not part of the public interface.
#include "VariableStatic.h"

namespace sf
{

VariableReference::VariableReference(bool global)
{
	_global = global;
	_type = Value::vitInvalid;
	_description = Value::_invalidStr;
	_name = Value::_invalidStr;
	_unit = Value::_invalidStr;
	// Add to global reference list declared in VariableStatic
	if (_global)
	{
		VariableStatic::_referenceList.Add(this);
	}
}

void VariableReference::copy(const VariableReference& ref)
{
//  bool _global;
//  Vector _list;
	_COND_RTTI_NOTIFY(this == VariableStatic::zero()._reference,
		DO_MSGBOX | DO_DBGBRK, "Cannot assign a ref to Zero Ref!")
	_valid = ref._valid;
	_id = ref._id;
	_flags = ref._flags;
	_curFlags = ref._curFlags;
	_type = ref._type;
	_name = ref._name;
	_description = ref._description;
	_unit = ref._unit;
	_curValue = ref._curValue;
	_defValue = ref._defValue;
	_maxValue = ref._maxValue;
	_minValue = ref._minValue;
	_rndValue = ref._rndValue;
	_sigDigits = ref._sigDigits;
	_states = ref._states;
	_localActive = ref._localActive;
	_convertUnit = ref._convertUnit;
	_convertCurValue = ref._convertCurValue;
	_convertDefValue = ref._convertDefValue;
	_convertMaxValue = ref._convertMaxValue;
	_convertMinValue = ref._convertMinValue;
	_convertRndValue = ref._convertRndValue;
	_convertSigDigits = ref._convertSigDigits;
	_convertMultiplier = ref._convertMultiplier;
	_convertOffset = ref._convertOffset;
}

VariableReference::~VariableReference()
{
	// If TVariables are still referenced these must be removed first
	// except if it is ZeroVariable itself.
	if (VariableStatic::zero()._reference == this)
	{
		// Check if there are still variables in the system
		// when zero variable is destructed
//    if (Variable::GetInstanceCount())
		unsigned total_count = 0;
		VariableStatic::ReferenceVector::iter_type i(VariableStatic::_referenceList);
		// Iterate through all variable references and count the usage count
		while (i)
		{
			VariableReference* ref = i++;
			total_count += ref->_list.Count();
			// Notification of warning
			// Check if current ref is the ZeroVariable reference.
			if (ref == VariableStatic::zero()._reference)
			{
				// Iterate through each variable entry in the list.
				size_type count = ref->_list.Count();
				// Skip the first one because there is always ZeroVariable itself.
				for (size_type idx = 1; i < count; i++)
				{
					// Notification of warning
					_NORM_NOTIFY
					(
						DO_MSGBOX | DO_DEFAULT,
						_RTTI_TYPENAME
							<< ": Dangling instance [" << idx << "/" << count
							<< "] with desired ID "
							<< stringf("0x%lX", ref->_list[idx]->_desiredId)
							<< " in this process!"
					)
					// Limit the amount shown to a maximum of 3.
					if (idx >= 3)
					{
						_NORM_NOTIFY(DO_MSGBOX | DO_DEFAULT,
							_RTTI_TYPENAME << ": Too many [" << count << "] dangling instances to be shown!")
						break;
					}
				}
			}
			else
			{
				_NORM_NOTIFY
				(
					DO_MSGBOX | DO_DEFAULT,
					_RTTI_TYPENAME
						<< ": Dangling reference owning instance with ID "
						<< stringf("0x%lX", ref->_id)
						<< " in this process!"
				)
			}
		}
		// Subtract 1 for zero variable itself.
		// There should only be one left in the list
		total_count--;
		// Notification of warning
		_COND_NORM_NOTIFY
		(
			total_count,
			DO_MSGBOX | DO_DEFAULT,
			_RTTI_TYPENAME
				<< ": Total of " << total_count
				<< " dangling Variable instances in this process!"
		)
	}
	else
	{
		size_type count = _list.Count();
		while (count--)
		{
			// Attach variable to Zero which is the default and the invalid one
			if (_list[count])
			{
				_list[count]->setup(0L, false);
			}
		}
	}
	// Remove from global reference list declared in VariableStatic
	VariableStatic::_referenceList.Detach(this);
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

Variable::Variable()
{
	_COND_RTTI_THROW(!VariableStatic::zero()._reference,
		"ZeroVariable has NOT been created yet! Library initialisation error!")
	_global = true;
	attachRef(VariableStatic::zero()._reference);
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
	if
		(
		!isOwner()            // Only an owner can be exported.
			|| !isFlag(flgEXPORT) // And it must be exportable.
			|| !_reference              // The reference pointer cannot be NULL.
			|| _reference == VariableStatic::zero()._reference  // The attached reference may not be the Zero one.
			|| _global            // To make an owner appear global it must be local.
			|| (global && !_reference->_id)// The local variable ID cannot be zero when global.
		)
	{
		// Signal failure to make it global.
		return false;
	}
	// Only continue when there is a real change at hand.
	if (_reference->_exported == global)
	{
		return true;
	}
	// Assign the new value.
	_reference->_exported = global;
	// Check if the ID already exist.
	// GetReferenceById returns Zero if not found.
	VariableReference* ref = getReferenceById(_reference->_id);
	// See if the variable must be made global.
	if (global)
	{
		// Check if the current reference is already global.
		if (ref == _reference)
		{
			// If so signal success.
			return true;
		}
		// When the id was found.
		if (ref != VariableStatic::zero()._reference)
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Tried to export ID:!\n"
				<< stringf("0x%lX,", _reference->_id) << _reference->_name << "\nover!\n"
				<< ref->_name << '!')
			// If the ID already exist return false.
			return false;
		}
		// Add this variable to the global static list.
		VariableStatic::_referenceList.Add(_reference);
		// Notify all global linked users of Variable instances of new ID.
		emitGlobalEvent(veNewId, true);
		// Iterate through global variable instances and attach those having
		// the desired ID just created.
		attachDesired();
	}
		// When variable must be made local again.
	else
	{
		// Remove the reference from the global static list.
		if (!VariableStatic::_referenceList.Detach(_reference))
		{
			return true;
		}
		// Create a temporary vector pointers to variables that must be
		// detached because the reference list is modified when a variable is detached.
		Variable::Vector list;
		// Iterate through all variables having the global flag Set.
		for (auto i: _reference->_list)
		{
			if (i->_global)
			{
				list.Add(i);
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
	if (_global != global)
	{
		// Only when variable is not attached to other then Zero.
		if (_reference == VariableStatic::zero()._reference)
		{
			_global = global;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void Variable::operator delete(void* p) // NOLINT(misc-new-delete-overloads)
{
	if (VariableStatic::_globalActive)
	{
		_NORM_NOTIFY(DO_DEFAULT, "Warning: Deleting Instance During An Event!")
		// Do not free allocated data before end of global event remove
		VariableStatic::_deleteWaitCache.Add(p);
	}
	else
	{
		// Call real destructor.
		::operator delete(p);
		// If deletion is allowed again delete the wait cache too.
		TVector<void*>::size_type count = VariableStatic::_deleteWaitCache.Count();
		if (count)
		{
			while (count--)
			{
				delete (char*) VariableStatic::_deleteWaitCache[count];
			}
			// Flush the entries from the list.
			VariableStatic::_deleteWaitCache.Flush();
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
	// if the temprary value is still in use do not forget to delete it.
	delete_null(_temporary);
}

Variable::size_type Variable::getVariableCount()
{
	// Return the actual count -1 because of Zero.
	return VariableStatic::_referenceList.Count() - 1;
}

Variable::size_type Variable::getInstanceCount()
{
	size_type rv = 0;
	// Iterate through all variable references and count the usage count.
	for (auto i: VariableStatic::_referenceList)
	{
		rv += i->_list.Count();
	}
	// Subtract 1 for zero variable before returning.
	return --rv;
}

const Variable* Variable::getVariableListItem(Variable::size_type p)
{
	// Returns the first Variable in the list because this
	// there is always one in there otherwise no Reference would exist
	// increase p by 1 to skip Zero which is the first in the list
	p++;
	VariableReference* ref = (p < VariableStatic::_referenceList.Count()) ? VariableStatic::_referenceList[p] : nullptr;
	return (ref) ? ref->_list[0] : &VariableStatic::zero();
}

VariableReference* Variable::getReferenceById(Variable::id_type id)
{
	if (id)
	{
		for (auto i: VariableStatic::_referenceList)
		{
			if (i->_id == id)
			{
				return i;
			}
		}
	}
	return VariableStatic::zero()._reference;
}

Variable& Variable::getVariableById(Variable::id_type id, Variable::Vector& list)
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
	// Do not change ownership if current instance is allready owner
	if (prev_owner == this)
	{
		return;
	}
	// Put this pointer at the beginning of the list which makes it the owner
	_reference->_list.Detach(this);
	_reference->_list.AddAt(this, 0);
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
	{ // When the reference pointer valid continue.
		if (_reference)
		{
			// Special handling when zero ref is the current reference.
			if (_reference == VariableStatic::zero()._reference)
			{
				// Remove this variable from the Zero reference variable list.
				_reference->_list.Detach(this);
				// Create a new local variable reference.
				_reference = new VariableReference(false);
				// Attach this variable to the new reference.
				_reference->_list.Add(this);
			}
			// Copy the reference fields from the global reference to the current reference.
			_reference->copy(*ref);
			// Notify the user of this instance by telling it was attached.
			emitEvent(veIdChanged, *this);
			// Attachment succeeded.
			return true;
		}
	}
	//
	bool rv = true;
	if (_reference)
	{
		// If this was the last one or the owner the referring reference will be deleted
		// _list[0] is the owner default the one who created the reference instance
		if (_reference->_list.Count() == 0 || this == _reference->_list[0])
		{
			// Notify all variables that this reference is to become invalid.
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
			if (!_reference->_list.Detach(this))
			{
				_RTTI_NOTIFY(DO_MSGBOX | DO_CERR, "Could not remove instance from list!")
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
		_reference->_list.Add(this);
		// Notify the user of this instance by telling it was attached.
		emitEvent(veIdChanged, *this);
	}
	return rv;
}

bool Variable::setId(Variable::id_type id, bool skip_self)
{
	// Only owner of local variable is allowed to do this.
	if (isOwner() && !_global)
	{ // Check if the local instance is currently exported.
		if (!_reference->_exported)
		{ // Check if the new ID is different and non zero.
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

bool Variable::setup(const Definition* definition)
{
	// Check for not owning and global variable.
	bool local_owner = !_global && isOwner() && getUsageCount() > 1;
	// If this is a not local owner attach it to the zero instance.
	if (!local_owner)
	{
		attachRef(VariableStatic::zero()._reference);
	}
	// Reset the desired ID data member.
	_desiredId = 0L;
	// The default return value is true.
	bool ret_val = true;
	// Check if variables other then the zero variable have an ID of zero.
	if (_reference != VariableStatic::zero()._reference && !local_owner && definition->_id == 0L)
	{
		ret_val = false;
	}
	// Don' bother to go on if an error occurred so far.
	if (ret_val)
	{ // Check if the ID already exist.
		// GetReferenceById returns Zero if not found or if
		// this instance is non global.
		VariableReference* ref = _global ? getReferenceById(definition->_id) : VariableStatic::zero()._reference;
		if (ref && ref != VariableStatic::zero()._reference)
		{
			_RTTI_NOTIFY(DO_DEFAULT | DO_MSGBOX, "Tried to create duplicate ID: !\n"
				<< definition->_name << " (0x" << std::hex << definition->_id << ")\nover!\n" << ref->_name << '!')
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
		ref->_id = definition->_id;
		ref->_name = definition->_name;
		ref->_unit = definition->_unit;
		ref->_convertOption = definition->_convertOption;
		ref->_description = definition->_description;
		ref->_flags = definition->_flags;
		ref->_type = definition->_type;
		// Use the original flags for the current ones.
		ref->_curFlags = ref->_flags;
		// Check for multi line string so the default value
		if (definition->_type == Value::vitString && ref->_unit.find('M') != std::string::npos)
		{
			ref->_defValue.set(unescape(definition->_default.getString()));
		}
		else
		{
			ref->_defValue = definition->_default;
		}
		// Do not Set the current value when it is a local owner.
		if (!local_owner)
		{
			ref->_curValue.set(ref->_defValue);
		}
		ref->_rndValue = definition->_round;
		ref->_minValue = definition->_min;
		ref->_maxValue = definition->_min;
		for (auto& state: definition->_states)
		{
			ref->_states.Add(state);
		}
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
			for (uint i = 0; i < ref->_states.Count(); i++)
			{
				ret_val &= ref->_states[i]._value.setType(ref->_type);
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
		{ // Notify all global linked users of Variable instances of new ID.
			emitGlobalEvent(veNewId, false);
			// Iterate through global variable instances and attach those having
			// the desired ID just created.
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
			// Notify the user of this instance of the redefined reference
			// using an ID change event.
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
	_COND_RTTI_NOTIFY(!ret_val, DO_DEFAULT | DO_MSGBOX, "Error Setup Definition: "
		<< definition->_name << " (0x" << std::hex << definition->_id << ")!")
	//
	return ret_val;
}

bool Variable::setup(const std::string& definition, Variable::id_type id_ofs)
{
	// Check for not owning and global variable.
	bool local_owner = !_global && isOwner() && getUsageCount() > 1;
	// If this is a not local owner attach it to the zero instance.
	if (!local_owner)
	{
		attachRef(VariableStatic::zero()._reference);
	}
	// Reset the desired ID data member.
	_desiredId = 0L;
	// Pointer that points to the place where the conversion of the ID went wrong.
	char* end_ptr = nullptr;
	// Get the result ID from the setup string.
	std::string tmp = GetCsfField(vfId, definition);
	id_type id = std::strtoull(tmp.c_str(), &end_ptr, 0);
	if (id)
	{
		id += id_ofs;
	}
	// The default return value is true.
	bool ret_val = true;
	// Return zero if an error occurred during conversion of the ID.
	if (end_ptr && *end_ptr != '\0')
	{
		ret_val = false;
	}
	// Check if variables other then the zero variable have an ID of zero.
	if (_reference != VariableStatic::zero()._reference && !local_owner && id == 0L)
	{
		ret_val = false;
	}
	// Don't bother to go on if an error occurred so far
	if (ret_val)
	{
		// Check if the ID already exist.
		// GetReferenceById returns Zero if not found or if
		// this instance is non global.
		VariableReference* ref = _global ? getReferenceById(id) : VariableStatic::zero()._reference;
		if (ref && ref != VariableStatic::zero()._reference)
		{
			_RTTI_NOTIFY(DO_DEFAULT | DO_MSGBOX, "Tried to create duplicate ID: !\n"
				<< definition << "\nover!\n" << ref->_name << '!')
			// If ref already exist return false.
			return false;
		}
		else
		{ // Create new or use old one.
			if (local_owner)
			{
				// Reuse the current reference
				ref = _reference;
				// Invalidate the conversion values by clearing the conversion unit.
				_reference->_convertUnit.resize(0);
			}
			else
			{
				// Create new global or non global reference for this variable instance.
				ref = new VariableReference(_global);
			}
		}
		// Initialize reference members from here
		// copy definition std::string in to buffer
		const std::string defin = unique(definition);
		ref->_valid = true;
		// read all values in as strings to convert
		// them later to the actual form
		ref->_id = id;
		ref->_name = GetCsfField(vfName, defin);
		ref->_unit = GetCsfField(vfUnit, defin);
		ref->_convertOption = GetCsfField(vfConversionType, defin);
		ref->_description = escape(GetCsfField(vfDescription, defin));
		ref->_flags = toFlags(GetCsfField(vfFlags, defin).c_str());
		ref->_curFlags = ref->_flags;// copy of original flags
		// Check for multi line string so the default value
		Value::EType type = (Value::EType) Value::getType(GetCsfField(vfType, defin).c_str());
		if (type == Value::vitString && ref->_unit.find('M') != std::string::npos)
		{
			ref->_defValue.set(unescape(GetCsfField(vfDefault, defin)));
		}
		else
		{
			ref->_defValue.set(GetCsfField(vfDefault, defin));
		}
		// Do not Set the current value when it is a local owner..
		if (!local_owner)
		{
			ref->_curValue.set(ref->_defValue);
		}
		ref->_minValue.set(GetCsfField(vfMinimum, defin).c_str());
		ref->_maxValue.set(GetCsfField(vfMaximum, defin).c_str());
		ref->_rndValue.set(GetCsfField(vfRound, defin).c_str());
		// Get max state field count
		int state_count = 0;
		while (GetCsfField(vfFirstState + state_count, defin).length())
		{
			state_count++;
		}
		ref->_states.Resize(state_count);
		for (int i = 0; i < state_count; i++)
		{
			ref->_states[i]._name = GetCsfField(vfFirstState + i, defin);
			if (ref->_states[i]._name.length())
			{
				size_t pos = ref->_states[i]._name.find_first_of('=');
				// If equal sign has been found add state value
				if (pos != std::string::npos)
				{
					ref->_states[i]._value.set(ref->_states[i]._name.substr(pos + 1).c_str());
					// Truncate name to position of the equal sign
					ref->_states[i]._name.resize(pos);
				}
			}
			else
			{
				break;
			}
		}
		// Get significant digits for the float type variable.
		if (Value::vitFloat == type)
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
			ret_val &= ref->_curValue.setType(type);
			ret_val &= ref->_defValue.setType(type);
			ret_val &= ref->_maxValue.setType(type);
			ret_val &= ref->_minValue.setType(type);
			ret_val &= ref->_rndValue.setType(type);
			for (uint i = 0; i < ref->_states.Count(); i++)
			{
				ret_val &= ref->_states[i]._value.setType(type);
			}
			ref->_type = (Value::EType) type;
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
		{ // Notify all global linked users of Variable instances of new ID.
			emitGlobalEvent(veNewId, false);
			// Iterate through global variable instances and attach those having
			// the desired ID just created.
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
			// Notify the user of this instance of the redefined reference
			// using an ID change event.
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
	_COND_RTTI_NOTIFY(!ret_val, DO_DEFAULT | DO_MSGBOX, "Error In Setup String: " << definition << '!')
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
	if (state < _reference->_states.Count())
	{
		return _reference->_states[state]._value;
	}
	// if out of range return State from ZeroRef
	return VariableStatic::zero()._reference->_states[0]._value;
}

std::string Variable::getStateName(Variable::size_type state) const
{ // check index versus range
	if (state < _reference->_states.Count())
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
	// If the caller is an other instance teh this one when the value changes.
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
	Vector ev_list;
	// Iterate through list and generate the variables from which
	// the event handler needs to be called.
	for (auto i: _reference->_list)
	{
		// Check for skip_self
		if (i && (!skip_self || i != this))
		{
			ev_list.Add(i);
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
	return ev_list.Count();
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
	for (auto ref: VariableStatic::_referenceList)
	{
		// Iterate through all variables of the reference.
		for (auto var: ref->_list)
		{
			// Check for skip_self and if the variable is global.
			// Only global variables get a global event.
			if (var && (!skip_self || var != this) && var->_global)
			{
				// Add variable to list.
				ev_list.Add(var);
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
	// Signal other event handler functions are called using a
	// global predefined list.
	Vector ev_list;
	// Iterate through all references generating pointers to instances
	// having a desired ID that must be attached.
	for (auto ref: VariableStatic::_referenceList)
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
				ev_list.Add(var);
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
	return ev_list.Count();
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
	// Get the total count of variable references.
	unsigned vc = VariableStatic::VariableStatic::_referenceList.Count();
	// iterate through variable references
	for (unsigned i = 0; i < vc; i++)
	{
		// Get the current variable pointer.
		VariableReference* ref = VariableStatic::_referenceList[i];
		// get the total amount of variables attached to this reference.
		unsigned vrc = ref->_list.Count();
		// iterate through the variables.
		for (unsigned j = 0; j < vrc; j++)
		{
			// Check if the 2 links are the same.
			if (ref->_list[j]->_handler == handler)
			{
				// Set the link to NULL so the event function isn't called.
				ref->_list[j]->_handler = nullptr;
			}
		}
	}
}

int Variable::toFlags(const char* flags)
{
	if (!flags) {return 0;}
	struct
	{
		char ch;
		int flag;
	}
		a[] =
		{
			{'R', flgREADONLY},
			{'A', flgARCHIVE},
			{'S', flgSHARE},
			{'L', flgLINK},
			{'F', flgFUNCTION},
			{'P', flgPARAMETER},
			{'H', flgHIDDEN},
			{'E', flgEXPORT},
			{'W', flgWRITEABLE},
			{0, 0}
		};

	int retval = 0;
	int i = 0;
	while (flags[i])
	{
		int t = 0;
		while (a[t].ch)
		{
			if (a[t].ch == flags[i])
			{
				retval |= 1 << t;
			}
			t++;
		}
		i++;
	}
	return retval;
}

std::string Variable::getFlagsString(Variable::flags_type flg)
{
	struct
	{
		char ch;
		int flag;
	}
		a[] =
		{
			{'R', flgREADONLY},
			{'A', flgARCHIVE},
			{'S', flgSHARE},
			{'L', flgLINK},
			{'F', flgFUNCTION},
			{'P', flgPARAMETER},
			{'H', flgHIDDEN},
			{'E', flgEXPORT},
			{'W', flgWRITEABLE},
			{0, 0}
		};
	char flags[33];
	char* tmp = &flags[0];
	memset(flags, 0, sizeof(flags));
	for (int i = 0; a[i].ch; i++)
	{
		if (flg & a[i].flag)
		{
			*tmp++ = a[i].ch;
		}
	}
	return &flags[0];
}

/**
 * Sets a flag or multiple flags on the reference
 */
void Variable::setFlag(int flag, bool skip_self)
{
	// Only owner is allowed to Set flags.
	if (isOwner())
	{
		auto tflgs = _reference->_curFlags;
		_reference->_curFlags |= flag;
		if (_reference->_curFlags != tflgs)
		{
			emitLocalEvent(veFlagsChange, skip_self);
		}
	}
}

/**
 * Unsets a flag or multiple flags of the attached VariableReference.
 */
void Variable::unsetFlag(int flag, bool skip_self)
{
	if (isOwner())
	{
		auto tflgs = _reference->_curFlags;
		_reference->_curFlags &= ~flag;
		if (_reference->_curFlags != tflgs)
		{
			emitLocalEvent(veFlagsChange, skip_self);
		}
	}
}

/**
 * Unsets a flag or multiple flags of the attached VariableReference.
 */
void Variable::updateFlags(int flag, bool skip_self)
{
	if (isOwner())
	{
		auto tflgs = _reference->_curFlags;
		_reference->_curFlags = flag;
		if (_reference->_curFlags != tflgs)
		{ // skip self
			emitLocalEvent(veFlagsChange, skip_self);
		}
	}
}

const Value& Variable::getCur() const
{
	// When temporary is used return the temporary value.
	return _temporary ? (*_temporary) : (isConverted() ? _reference->_convertCurValue : _reference->_curValue);
}

const Value& Variable::getCur(bool converted) const
{
	// When temporary is used return the temporary value.
	return (converted && _reference->_convertUnit.length()) ? _reference->_convertCurValue : _reference->_curValue;
}

bool Variable::setCur(const Value& value, bool skip_self)
{
	// When the temporary value is used skip updating the real value.
	if (_temporary)
	{
		return updateTempValue(value, skip_self);
	}
	// If this instance uses the converted value it must
	// be converted first to the real value.
	if (isConverted())
	{
		return updateValue(convert(value, true), skip_self);
	}
	// Update using the straight value.
	return updateValue(value, skip_self);
}

bool Variable::isReadOnly() const
{
	// If it is the owner it is not readonly by defaylt.
	// Only the owner can change a readonly instance.
	if (!isOwner())
	{
		// If the flag READONLY is Set the not owned variable is
		// read only by default.
		if (isFlag(flgREADONLY))
		{
			return true;
		}
		// When the variable is a local exported one and this instance is
		// globally referencing it is also read only because only
		// local referencing variables may change the current value or flags.
		if (!const_cast<Variable*>(this)->getOwner()._global && _global)
		{
			if (isFlag(flgWRITEABLE))
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
		if (!isFlag(flgREADONLY) && const_cast<Variable*>(this)->getOwner()._global)
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
		_RTTI_NOTIFY(DO_DEFAULT, "Cannot load current value of non owning instance!")
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
		}
			break;

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
			Value newval = value;
			// adjust the newval type
			if (!newval.setType(getType()))
			{
				// on failure return
				return false;
			}
			// check for change of value
			if (_reference->_curValue != newval)
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
			// Adjeust the newval type
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
		}
			break;

		case Value::vitString:
		{ // create non const std::string to modify
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
		}
			break;

		case Value::vitBinary:
		case Value::vitCustom:
		{ // Create non const Value type.
			Value new_val = value;
			// adjeust the new_val type
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
		}
			break;

		default:
			break;
	} // switch
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
		auto& var(not_ref_null(list) ? getVariableById(id, list) : const_cast<Variable&>(getVariableById(id)));
		var.updateValue(value, skip_self);
		var.updateFlags(toFlags(flags.c_str()), skip_self);
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
	_COND_NORM_NOTIFY(!value.isValid(), DO_DEFAULT, "Variable: Stream format Corrupt!")
	if (!is.fail() && !is.bad() && value.isValid())
	{
		// Check list for a NULL_REF and use
		auto& var(not_ref_null(list) ? getVariableById(id, list) : const_cast<Variable&>(getVariableById(id)));
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
					list.Add(var);
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
			// there are states return the state name if 'states' is true
			// if not run in to default switch
			unsigned count = _reference->_states.Count();
			if (use_states && count)
			{
				// Find the state of the current value.
				for (unsigned i = 0; i < count; i++)
				{
					if (_reference->_states[i]._value == (_temporary ? (*_temporary) : _reference->_curValue))
					{
						return _reference->_states[i]._name;
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

void Variable::setConvertHandler(VariableHandler* link)
{
	if (VariableStatic::_convertLink != link)
	{
		VariableStatic::_convertLink = link;
	}
}

bool Variable::setConvertValues(bool convert)
{
	// Only owners are allowed to Set the conversion values.
	// Check if the type is a floating point value that can be converted.
	if (isOwner() && _reference->_type == Value::vitFloat && _reference->_unit.length())
	{
		double mult = 1;
		double ofs = 0;
		if (convert)
		{
			// If the global covert link has been Set use it.
			if (VariableStatic::_convertLink)
			{ // Call the handler setting the linker to the zero variable to
				// indicate the global nature of the call.
				VariableStatic::_convertLink->VariableEventHandler(veConvert, *this, VariableStatic::zero(), false);
			}
			else
			{
				int sig = _reference->_sigDigits;
				std::string new_unit = _reference->_unit;
				// Check if a conversion is found.
				if (GetUnitConversion(_reference->_convertOption, new_unit, sig, mult, ofs, new_unit, sig))
				{
					// Set the convert values.
					return setConvertValues(new_unit, Value(mult), Value(ofs), sig);
				}
			}
		}
		else
		{ // Check if a conversion value must be disabled.
			if (_reference->_convertUnit.length())
			{ // Set the convert values to the original.
				return setConvertValues("", Value(mult), Value(ofs));
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
	{ // Make the change.
		_converted = enable;
		// Signal only if a conversion exists.
		if (_reference->_convertUnit.length())
		{
			// Signal this event to this single instance.
			emitEvent(veConverted, *this);
		}
	}
	//
	return isConverted();
}

Value Variable::convert(const Value& value, bool to_org) const
{
	Value ret_val = value;
	// Check if a conversion must be perfomed.
	if (_reference->_convertUnit.length() && getType() == Value::vitFloat)
	{
		// Abort the operation when conversion fails.
		if (!ret_val.setType(Value::vitFloat))
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Convert" << ret_val << " for passed value failed!")
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

			// Delete and disable the temporary value.
			delete_null(_temporary);
		}
	}
}

bool Variable::applyTemporary(bool skip_self)
{
	// Check if the temporary value is used.
	if (_temporary)
	{
		// If this instance uses the converted value it must
		// be converted first to the real value.
		if (isConverted())
		{
			return updateValue(convert(*_temporary, true), skip_self);
		}
		// Update using the straight value when not converted.
		return updateValue(*_temporary, skip_self);
	}
	return false;
}

bool Variable::temporaryDiffs() const
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

std::ostream& operator<<(std::ostream& os, const Variable::StateVector& v)
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
			rv = "GetOwner";
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

const char* Variable::getStringType(Variable::EStringType str)
{
	switch (str)
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
	return VariableStatic::_convertLink;
}

void Variable::initialize() noexcept
{
	VariableStatic::initialize(true);
}

void Variable::deinitialize() noexcept
{
	VariableStatic::initialize(false);
}

}
