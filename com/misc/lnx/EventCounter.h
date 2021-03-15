#pragma once

#include "Sync.h"
#include "../global.h"

namespace sf
{

/**
* TEventSemaphore class is build around the system call eventfd().
* This class has a file descriptor which could be used in an epoll object.
*/
class TEventCounter :private Sync
{
	public:
		/**
		* Handle type.
		*/
		typedef int handle_type;

		/**
		 * Default constructor.
		 */
		TEventCounter();

		/**
		 * Destructor.
		 */
		~TEventCounter();

		/**
		 * Creates the object annex file descriptor.
		 * Setting the initial value with 'initval'.
		 * When countdown is true the Get() function will countdown the counter one
		 * by one and the return value is each time 0.
		 */
		void Create(unsigned int initval, bool countdown = false, bool blocking = false);

		/**
		 * Destroys the object/file descriptor.
		 */
		void Destroy();

		/**
		* Returns the file descriptor to the object.
		*/
		[[nodiscard]] handle_type GetHandle() const;

		/**
		 * Sets a new value by incrementing the existing one.
		 * On failure to do so it returns false.
		 */
		void Set(unsigned int increment = 1);

		/**
		 * Gets the counter value and decrements it.
		 * Returns true when it would block when blocking is disabled.
		 * When counting down was enabled the returned value is always 1 until
		 * the internal counter is zero. When not counting down the return value is
		 * the counter value and the internal counter is reset to zero.
		 */
		bool Get(unsigned int& value) const;

		/**
		 * Same as above but when the would block it return 0 otherwise the value
		 * is returned as described above.
		 */
		[[nodiscard]] unsigned Get() const;

	private:
		/**
		 * Holds the file descriptor/handle to the object.
		 */
		handle_type _descriptor;
};

//
inline
TEventCounter::handle_type TEventCounter::GetHandle() const
{
	return _descriptor;
}

//
inline
unsigned int TEventCounter::Get() const
{
	unsigned int value;
	Get(value);
	return value;
}

}
