#pragma once

#include <functional>
#include "gen_utils.h"

namespace sf
{

/**
 * @brief Base class used as container for classes having listeners created by #sf::TListener template.
 */
class ListenerList
{
	public:
		/**
		 * @brief Type definition of a class having a virtual destructor used as a base class for #sf::TListener.
		 */
		struct base_type
		{
			virtual ~base_type() = default;

			/**
			 * @brief Will be over loaded in template to check the emitter has gone away.
			 *
			 * @return True when shared pointer is still shared.
			 */
			virtual bool validListener() = 0;
		};

		/**
		 * @brief Destructor deleting all entries by calling #flushListeners().
		 */
		~ListenerList()
		{
			flushListeners();
		}

		/**
		 * @brief Deletes all the linked entries in the list.
		 *
		 * @return A mount of flushed entries.
		 */
		size_t flushListeners()
		{
			size_t rv = list.size();
			for (auto entry: list)
			{
				delete_null(entry);
			}
			list.clear();
			return rv;
		}

		/**
		 * @brief Removes the given entry from the list.
		 *
		 * @return Amount of removed entries.
		 */
		size_t removeListener(base_type* entry)
		{
			size_t rv = 0;
			for (list_type::size_type i = 0; i < list.size(); i++)
			{
				if (list.at(i) == entry)
				{
					list.erase(list.begin() + i, list.begin() + i + 1); // NOLINT(cppcoreguidelines-narrowing-conversions)
					rv++;
				}
			}
			return rv;
		}

		/**
		 * @brief Removes all invalid/unused entries.
		 *
		 * There is no real need call this method explicitly since it is called when a new link is added.
		 * It is mainly used during unit tests to check proper functioning.
		 *
		 * @return Amount of removed entries.
		 */
		size_t cleanupListeners()
		{
			size_t rv = 0;
			// Iterate through the list of weak pointers.
			for (auto it = list.begin(); it != list.end();)
			{
				auto entry = *it;
				if (!entry->validListener())
				{
					// Remove the expired entry.
					list.erase(it);
					// Delete the entry.
					delete entry;
					// Increment the return value/
					rv++;
					// Do not move the iterator since erase has the same effect.
					continue;
				}
				++it;
			}
			return rv;
		}

	private:
		/**
		 * @brief Appends the given entry to the and of the list.
		 *
		 * Also calls #cleanupListeners() before adding the new entry.
		 */
		void appendListener(base_type* entry)
		{
			// Do some cleanup if possible.
			cleanupListeners();
			// Add the new entry.
			list.insert(list.end(), entry);
		}

		typedef std::vector<base_type*> list_type;
		list_type list;

		/**
		 * @brief Allows access to appendListener() method from the TListener template.
		 */
		template<typename... Args> friend
		class TListener;
};

/**
 * @brief Template class used to bind listeners to a handler_type instance.
 *
 * @tparam Args
 */
template<typename... Args>
class TListener :private ListenerList::base_type
{
	public:
		/**
		 * @brief Function type of the lambda or function.
		 */
		typedef std::function<void(Args...)> func_type;
		/**
		 * @brief Listener type of the lambda or function.
		 *
		 * This type is wraps around a std::function
		 */
		typedef TListener<Args...> listener_type;

		/**
		 * @brief Constructor.
		 *
		 * @param list Owning list
		 * @param lambda The actual function to call.
		 */
		explicit TListener(ListenerList* list, const func_type& lambda)
			:owner(list)
		{
			owner->appendListener(this);
			ptr.reset(new func_type(lambda));
		}

		/**
		 * @brief Do not allow copying.
		 */
		TListener(const TListener&) = delete;

	public:
		/**
		 * @brief Overridden destructor from ListenerList::base_type in order to delete entries of any instantiated template.
		 */
		~TListener() override
		{
			ptr.reset();
			owner->removeListener(this);
		}

		/**
		 * @brief Emitter class type for creating an instance which is emitting events to listeners.
		 */
		class emitter_type
		{
			public:

				/**
				 * @brief Destructor clearing all shared pointers.
				 */
				~emitter_type() // NOLINT(modernize-use-equals-default)
				{
					// Iterate through the list of weak pointers.
					for (auto& i: list)
					{
						if (!i.expired())
						{
							// Assign an empty.
							*i.lock().get() = func_type();
						}
					}
				}

				/**
				 * @brief Assigns a listener instance to this handler instance and also to the passed listener list.
				 *
				 * @return Pointer to created TListener which can be deleted to unlink.
				 */
				TListener* linkListener(ListenerList* listener_list, const func_type& function)
				{
					auto rv = new TListener(listener_list, function);
					list.insert(list.end(), rv->ptr);
					return rv;
				}

				/**
				 * @brief Call the registered listeners.
				 *
				 * Also cleans up when pointer expired.
				 *
				 * @param args Arguments set using this template.
				 */
				void callListeners(Args... args)
				{
					// Iterate through the list of weak pointers.
					for (auto it = list.begin(); it != list.end();)
					{
						if (it->expired())
						{
							// Erase the expired entry.
							list.erase(it);
							// Do not move the iterator since erase has the same effect.
							continue;
						}
						// Execute the function.
						(*it->lock().get())(args...);
						// Move to the next instance in the list.
						++it;
					}
				}

				/**
				 * @brief Removes expired listeners.
				 *
				 * @return Amount of removed entries.
				 */
				size_t cleanup()
				{
					size_t rv = 0;
					// Iterate through the list of weak pointers.
					for (auto it = list.begin(); it != list.end();)
					{
						if (it->expired())
						{
							// Erase the expired entry.
							list.erase(it);
							// Increment the return value/
							rv++;
						}
						else
						{
							// Move to the next instance in the list.
							++it;
						}
					}
					return rv;
				}

			private:
				/**
				 * @brief List of weak pointers to an std::function.
				 */
				std::vector<std::weak_ptr<func_type>> list;
		};

	private:
		/**
		 * @brief Overrides the needed valid() method.
		 *
		 * @return
		 */
		bool validListener() override
		{
			return !!*ptr.get();
		}

		/**
		 * @brief Container this instance is added to.
		 */
		ListenerList* owner;
		/**
		 * @brief Holds the shared pointer to the std::function.
		 */
		std::shared_ptr<func_type> ptr{};
};

}
