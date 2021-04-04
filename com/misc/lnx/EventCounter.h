#pragma once

#include "gen/Sync.h"
#include "../global.h"

namespace sf
{

/**
* TEventSemaphore class is build around the system call eventfd().
* This class has a file descriptor which could be used in an epoll object.
*/
class EventCounter :private Sync
{
	public:
		/**
		* Handle type.
		*/
		typedef int handle_type;

		/**
		 * Default constructor.
		 */
		EventCounter();

		/**
		 * Destructor.
		 */
		~EventCounter();

		/**
		 * Creates the object annex file descriptor.
		 * Setting the initial value with 'initval'.
		 * When countdown is true the Get() function will countdown the counter one
		 * by one and the return value is each time 0.
		 */
		void create(unsigned int initval, bool countdown = false, bool blocking = false);

		/**
		 * Destroys the object/file descriptor.
		 */
		void destroy();

		/**
		* Returns the file descriptor to the object.
		*/
		[[nodiscard]] handle_type getHandle() const;

		/**
		 * Sets a new value by incrementing the existing one.
		 * On failure to do so it returns false.
		 */
		void set(unsigned int increment = 1);

		/**
		 * Gets the counter value and decrements it.
		 * Returns true when it would block when blocking is disabled.
		 * When counting down was enabled the returned value is always 1 until
		 * the internal counter is zero. When not counting down the return value is
		 * the counter value and the internal counter is reset to zero.
		 */
		bool get(unsigned int& value) const;

		/**
		 * Same as above but when the would block it return 0 otherwise the value
		 * is returned as described above.
		 */
		[[nodiscard]] unsigned get() const;

	private:
		/**
		 * Holds the file descriptor/handle to the object.
		 */
		handle_type _descriptor;
};

//
inline
EventCounter::handle_type EventCounter::getHandle() const
{
	return _descriptor;
}

//
inline
unsigned int EventCounter::get() const
{
	unsigned int value;
	get(value);
	return value;
}

}
