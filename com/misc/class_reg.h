#pragma once

#include <vector>
#include "dbgutils.h"
#include "closure.h"
#include "global.h"

/**
 * Defines marker string used to find the start of a naming and description block in a compiled binary
 */
#define _DL_MARKER_BEGIN "\n#@$!*^\n"
/**
 * End marker
 */
#define _DL_MARKER_END "\n^*!$@#\n"

/**
 * Separator between name and description.
 */
#define _DL_NAME_SEPARATOR "\n\n"

#define _DL_INFORMATION(Name, Description) \
namespace \
{ \
const char* _dl_ = \
  _DL_MARKER_BEGIN \
  Name \
  _DL_NAME_SEPARATOR \
  Description \
  _DL_MARKER_END; \
}

/**
 * Declares a public static function in the class where it is used.
 * Where:
 * 	InterfaceType: Global typename of the interface class (virtual base class).
 * 	ParamType: Global typename of the parameters passed.
 * 	FuncName: Name of the public function in the interface class for
 * 		registering derived classes or to create them.
 */
#define SF_DECL_IFACE(InterfaceType, ParamType, FuncName) \
  private: \
    std::string _Register_Name_; \
  public: \
    static sf::TClassRegistration<InterfaceType, ParamType> FuncName(); \
 	friend class sf::TClassRegistration<InterfaceType, ParamType>;
/*
static sf::TClassRegistration<RuntimeIface, RuntimeIface::Parameters> Interface();
*/

/**
 * Implements the public static function in the class where it is used.
 */
#define SF_IMPL_IFACE(InterfaceType, ParamType, FuncName) \
sf::TClassRegistration<InterfaceType, ParamType> InterfaceType::FuncName() \
{ static sf::TClassRegistration<InterfaceType, ParamType>::entries_t entries; \
return entries; }


/*
sf::TClassRegistration<RuntimeIface, RuntimeIface::Parameters> RuntimeIface::Interface()
{
	// Static storage declaration here so it resides in this DL.
	static sf::TClassRegistration<RuntimeIface, RuntimeIface::Parameters>::entries_t entries;
	// Return the entries and the assignment constructor is called for.
	return entries;
}
*/

/**
 * Registers a derived type from the interface interface type.
 * Where:
 * 	DerivedType: The derived typename from InterfaceType
 * 	RegName: Quoted character string containing the  name.
 * 	Decription: Quoted character string holding the name.
 */
#define SF_REG_CLASS(InterfaceType, ParamType, FuncName, DerivedType, RegName, Description) \
namespace { \
__attribute__((constructor)) void _##DerivedType##_() { \
size_t dist = InterfaceType::FuncName().Register \
  ( \
    RegName, \
    Description, \
    sf::TClassRegistration<InterfaceType, ParamType>::callback_t \
      ([](const ParamType& params)->InterfaceType* \
      { \
        auto inst = new DerivedType(params); \
        sf::TClassRegistration<InterfaceType, ParamType>::SetRegisterName(inst, RegName); \
        return inst;\
       }) \
  ); } \
}

/*
namespace
{
__attribute__((constructor)) void register_on_load()
{
	size_t dist = RuntimeIface::InterfaceClassRegistration().Register
		(
			"Implementation_A",
			"A description of the class.",
			sf::TClassRegistration<RuntimeIface, RuntimeIface::Parameters>::callback_t
				(
					[](const RuntimeIface::Parameters& params)->RuntimeIface*
					{
						return new RuntimeLibImplementationA(params);
					}
				)
		);
}
}
*/

namespace sf
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
		// Type and forward definitions.
		typedef TClassRegistration<T, P> self_t;
		typedef Closure<T*, const P&> callback_t;
		struct entry_t;
		typedef std::vector<entry_t> entries_t;

		/**
		 * Constructor For the base class.
		 */
		TClassRegistration<T, P>(entries_t& entries); // NOLINT(google-explicit-constructor)
		/**
		 * Copying is allowed.
		 */
		TClassRegistration<T, P>(const TClassRegistration<T, P>& inst);

		/**
		 * Assignment is not allowed.
		 * @param inst
		 * @return
		 */
		TClassRegistration<T, P>& operator=(const TClassRegistration<T, P>& inst)
		{
			// Prevent self assignment.
			if (this != &inst)
			{
				Entries = inst.Entries;
			}
			return *this;
		}

		/**
		 * Register a derived class.
		 * @param name
		 * @param description
		 * @param callback
		 */
		size_t Register(const char* name, const char* description, const callback_t& callback);
		/**
		 * Find a registered class structure index.
		 * @param name Registered name of the class
		 * @return Non zero index/position of the class in the registration list where Zero means not found.
		 */
		[[nodiscard]] size_t IndexOf(const std::string& name) const;
		/**
		 * Find and create registered class by name.
		 * @param name Registered name of the class
		 * @param params Parameter type instance for the constructor.
		 * @return nullptr on failure on success a new instance of the registered class.
		 */
		T* Create(const std::string& name, const P& params) const;
		/**
		 * Function create registered class by index.
		 * @param index
		 * @param params
		 * @return nullptr on failure on success a new instance of the registered class.
		 */
		T* Create(size_t index, const P& params) const;
		/**
		 * Returns the amount of registered entries.
		 * @return Derived instance of type T.
		 */
		[[nodiscard]] size_t Size() const;
		/**
		 * Sets the register name on the passed instance.
		 * This automatically is called from the callback_t when a class is registered.
		 */
		static void SetRegisterName(T* inst, const char* name)
		{
			inst->_Register_Name_ = name;
		}
		/**
		 * Gets the register name of the passed.
		 */
		const std::string& RegisterName(T* inst) const
		{
			return inst->_Register_Name_;
		}
		/**
		 * Returns the name of the class from the passed index.
		 */
		[[nodiscard]] const char* Name(size_t index) const;
		/**
		 * Returns the description of the class from the passed index.
		 */
		[[nodiscard]] const char* Description(size_t index) const;

		// Type of registered entry in the list.
		struct entry_t
		{
			// Constructor.
			inline
			entry_t(const char* name, const char* description, const callback_t& callback)
				: Name(name), Description(description), Callback(callback) {}

			// Copy constructor.
			inline
			entry_t(const entry_t& e)
				: Name(e.Name), Description(e.Description), Callback(e.Callback) {}

			// Holds registered name.
			const char* Name;
			// Holds registered description.
			const char* Description;
			// Holds the passed callback function.
			callback_t Callback;
		};

	private:
		/**
		 * Find a registered class structure.
		 * @param name Registered name of the class
		 * @return Entry pointer of the found class or nullptr on not found.
		 */
		const entry_t* Find(const std::string& name) const;
		// Pointer to list where instances are registered.
		entries_t* Entries;
		// Returns the found const iterator.
		typename entries_t::const_iterator Lookup(const std::string& name) const;
};

template <typename T, typename P>
TClassRegistration<T, P>::TClassRegistration(entries_t& entries)
	: Entries(&entries) {}

template <typename T, typename P>
TClassRegistration<T, P>::TClassRegistration(const TClassRegistration<T, P>& inst)
	: Entries(inst.Entries) {}

template <typename T, typename P>
size_t TClassRegistration<T, P>::Size() const
{
	return Entries->size();
}

template <typename T, typename P>
size_t TClassRegistration<T, P>::Register(const char* name, const char* description,
	const typename TClassRegistration<T, P>::callback_t& callback)
{
	return std::distance(Entries->begin(), Entries->insert(Entries->end(), entry_t(name, description, callback)));
}

template <typename T, typename P>
typename TClassRegistration<T, P>::entries_t::const_iterator
TClassRegistration<T, P>::Lookup(const std::string& name) const
{
	// Sanity check.
	if (!name.empty() && !Entries->empty())
	{
		// Iterate through the list.
		for (typename entries_t::const_iterator it = Entries->begin(); it != Entries->end(); ++it)
		{
			// Compare the name case insensitive.
			if (name.compare(it->Name) == 0)
			{
				return it;
			}
		}
	}
	// Signal not found.
	return Entries->end();
}

template <typename T, typename P>
size_t TClassRegistration<T, P>::IndexOf(const std::string& name) const
{
	auto it = Lookup(name);
	// When not found
	if (it == Entries->end())
	{
		return 0;
	}
	// Return the mount on instances between begin and the looked up one with the passed name.
	// Add one because zero means not found.
	return std::distance(const_cast<const entries_t*>(Entries)->begin(), it) + 1;
}

template <typename T, typename P>
const typename TClassRegistration<T, P>::entry_t* TClassRegistration<T, P>::Find(const std::string& name) const
{
	auto it = Lookup(name);
	// When the lookup points to the end the entry is not found.
	if (it == Entries->end())
	{
		return nullptr;
	}
	// Convert iterator to entry pointer. (calls a dereference operator first).
	return &(*it);
}

template <typename T, typename P>
T* TClassRegistration<T, P>::Create(const std::string& name, const P& params) const
{
	auto entry = Find(name);
	if (entry == nullptr)
	{
		return nullptr;
	}
	return const_cast<entry_t*>(entry)->Callback(params);
}

template <typename T, typename P>
T* TClassRegistration<T, P>::Create(size_t index, const P& params) const
{
	return Entries->at(index).Callback(params);
}

template <typename T, typename P>
const char* TClassRegistration<T, P>::Name(size_t index) const
{
	return Entries->at(index).Name;
}

template <typename T, typename P>
const char* TClassRegistration<T, P>::Description(size_t index) const
{
	return Entries->at(index).Description;
}

/**
 * Information on a dynamic library set using _DL_INFORMATION macro.
 */
struct _MISC_CLASS DynamicLibraryInfo
{
	/**
	 * Default destructor.
	 */
	DynamicLibraryInfo() = default;
	/**
	 * Copy destructor.
	 */
	DynamicLibraryInfo(const DynamicLibraryInfo&);
	/**
	 * Path of the dynamic library.
	 */
	std::string Path;
	/**
	 * Name of the dynamic library.
	 */
	std::string Name;
	/**
	 * Description of the dynamic library.
	 */
	std::string Description;
	/**
	 * Reads the information from the file and return true on success.
	 */
	bool read(const std::string& filepath);
	/**
	 * Clears the members.
	 */
	void reset();
};

} // namespace
