#pragma once

#include <misc/gen/TVector.h>
#include "InformationBase.h"
#include "FileMappedStorage.h"

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
 * @brief This base class contains all local types of used in the #sf::ResultData class.<br>
 * Multiple inheritance makes these types local for other classes as well.<br>
 * Making code more readable and also prevents name space problems.<br>
 */
class _GII_CLASS ResultDataTypes :public InformationTypes
{
	public:
		/**
		 * @brief Vector for pointer to results.
		 */
		typedef TVector<ResultData*> PtrVector;

		/**
		 * @brief Vector for instances of results.
		 */
		typedef TVector<ResultData> Vector;

		/**
		 * @brief Type for internal use.
		 */
		typedef TVector<ResultDataReference*> ReferenceVector;

		/**
		 * @brief Event enumerate values used in broadcasting where global events have a negative value.
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
			/** Current data will be cleared after this event.*/
			reClear,
			/** First user local event.*/
			reUserLocal,
			// Events from here are only generated for results.
			/** Indicates first private event.*/
			reFirstPrivate = 16000,
			/** This instance was setup.*/
			reSetup,
			/** The instance's ID changed.*/
			reIdChanged,
			/** This instance desired ID changed.*/
			reDesiredId,
			/** Instance is being removed.*/
			reRemove,
			/** Instance has become owner.*/
			reGetOwner,
			/** Instance has lost ownership.*/
			reLostOwner,
			/** Instance has been linked.*/
			reLinked,
			/** Instance has lost link.*/
			reUnlinked,
			/** Clients receive this event when a request is answered.*/
			reGotRange,
			/** The owner must get the data specified by the passed range.*/
			reGetRange,
			/**  First user private event.*/
			reUserPrivate
		};
		/**
		 * @brief This enumerate is the order of fields in the setup string
		 */
		enum EField :int
		{
			/** Identifier field.*/
			rfId = 0,
			/** Name field.*/
			rfName,
			/** Flags field.*/
			rfFlags,
			/** description field.*/
			rfDescription,
			/** Type string field @see #EType */
			rfType,
			/** Size of the block in amount of types.*/
			rfBlockSize,
			/** Size of segment in blocks.*/
			rfSegmentSize,
			/** Significant bits for this instance this can not be larger then the bit count of the type.*/
			rfSigBits,
			/** Negative offset of values.
			 * To get the real value clients must subtract this value from the read unsigned value.*/
			rfOffset
		};
		/**
		 * @brief This enumerate is used to identify the type of data.
		 */
		enum EType :int
		{
			/** Invalid type indicating a faulty setup string represented by  'INVALID'.*/
			rtInvalid = 0,
			/** Type containing a text represented by  'STRING'.*/
			rtString,
			/** 8 bit integers represented by  'INT8'.*/
			rtInt8,
			/** 16 bit integers represented by  'INT16'.*/
			rtInt16,
			/** 32 bit integers represented by  'INT32'.*/
			rtInt32,
			/** 64 bit integers represented by  'INT64'.*/
			rtInt64,
			/** Used for iterations.*/
			rtLastEntry
		};
		/**
		 * @brief Flags of the flags description field.
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
		 * @brief Enumerate for range information bit values
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
		 * @brief Structure used to setup a result reference.
		 */
		struct Definition
		{
			/**
			 * @brief Flag indicating this structure is valid.
			 */
			bool _valid{false};
			/**
			 * @brief ID of the new global instance.
			 */
			id_type _id{0};
			/**
			 * @brief Name path separated using '|' characters.
			 */
			std::string _name;
			/**
			 * @brief Combination of EFlag flags.
			 */
			flags_type _flags{0};
			/**
			 * @brief Description of the instance without comma's.
			 */
			std::string _description;
			/**
			 * @brief Type of a single the element.
			 */
			EType _type{rtInvalid};
			/**
			 * @brief Amount of elements in a block.
			 */
			size_type _blockSize{0};
			/**
			 * @brief Amount of segments in a block.
			 */
			size_type _segmentSize{0};
			/**
			 * @brief Amount of significant bits in an element.
			 */
			size_t _significantBits{0};
			/**
			 * @brief Offset to produce a negative value or better set the nul value base line.
			 * Needed to produce an element value to calculate with.
			 */
			size_type _offset{0};
		};
};

}
