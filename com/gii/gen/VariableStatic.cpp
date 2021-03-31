#include <misc/gen/dbgutils.h>

#include "VariableStatic.h"
#include "VariableReference.h"
#include "Variable.h"

namespace sf
{

VariableHandler* VariableStatic::_convertHandler = nullptr;

VariableTypes::ReferenceVector VariableStatic::_references;

TVector<void*> VariableStatic::_deleteWaitCache;

int VariableStatic::_globalActive = 0;

Variable* VariableStatic::_zero = nullptr;

Variable& VariableStatic::zero()
{
	if (!VariableStatic::_zero)
	{
		_NORM_THROW("VariableStatic::" << __FUNCTION__ << "() Zero variable is NOT present yet or NOT anymore!")
	}
	return *_zero;
}

void sf::VariableStatic::initialize(bool init)
{
	if (init)
	{
		if (!VariableStatic::_zero)
		{
			// Call special private constructor to create the Zero variable.
			new Variable(nullptr, nullptr);
			_COND_NORM_THROW(!VariableStatic::_zero,
				"VariableStatic::" << __FUNCTION__ << "() Zero should have been set by now!")
		}
	}
	else
	{
		// Get the amount of references that still exist.
		auto sz = _references.size();
		// Prevent deletion of zero before all other instances.
		if (sz > 1)
		{
			std::stringstream os;
			// Skip the first one which is zero variable.
			for (ReferenceVector::size_type i = 1; i < sz; i++)
			{
				auto k = _references.at(i);
				os << "(0x" << std::hex << k->_id << ") '" << k->_name << (i != sz - 1 ? "', " : "' ");
			}
			_NORM_NOTIFY(DO_CERR, "VariableStatic::" << __FUNCTION__ << "(false) Unable to perform, ("
				<< (sz - 1) << ") references still remain!" << std::endl << '\t' << os.str())
		}
		else
		{
			delete VariableStatic::_zero;
			_COND_NORM_THROW(VariableStatic::_zero,
				__FUNCTION__ << "VariableStatic: Zero variable should have been removed by now!")
		}
	}
}

VariableStatic::FlagLetters VariableStatic::_flagLetters[] =
	{
		{'R', flgReadonly},
		{'A', flgArchive},
		{'S', flgShare},
		{'L', flgLink},
		{'F', flgFunction},
		{'P', flgParameter},
		{'H', flgHidden},
		{'E', flgExport},
		{'W', flgWriteable},
	};

}

