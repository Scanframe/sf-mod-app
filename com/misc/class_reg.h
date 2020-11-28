#ifndef MISC_CLASS_REG_H
#define MISC_CLASS_REG_H

// Import of debugging defines and macros.
#include "com/misc/dbgutils.h"
// Import of debugging defines and macros.
#include "com/misc/genutils.h"
//
#include "com/misc/closure.h"

namespace misc
{

/**
 * Template class used to register derived classes by name.
 * @tparam T Base type of the class being registered
 * @tparam P Parameter type passed to the constructor.
 * @tparam F Callback function to create the instance.
 */
template <typename T, typename P>
class TClassRegistration
{
	public:
		// Type definitions.
		typedef TClassRegistration<T, P> self_t;
		typedef Closure<T*, const P&> callback_t;

		/**
		 * Constructor For the base class.
		 */
		TClassRegistration<T, P>();
		/**
		 * Register a derived class.
		 * @param name
		 * @param description
		 * @param callback
		 */
		void Register(const char* name, const char* description, const callback_t& callback);
		/**
		 * Find a registered class structure.
		 * @param name Registered name of the class
		 * @return Instance of this class.
		 */
		static self_t* Find(const std::string& name);
		/**
		 * Find a registered class structure index.
		 * @param name Registered name of the class
		 * @return Index of the class in the registration list.
		 */
		static unsigned IndexOf(const std::string& name);
		/**
		 * Find and create registered class by name.
		 * @param name Registered name of the class
		 * @param params Parameter type instance for the constructor.
		 * @return nullptr on failure on success a new instance of the registered class.
		 */
		static T* Create(std::string& name, const P& params);
		/**
		 * Function to find and create registered class by index.
		 * @param index
		 * @param params
		 * @return nullptr on failure on success a new instance of the registered class.
		 */
		static T* Create(unsigned index, const P& params);

	private:
		// Registered entry in the list.
		struct entry_t
		{
			entry_t(const char* name, const char* description, const callback_t& calback)
				: Name(name), Description(description), Callback(calback) {}

			// Holds registered name.
			const char* Name;
			// Holds registered description.
			const char* Description;
			// Holds the passed callback function.
			callback_t Callback;
		};

		//
		typedef std::vector<entry_t> entries_t;
		// Static list where instances are registered.
		static entries_t Entries;
};

template <typename T, typename P>
TClassRegistration<T, P>::TClassRegistration()
{
}

template <typename T, typename P>
typename TClassRegistration<T, P>::entries_t TClassRegistration<T, P>::Entries;

template <typename T, typename P>
void TClassRegistration<T, P>::Register(const char* name, const char* description,
	const typename TClassRegistration<T, P>::callback_t& callback)
{
	Entries.insert(Entries.end(), entry_t(name, description, callback));
}

}
#endif // MISC_CLASS_REG_H