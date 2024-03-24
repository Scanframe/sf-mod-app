#include "BufferStitcher.h"

namespace sf
{

void BufferStitcher::assign(char* data, size_t sz)
{
	_data = data;
	_size = sz;
	_pos = 0;
}

char* BufferStitcher::getWritePointer() const
{
	return _data ? &_data[_pos] : nullptr;
}

size_t BufferStitcher::getWriteSize() const
{
	if (_data)
	{
		return _size - _pos;
	}
	return 0;
}

bool BufferStitcher::movePosition(size_t sz)
{
	if (!_data || _pos + sz >= _size)
	{
		_data = nullptr;
	}
	else
	{
		_pos += sz;
	}
	return isDone();
}

bool BufferStitcher::isDone()
{
	return !_data;
}

}