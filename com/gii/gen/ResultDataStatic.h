#pragma once

#include "ResultDataTypes.h"

namespace sf
{

class _GII_CLASS ResultDataStatic :public ResultDataTypes
{
	public:
		/**
		 * @brief Result that always exists in the system and is referenced if no other is available or applicable.
		 */
		static ResultData& zero();

		/**
		 * @brief Called to initialize or deinitialize the variable system.
		 */
		static void initialize(bool);
		/**
		 * @brief Gets the enumerate type from the passed type string.
		 */
		static EType getType(const char* type);

	private:
		/**
		 * @brief Holds the first created instance and place holder with id zero.
		 */
		static ResultData* _zero;

		/**
		 * @brief Structure for storing information about a type.
		 */
		struct TTypeInfo
		{
			const char* Name;
			long Size;
		};
		/**
		 * @brief Static list to hold all references.
		 */
		static ReferenceVector _references;
		/**
		 * @brief Array that holds information of the different types
		 */
		static const TTypeInfo _typeInfoArray[];
		/**
		 * @brief Static this counter is increased if a global event is generated.
		 *
		 * It is decreased when it returns from the event handler.
		 */
		static int _globalActive;
		/**
		 * @brief Each result data reference that is created with get an 'SeqId' assigned
		 * which is the incremented value of 'SeqIdCounter'
		 */
		static size_type _sequenceIdCounter;
		/**
		 * @brief Keeps the allocated data in this list if it may not be deleted yet
		 * because it is obstructed by an event being handled.
		 */
		static TVector<void*> _deleteWaitCache;
		/**
		 * @brief Amount of segments to recycle for each result data instance.
		 */
		static size_type _recycleSize;
		/**
		 * @brief Debugging value for the segment size.
		 */
		static size_type _debugSegmentSize;

		/**
		 * @brief Lookup list for flags.
		 */
		struct FlagLetters
		{
			char _letter;
			int _flag;
		} static _flagLetters[9];

		friend class ResultData;

		friend class ResultDataReference;
};

}
