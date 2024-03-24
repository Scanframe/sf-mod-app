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
	VariableStatic::_references->add(this);
}

void VariableReference::copy(const VariableReference& ref)
{
	SF_COND_RTTI_NOTIFY(this == VariableStatic::zero()._reference, DO_CLOG, "Cannot assign a ref to Zero Ref!")
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
	// If Variables are still referenced these must be removed first except if it is Zero instance itself.
	if (VariableStatic::zero()._reference == this)
	{
		// Qt Designer has something weird going on. Memory seems to be garbled.
		if (!VariableStatic::_references->empty() && VariableStatic::_references->at(0) != this)
		{
			// Notification of warning
			SF_RTTI_NOTIFY(DO_MSGBOX | DO_DEFAULT, "Unexpected Zero pointer not as first in list!");
		}
		else
		{
			// Check if there are still instances in the system when Zero instance is destructed
			size_t total_count = 0;
			// Iterate through all references and count the usage count
			for (auto ref: *VariableStatic::_references)
			{
				total_count += ref->_list.size();
				// Notification of warning
				// Check if current ref is the Zero instance reference.
				if (ref == VariableStatic::zero()._reference)
				{
					// Iterate through each entry in the list.
					size_type count = ref->_list.count();
					// Skip the first one because there is always Zero instance itself.
					for (size_type idx = 1; idx < count; idx++)
					{
						if (ref->_list[idx])
						{
							// Notification of warning
							SF_RTTI_NOTIFY(DO_CLOG, "Dangling instance [" << idx << "/" << count << "] with desired ID 0x"
								<< std::hex << ref->_list[idx]->_desiredId << " in this process!")
						}
						// Limit the amount shown to a maximum of 3.
						if (idx >= 3)
						{
							SF_RTTI_NOTIFY(DO_MSGBOX | DO_DEFAULT, ": Too many [" << count << "] dangling instances to be shown!")
							break;
						}
					}
				}
				else
				{
					SF_RTTI_NOTIFY(DO_MSGBOX | DO_DEFAULT,
						": Dangling reference owning instance with ID 0x" << std::hex << ref->_id << " in this process!")
				}
			}
			// Subtract 1 for zero instance itself. There should only be one left in the list.
			total_count--;
			// Notification of warning
			SF_COND_RTTI_NOTIFY(total_count, DO_MSGBOX | DO_DEFAULT, "Total of " << total_count << " dangling instances in this process!")
		}
	}
	else
	{
		size_type count = _list.size();
		while (count--)
		{
			// Attach instance to Zero which is the default and the invalid one.
			if (_list[count])
			{
				_list[count]->setup(0L, false);
			}
		}
	}
	// Remove from reference from the vector.
	VariableStatic::_references->detach(this);
}

}
