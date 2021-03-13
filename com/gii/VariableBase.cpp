#include "VariableBase.h"

namespace sf
{

// Global list of unique variable references.
TVariableTypes::TRefPtrVector TVariableBase::RefList;

// Holds memory blocks occupied by variables which were not able to be
// deleted during a global event.
mcvector<void*> TVariableBase::DeleteWaitCache;

// Holds the nesting count of global events.
int TVariableBase::GlobalActive = 0;

}
