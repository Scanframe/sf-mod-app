#include <misc/gen/dbgutils.h>
#include "ResultDataStatic.h"
#include "ResultDataReference.h"
#include "ResultData.h"

namespace sf
{

ResultData* ResultDataStatic::_zero{nullptr};

ResultData::ReferenceVector* ResultDataStatic::_references{nullptr};

TVector<void*> ResultDataStatic::_deleteWaitCache;

ResultDataTypes::size_type ResultDataStatic::_debugSegmentSize{0};

ResultDataTypes::size_type ResultDataStatic::_recycleSize{2};

/**
 * Array used for conversion.
 * Follows enumerate EType
 */
const ResultDataStatic::TTypeInfo ResultDataStatic::_typeInfoArray[] =
	{
		{"INVALID", 1}, // rtInvalid = 0,
		{"STRING", 1},  // rtString
		{"INT8", 1},    // rtInt8
		{"INT16", 2},   // rtInt16
		{"INT32", 4},   // rtInt32
		{"INT64", 8},   // rtInt64
	};

ResultDataStatic::FlagLetters ResultDataStatic::_flagLetters[] =
	{
		{'A', flgArchive},
		{'S', flgShare},
		{'H', flgHidden},
		{'R', flgRecycle}
	};

int ResultDataStatic::_globalActive = 0;

ResultDataTypes::size_type ResultDataStatic::_uniqueIdCounter = 0;

ResultData& ResultDataStatic::zero()
{
	if (!_zero)
	{
		throw notify_exception(SF_RTTI_NAME(_zero).append("::").append(__FUNCTION__)
			.append("() Zero result data is NOT present yet or NOT anymore!"));
	}
	return *_zero;
}

ResultDataStatic::size_type ResultDataStatic::getUniqueId()
{
	return ++ResultDataStatic::_uniqueIdCounter;
}

void sf::ResultDataStatic::initialize(bool init)
{
	if (init)
	{
		if (!ResultDataStatic::_zero)
		{
			// Reset the sequence ID counter.
			ResultDataStatic::_uniqueIdCounter = 0;
			_references = new ResultDataTypes::ReferenceVector;
			// Call special private constructor to create the Zero instance.
			new ResultData(nullptr, nullptr);
		}
	}
	else
	{
		// Variable zero is also the sentry.
		if (ResultDataStatic::_zero)
		{
			// Get the amount of references that still exist.
			auto sz = _references->size();
			// Prevent deletion of zero before all other instances.
			if (sz > 1)
			{
				std::stringstream os;
				// Skip the first one which is zero variable.
				for (ReferenceVector::size_type i = 1; i < sz; i++)
				{
					auto k = _references->at(i);
					os << "(0x" << std::hex << k->_id << ") '" << k->_name << (i != sz - 1 ? "', " : "' ");
				}
				SF_NORM_NOTIFY(DO_CERR, "ResulDataStatic::" << __FUNCTION__ << "(false) Unable to perform, ("
				<< (sz - 1) << ") references still remain!" << std::endl << '\t' << os.str())
			}
			else
			{
				delete ResultDataStatic::_zero;
				// Delete the reference vector.
				delete_null(_references);
			}
		}
	}
}

ResultDataTypes::EType ResultDataStatic::getType(const char* type)
{
	int i = 0;
	for (auto n: _typeInfoArray)
	{
		if (!std::strcmp(type, n.Name))
		{
			return (EType) i;
		}
		i++;
	}
	return rtInvalid;
}

}