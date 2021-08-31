#pragma once

#include "VariableTypes.h"
#include "VariableHandler.h"
#include "Variable.h"

#include "global.h"

namespace sf
{

/**
 * @brief Base class of the #sf::Variable having all global static data members.
 */
class _GII_DATA VariableStatic :public VariableTypes
{
	public:
		/**
		 * @brief Returns Variable instance that always exist in the system and is referenced if no other is available or applicable.
		 */
		static Variable& zero();

		/**
		 * @brief Called to initialize or deinitialize the variable system.
		 */
		static void initialize(bool);

	private:
		/**
		 * @brief Holds the first created instance and place holder with ID zero.
		 */
		static Variable* _zero;
		/**
		 * @brief Static reference list to all global references.
		 */
		static ReferenceVector* _references;
		/**
		 * @brief Static this counter is increased if a global event is generated.
		 * It is decreased when it returns from the event handler.
		 */
		static int _globalActive;
		/**
		 * @brief Keeps the allocated data in this list if it may not be deleted yet
		 * because it is obstructed by an event being handled.
		 */
		static TVector<void*>* _deleteWaitCache;
		/**
		 * @brief Holds the link to the global conversion handler.
		 */
		static VariableHandler* _convertHandler;

		/**
		 * @brief Lookup list for flags.
		 */
		struct FlagLetters
		{
			char _letter;
			int _flag;
		} static _flagLetters[9];

		friend class Variable;

		friend class VariableReference;
};

}
