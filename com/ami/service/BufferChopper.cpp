#include "BufferChopper.h"

namespace sf
{

BufferChopper::BufferChopper(size_t chopSize)
{
	_chopSize = chopSize;
}

void BufferChopper::assign(char* data, size_t sz)
{
	_data = data;
	_size = sz;
	_pos = 0;
}

bool BufferChopper::moveNext()
{
	_pos += _chopSize;
	// When moving the position beyond the end of the buffer work is done.
	if (_pos >= _size)
	{
		_data = nullptr;
		_pos = 0;
	}
	return !!_data;
}

bool BufferChopper::isDone() const
{
	// When the data is not null it is not done yet.
	return !_data;
}

char* BufferChopper::getChunkData()
{
	return &_data[_pos];
}

size_t BufferChopper::getChunkSize() const
{
	if (!_data)
	{
		return 0;
	}
	if (_pos + _chopSize < _size)
	{
		return _chopSize;
	}
	return _size - _pos;
}

}
