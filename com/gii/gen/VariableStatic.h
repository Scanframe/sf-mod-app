#pragma once

#include "VariableTypes.h"
#include "VariableHandler.h"
#include "Variable.h"

#include "global.h"

namespace sf
{

/**
 * Base class of the Variable having all global static data members.
 */
class _GII_DATA VariableStatic :public VariableTypes
{
	public:
		/**
		 * Returns Variable instance that always exists in the system and is referenced if no other is available or applicable.
		 */
		static Variable& zero();

		/**
		 * Called to initialize or deinitialize the variable system.
		 */
		static void initialize(bool);

	private:
		/**
		 * Holds the first created instance and place holder with ID zero.
		 */
		static Variable* _zero;
		/**
		 * Static reference list to all references.
		 */
		static ReferenceVector _referenceList;
		/**
		 * Static this counter is increased if a global event is generated.
		 * It is decreased when it returns from the event handler.
		 */
		static int _globalActive;
		/**
		 * Keeps the allocated data in this list if it may not be deleted yet
		 * because it is obstructed by an event being handled.
		 */
		static TVector<void*> _deleteWaitCache;
		/**
		 * Holds the link to the global conversion handler.
		 */
		static VariableHandler* _convertLink;

		friend class Variable;

		friend class VariableReference;
};

}
