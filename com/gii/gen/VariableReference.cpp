#include <misc/gen/dbgutils.h>
#include "VariableReference.h"
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
	// Add to static reference list declared in VariableStatic
	VariableStatic::_references.add(this);
}

void VariableReference::copy(const VariableReference& ref)
{
	_COND_RTTI_NOTIFY(this == VariableStatic::zero()._reference, DO_CLOG, "Cannot assign a ref to Zero Ref!")
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
	// If Variables are still referenced these must be removed first except if it is ZeroVariable itself.
	if (VariableStatic::zero()._reference == this)
	{
		// Check if there are still variables in the system when zero variable is destructed
		size_t total_count = 0;
		VariableStatic::ReferenceVector::iter_type i(VariableStatic::_references);
		// Iterate through all variable references and count the usage count
		while (i)
		{
			VariableReference* ref = i++;
			total_count += ref->_list.count();
			// Notification of warning
			// Check if current ref is the ZeroVariable reference.
			if (ref == VariableStatic::zero()._reference)
			{
				// Iterate through each variable entry in the list.
				size_type count = ref->_list.count();
				// Skip the first one because there is always Zero Variable itself.
				for (size_type idx = 1; idx < count; idx++)
				{
					if (ref->_list[idx])
					{
						// Notification of warning
						_RTTI_NOTIFY(DO_CLOG, "Dangling instance [" << idx << "/" << count << "] with desired ID 0x" << std::hex
							<< ref->_list[idx]->_desiredId << " in this process!")
					}
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
		size_type count = _list.count();
		while (count--)
		{
			// Attach variable to Zero which is the default and the invalid one
			if (_list[count])
			{
				_list[count]->setup(0L, false);
			}
		}
	}
	// Remove from reference from the vector.
	VariableStatic::_references.detach(this);
}

}
