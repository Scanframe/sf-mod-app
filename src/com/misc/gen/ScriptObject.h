#pragma once

#include <cstring>
#include "gen_utils.h"
#include "Value.h"
#include "TClassRegistration.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Info structure for objects used in scripts.
 */
class _MISC_CLASS ScriptObject
{
	public:
		/**
		 * @brief Type to pass to registered classes.
		 */
		struct Parameters
		{
			explicit Parameters(Value::vector_type* arguments, ScriptObject* parent = nullptr)
			:_parent(parent)
			, _arguments(arguments)
			{}
			ScriptObject* _parent;
			Value::vector_type* _arguments;
		};

		/**
		 * @brief Source position type.
		 */
		typedef std::string::size_type pos_type;
		/**
		 * @brief Instruction pointer type.
		 */
		typedef ssize_t ip_type;
		/**
		 * @brief Keyword identifiers.
		 */
		enum EIdentifier
		{
			/** Unknown to script */
			idUnknown = 0,
			/** Is a constant like PI.*/
			idConstant,
			/** Is a variable which can be assigned.*/
			idVariable,
			/** Is a callable function.*/
			idFunction,
			/** Is a type definition like 'int', 'float', string, 'undef' and 'object'.*/
			idTypedef,
			/** Keyword like 'if' and not used at the moment.*/
			idKeyword
		};

		/**
		 * @brief Used to create static lookup lists.
		 */
		struct IdInfo
		{
			/**
			 * @brief Index for decoding. Could be regarded as function or variable address.
			 */
			int _index{0};
			/**
			 * @brief Function of the entry.
			 */
			EIdentifier _id{idUnknown};
			/**
			 * @brief Name of the entry.
			 */
			const char* _name{nullptr};
			/**
			 * @brief In case of a function the amount of parameters where std::numeric_limits<int>::max()
			 * is infinite -2 is at least 2 and when 3 is exactly 3 parameters are required.
			 */
			int _paramCount{0};
			/**
			 * @brief Pointer referring to info object. If nullptr it is a static entry.
			 */
			void* _data{nullptr};
		};

		/**
		 * Constructor.
		 */
		explicit ScriptObject(const char* type_name, ScriptObject* parent = nullptr);

		/**
		 * @brief Virtual destructor which can be overloaded to clean up objects.
		 */
		virtual ~ScriptObject() = default;

		/**
		 * @brief Must be overloaded for member namespace.
		 */
		[[nodiscard]] virtual const IdInfo* getInfo(const std::string& name) const = 0;

		/**
		 * @brief Gets the status text of this object for debugging purposes.
		 */
		virtual std::string getStatusText();
		/**
		 * @brief Gets or sets the a passed data member. Must be overloaded in derived class.
		 */
		virtual bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) = 0;

		/**
		 * @brief Asks if the object should be deleted after having made this call.
		 */
		virtual void destroyObject(bool& should_delete) = 0;

		/**
		 * @brief Gets the reference count.
		 */
		[[nodiscard]] inline int getRefCount() const
		{
			return _refCount;
		}

		/**
		 * @brief Cast operator to be able to return this instance as a Value.
		 */
		explicit operator Value() const;

		/**
		 * @brief Returns the static Info structure for unknowns.
		 */
		static const IdInfo* getInfoUnknown();

		/**
		 * @brief Returns the type name Set at the constructor.
		 */
		[[nodiscard]] std::string getTypeName() const;

		/**
		 * @brief Gets the script object owner.
		 */
		inline ScriptObject* getParent()
		{
			return _parent;
		}

		/**
		 * @brief Gets the script object owner.
		 */
		[[nodiscard]] inline const ScriptObject* getParent() const
		{
			return _parent;
		}

		/**
		 * @brief Casts a #sf::Value::vitCustom typed #sf::Value to a #ScriptObject typed pointer.
		 */
		ScriptObject* castToObject(const Value& value);

	protected:
		/**
		 * @brief Makes this object the owner of the other object.
		 */
		void makeParent(ScriptObject* so);
		/**
		 * @brief Sets the owner to the pass script object.
		 */
		void setParent(ScriptObject* parent);

	private:
		/**
		 * @brief Reference count for internal use to determine if this instance is to be deleted.
		 */
		int _refCount{0};
		/**
		 * @brief Type name used in the script for debugging.
		 */
		const char* _typeName{nullptr};
		/**
		 * @brief Script object which owns/created this instance. Is used to call labels of for events.
		 */
		ScriptObject* _parent{nullptr};

		friend class ScriptInterpreter;

		// Declarations of static functions and data members to be able to create implementations.
	SF_DECL_IFACE(ScriptObject, ScriptObject::Parameters, Interface)

};

}
