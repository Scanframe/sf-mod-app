#pragma once

#include "ResultDataTypes.h"
#include "../global.h"

namespace sf
{

class _GII_CLASS ResultDataStatic :public ResultDataTypes
{
	public:
		/**
		 * Result that always exists in the system and is referenced if no other is available or applicable.
		 */
		static ResultData& zero();

		/**
		 * Called to initialize or deinitialize the variable system.
		 */
		static void initialize(bool);

	private:
		/**
		 * Holds the first created instance and place holder with ID zero.
		 */
		static ResultData* _zero;

		/**
		 * Structure for storing information about a type.
		 */
		struct TTypeInfo
		{
			const char* Name;
			long Size;
		};
		/**
		 * Static list to hold all references.
		 */
		static ReferenceVector* RefList;
		/**
		 * Array that holds information of the different types
		 */
		static const TTypeInfo TypeInfoArray[];
		/**
		 * Static this counter is increased if a global event is generated.
		 * It is decreased when it returns from the event handler.
		 */
		static int GlobalActive;
		/**
		 * Each result data reference that is created with get an 'SeqId' assigned
		 * which is the incremented value of 'SeqIdCounter'
		 */
		static size_type SeqIdCounter;
		/**
		 * Keeps the allocated data in this list if it may not be deleted yet
		 * because it is obstructed by an event being handled.
		 */
		static TVector<void*>* DeleteWaitCache;
		/**
		 * Amount of segments to recycle for each result data instance.
		 */
		static size_type RecycleSize;
		/**
		 * Debugging value for the segment size.
		 */
		static size_type DebugSegSize;

		friend class ResultData;

		friend class ResultDataReference;
};

}
