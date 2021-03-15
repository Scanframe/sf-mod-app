#include <misc/gen/dbgutils.h>
#include "ResultDataStatic.h"
#include "ResultData.h"

namespace sf
{

ResultData* ResultDataStatic::_zero = nullptr;

ResultDataTypes::size_type ResultDataStatic::DebugSegSize = 0;

ResultDataTypes::size_type ResultDataStatic::RecycleSize = 2;

/**
 * Array used for conversion. Follows enumerate EType
 */
const ResultDataStatic::TTypeInfo ResultDataStatic::TypeInfoArray[] =
	{
		{"INVALID", 1}, // rtInvalid = 0,
		{"STRING", 1},  // rtString
		{"INT8", 1},    // rtInt8
		{"INT16", 2},   // rtInt16
		{"INT32", 4},   // rtInt32
		{"INT64", 8},   // rtInt64
		{nullptr, 0}    // rtLastEntry
	};

ResultDataStatic::ReferenceVector* ResultDataStatic::RefList = nullptr;

TVector<void*>* ResultDataStatic::DeleteWaitCache = nullptr;

int ResultDataStatic::GlobalActive = 0;

ResultDataTypes::size_type ResultDataStatic::SeqIdCounter = 0;

ResultData& ResultDataStatic::zero()
{
	_COND_NORM_THROW(!ResultDataStatic::_zero, "ZeroResultData has NOT been created yet! Library initialisation error!")
	return *_zero;
}

void sf::ResultDataStatic::initialize(bool init)
{
	static ResultData* zero(nullptr);
	if (init)
	{
		if (!zero)
		{
			// Call special private constructor to create the Zero variable.
			zero = new ResultData(nullptr, nullptr);
			_COND_NORM_THROW(_zero != zero, "VariableStatic: Zero should have been Set by now!")
		}
	}
	else
	{
		delete zero;
	}
}


}