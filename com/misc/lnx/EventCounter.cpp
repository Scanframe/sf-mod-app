#include "EventCounter.h"
#include <sys/eventfd.h>
#include <unistd.h>

namespace sf
{

EventCounter::EventCounter()
	:_descriptor(-1)
{
}

EventCounter::~EventCounter()
{
	destroy();
}

void EventCounter::create(unsigned int initval, bool countdown, bool blocking)
{
	Lock lock(this);
	// Destroy any existing descriptor.
	destroy();
	// Assemble the flags.
	int flags = 0;/* EFD_CLOEXEC */;
	flags |= countdown ? EFD_SEMAPHORE : 0;
	flags |= blocking ? 0 : EFD_NONBLOCK;
	// Create the descriptor
	_descriptor = ::eventfd(0, flags);
	// Check for an error and throw an exception.
	if (_descriptor == -1)
	{
		throw ExceptionSystemCall("eventfd", errno, typeid(*this).name(), __FUNCTION__);
	}
}

void EventCounter::destroy()
{
	// Cannot destroy while Wait() is called.
	Lock lock(this);
	// If a descriptor exists close it first be creating a new one.
	if (_descriptor != -1)
	{
		// Close the file descriptor.
		if (close(_descriptor) == -1)
		{
			throw ExceptionSystemCall("close", errno, typeid(*this).name(), __FUNCTION__);
		}
		// Reset the descriptor member.
		_descriptor = 0;
	}
}

void EventCounter::set(unsigned int value)
{
	/*
	TLock lock(this);
	// Get a copy of the file descriptor.
	int fd = Descriptor;
	lock.Release();
	*/
	// Write the event increment and check for an error.
	if (::eventfd_write(_descriptor, value) == -1)
	{
		throw ExceptionSystemCall("eventfd_write", errno, typeid(*this).name(), __FUNCTION__);
	}
}

bool EventCounter::get(unsigned int& value) const
{
	// Initialize the return value.
	value = 0;
	eventfd_t val = 0;
	// Read the event counter.
	int result = ::eventfd_read(_descriptor, &val);
	// Check for an error.
	if (result == -1)
	{
		if (errno == EWOULDBLOCK)
		{
			return false;
		}
		else
		{
			throw ExceptionSystemCall("eventfd_read", errno, typeid(*this).name(), __FUNCTION__);
		}
	}
	// Assign the return value.
	value = val;
	// Signal success.
	return true;
}

}