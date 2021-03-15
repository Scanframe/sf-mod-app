#pragma once

#include "ResultDataTypes.h"
#include "misc/gen/RangeManager.h"

namespace sf
{
/**
 * This structure is referenced by class TResultData.
 * Several TResultData instances can reference to one TResultDataReference
 * instance. Only one TResultDataReference can exist per ID.
 */
struct ResultDataReference :ResultDataTypes
{
	/**
	 * Default constructor.
	 */
	ResultDataReference();

	/**
	 * Destructor
	 */
	~ResultDataReference();

	/**
	 * Contains the ID of the result data.
	 */
	long _id{0};
	/**
	 * Contains the sequential ID of the result data.
	 */
	long _sequenceId{0};
	/**
	 * Flag if this instance is valid.
	 */
	bool _valid{false};
	/**
	 * Contains the type of data of the result.
	 */
	EType _type;
	/**
	 * Contains the correction offset for each value to get the real value.
	 */
	size_type Offset;
	/**
	 * Contains the type of data of the result.
	 */
	unsigned SignificantBits;
	/**
	 * Member that contains the initial flags of the setup.
	 */
	flags_type _flags;
	/**
	 * Member that contains the current flags of the setup.
	 */
	flags_type _curFlags;
	/**
	 * Contains the name string of the result data.
	 */
	std::string _name;
	/**
	 * Contains the description string of the result data.
	 */
	std::string _description;
	/**
	 * Container for segments of data.
	 */
	ResultDataStorage* _data;
	/**
	 * Range manager for accessible entries.
	 */
	RangeManager* _rangeManager;
	/**
	 * This Vector of ranges contains ranges that are validated
	 * by writing to the memory by the owner of the result.
	 */
	Range::Vector _validatedCache;
	/**
	 * List of result data instances attached to this reference.
	 */
	Vector _list;
};

}
