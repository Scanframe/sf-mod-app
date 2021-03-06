#include <misc/gen/dbgutils.h>

#include "VariableStatic.h"
#include "Variable.h"

namespace sf
{

VariableHandler* VariableStatic::_convertLink = nullptr;

VariableTypes::ReferenceVector VariableStatic::_referenceList;

TVector<void*> VariableStatic::_deleteWaitCache;

int VariableStatic::_globalActive = 0;

Variable* VariableStatic::_zero = nullptr;

Variable& VariableStatic::zero()
{
	_COND_NORM_THROW(!VariableStatic::_zero, "ZeroVariable has NOT been created yet! Library initialisation error!")
	return *_zero;
}

void sf::VariableStatic::initialize(bool init)
{
	static Variable* zero(nullptr);
	if (init)
	{
		if (!zero)
		{
			// Call special private constructor to create the Zero variable.
			zero = new Variable(nullptr, nullptr);
			_COND_NORM_THROW(_zero != zero, "VariableStatic: Zero should have been Set by now!")
		}
	}
	else
	{
		delete zero;
	}
}

}

