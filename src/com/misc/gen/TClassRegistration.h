#pragma once

#include "../global.h"
#include "DynamicLibraryInfo.h"
#include "Exception.h"
#include "TClosure.h"
#include "TVector.h"
#include "dbgutils.h"
#include <vector>

/**
 * @brief Name of the function to resolve from a library to be able to set the library filename when dynamically loaded.
 */
#define SF_DL_NAME_FUNC_NAME "_dl_name_"
#define SF_DL_NAME_FUNC _dl_name_

// Select copy function which is allowed by MS Windows.
#if IS_WIN
	#define SF_DL_STRNCPY(dest, src, sz) strncpy_s(dest, sz, src, sz)
#else
	#define SF_DL_STRNCPY(dest, src, sz) strncpy(dest, src, sz)
#endif

/**
 * Declaration of dynamically loadable library information and function for set/get the library filename.
 */
#define SF_DL_INFORMATION(name, description)                           \
	namespace                                                            \
	{                                                                    \
	const char* _dl_ =                                                   \
		SF_DL_MARKER_BEGIN                                                 \
			name                                                             \
				SF_DL_NAME_SEPARATOR                                           \
					description                                                  \
						SF_DL_MARKER_END;                                          \
                                                                       \
	extern "C" TARGET_EXPORT const char* SF_DL_NAME_FUNC(const char* nm) \
	{                                                                    \
		static char storage[256];                                          \
		if (nm)                                                            \
			SF_DL_STRNCPY(storage, nm, sizeof(storage));                     \
		return storage;                                                    \
	}                                                                    \
	}

/**
 * @brief Declaration of function to set and get the dynamic library name when loading.
 */
extern "C" const char* SF_DL_NAME_FUNC(const char* name);

/**
 * @brief Type for casting a resolved function pointer to.
 */
#define SF_DL_NAME_FUNC_TYPE \
	const char* (*) (const char*)

namespace sf
{

/**
 * @brief function to get the library filename this function is called from.
 */
inline std::string getLibraryName()
{
	return SF_DL_NAME_FUNC(nullptr);
}

}// namespace sf

/**
 * @brief Declares a public static function in the class where it is used.
 *
 * Where:
 * 	InterfaceType: Global typename of the interface class (virtual base class).
 * 	ParamType: Global typename of the parameters passed.
 * 	FuncName: Name of the public function in the interface class for
 * 		registering derived classes or to create them.
 */
#define SF_DECL_IFACE(InterfaceType, ParamType, FuncName)             \
private:                                                              \
	std::string _Register_Name_;                                        \
                                                                      \
public:                                                               \
	static sf::TClassRegistration<InterfaceType, ParamType> FuncName(); \
	friend class sf::TClassRegistration<InterfaceType, ParamType>;

/**
 * @brief Implements the public static function in the class where it is used.
 */
#define SF_IMPL_IFACE(InterfaceType, ParamType, FuncName)                       \
	sf::TClassRegistration<InterfaceType, ParamType> InterfaceType::FuncName()    \
	{                                                                             \
		static sf::TClassRegistration<InterfaceType, ParamType>::entries_t entries; \
		return sf::TClassRegistration<InterfaceType, ParamType>(entries);           \
	}

/**
 * @brief Registers a derived type from the interface type.
 *
 * Where:
 * 	DerivedType: The derived typename from InterfaceType
 * 	RegName: Quoted character string containing the  name.
 * 	Description: Quoted character string holding the name.
 */
#define SF_REG_CLASS(InterfaceType, ParamType, FuncName, DerivedType, RegName, Description)                        \
	namespace                                                                                                        \
	{                                                                                                                \
	__attribute__((constructor)) void _##DerivedType##_()                                                            \
	{                                                                                                                \
		size_t dist = InterfaceType::FuncName().registerClass(                                                         \
			RegName,                                                                                                     \
			Description,                                                                                                 \
			sf::TClassRegistration<InterfaceType, ParamType>::callback_t([](const ParamType& params) -> InterfaceType* { \
				auto inst = new DerivedType(params);                                                                       \
				sf::TClassRegistration<InterfaceType, ParamType>::setRegisterName(inst, RegName);                          \
				return inst;                                                                                               \
			})                                                                                                           \
		);                                                                                                             \
	}                                                                                                                \
	}

namespace sf
{

/**
 * @brief Template class used to register derived classes from a (virtual) interface class.
 *
 * Registering name and description.<br>
 * [Example](sf-TClassRegistration.html)
 * @tparam T Base type of the class being registered
 * @tparam P Parameter type passed to the constructor.
 * @tparam F Callback function to create the instance.
 *
 */
template<typename T, typename P>
class TClassRegistration
{
	public:
		// Type and forward definitions.
		typedef TClassRegistration<T, P> self_t;
		typedef TClosure<T*, const P&> callback_t;
		struct entry_t;
		/**
		 * @brief Vector type to register implementations in.
		 */
		typedef std::vector<entry_t> entries_t;

		/**
		 * @brief Type used to report the list of implementations in.
		 */
		typedef TVector<std::string> strings_t;

		/**
		 * @brief Index value for when not found.
		 */
		static auto constexpr npos = std::numeric_limits<size_t>::max();

		/**
		 * @brief Constructor for the base class.
		 */
		inline explicit TClassRegistration<T, P>(entries_t& entries);

		/**
		 * @brief Copy constructor
		 */
		inline TClassRegistration<T, P>(const TClassRegistration<T, P>& inst);

		/**
		 * @brief Move constructor
		 */
		inline TClassRegistration<T, P>(TClassRegistration<T, P>&& inst);

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
		inline size_t registerClass(const char* name, const char* description, const callback_t& callback);

		/**
		 * @brief Find a registered class structure index.
		 * When not found it returns
		 *
		 * @param name Registered name of the class.
		 * @return Nonzero index/position of the class in the registration list where Zero means not found.
		 */
		[[nodiscard]] inline size_t indexOf(const std::string& name) const;

		/**
		 * @brief Find and create registered class by name.
		 *
		 * @param name Registered name of the class
		 * @param params Parameter type instance for the constructor.
		 * @return nullptr on failure on success a new instance of the registered class.
		 */
		inline T* create(const std::string& name, const P& params = P{}) const;

		/**
		 * @brief Function create registered class by index.
		 *
		 * @param index Index in the list of classes.
		 * @param params Parameter structure.
		 * @return nullptr on failure on success a new instance of the registered class.
		 */
		inline T* create(size_t index, const P& params = P{}) const;

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
		 * @brief Gets the name of the class from the passed index.
		 */
		[[nodiscard]] const char* getName(size_t index) const;

		/**
		 * @brief Gets the all the registered names.
		 */
		[[nodiscard]] strings_t getNames() const;

		/**
		 * @brief Returns the description of the class from the passed index.
		 */
		[[nodiscard]] const char* getDescription(size_t index) const;

		/**
		 * @brief Type of registered entry in the list.
		 */
		class entry_t
		{
			public:
				/**
				 * @brief Constructor.
				 */
				inline entry_t(const char* name, const char* description, const callback_t& callback)
					: _name(name)
					, _description(description)
					, _callback(callback)
				{}

				/**
				 * @brief Copy constructor.
				 */
				inline entry_t(const entry_t& e)
					: _name(e._name)
					, _description(e._description)
					, _callback(e._callback)
				{}

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

}// namespace sf

#include "TClassRegistration.hpp"
