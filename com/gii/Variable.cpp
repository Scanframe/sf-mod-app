#include <cmath>
#include <cstdio>

#include "UnitConversion.h"
#include "Variable.h"

#include "misc/dbgutils.h"
#include "misc/value.h"
#include "misc/csf.h"

namespace sf
{

// Holds the link to the global conversion handler.
TVariableLink* TVariable::FConvertLink = nullptr;

/**
 * To create a dynamically allocated TVariable instance for ZeroVariable.
 */
TVariable ZeroVariable("0x0,n/a,n/a,R,n/a,n/a,n/a,,,,,?=?,");

TVariable& TVariableBase::Zero(ZeroVariable);

TVariableReference::TVariableReference(bool global)
	:StateArray()
{
	Global = global;
	Exported = false;
	Valid = false;
	Id = 0;
	Flags = 0;
	CurFlags = 0;
	Type = TValue::vitINVALID;
	Descr = TValue::_invalidStr;
	Name = Descr.substr(0, 12);
	Unit = Descr.substr(0, 4);
	LocalActive = 0;
	SigDigits = 0;
	CnvSigDigits = 0;
	// Add to global reference list declared in TVariableBase
	if (global)
	{
		TVariableBase::RefList.Add(this);
	}
}

void TVariableReference::Copy(const TVariableReference& ref)
{
//  bool Global;
//  TPtrVector List;
	_COND_RTTI_NOTIFY(this == TVariableBase::Zero.FRef,
		DO_MSGBOX | DO_DBGBRK, "Cannot assign a ref to Zero Ref!");
	Valid = ref.Valid;
	Id = ref.Id;
	Flags = ref.Flags;
	CurFlags = ref.CurFlags;
	Type = ref.Type;
	Name = ref.Name;
	Descr = ref.Descr;
	Unit = ref.Unit;
	CurVal = ref.CurVal;
	DefVal = ref.DefVal;
	MaxVal = ref.MaxVal;
	MinVal = ref.MinVal;
	RndVal = ref.RndVal;
	SigDigits = ref.SigDigits;
	StateArray = ref.StateArray;
	LocalActive = ref.LocalActive;
	CnvUnit = ref.CnvUnit;
	CnvCurVal = ref.CnvCurVal;
	CnvDefVal = ref.CnvDefVal;
	CnvMaxVal = ref.CnvMaxVal;
	CnvMinVal = ref.CnvMinVal;
	CnvRndVal = ref.CnvRndVal;
	CnvSigDigits = ref.CnvSigDigits;
	CnvMult = ref.CnvMult;
	CnvOffset = ref.CnvOffset;
}

TVariableReference::~TVariableReference()
{
	// If TVariables are still referenced these must be removed first
	// except if it is ZeroVariable itself.
	if (ZeroVariable.FRef == this)
	{
		// Check if there are still variables in the system
		// when zero variable is destructed
//    if (TVariable::GetInstanceCount())
		unsigned totalcount = 0;
		TVariableBase::TRefPtrIterator i(TVariableBase::RefList);
		// Iterate through all variable references and count the usage count
		while (i)
		{
			TVariableReference* ref = i++;
			totalcount += ref->List.Count();
			// Notification of warning
			// Check if current ref is the ZeroVariable reference.
			if (ref == ZeroVariable.FRef)
			{
				// Iterate through each variable entry in the list.
				unsigned count = ref->List.Count();
				// Skip the first one because there is always ZeroVariable itself.
				for (unsigned idx = 1; i < count; i++)
				{
					// Notification of warning
					_NORM_NOTIFY
					(
						DO_MSGBOX | DO_DEFAULT,
						_RTTI_TYPENAME
							<< ": Dangling instance [" << idx << "/" << count
							<< "] with desired ID "
							<< stringf("0x%lX", ref->List[idx]->FDesiredId)
							<< " in this process!"
					);
					// Limit the amount shown to a maximum of 3.
					if (idx >= 3)
					{
						_NORM_NOTIFY(DO_MSGBOX | DO_DEFAULT,
							_RTTI_TYPENAME << ": Too many [" << count << "] dangling instances to be shown!");
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
						<< stringf("0x%lX", ref->Id)
						<< " in this process!"
				);
			}
		}
		// Subtract 1 for zero variable itself.
		// There should only be one left in the list
		totalcount--;
		// Notification of warning
		_COND_NORM_NOTIFY
		(
			totalcount,
			DO_MSGBOX | DO_DEFAULT,
			_RTTI_TYPENAME
				<< ": Total of " << totalcount
				<< " dangling TVariable instances in this process!"
		);
	}
	else
	{
		unsigned count = List.Count();
		while (count--)
		{
			// Attach variable to zerovariable which is the default and the invalid one
			if (List[count])
			{
				List[count]->Setup(0L, false);
			}
		}
	}
	// Remove from global reference list declared in TVariableBase
	TVariableBase::RefList.Detach(this);
}

TVariable::TVariable()
{
	_COND_RTTI_THROW(!ZeroVariable.FRef, "ZeroVariable is NOT created! Library initialisation error!");
	InitMembers(true);
	AttachRef(ZeroVariable.FRef);
}

TVariable::TVariable(bool global)
{
	InitMembers(global);
	AttachRef(ZeroVariable.FRef);
}

bool TVariable::IsExported() const
{
	return FRef->Exported;
}

bool TVariable::SetExport(bool global)
{
	// Check out before continuing
	if
		(
		!IsOwner()            // Only an owner can be exported.
			|| !IsFlag(flgEXPORT) // And it must be exportable.
			|| !FRef              // The reference pointer cannot be NULL.
			|| FRef == Zero.FRef  // The attached reference may not be the Zero one.
			|| FGlobal            // To make an owner appear global it must be local.
			|| (global && !FRef->Id)// The local variable ID cannot be zero when global.
		)
	{
		// Signal failure to make it global.
		return false;
	}
	// Only continue when there is a real change at hand.
	if (FRef->Exported == global)
	{
		return true;
	}
	// Assign the new value.
	FRef->Exported = global;
	// Check if the ID already exist.
	// GetVarRefById returns ZeroVariable if not found.
	TVariableReference* ref = GetVarRefById(FRef->Id);
	// See if the variable must be made global.
	if (global)
	{
		// Check if the current reference is already global.
		if (ref == FRef)
		{
			// If so signal success.
			return true;
		}
		// When the id was found.
		if (ref != ZeroVariable.FRef)
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Tried to export ID:!\n"
				<< stringf("0x%lX,", FRef->Id) << FRef->Name << "\nover!\n"
				<< ref->Name << '!');
			// If the ID already exist return false.
			return false;
		}
		// Add this variable to the global static list.
		TVariableBase::RefList.Add(FRef);
		// Notify all global linked users of TVariable instances of new ID.
		GlobalEvent(veNEWID, true);
		// Iterate through global variable instances and attach those having
		// the desired ID just created.
		AttachDesired();
	}
		// When variable must be made local again.
	else
	{
		// Remove the reference from the global static list.
		if (!TVariableBase::RefList.Detach(FRef))
		{
			return true;
		}
		// Create a temporary vector pointers to variables that must be
		// detached because the reference list is modified when a variable
		// is detached.
		TVariable::TPtrVector list;
		// Iterate through all variables having the global flag set.
		TVariable::TPtrIterator i(FRef->List);
		while (i)
		{
			// Get variable pointer.
			TVariable* var = (i++);
			// When instance is global add it to the list ot to be detached instances.
			if (var->FGlobal)
			{
				list.Add(var);
			}
		}
		// Now iterated through the new list.
		TVariable::TPtrIterator j(list);
		while (j)
		{
			TVariable* var = (j++);
			var->AttachRef(ZeroVariable.FRef);
		}
	}
	// Signal success.
	return true;
}

bool TVariable::SetGlobal(bool global)
{
	if (FGlobal != global)
	{
		// Only when variable is not attached to other then Zero.
		if (FRef == Zero.FRef)
		{
			FGlobal = global;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void TVariable::InitMembers(bool global)
{
	FGlobal = global;
	FRef = nullptr;
	FLink = nullptr;
	FDesiredId = 0;
	FData = 0;
	FConverted = false;
	FTemporary = nullptr;
}

void TVariable::operator delete(void* p)
{
	if (GlobalActive)
	{
		_NORM_NOTIFY(DO_DEFAULT, "Warning: Deleting Instance During An Event!");
		// Do not free allocated data before end of global event remove
		DeleteWaitCache.Add(p);
	}
	else
	{
		// Call real destructor.
		::operator delete(p);
		// If deletion is allowed again delete the wait cache too.
		mcvector<void*>::size_type count = DeleteWaitCache.Count();
		if (count)
		{
			while (count--)
			{
				delete (char*) DeleteWaitCache[count];
			}
			// Flush the entries from the list.
			DeleteWaitCache.Flush();
		}
	}
}

TVariable::~TVariable()
{
	// if a Link function exist notify removal of this instance.
	NotifyVariable(veREMOVE, *this);
	// Set link also signals the unlinking of the variable.
	SetLink(nullptr);
	// Remove this instance from the list by attach reference NULL.
	AttachRef(nullptr);
	// if the temprary value is still in use do not forget to delete it.
	delete_null(FTemporary);
}

unsigned TVariable::GetVariableCount()
{
	// Return the actual count -1 because of ZeroVariable.
	return RefList.Count() - 1;
}

unsigned TVariable::GetInstanceCount()
{
	unsigned count = 0;
	TRefPtrIterator i(RefList);
	// Iterate through all variable references and count the usage count.
	while (i)
	{
		count += (i++)->List.Count();
	}
	// Substract 1 for zero variable before returning.
	return --count;
}

const TVariable* TVariable::GetVariableListItem(unsigned p)
{  // return the first TVariable in the list because this
	// there is always one in there otherwise no Reference would exist
	// increase p by 1 to skip ZeroVariable which is the first in the list
	p++;
	TVariableReference* ref = (p < RefList.Count()) ? RefList[p] : NULL;
	return (ref) ? ref->List[0] : &ZeroVariable;
}

TVariableReference* TVariable::GetVarRefById(TVariable::id_type id)
{
	if (id)
	{
		TRefPtrIterator i(RefList);
		while (i)
		{
			if (i.Current()->Id == id)
			{
				return i.Current();
			}
			i++;
		}
	}
	return ZeroVariable.FRef;
}

TVariable& TVariable::GetVariableById(TVariable::id_type id, TPtrVector& list)
{
	TVariable::TPtrIterator i(list);
	while (i)
	{
		if (i.Current()->GetId() == id)
		{
			return *i.Current();
		}
		i++;
	}
	return ZeroVariable;
}

void TVariable::MakeOwner()
{
	// Do not change ownership if it is ZeroRef
	if (FRef == ZeroVariable.FRef)
	{
		return;
	}
	// Get the previous owner for signaling loosing of ownership.
	TVariable* prevowner = FRef->List[0];
	// Do not change ownership if current instance is allready owner
	if (prevowner == this)
	{
		return;
	}
	// Put this pointer at the beginning of the list which makes it the owner
	FRef->List.Detach(this);
	FRef->List.AddAt(this, 0);
	// Notify previous owner of losing ownership
	prevowner->NotifyVariable(veLOSTOWNER, *this);
	// Notify thish instance of getting ownership.
	NotifyVariable(veGETOWNER, *this);
/*
  // Check
  if (FRef->List[0] != this)
    RTTI_NOTIFY(DO_DEFAULT, "Variable is NOT Owner!");
*/
}

bool TVariable::AttachRef(TVariableReference* ref)
{
	// If the new ref is the same as the current do nothing.
	if (FRef == ref)
	{
		return true;
	}
	// If the current instance is not global and the ref is global
	// do not attach the reference but copy the reference members.
	if ((ref != Zero.FRef) && ref && !FGlobal && ref->Global)
	{ // When the reference pointer valid continue.
		if (FRef)
		{
			// Special handling when zero ref is the current reference.
			if (FRef == Zero.FRef)
			{
				// Remove this variable from the Zero reference variable list.
				FRef->List.Detach(this);
				// Create a new local variable reference.
				FRef = new TVariableReference(false);
				// Attach this variable to the new reference.
				FRef->List.Add(this);
			}
			// Copy the reference fields from the global reference to the current reference.
			FRef->Copy(*ref);
			// Notify the user of this instance by telling it was attached.
			NotifyVariable(veIDCHANGED, *this);
			// Attachment succeeded.
			return true;
		}
	}
	//
	bool rv = true;
	if (FRef)
	{
		// If this was the last one or the owner the refering FRef will be deleted
		// List[0] is the owner default the one who created the FRef instance
		if (FRef->List.Count() == 0 || this == FRef->List[0])
		{
			// Notify all variables that this reference is to become invalid.
			LocalEvent(veINVALID, false);
			// Make pointer to this variable NULL so it isn't attached again in
			// the reference destructor and detach it when the ref is deleted
			FRef->List[0] = NULL;
			// the destructor reattaches all variables that are left
			// in the list to the ZeroVariable reference except for the first
			// that is a NULL pointer and prevents this way the rentry of this part
			// of this function because the first in the list is the owner.
			delete FRef;
		}
		else
		{ // Just remove variable from list if it is not the owner.
			if (!FRef->List.Detach(this))
			{
				_RTTI_NOTIFY(DO_MSGBOX | DO_CERR, "Could not remove instance from list!");
				rv = false;
			}
		}
		// member FRef is assigned NULL to indicate no attachment
		FRef = nullptr;
	}
	//
	if (ref)
	{
		FRef = ref;
		// Add this instance to the the Variable Reference List
		FRef->List.Add(this);
		// Notify the user of this instance by telling it was attached.
		NotifyVariable(veIDCHANGED, *this);
	}
	return rv;
}

bool TVariable::SetId(TVariable::id_type id, bool skip_self)
{
	// Only owner of local variable is allowed to do this.
	if (IsOwner() && !FGlobal)
	{ // Check if the local instance is currently exported.
		if (!FRef->Exported)
		{ // Check if the new ID is different and non zero.
			if (id && FRef->Id != id)
			{ // Assign the new ID.
				FRef->Id = id;
				// Notify all local referenced instances.
				LocalEvent(veIDCHANGED, skip_self);
				// Signal success.
				return true;
			}
		}
	}
	return false;
}

bool TVariable::Setup(const TDefinition* def)
{
	// Check for not owning and global variable.
	bool localowner = !FGlobal && IsOwner() && GetUsageCount() > 1;
	// If this is a not local owner attach it to the zero instance.
	if (!localowner)
	{
		AttachRef(ZeroVariable.FRef);
	}
	// Reset the desired ID data member.
	FDesiredId = 0L;
	// The default return value is true.
	bool retval = true;
	// Check if variables other then the zero variable have an ID of zero.
	if (FRef != ZeroVariable.FRef && !localowner && def->Id == 0L)
	{
		retval = false;
	}
	// Don' bother to go on if an error occurred sofar
	if (retval)
	{ // Check if the ID already exist.
		// GetVarRefById returns ZeroVariable if not found or if
		// this instance is non global.
		TVariableReference* ref = FGlobal ? GetVarRefById(def->Id) : ZeroVariable.FRef;
		if (ref && ref != ZeroVariable.FRef)
		{
			_RTTI_NOTIFY(DO_DEFAULT | DO_MSGBOX, "Tried to create duplicate ID: !\n"
				<< def->Name << " (0x" << std::hex << def->Id << ")\nover!\n" << ref->Name << '!');
			// If ref already exist return false.
			return false;
		}
		else
		{ // Create new or use old one.
			if (localowner)
			{
				// Reuse the current reference
				ref = FRef;
			}
			else
			{
				// Create new global or non global reference for this variable instance.
				ref = new TVariableReference(FGlobal);
			}
		}
		// Initialize reference members from here.
		ref->Valid = true;
		ref->Id = def->Id;
		ref->Name = def->Name;
		ref->Unit = def->Unit;
		ref->CnvOption = def->ConvertOption;
		ref->Descr = def->Descr;
		ref->Flags = def->Flags;
		ref->Type = def->Type;
		// Use the original flags for the current ones.
		ref->CurFlags = ref->Flags;
		// Check for multi line string so the default value
		if (def->Type == TValue::vitSTRING && ref->Unit.find("M") != std::string::npos)
		{
			ref->DefVal.Set(unescape(def->Default.GetString()));
		}
		else
		{
			ref->DefVal = def->Default;
		}
		// Do not set the current value when it is a local owner.
		if (!localowner)
		{
			ref->CurVal.Set(ref->DefVal);
		}
		ref->RndVal = def->Round;
		ref->MinVal = def->Min;
		ref->MaxVal = def->Min;
		for (unsigned i = 0; i < def->States.Count(); i++)
		{
			ref->StateArray.Add(def->States[i]);
		}
		// Get significant digits for the float type variable.
		if (TValue::vitFLOAT == ref->Type)
		{
			// Get the significant digits from the round value.
			if (ref->RndVal)
			{
				ref->SigDigits = digits(ref->RndVal.GetFloat());
			}
			else
			{
				// Digits to maximum int because round is zero.
				ref->SigDigits = INT_MAX;
			}
		}
		else
		{
			ref->SigDigits = 0;
		}
		// Skip type conversion if an error occurred so far.
		if (retval)
		{
			// Convert all TValue reference data members to the wanted type
			// and check for errors during the conversion.
			retval &= ref->CurVal.SetType(ref->Type);
			retval &= ref->DefVal.SetType(ref->Type);
			retval &= ref->MaxVal.SetType(ref->Type);
			retval &= ref->MinVal.SetType(ref->Type);
			retval &= ref->RndVal.SetType(ref->Type);
			for (uint i = 0; i < ref->StateArray.Count(); i++)
			{
				retval &= ref->StateArray[i].Value.SetType(ref->Type);
			}
		}
		// Attach ref to this variable if possible if everything went well so far.
		if (!retval || !AttachRef(ref))
		{
			// Set 'retval' to false so an error message is generated
			retval = false;
			// When it is the local owner the reference is automatically
			// deleted further on.
			if (!localowner)
			{
				// Delete the reference made with new because it failed to attach.
				delete ref;
			}
		}
		// Signal global variables of a new created variable ID.
		if (retval && FGlobal)
		{ // Notify all global linked users of TVariable instances of new ID.
			GlobalEvent(veNEWID, false);
			// Iterate through global variable instances and attach those having
			// the desired ID just created.
			AttachDesired();
		}
	}
	// When the reference was reused with all users attached.
	if (localowner)
	{
		// When setup fails to setup this local owning instance.
		// Attach it to the Zero instance.
		if (retval)
		{
			// Notify the user of this instance of the redefined reference
			// using an ID change event.
			LocalEvent(veIDCHANGED, false);
		}
		else
		{
			// This also signals the users.
			AttachRef(ZeroVariable.FRef);
		}
	}
	// If the setup succeeded notify the handler of this event.
	if (retval)
	{
		NotifyVariable(veSETUP, *this);
	}
	// In case of an error report to standard out.
	_COND_RTTI_NOTIFY(!retval, DO_DEFAULT | DO_MSGBOX, "Error Setup Definition: "
		<< def->Name << " (0x" << std::hex << def->Id << ")!");
	//
	return retval;
}

bool TVariable::Setup(const std::string& definition, TVariable::id_type id_ofs)
{
	// Check for not owning and global variable.
	bool localowner = !FGlobal && IsOwner() && GetUsageCount() > 1;
	// If this is a not local owner attach it to the zero instance.
	if (!localowner)
	{
		AttachRef(ZeroVariable.FRef);
	}
	// Reset the desired ID data member.
	FDesiredId = 0L;
	// Pointer that points to the place where the conversion of the ID went wrong.
	char* endptr = nullptr;
	// Get the result ID from the setup string.
	std::string tmp = GetCsfField(vfeID, definition);
	id_type id = std::strtoull(tmp.c_str(), &endptr, 0);
	if (id)
	{
		id += id_ofs;
	}
	// The default return value is true.
	bool ret_val = true;
	// Return zero if an error occurred during conversion of the ID.
	if (endptr && *endptr != '\0')
	{
		ret_val = false;
	}
	// Check if variables other then the zero variable have an ID of zero.
	if (FRef != ZeroVariable.FRef && !localowner && id == 0L)
	{
		ret_val = false;
	}
	// Don' bother to go on if an error occurred so far
	if (ret_val)
	{ // Check if the ID already exist.
		// GetVarRefById returns ZeroVariable if not found or if
		// this instance is non global.
		TVariableReference* ref = FGlobal ? GetVarRefById(id) : ZeroVariable.FRef;
		if (ref && ref != ZeroVariable.FRef)
		{
			_RTTI_NOTIFY(DO_DEFAULT | DO_MSGBOX, "Tried to create duplicate ID: !\n"
				<< definition << "\nover!\n" << ref->Name << '!');
			// If ref already exist return false.
			return false;
		}
		else
		{ // Create new or use old one.
			if (localowner)
			{
				// Reuse the current reference
				ref = FRef;
				// Invalidate the conversion values by clearing the conversion unit.
				FRef->CnvUnit.resize(0);
			}
			else
			{
				// Create new global or non global reference for this variable instance.
				ref = new TVariableReference(FGlobal);
			}
		}
		// initialize reference members from here
		// copy definition std::string in to buffer
		const std::string defin = unique(definition);
		ref->Valid = true;
		// read all values in as strings to convert
		// them later to the actual form
		ref->Id = id;
		ref->Name = GetCsfField(vfeNAME, defin);
		ref->Unit = GetCsfField(vfeUNIT, defin);
		ref->CnvOption = GetCsfField(vfeCNV, defin);
		ref->Descr = escape(GetCsfField(vfeDESCR, defin));
		ref->Flags = StringToFlags(GetCsfField(vfeFLAGS, defin).c_str());
		ref->CurFlags = ref->Flags;// copy of original flags
		// Check for multi line string so the default value
		TValue::EType type = (TValue::EType) TValue::GetType(GetCsfField(vfeTYPE, defin).c_str());
		if (type == TValue::vitSTRING && ref->Unit.find('M') != std::string::npos)
		{
			ref->DefVal.Set(unescape(GetCsfField(vfeDEF, defin)));
		}
		else
		{
			ref->DefVal.Set(GetCsfField(vfeDEF, defin));
		}
		// Do not set the current value when it is a local owner..
		if (!localowner)
		{
			ref->CurVal.Set(ref->DefVal);
		}
		ref->MinVal.Set(GetCsfField(vfeMIN, defin).c_str());
		ref->MaxVal.Set(GetCsfField(vfeMAX, defin).c_str());
		ref->RndVal.Set(GetCsfField(vfeRND, defin).c_str());
		// Get max state field count
		int state_count = 0;
		while (GetCsfField(vfeFIRSTSTATE + state_count, defin).length())
		{
			state_count++;
		}
		ref->StateArray.Resize(state_count);
		for (int i = 0; i < state_count; i++)
		{
			ref->StateArray[i].Name = GetCsfField(vfeFIRSTSTATE + i, defin);
			if (ref->StateArray[i].Name.length())
			{
				size_t pos = ref->StateArray[i].Name.find_first_of('=');
				// If equal sign has been found add state value
				if (pos != std::string::npos)
				{
					ref->StateArray[i].Value.Set(ref->StateArray[i].Name.substr(pos + 1).c_str());
					// Truncate name to position of the equal sign
					ref->StateArray[i].Name.resize(pos);
				}
			}
			else
			{
				break;
			}
		}
		// Get significant digits for the float type variable.
		if (TValue::vitFLOAT == type)
		{
			// Get the significant digits from the round value.
			if (ref->RndVal)
			{
				ref->SigDigits = digits(ref->RndVal.GetFloat());
			}
			else
			{
				// Digits to maximum int because round is zero.
				ref->SigDigits = INT_MAX;
			}
		}
		else
		{
			ref->SigDigits = 0;
		}
		// Skip type conversion if an error occurred so far.
		if (ret_val)
		{
			// Convert all TValue reference data members to the wanted type
			// and check for errors during the conversion.
			ret_val &= ref->CurVal.SetType(type);
			ret_val &= ref->DefVal.SetType(type);
			ret_val &= ref->MaxVal.SetType(type);
			ret_val &= ref->MinVal.SetType(type);
			ret_val &= ref->RndVal.SetType(type);
			for (uint i = 0; i < ref->StateArray.Count(); i++)
			{
				ret_val &= ref->StateArray[i].Value.SetType(type);
			}
			ref->Type = (TValue::EType) type;
		}
		// Attach ref to this variable if possible if everything went well so far.
		if (!ret_val || !AttachRef(ref))
		{
			// Set 'ret_val' to false so an error message is generated
			ret_val = false;
			// When it is the local owner the reference is automatically
			// deleted further on.
			if (!localowner)
			{
				// Delete the reference made with new because it failed to attach.
				delete ref;
			}
		}
		// Signal global variables of a new created variable ID.
		if (ret_val && FGlobal)
		{ // Notify all global linked users of TVariable instances of new ID.
			GlobalEvent(veNEWID, false);
			// Iterate through global variable instances and attach those having
			// the desired ID just created.
			AttachDesired();
		}
	}
	// When the reference was reused with all users attached.
	if (localowner)
	{
		// When setup fails to setup this local owning instance.
		// Attach it to the Zero instance.
		if (ret_val)
		{
			// Notify the user of this instance of the redefined reference
			// using an ID change event.
			LocalEvent(veIDCHANGED, false);
		}
		else
		{
			// This also signals the users.
			AttachRef(ZeroVariable.FRef);
		}
	}
	// If the setup succeeded notify the handler of this event.
	if (ret_val)
	{
		NotifyVariable(veSETUP, *this);
	}
	// Incase of an error report to standard out.
	_COND_RTTI_NOTIFY(!ret_val, DO_DEFAULT | DO_MSGBOX, "Error In Setup String: "
		<< definition << '!');
	//
	return ret_val;
}

void TVariable::SetDesiredId(TVariable::id_type id)
{
	if (FDesiredId != id)
	{ // Make the change.
		FDesiredId = id;
		// Signal this event to this single instance.
		NotifyVariable(veDESIREDID, *this);
	}
}

std::string TVariable::GetName(int levels) const
{
	// Return full name path at levels equals zero.
	std::string retval = FRef->Name;
	int len = retval.length();
	// Check if levels is non zero.
	if (levels)
	{
		if (levels > 0)
		{
			int i = len;
			while (i--)
			{ // Is the character a separator character.
				if (retval[i] == '|')
				{
					retval[i] = ' ';
					if (!--levels)
					{
						break;
					}
				}
			}
			if (i && i < len - 1)
			{
				return retval.substr(i + 1);
			}
		}
		else
		{
			unsigned p = 0;
			for (unsigned i = 0; i < retval.length(); i++)
			{
				// Is the character a separator character.
				if (retval[i] == '|')
				{ // Clear the separator character.
					retval[i] = ' ';
					//
					if (!++levels)
					{
						p = i + 1;
					}
				}
			}
			if (p < (unsigned) len)
			{
				return retval.substr(p, retval.length() - p);
			}
		}
	}
	return retval;
}

int TVariable::GetNameLevelCount() const
{
	int levels = 0;
	int i = FRef->Name.length();
	while (i--)
	{ // Is the character a separator character.
		if (FRef->Name[i] == '|')
		{
			levels++;
		}
	}
	return levels;
}

unsigned TVariable::GetState(const TValue& v) const
{
	unsigned count = FRef->StateArray.Count();
	for (unsigned i = 0; i < count; i++)
	{
		if (FRef->StateArray[i].Value == v)
		{
			return i;
		}
	}
	return UINT_MAX;
}

const TValue& TVariable::GetStateValue(unsigned state) const
{ // Check index versus range
	if (state < FRef->StateArray.Count())
	{
		return FRef->StateArray[state].Value;
	}
	// if out of range return TState from ZeroRef
	return ZeroVariable.FRef->StateArray[0].Value;
}

std::string TVariable::GetStateName(unsigned state) const
{ // check index versus range
	if (state < FRef->StateArray.Count())
	{
		return FRef->StateArray[state].Name;
	}
	// if out of range return TState from ZeroRef
	return ZeroVariable.FRef->StateArray[0].Name;
}

bool TVariable::Increase(int steps, bool skip_self)
{
	// Skip certain variables from this function.
	if (!IsNumber() || FRef->RndVal.IsZero())
	{
		return false;
	}
	//
	if (FTemporary)
	{
		TValue& value(IsConverted() ? FRef->CnvRndVal : FRef->RndVal);
		return UpdateTempValue(*FTemporary + (value * TValue(steps)), skip_self);
	}
	else
	{
		return UpdateValue(FRef->CurVal + (FRef->RndVal * TValue(steps)), skip_self);
	}
}

void TVariable::NotifyVariable
	(
		EEvent event,
		const TVariable& caller
	)
{
	// If the caller is an other instance teh this one when the value changes.
	// When one these events passes, Update the temporary value.
	if (FTemporary)
	{
		if ((&caller != this && event == veVALUECHANGE)
			|| event == veIDCHANGED || event == veCONVERTED)
		{
			// Skipself here because we're already handling an event.
			UpdateTemporary(true);
		}
	}
	// Check if a handler was linked and if so call it.
	if (FLink)
	{
		FLink->VariableEventHandler(event, caller, *this, &caller == this);
	}
	else
	{
#if IS_VCL_TARGET
		// Check if a VCL closure was linked.
		if (FVclLink)
			FVclLink(event, caller, *this, &caller==this);
#endif
	}
}

unsigned TVariable::VariableEvent(EEvent event, bool skipself)
{ // check if the event must be global or not
	if (event < veFIRSTLOCAL)
	{
		return GlobalEvent(event, skipself);
	}
	else
	{
		if (event > veFIRSTPRIVATE)
		{
			NotifyVariable(event, *this);
			return 1;
		}
		else
		{
			return LocalEvent(event, skipself);
		}
	}
}

unsigned TVariable::LocalEvent(EEvent event, bool skipself)
{ // Disable deletion of local instances.
	GlobalActive++;
	TPtrVector evlist;
	TPtrIterator i(FRef->List);
	TVariable* var;
	// Iterate through list and generate the variables from which
	// the event handler needs to be called.
	while (i)
	{
		var = i++;
		// Check for skipself
		if (var && (!skipself || var != this))
		{
			evlist.Add(var);
		}
	}
	// Iterate through the generated list and call the event handler.
	// Any changes made by event handlers that could affect the list is avoided.
	TPtrIterator j(evlist);
	while (j)
	{
		// Call the event handler.
		(j++)->NotifyVariable(event, *this);
	}
	// Enable deletion of local instances.
	GlobalActive--;
	// return the amount of variables that were effected by the call to this function.
	return evlist.Count();
}

unsigned TVariable::GlobalEvent(EEvent event, bool skipself)
{
	// When the variable is not global send the event locally.
	if (!FGlobal && !FRef->Exported)
	{
		return LocalEvent(event, skipself);
	}
	// Disable deletion of instances.
	GlobalActive++;
	// Declare event list for instance pointers.
	TPtrVector evlist;
	TRefPtrIterator i(RefList);
	// Iterate through all references.
	while (i)
	{
		TVariableReference* ref = i++;
		TPtrIterator j(ref->List);
		// Iterate through all variables of the reference.
		while (j)
		{
			TVariable* var = j++;
			// Check for skipself and if the variable is global.
			// Only global variables get a global event.
			if (var && (!skipself || var != this) && var->FGlobal)
			{
				// Add variable to list.
				evlist.Add(var);
			}
		}
	}
	// Iterate through the generated variable list and call the handler of them.
	// Any changes made by event handlers that could affect the list is avoided.
	TPtrIterator k(evlist);
	while (k)
	{
		(k++)->NotifyVariable(event, *this);
	}
	// Enable deletion of instances.
	GlobalActive--;
	// Return  the lists size as the count of the events.
	return evlist.Count();
}

unsigned TVariable::AttachDesired()
{
	// Disable deletion of instances.
	GlobalActive++;
	// Signal other event handler functions are called using a
	// global predefined list.
	TPtrVector evlist;
	TRefPtrIterator i(RefList);
	// Iterate through all references generating pointers to instances
	// having a desired ID that must be attached.
	while (i)
	{
		TVariableReference* ref = i++;
		TPtrIterator j(ref->List);
		// Iterate through all variables of the reference.
		while (j)
		{
			TVariable* var = j++;
			// Check if the desired ID is non-zero which means it is enabled.
			// Also compare the desired and this ID to check if it has not be
			// linked to the reference. Also check if the variable is global
			// because globale variables can only be attached by a desired ID.
			if (var && var->FGlobal && var->FDesiredId && var->FDesiredId == FRef->Id)
			{
				// Add instance pointer the to list.
				evlist.Add(var);
			}
		}
	}
	// Iterate through the generated variable list and attach them.
	// Any changes made by event handlers that could affect the list is avoided.
	TPtrIterator k(evlist);
	while (k)
	{ // Assgin current to temporary pointer.
		TVariable* var = k++;
		// Compare them again to be shure after Attaching the others.
		if (var->FDesiredId && var->FDesiredId == FRef->Id)
		{
			// Attach the one having the correct desired ID.
			var->AttachRef(FRef);
		}
	}
	// Enable deletion of instances.
	GlobalActive--;
	// Return  the lists size as the count of the events.
	return evlist.Count();
}

#if IS_VCL_TARGET
void __fastcall TVariable::SetVclLink(TNotifyVariableEvent vcllink)
{
	// only generate events if there is a change of link
	if (FVclLink != vcllink)
	{
		if (vcllink)
		{ // notify instance getting event link
			FVclLink = vcllink;
			NotifyVariable(veLINKED, *this);
		}
		else
		{ // notify instance losing event link in the was already a link
			if (FVclLink)
			{
				NotifyVariable(veUNLINKED, *this);
				FVclLink = NULL;
			}
		}
	}
}
#endif

void TVariable::SetLink(TVariableLink* link)
{
	// only generate events if there is a change of link
	if (FLink != link)
	{
		if (link)
		{ // notify instance getting event link
			FLink = link;
			NotifyVariable(veLINKED, *this);
		}
		else
		{ // notify instance losing event link in the was already a link
			if (FLink)
			{
				NotifyVariable(veUNLINKED, *this);
				FLink = nullptr;
			}
		}
	}
}

void TVariable::RemoveLink(TVariableLink* link)
{ // get the total count of variable references
	unsigned vc = RefList.Count();
	// iterate through variable references
	for (unsigned i = 0; i < vc; i++)
	{
		// get the current variable pointer
		TVariableReference* ref = RefList[i];
		// get the total amount of variables attached to this reference
		unsigned vrc = ref->List.Count();
		// iterate through the variables
		for (unsigned j = 0; j < vrc; j++)
		{
			// check if the varlinks are the same
			if (ref->List[j]->FLink == link)
			{
				// set the link to NULL so the event function isn't called
				ref->List[j]->FLink = nullptr;
			}
		}
	}
}

int TVariable::StringToFlags(const char* flags)
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

std::string TVariable::GetFlagsString(TVariable::flags_type flg)
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
 * sets a flag or multiple flags on the reference
 */
void TVariable::SetFlag(int flag, bool skip_self)
{
	// Only owner is allowed to set flags.
	if (IsOwner())
	{
		auto tflgs = FRef->CurFlags;
		FRef->CurFlags |= flag;
		if (FRef->CurFlags != tflgs)
		{
			LocalEvent(veFLAGSCHANGE, skip_self);
		}
	}
}

/**
 * Unsets a flag or multiple flags of the attached TVariableReference.
 */
//
void TVariable::UnsetFlag(int flag, bool skip_self)
{
	if (IsOwner())
	{
		auto tflgs = FRef->CurFlags;
		FRef->CurFlags &= ~flag;
		if (FRef->CurFlags != tflgs)
		{
			LocalEvent(veFLAGSCHANGE, skip_self);
		}
	}
}

/**
 * Unsets a flag or multiple flags of the attached TVariableReference.
 */
void TVariable::UpdateFlags(int flag, bool skip_self)
{
	if (IsOwner())
	{
		auto tflgs = FRef->CurFlags;
		FRef->CurFlags = flag;
		if (FRef->CurFlags != tflgs)
		{ // skip self
			LocalEvent(veFLAGSCHANGE, skip_self);
		}
	}
}

const TValue& TVariable::GetCur() const
{
	// When temporary is used return the temporary value.
	return FTemporary ? (*FTemporary) : (IsConverted() ? FRef->CnvCurVal : FRef->CurVal);
}

const TValue& TVariable::GetCur(bool converted) const
{
	// When temporary is used return the temporary value.
	return (converted && FRef->CnvUnit.length()) ? FRef->CnvCurVal : FRef->CurVal;
}

bool TVariable::SetCur(const TValue& value, bool skip_self)
{
	// When the temporary value is used skip updating the real value.
	if (FTemporary)
	{
		return UpdateTempValue(value, skip_self);
	}
	// If this instance uses the converted value it must
	// be converted first to the real value.
	if (IsConverted())
	{
		return UpdateValue(Convert(value, true), skip_self);
	}
	// Update using the straight value.
	return UpdateValue(value, skip_self);
}

bool TVariable::IsReadOnly() const
{
	// If it is the owner it is not readonly by defaylt.
	// Only the owner can change a readonly instance.
	if (!IsOwner())
	{
		// If the flag READONLY is set the not owned variable is
		// read only by default.
		if (IsFlag(flgREADONLY))
		{
			return true;
		}
		// When the variable is a local exported one and this instance is
		// globally referencing it is also read only because only
		// local referencing variables may change the current value or flags.
		if (!const_cast<TVariable*>(this)->GetOwner().FGlobal && FGlobal)
		{
			if (IsFlag(flgWRITEABLE))
			{
				return false;
			}
			return true;
		}
	}
	return FRef == Zero.FRef;
}

bool TVariable::LoadCur(const TValue& value) const
{ // Only allowed by owners.
	if (IsOwner())
	{ // Check if this instance is not read-only and global because
		// only globals are loadable.
		if (!IsFlag(flgREADONLY) && const_cast<TVariable*>(this)->GetOwner().FGlobal)
		{  // Make a writable copy of the passed value.
			TValue newval(value);
			// Adjust the newval type
			if (!newval.SetType(GetType()))
			{
				return false;
			}
			// Clip when cipping is needed.
			if (IsNumber() && FRef->MaxVal != FRef->MinVal)
			{ // Adjust new value to the allowed range.
				newval = (newval > FRef->MaxVal) ? FRef->MaxVal : newval;
				newval = (newval < FRef->MinVal) ? FRef->MinVal : newval;
			}
			// Signal when changed and do not skip the event for this instannce.
			return const_cast<TVariable*>(this)->UpdateValue(newval, false);
		}
	}
	else
	{
		_RTTI_NOTIFY(DO_DEFAULT, "Cannot load current value of non owning instance!");
	}
	// Signal failure or no change.
	return false;
}

bool TVariable::UpdateValue(const TValue& value, bool skip_self)
{
	// Check if this instance can change its value.
	if (IsReadOnly())
	{
		return false;
	}
	// Flag that is evaluated at the end of the function for triggering an event.
	bool changed = false;
	// Make a difference between the several types.
	switch (GetType())
	{
		case TValue::vitINTEGER:
		case TValue::vitFLOAT:
		{
			// If the new value is numerical adjust type to CurVal type
			// create non const TValue type
			TValue newval = value;
			// Adjust the newval type
			if (!newval.SetType(GetType()))
			{
				// on failure return
				return false;
			}
			// Check if limits must be checked by comparing a difference and
			// checking the ownership.
			if (!IsOwner() && FRef->MaxVal != FRef->MinVal)
			{
				// Adjust new value to the allowed range.
				newval = (newval > FRef->MaxVal) ? FRef->MaxVal : newval;
				newval = (newval < FRef->MinVal) ? FRef->MinVal : newval;
			}
			// Check if new value must be rounded
			if (!FRef->RndVal.IsZero())
			{
				newval.Round(FRef->RndVal);
			}
			// set local 'changed ' to trigger event.
			changed = FRef->CurVal != newval;
			FRef->CurVal = newval;
		}
			break;

		case TValue::vitSTRING:
		{
			// Create non const std::string that can be modified.
			std::string s;
			// Get the filter string from the type.
			switch (GetStringType())
			{
				case stPATH:
					s = filter(value.GetString(), "/*?\"<>|,");
					// Strip also space from both ends.
					s = trim(s, " ");
					break;

				case stDIRECTORY:
					s = filter(value.GetString(), "/*?\"<>|,");
					// Strip also space from both ends.
					s = trim(s, " ");
					if (s.length() && s[s.length() - 1] != '\\')
					{
						s.append(1, '\\');
					}
					break;

				case stFILENAME:
					s = filter(value.GetString(), "/\\:*?\"<>|,");
					// Strip also space from both ends.
					s = trim(s, " ");
					break;

				case stSUBDIRECTORY:
					s = filter(value.GetString(), "/:*?\"<>|,");
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
					s = value.GetString();
			}
			// Get the maximum allowed length of the round field.
			auto max_len = (size_t) FRef->RndVal.GetInteger();
			// adjust length if max_len is non-zero and length > std::string length
			if (max_len && (s.length() > max_len))
			{
				s = s.substr(0, max_len);
			}
			// create TValue instance to compare and assign
			TValue v(s);
			// set local 'changed ' to trigger event
			changed = FRef->CurVal != v;
			// assign the value
			FRef->CurVal = v;
		}
			break;

		case TValue::vitBINARY:
		case TValue::vitCUSTOM:
		{
			// create non const TValue type
			TValue newval = value;
			// adjust the newval type
			if (!newval.SetType(GetType()))
			{
				// on failure return
				return false;
			}
			// check for change of value
			if (FRef->CurVal != newval)
			{
				FRef->CurVal = value;
				// set changed local variable to trigger event
				changed = true;
			}
		}
			break;

	} // switch
	// Check if there was a change of the current value.
	if (changed)
	{ // Update the Converted value if the unit is filled in.
		if (FRef->CnvUnit.length())
		{
			FRef->CnvCurVal = Convert(FRef->CurVal, false);
		}
		// notify all variables referencing this variable that
		// the current value has changed
		LocalEvent(veVALUECHANGE, skip_self);
	}
	return changed;
}

bool TVariable::UpdateTempValue(const TValue& value, bool skipself)
{
	// Check if the instance can change its value.
	if (IsReadOnly())
	{
		return false;
	}
	// Flag that is evaluated at the end of the function for triggering an event.
	bool changed = false;
	// Make a difference between the several types.
	switch (GetType())
	{
		case TValue::vitINTEGER:
		case TValue::vitFLOAT:
		{
			// If the new value is numerical adjust type to CurVal type
			// create non const TValue type
			TValue newval = value;
			// Adjeust the newval type
			if (!newval.SetType(GetType()))
			{
				// on failure return
				return false;
			}
			// Clip and round the new value.
			ClipRound(newval);
			// Set local 'changed ' to trigger an event.
			changed = *FTemporary != newval;
			*FTemporary = newval;
		}
			break;

		case TValue::vitSTRING:
		{ // create non const std::string to modify
			std::string s = value.GetString();
			size_t maxlen = (size_t) FRef->RndVal.GetInteger();
			// adjust length if maxlen is non-zero and length > std::string length
			if (maxlen && (s.length() > maxlen))
			{
				s = s.substr(0, maxlen);
			}
			// Create TValue instance to compare and assign.
			TValue v(s);
			// Set local 'changed ' to trigger an event
			changed = *FTemporary != v;
			// Assign the new value.
			*FTemporary = v;
		}
			break;

		case TValue::vitBINARY:
		case TValue::vitCUSTOM:
		{ // Create non const TValue type.
			TValue newval = value;
			// adjeust the newval type
			if (!newval.SetType(GetType()))
			{
				// on failure return
				return false;
			}
			// check for change of value
			if (*FTemporary != newval)
			{
				*FTemporary = value;
				// Set changed local variable to trigger event
				changed = true;
			}
		}
			break;

	} // switch

	// Check if there was a change of the current value.
	if (changed)
	{
		// Notify only the current instance of the change of the value.
		if (!skipself)
		{
			NotifyVariable(veVALUECHANGE, *this);
		}
	}
	return changed;
}

bool TVariable::WriteUpdate(std::ostream& os) const
{
	os << '(' << FRef->Id << ',' << FRef->CurVal << ','
		<< GetFlagsString(FRef->CurFlags) << ')' << '\n';
	// check if out stream is valid
	return !os.fail() && !os.bad();
}

// Function used for process communication.
bool TVariable::ReadUpdate(std::istream& is, bool skip_self, TPtrVector& list)
{
	TValue value;
	id_type id = 0;
	std::string flags;
	char c;

	is >> c >> id >> c >> value >> c;
	getline(is, flags, ')');

	if (!is.fail() && !is.bad() && value.IsValid())
	{ // GetVarRefById get reference to owner to beable to update readonly vars
		TVariable& var = ((&list) ? GetVariableById(id, list) : (TVariable&) GetVariableById(id));
		var.UpdateValue(value, skip_self);
		var.UpdateFlags(StringToFlags(flags.c_str()), skip_self);
		return true;
	}
	return false;
}

bool TVariable::Write(std::ostream& os) const
{
	os << '(' << FRef->Id << ',' << FRef->CurVal << ')';
	// check if out stream is valid
	return !os.fail() && !os.bad();
}

bool TVariable::Read(std::istream& is, bool skip_self, TPtrVector& list)
{
	TValue value;
	id_type id = 0;
	char c = 0;
	is >> c >> id >> c >> value >> c;
	_COND_NORM_NOTIFY(!value.IsValid(), DO_DEFAULT, "TVariable: Stream format Corrupt!");
	if (!is.fail() && !is.bad() && value.IsValid())
	{
		// Check list for a NULL_REF and use
		TVariable& var((&list) ? GetVariableById(id, list) : (TVariable&) GetVariableById(id));
		var.UpdateValue(value, skip_self);
		return true;
	}
	return false;
}

bool TVariable::Create(std::istream& is, TVariable::TPtrVector& list, bool global, int& line)
{
	bool retval = true;
	// Variable keeps track of the lines read.
	if (&line)
	{
		line = 1;
	}
	while (is.good() && retval)
	{
		std::string st;
		// Get line with max length of 1024
		getline(is, st);
		// Test ref pointer before increasing line count.
		if (&line)
		{
			line++;
		}
		// Check length and ignore lines starting with character ';'
		if (st.length() > 10 && st[0] != ';')
		{
			TVariable* var = new TVariable();
			var->SetGlobal(global);
			var->Setup(st);
			if (var->IsValid())
			{
				if (&list)
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
				retval = false;
			}
		}
	} // while
	return retval;
}

std::string TVariable::GetSetupString() const
{
	const char sep = ',';
	//  vfeVARID,
	std::string retval = stringf("0x%lX", GetId());
	retval += sep;
	//  vfeNAME,
	retval += GetName() + sep;
	//  vfeUNIT,
	retval += FRef->Unit + sep;
	//  vfeFLAGS,
	retval += GetFlagsString() + sep;
	//  vfeDESCR,
	retval += unescape(GetDescr()) + sep;
	//  vfeTYPE,
	const char* tmp = GetType(GetType());
	retval += tmp;
	retval += sep;
	//  vfeCNV,
	retval += GetConvertOption() + sep;
	//  vfeRND,
	retval += FRef->RndVal.GetString() + sep;
	//  vfeDEF,
	retval += FRef->DefVal.GetString() + sep;
	//  vfeMIN,
	retval += FRef->MinVal.GetString() + sep;
	//  vfeMAX,
	retval += FRef->MaxVal.GetString();
	//  vfeFIRSTSTATE
	const TStateArray& sa(FRef->StateArray);
	if (sa.Count())
	{
		for (uint i = 0; i < sa.Count(); i++)
		{
			retval += sep;
			retval += sa[i].Name + '=' + sa[i].Value.GetString();
		}
	}
	return retval;
}

void TVariable::ClipRound(TValue& value) const
{ // Can only clip numeric values.
	if (IsNumber())
	{
		// Check which balues nust be used.
		if (IsConverted())
		{
			// Check if limits must be checked by comparing a difference.
			if (FRef->CnvMaxVal != FRef->CnvMinVal)
			{
				// Adjust new value to the allowed range.
				value = (value > FRef->CnvMaxVal) ? FRef->CnvMaxVal : value;
				value = (value < FRef->CnvMinVal) ? FRef->CnvMinVal : value;
			}
			// Check if new value must be rounded
			if (!FRef->RndVal.IsZero())
			{
				value.Round(FRef->RndVal);
			}
		}
		else
		{
			// Check if limits must be checked by comparing a difference.
			if (FRef->MaxVal != FRef->MinVal)
			{
				// Adjust new value to the allowed range.
				value = (value > FRef->MaxVal) ? FRef->MaxVal : value;
				value = (value < FRef->MinVal) ? FRef->MinVal : value;
			}
			// Check if new value must be rounded
			if (!FRef->RndVal.IsZero())
			{
				value.Round(FRef->CnvRndVal);
			}
		}
	}
};

std::string TVariable::GetCurString(bool use_states) const
{
	switch (FRef->Type)
	{
		default:
		case TValue::vitBINARY:
		case TValue::vitCUSTOM:
		case TValue::vitSTRING:
		{
			if (FTemporary)
			{
				return FTemporary->GetString();
			}
			else
			{
				return FRef->CurVal.GetString();
			}
		}

		case TValue::vitINTEGER:
		{ // there are states return the state name if 'states' is true
			// if not run in to default switch
			unsigned count = FRef->StateArray.Count();
			if (use_states && count)
			{
				// Find the state of the current value.
				for (unsigned i = 0; i < count; i++)
				{
					if (FRef->StateArray[i].Value == (FTemporary ? (*FTemporary) : FRef->CurVal))
					{
						return FRef->StateArray[i].Name;
					}
				}
			}
			// run into next switch is state was not found
		}

		case TValue::vitFLOAT:
		{
			// When a temporary is used then it is already converted or not.
			if (IsConverted())
			{
				return (FTemporary ? (*FTemporary) : FRef->CnvCurVal).GetString(FRef->CnvSigDigits);
			}
			else
			{
				return (FTemporary ? (*FTemporary) : FRef->CurVal).GetString(FRef->SigDigits);
			}
		}
	} // switch
}

bool TVariable::SetConvertValues
	(
		const std::string& unit,
		const TValue& mult,
		const TValue& ofs,
		int digits
	)
{
	// Only owners are allowed to set the conversion values.
	if (IsOwner() && FRef->Type == TValue::vitFLOAT)
	{
		// Check if something has changed.
		if (FRef->CnvMult == mult && FRef->CnvOffset == ofs && FRef->CnvUnit == unit)
		{
			// Check if the siginificant dgits are to be calculated here or passed here.
			if (!(digits != INT_MAX && FRef->CnvSigDigits != digits))
			{
				return true;
			}
		}
		// Correct the type of the passed values.
		FRef->CnvMult = mult;
		FRef->CnvOffset = ofs;
		// Correct the type of the passed values.
		FRef->CnvMult.SetType(FRef->Type);
		FRef->CnvOffset.SetType(FRef->Type);
		// Set the static conversion values.
		FRef->CnvCurVal = FRef->CurVal * FRef->CnvMult + FRef->CnvOffset;
		FRef->CnvDefVal = FRef->DefVal * FRef->CnvMult + FRef->CnvOffset;
		FRef->CnvDefVal = FRef->DefVal * FRef->CnvMult + FRef->CnvOffset;
		FRef->CnvMinVal = FRef->MinVal * FRef->CnvMult + FRef->CnvOffset;
		FRef->CnvMaxVal = FRef->MaxVal * FRef->CnvMult + FRef->CnvOffset;
		// Determine the conversion round value.
		FRef->CnvRndVal = FRef->RndVal * FRef->CnvMult;
		// Calculate siginificant digits here.
		if (digits == INT_MAX)
		{
			// When the digits number is INT_MAX round was zero to start with.
			if (FRef->SigDigits != INT_MAX)
			{
				// Adjust the significant digits value with the magnitude of the multiplication factor.
				FRef->CnvSigDigits = FRef->SigDigits + magnitude(FRef->CnvMult.GetFloat()) + 1;
			}
			else
			{
				FRef->CnvSigDigits = INT_MAX;
			}
		}
		else
		{
			FRef->CnvSigDigits = digits;
		}
		// If the unit length is non zero it allways generates an event.
		// Only when both length values are zero there is no event generated.
		if (unit.length() || FRef->CnvUnit.length())
		{
			// Assigning the name will enable the conversion.
			FRef->CnvUnit = unit;
			// Signal the clients.
			LocalEvent(veCONVERTED, false);
		}
		return true;
	}
	return false;
}

void TVariable::SetConvertLink(TVariableLink* link)
{
	if (FConvertLink != link)
	{
		FConvertLink = link;
	}
}

bool TVariable::SetConvertValues(bool convert)
{
	// Only owners are allowed to set the conversion values.
	// Check if the type is a floating point value that can be converted.
	if (IsOwner() && FRef->Type == TValue::vitFLOAT && FRef->Unit.length())
	{
		double mult = 1;
		double ofs = 0;
		if (convert)
		{
			// If the global covert link has been set use it.
			if (FConvertLink)
			{ // Call the handler setting the linker to the zero variable to
				// indicate the global nature of the call.
				FConvertLink->VariableEventHandler(veCONVERT, *this, ZeroVariable, false);
			}
			else
			{
				int sig = FRef->SigDigits;
				std::string newunit = FRef->Unit;
				// Check if a conversion is found.
				if (GetUnitConversion(FRef->CnvOption, newunit, sig, mult, ofs, newunit, sig))
				{
					// Set the convert values.
					return SetConvertValues(newunit.c_str(), TValue(mult), TValue(ofs), sig);
				}
			}
		}
		else
		{ // Check if a conversion value must be disabled.
			if (FRef->CnvUnit.length())
			{ // Set the convert values to the original.
				return SetConvertValues("", TValue(mult), TValue(ofs));
			}
			// Was al ready
			return true;
		}
	}
	// Signal failure.
	return false;
}

bool TVariable::SetConvert(bool enable)
{
	// Only when there is a change.
	if (FConverted != enable)
	{ // Make the change.
		FConverted = enable;
		// Signal only if a conversion exists.
		if (FRef->CnvUnit.length())
		{
			// Signal this event to this single instance.
			NotifyVariable(veCONVERTED, *this);
		}
	}
	//
	return IsConverted();
}

TValue TVariable::Convert(const TValue& value, bool to_org) const
{
	TValue retval = value;
	// Check if a conversion must be perfomed.
	if (FRef->CnvUnit.length() && GetType() == TValue::vitFLOAT)
	{
		// Abort the operation when conversion fails.
		if (!retval.SetType(TValue::vitFLOAT))
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Convert" << retval << " for passed value failed!");
			return value;
		}
		if (to_org)
		{
			retval -= FRef->CnvOffset;
			retval /= FRef->CnvMult;
		}
		else
		{
			retval *= FRef->CnvMult;
			retval += FRef->CnvOffset;
		}
	}
	return retval;
}

void TVariable::SetTemporary(bool flag)
{
	// Check if there is a change.
	if (flag != (FTemporary != nullptr))
	{
		if (flag)
		{
			// Create the temporary value and set the flag pointer as wel.
			FTemporary = new TValue();
			// Update the temporary value and signal this instance.
			UpdateTemporary(false);
		}
		else
		{
			// Only signal when there is a change between the temporary and the real value.
			if (*FTemporary != (IsConverted() ? FRef->CnvCurVal : FRef->CurVal))
			{
				// Notify the variable of the change.
				NotifyVariable(veVALUECHANGE, *this);
			}

			// Delete and disable the temporary value.
			delete_null(FTemporary);
		}
	}
}

bool TVariable::ApplyTemporary(bool skipself)
{
	// Check if the temporary value is used.
	if (FTemporary)
	{
		// If this instance uses the converted value it must
		// be converted first to the real value.
		if (IsConverted())
		{
			return UpdateValue(Convert(*FTemporary, true), skipself);
		}
		// Update using the straight value when not converted.
		return UpdateValue(*FTemporary, skipself);
	}
	return false;
}

bool TVariable::TemporaryDifs() const
{
	// Check if the temporary value is used.
	if (FTemporary)
	{
		// Only when there is a change.
		TValue* curval = IsConverted() ? &FRef->CnvCurVal : &FRef->CurVal;
		bool retval = *FTemporary != *curval;
		return retval;
	}
	return false;
}

bool TVariable::UpdateTemporary(bool skipself)
{
	// Check if the temporary value is used.
	if (FTemporary)
	{
		// Only when there is a change.
		if (*FTemporary != (IsConverted() ? FRef->CnvCurVal : FRef->CurVal))
		{
			// Update the temporary value.
			*FTemporary = (IsConverted() ? FRef->CnvCurVal : FRef->CurVal);
			// Notify the variable when skipself is false.
			if (!skipself)
			{
				NotifyVariable(veVALUECHANGE, *this);
			}
			// Variable temporary value was changed.
			return true;
		}
	}
	// Variable temporary value was not changed.
	return false;
}

std::ostream& operator<<(std::ostream& os, const TVariable& v)
{
	return os << v.GetSetupString() << '\n';
}

std::istream& operator>>(std::istream& is, TVariable& v)
{
	std::string st;
	// get one line
	getline(is, st);
	// check stream on errors
	int retval = (!is.fail() && !is.bad());
	// check stream state length and ignore lines starting with character ';'
	if (!retval && st.length() > 10 && st[0] != ';')
	{ // setup this variable with the read line from the input stream
		v.Setup(st);
	}
	// return is input stream
	return is;
}

/**
 * Implementation of undocumented debugging functions
 */
const char* TVariableBase::GetEventName(EEvent event)
{
	const char* retval = "Unknown";
	switch (event)
	{
		case veNEWID:
			retval = "NEWID";
			break;

		case veCONVERTED:
			retval = "CONVERTED";
			break;

		case veFLAGSCHANGE:
			retval = "FLAGSCHANGE";
			break;

		case veIDCHANGED:
			retval = "IDCHANGED";
			break;

		case veREMOVE:
			retval = "REMOVE";
			break;

		case veGETOWNER:
			retval = "GETOWNER";
			break;

		case veLOSTOWNER:
			retval = "LOSTOWNER";
			break;

		case veLINKED:
			retval = "LINKED";
			break;

		case veUNLINKED:
			retval = "UNLINKED";
			break;

		case veSETUP:
			retval = "SETUP";
			break;

		case veVALUECHANGE:
			retval = "VALUECHANGE";
			break;

		case veINVALID:
			retval = "INVALID";
			break;

		default:
			// Check for global event.
			if (event < 0)
			{
				retval = "USERGLOBAL";
				break;
			}
			else
			{
				if (event >= 0 && event < veFIRSTPRIVATE)
				{
					retval = "USERLOCAL";
				}
				else
				{
					if (event >= veUSERPRIVATE)
					{
						retval = "USERPRIVATE";
					}
				}
			}
	}
	return retval;
}

TVariable::EStringType TVariable::GetStringType() const
{
	// Check for string type kind of variable
	if (FRef->Type == TValue::vitSTRING)
	{
		size_t pos = FRef->Unit.find_first_of("SNMPDF");
		if (pos != std::string::npos)
		{
			switch (FRef->Unit[pos])
			{
				case 'N':
					return stNORMAL;
				case 'M':
					return stMULTI;
				case 'P':
					return stPATH;
				case 'D':
					return stDIRECTORY;
				case 'S':
					return stSUBDIRECTORY;
				case 'F':
					return stFILENAME;
			}
		}
	}
	// By default return the normal string type.
	return stNORMAL;
}

const char* TVariable::GetStringType(TVariable::EStringType st)
{
	switch (st)
	{
		case stNORMAL:
			return "Normal";
		case stMULTI:
			return "Multi";
		case stPATH:
			return "Path";
		case stDIRECTORY:
			return "Dir";
		case stSUBDIRECTORY:
			return "Subdir";
		case stFILENAME:
			return "File";
		default:
			break;
	};
	return "Unknown";
}

std::string TVariable::GetConvertOption() const
{
	// Only the float type can have a convert option.
	if (FRef->Type == TValue::vitFLOAT)
	{
		return FRef->CnvOption;
	}
	return "";
}

std::string TVariable::GetUnit() const
{
	// Check for string type kind of variable
	if (FRef->Type == TValue::vitSTRING)
	{
		return GetStringType(GetStringType());
	}
	else
	{
		return IsConverted() ? FRef->CnvUnit : FRef->Unit;
	}
}

std::string TVariable::GetUnit(bool converted) const
{
	// Check for string type kind of variable
	if (FRef->Type == TValue::vitSTRING)
	{
		return GetStringType(GetStringType());
	}
	else
	{
		return (converted && FRef->CnvUnit.length()) ? FRef->CnvUnit : FRef->Unit;
	}
}

}
