#include "SegmentFaultHandler.h"

namespace sf
{

SegmentFaultHandler::SegmentFaultHandler(const std::function<void()>& cb)
{
	// When first install the handler
	if (_buffers.empty())
	{
		_savedHandler = std::signal(SIGSEGV, handler);
	}
	// Add an entry to the back of the list.
	_buffers.resize(_buffers.size() + 1);
	//
	_entry = &_buffers[_buffers.size() - 1];
	// Set the jump.
	setjmp(_entry->_buf);
	// Prevent making the call.
	if (!_entry->_counter)
	{
		// Call the callback lambda.
		cb();
	}
}

SegmentFaultHandler::~SegmentFaultHandler()
{
	// Remove last entry in the vector.
	_buffers.pop_back();
	if (_buffers.empty())
	{
		signal(SIGSEGV, _savedHandler);
	}
}

void SegmentFaultHandler::handler(int cause)
{
	Entry& entry(_buffers[_buffers.size() - 1]);
	entry._counter++;
	longjmp(entry._buf, 0);// NOLINT(cert-err52-cpp)
}

SegmentFaultHandler::operator bool() const
{
	return _entry->_counter == 0;
}

__sighandler_t SegmentFaultHandler::_savedHandler = nullptr;

std::vector<SegmentFaultHandler::Entry> SegmentFaultHandler::_buffers;

}// namespace sf