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
	 * @brief Default constructor.
	 */
	ResultDataReference();

	/**
	 * @brief Destructor
	 */
	~ResultDataReference();

	/**
	 * @brief Contains the ID of the result data.
	 */
	id_type _id{0};
	/**
	 * @brief Contains the sequential ID of the result data.
	 */
	id_type _sequenceId{0};
	/**
	 * @brief Flag if this instance is valid.
	 */
	bool _valid{false};
	/**
	 * @brief Contains the type of data of the result.
	 */
	EType _type{rtInvalid};
	/**
	 * @brief Contains the correction offset for each value to get the real value.
	 */
	data_type _offset{0};
	/**
	 * @brief Contains the type of data of the result.
	 */
	size_type _significantBits{0};
	/**
	 * @brief Member that contains the initial flags of the setup.
	 */
	flags_type _flags{0};
	/**
	 * @brief Member that contains the current flags of the setup.
	 */
	flags_type _curFlags{rtInvalid};
	/**
	 * @brief Contains the name string of the result data.
	 */
	std::string _name;
	/**
	 * @brief Contains the description string of the result data.
	 */
	std::string _description;
	/**
	 * @brief Flag for debugging output.
	 */
	bool _debug{false};
	/**
	 * @brief Container for segments of data.
	 */
	FileMappedStorage* _data{nullptr};
	/**
	 * @brief Range manager for accessible entries.
	 */
	RangeManager* _rangeManager{nullptr};
	/**
	 * @brief Vector containing ranges which are validated by the owner.
	 */
	Range::Vector _validatedCache;
	/**
	 * List of result data instances attached to this reference.
	 */
	PtrVector _list;
};

}
