#pragma once

#include <vector>
#include "dbgutils.h"
#include "DynamicLibraryInfo.h"
#include "TClosure.h"
#include "../global.h"

#define _DL_INFORMATION(name, description) \
namespace \
{ \
const char* _dl_ = \
  _DL_MARKER_BEGIN \
  name \
  _DL_NAME_SEPARATOR \
  description \
  _DL_MARKER_END; \
}

/**
 * @brief Declares a public static function in the class where it is used.
 *
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

/**
 * @brief Implements the public static function in the class where it is used.
 */
#define SF_IMPL_IFACE(InterfaceType, ParamType, FuncName) \
sf::TClassRegistration<InterfaceType, ParamType> InterfaceType::FuncName() \
{ static sf::TClassRegistration<InterfaceType, ParamType>::entries_t entries; \
return entries; }

/**
 * @brief Registers a derived type from the interface interface type.
 *
 * Where:
 * 	DerivedType: The derived typename from InterfaceType
 * 	RegName: Quoted character string containing the  name.
 * 	Description: Quoted character string holding the name.
 */
#define SF_REG_CLASS(InterfaceType, ParamType, FuncName, DerivedType, RegName, Description) \
namespace { \
__attribute__((constructor)) void _##DerivedType##_() { \
size_t dist = InterfaceType::FuncName().registerClass \
  ( \
    RegName, \
    Description, \
    sf::TClassRegistration<InterfaceType, ParamType>::callback_t \
      ([](const ParamType& params)->InterfaceType* \
      { \
        auto inst = new DerivedType(params); \
        sf::TClassRegistration<InterfaceType, ParamType>::setRegisterName(inst, RegName); \
        return inst;\
       }) \
  ); } \
}

namespace sf
{

/**
 * @brief Template class used to register derived classes from an (virtual) interface class.<br>
 *
 * Registering name and description.<br>
 * [Example](sf-TClassRegistration.html)
 * @tparam T Base type of the class being registered
 * @tparam P Parameter type passed to the constructor.
 * @tparam F Callback function to create the instance.
 *
 */
template <typename T, typename P>
class TClassRegistration
{
	public:
		// Type and forward definitions.
		typedef TClassRegistration<T, P> self_t;
		typedef TClosure<T*, const P&> callback_t;
		struct entry_t;
		typedef std::vector<entry_t> entries_t;

		/**
		 * @brief Constructor for the base class.
		 */
		TClassRegistration<T, P>(entries_t& entries); // NOLINT(google-explicit-constructor)
		/**
		 * @brief Copy constructor
		 */
		TClassRegistration<T, P>(const TClassRegistration<T, P>& inst);

		/**
		 * @brief Assignment operator
		 */
		TClassRegistration<T, P>& operator=(const TClassRegistration<T, P>& inst)
		{
			// Prevent self assignment.
			if (this != &inst)
			{
				_entries = inst._entries;
			}
			return *this;
		}

		/**
		 * @brief Register a derived class.
		 *
		 * @param name Name of the class.
		 * @param description Description of the class.
		 * @param callback Callback function to create instance of the class.
		 */
		size_t registerClass(const char* name, const char* description, const callback_t& callback);
		/**
		 * @brief Find a registered class structure index.
		 *
		 * @param name Registered name of the class.
		 * @return Non zero index/position of the class in the registration list where Zero means not found.
		 */
		[[nodiscard]] size_t indexOf(const std::string& name) const;
		/**
		 * @brief Find and create registered class by name.
		 *
		 * @param name Registered name of the class
		 * @param params Parameter type instance for the constructor.
		 * @return nullptr on failure on success a new instance of the registered class.
		 */
		T* create(const std::string& name, const P& params) const;
		/**
		 * @brief Function create registered class by index.
		 *
		 * @param index Index in the list of classes.
		 * @param params Parameter structure.
		 * @return nullptr on failure on success a new instance of the registered class.
		 */
		T* create(size_t index, const P& params) const;
		/**
		 * @brief Returns the amount of registered entries.
		 *
		 * @return Derived instance of type T.
		 */
		[[nodiscard]] size_t size() const;
		/**
		 * @brief Sets the register name on the passed instance.
		 *
		 * This automatically is called from the callback_t when a class is registered.
		 */
		static void setRegisterName(T* inst, const char* name)
		{
			inst->_Register_Name_ = name;
		}
		/**
		 * @brief Gets the register name of the passed.
		 */
		const std::string& getRegisterName(T* inst) const
		{
			return inst->_Register_Name_;
		}
		/**
		 * @brief Returns the name of the class from the passed index.
		 */
		[[nodiscard]] const char* getName(size_t index) const;
		/**
		 * @brief Returns the description of the class from the passed index.
		 */
		[[nodiscard]] const char* getDescription(size_t index) const;

		/**
		 * @brief Type of registered entry in the list.
		 */
		struct entry_t
		{
			/**
			 * @brief Constructor.
			 * @param name
			 * @param description
			 * @param callback
			 */
			inline
			entry_t(const char* name, const char* description, const callback_t& callback)
				:_name(name), _description(description), _callback(callback) {}

			/**
			 * @brief Copy constructor.
			 * @param e
			 */
			inline
			entry_t(const entry_t& e)
				:_name(e._name), _description(e._description), _callback(e._callback) {}

			/**
			 * @brief Holds registered name.
			 */
			const char* _name;
			/**
			 * @brief Holds registered description.
			 */
			const char* _description;
			/**
			 * @brief Holds the passed callback function.
			 */
			callback_t _callback;
		};

	private:
		/**
		 * @brief Find a registered class structure.
		 *
		 * @param name Registered name of the class
		 * @return Entry pointer of the found class or nullptr on not found.
		 */
		const entry_t* find(const std::string& name) const;
		/**
		 * @brief Pointer to list where instances are registered.
		 */
		entries_t* _entries;
		/**
		 * @brief Returns the found const iterator.
		 */
		typename entries_t::const_iterator lookup(const std::string& name) const;
};

template <typename T, typename P>
TClassRegistration<T, P>::TClassRegistration(entries_t& entries)
	: _entries(&entries) {}

template <typename T, typename P>
TClassRegistration<T, P>::TClassRegistration(const TClassRegistration<T, P>& inst)
	: _entries(inst._entries) {}

template <typename T, typename P>
size_t TClassRegistration<T, P>::size() const
{
	return _entries->size();
}

template <typename T, typename P>
size_t TClassRegistration<T, P>::registerClass(const char* name, const char* description,
	const typename TClassRegistration<T, P>::callback_t& callback)
{
	return std::distance(_entries->begin(), _entries->insert(_entries->end(), entry_t(name, description, callback)));
}

template <typename T, typename P>
typename TClassRegistration<T, P>::entries_t::const_iterator
TClassRegistration<T, P>::lookup(const std::string& name) const
{
	// Sanity check.
	if (!name.empty() && !_entries->empty())
	{
		// Iterate through the list.
		for (typename entries_t::const_iterator it = _entries->begin(); it != _entries->end(); ++it)
		{
			// Compare the name case insensitive.
			if (name.compare(it->_name) == 0)
			{
				return it;
			}
		}
	}
	// Signal not found.
	return _entries->end();
}

template <typename T, typename P>
size_t TClassRegistration<T, P>::indexOf(const std::string& name) const
{
	auto it = lookup(name);
	// When not found
	if (it == _entries->end())
	{
		return 0;
	}
	// Return the mount on instances between begin and the looked up one with the passed name.
	// Add one because zero means not found.
	return std::distance(const_cast<const entries_t*>(_entries)->begin(), it) + 1;
}

template <typename T, typename P>
const typename TClassRegistration<T, P>::entry_t* TClassRegistration<T, P>::find(const std::string& name) const
{
	auto it = lookup(name);
	// When the lookup points to the end the entry is not found.
	if (it == _entries->end())
	{
		return nullptr;
	}
	// Convert iterator to entry pointer. (calls a dereference operator first).
	return &(*it);
}

template <typename T, typename P>
T* TClassRegistration<T, P>::create(const std::string& name, const P& params) const
{
	auto entry = find(name);
	if (entry == nullptr)
	{
		return nullptr;
	}
	return const_cast<entry_t*>(entry)->_callback(params);
}

template <typename T, typename P>
T* TClassRegistration<T, P>::create(size_t index, const P& params) const
{
	return _entries->at(index)._callback(params);
}

template <typename T, typename P>
const char* TClassRegistration<T, P>::getName(size_t index) const
{
	return _entries->at(index)._name;
}

template <typename T, typename P>
const char* TClassRegistration<T, P>::getDescription(size_t index) const
{
	return _entries->at(index)._description;
}

} // namespace
