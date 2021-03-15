#pragma once

#include "misc/gen/TVector.h"
#include "InformationBase.h"
#include "ResultDataStorage.h"
#include "../global.h"

namespace sf
{

/**
 * Forward declaration.
 */
class ResultDataReference;

/**
 * Forward declaration.
 */
class ResultData;

/**
 * This base class contains all local types of used in the TVariable class.
 * Multiple inheritance makes these types local for other classes as well.
 * Making code more readable and also prevents name space problems.
 */
class _GII_CLASS ResultDataTypes :public InformationTypes
{
	public:
		/**
		 * Vector for pointer to results.
		 */
		typedef TVector<ResultData*> Vector;
		/**
		 * Type for internal use.
		 */
		typedef TVector<ResultDataReference*> ReferenceVector;

		/**
		 * For broadcast events the result parameter is not the result
		 * who set the link with function 'setHandler()' and the param parameter
		 * isn't also the one passed to function 'setHandler()' either
		 * Global events have a negative value.
		 */
		enum EEvent :int
		{
			/** New ID was created.*/
			reNewId = -32000,
			/** First user global.*/
			reUserGlobal,
			/** Indicates first local event.*/
			reFirstLocal = 0,
			/** Flags have been changed.*/
			reFlagsChange,
			/** Change in accessible range of this instance.*/
			reAccessChange,
			/** A range has been committed of this instance.*/
			reCommitted,
			/** Change of reserved blocks (always incremental).*/
			reReserve,
			/** Result reference is invalid after this event.*/
			reInvalid,
			/** current data will be cleared after this event.*/
			reClear,
			/** First user local event.*/
			reUserLocal,
			// Events from here are only generated for results.
			/** Indicates first private event.*/
			reFirstPrivate = 16000,
			/** The result instance's ID changed.*/
			reIdChanged,
			/** This result instance desired ID changed.*/
			reDesiredId,
			/** Result instance is being removed.*/
			reRemove,
			/** Result instance has become owner.*/
			reGetOwner,
			/** Result instance has lost ownership.*/
			reLostOwner,
			/** Result instance has been linked.*/
			reLinked,
			/** Result instance has lost link.*/
			reUnlinked,
			/** Clients receive this event when a request is answered.*/
			reGotRange,
			/** The owner must get the data specified by the passed range.*/
			reGetRange,
			/**  First user private event.*/
			reUserPrivate
		};
		/**
		 * This enumerate is the order of fields in the setup string
		 */
		enum EField :int
		{
			rfeId = 0,
			rfeName,
			rfeFlags,
			rfeDescription,
			rfeType,
			rfeBlockSize,
			rfeSegmentSize,
			/** Significant bits for this instance this can not be larger then the bit count of the type.*/
			rfeSigBits,
			/** Negative offset of values.
			 * To get the real value clients must subtract this value from the read unsigned value.*/
			rfeOffset,
			rfeLastEntry
		};
		/**
		 * This enumerate is used to identify the type of data.
		 */
		enum EType :int
		{
			rtInvalid = 0,
			rtString,
			rtInt8,
			rtInt16,
			rtInt32,
			rtInt64,
			rtLastEntry
		};
		/**
		 * Flags of the flags description field.
		 */
		enum EFlag :flags_type
		{
			/** Represented by character 'R' only the owner can change set it. */
			flgRecycle = 1 << 0,
			/** Represented by character 'A'  for clients to select instance for archiving data. */
			flgArchive = 1 << 1,
			/** Represented by character 'S' this data may be used by clients*/
			flgShare = 1 << 2,
			/** Represented by character 'H' for selection of client only when listing.*/
			flgHidden = 1 << 3
		};
		/**
		 * Enumerate for range information bit values
		 */
		enum ERangeInfo
		{
			/** Range is available.*/
			riAvailable = 1 << 0,
			/** Range is fully accessible.*/
			riAccessible = 1 << 1,
			/** Range is partial accessible.*/
			riPartialAccessible = 1 << 2,
			/** Range is already requested and the transaction id is returned in tranid*/
			riRequested = 1 << 3,
			/** Range is addressable.*/
			riAddressable = 1 << 5,
			/** Range is spread over multiple segments.*/
			riSegmented = 1 << 4,
		};

		/**
		 * Structure used to setup a result.
		 */
		struct Definition
		{
			/**
			 * ID of the new global instance.
			 */
			id_type _id{0};
			/**
			 * Name path separated using '|' characters.
			 */
			std::string _name;
			/**
			 * Combination of EFlag flags.
			 */
			flags_type _flags{0};
			/**
			 * Description of the instance without comma's.
			 */
			std::string _description;
			/**
			 * Type of a single the element.
			 */
			EType _type{rtInvalid};
			/**
			 * Amount of elements in a block.
			 */
			size_type _blockSize{0};
			/**
			 * Amount of segments in a block.
			 */
			size_type _segmentSize{0};
			/**
			 * Amount of significant bits in an element.
			 */
			int _significantBits{0};
			/**
			 * Offset to produce a negative value or better set the nul value.
			 * Needed to produce an element value to calculate with.
			 */
			size_type _offset{0};
		};
};

} // namespace
