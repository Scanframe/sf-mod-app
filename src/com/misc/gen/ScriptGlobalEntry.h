#pragma once

#include "../global.h"
#include "ScriptObject.h"
#include "Value.h"

namespace sf
{

/**
 * @brief Pure virtual class to register script accessible entries through the use of #sf::ScriptGlobalObject class.
 */
class _MISC_CLASS ScriptGlobalEntry
{
	public:
		/**
		 * @brief Unsigned size value indicating not found or no index.
		 */
		static constexpr size_t unlimitedArgs = std::numeric_limits<int>::max();

		/**
		 * @brief Default Constructor adding itself to the global mapped entries.
		 *
		 * @param name Name of the function.
		 * @param description Description of the function.
		 * @param argumentCount Amount of arguments needed.
		 */
		ScriptGlobalEntry(const std::string& name, const std::string& description, int argumentCount);

		/**
		 * @brief Destructor.
		 *
		 * Removes itself from the mapped entries.
		 */
		virtual ~ScriptGlobalEntry();

		/**
		 * @brief Not allowed to copy.
		 */
		ScriptGlobalEntry(const ScriptGlobalEntry&) = delete;

		/**
		 * @brief Not allowed to copy.
		 */
		ScriptGlobalEntry& operator=(const ScriptGlobalEntry&) = delete;

		/**
		 * @brief Gets the name of the entry.
		 */
		[[nodiscard]] std::string getName() const
		{
			return _name;
		}

		/**
		 * @brief Sets the description of the entry.
		 */
		void setDescription(const std::string& description)
		{
			_description = description;
		}

		/**
		 * @brief Gets the description of the entry.
		 */
		[[nodiscard]] std::string getDescription() const
		{
			return _description;
		}

		/**
		 * @brief Gets the amount of parameters that is needed for this function.
		 *
		 * @return When #unlimitedArgs is unlimited, -2 is the same a unlimited but at least 2 arguments.
		 */
		[[nodiscard]] int getArgumentCount() const
		{
			return _argumentCount;
		}

		/**
		 * @brief Must be implemented to be able to call the function.
		 */
		virtual void call(const Value::vector_type& arguments, Value& result) const = 0;

		/**
		 * @brief Finds the function with name 'name' and returns true when the function was found passing the correct amount of parameters.
		 * When found the function is also executed.
		 *
		 * @param name Name of the entry.
		 * @param arguments Required arguments.
		 * @param result Result from the call.
		 * @return
		 */
		static bool call(const std::string& name, const Value::vector_type& arguments, Value& result);

		/**
		 * @brief Gets the entry pointer by name.
		 * @param name Name if the entry.
		 * @return nullptr when not found.
		 */
		static ScriptGlobalEntry* getEntry(const std::string& name);

	protected:
		/**
		 * @brief Holds the name of the entry.
		 */
		std::string _name;
		/**
		 * @brief Holds description of the entry.
		 */
		std::string _description;
		/**
		 * @brief Holds the number of parameters to pass to the entry.
		 */
		int _argumentCount{0};
};

/**
 * @brief Template for creating a script global entry linking a dynamic created method.
 *
 * @tparam T Class containing the method of type FunctionType.
 */
template<class T>
class TScriptGlobalEntry : public ScriptGlobalEntry
{
	public:
		typedef Value (T::*FunctionType)(const Value::vector_type&);

		TScriptGlobalEntry(T* self, FunctionType func, const std::string& name, int argumentCount, const std::string& description)
			: ScriptGlobalEntry(name, description, argumentCount)
			, _self(self)
			, _func(func)
		{
		}

		TScriptGlobalEntry(const TScriptGlobalEntry&) = delete;

		TScriptGlobalEntry& operator=(const TScriptGlobalEntry&) = delete;

	private:
		void call(const Value::vector_type& arguments, Value& result) const override
		{
			result = _self->*_func(arguments);
		}

		T* _self;
		FunctionType _func;
};

/**
 * @brief #sf::ScriptGlobalEntry derived class to link static functions or methods.
 */
class ScriptGlobalStaticEntry : public ScriptGlobalEntry
{
	public:
		typedef Value (*FunctionType)(const Value::vector_type& arguments);

		ScriptGlobalStaticEntry(FunctionType func, const std::string& name, const std::string& description, int argumentCount)
			: ScriptGlobalEntry(name, description, argumentCount)
			, _func(func)
		{
		}

		ScriptGlobalStaticEntry(const ScriptGlobalStaticEntry&) = delete;

		ScriptGlobalStaticEntry& operator=(const ScriptGlobalStaticEntry&) = delete;

	private:
		void call(const Value::vector_type& arguments, Value& result) const override
		{
			result = (*_func)(arguments);
		}

		FunctionType _func;
};

}// namespace sf
