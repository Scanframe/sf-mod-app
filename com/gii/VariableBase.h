#pragma once

#include "VariableTypes.h"

#include "global.h"

namespace sf
{

/**
 * Base class of the TVariable having all global static data members.
 */
class TVariableBase :public TVariableTypes
{
	public:
		/**
		 * Variable that always exists in the system and is referenced if no other is available or applicable.
		 */
		static _GII_DATA TVariable& Zero;

		/**
		 * Returns the event name to make debugging easier.
		 */
		static const char* GetEventName(EEvent event);

	private:
		/**
		 * Static reference list to all references.
		 */
		static TRefPtrVector RefList;
		/**
		 * Static this counter is increased if a global event is generated.
		 * It is decreased when it returns from the event handler.
		 */
		static int GlobalActive;
		/**
		 * Keeps the allocated data in this list if it may not be deleted yet
		 * because it is obstructed by an event being handled.
		 */
		static mcvector<void*> DeleteWaitCache;

		friend class TVariable;

		friend class TVariableReference;
};

}