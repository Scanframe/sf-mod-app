#include "FileMapper.h"
#include "../gen/Exception.h"
#include "../gen/gen_utils.h"
#include <sys/mman.h>
#include <unistd.h>

namespace sf::lnx
{

FileMapper::FileMapper(const std::string& filepath, size_t ofs, size_t sz)
	: IFileMapper()
{
	initialize(filepath, ofs, sz);
}

FileMapper::FileMapper()
	: IFileMapper()
{
}

FileMapper::~FileMapper()
{
	reset();
	if (_temp)
	{
		_file.unlink();
	}
}

void FileMapper::initialize()
{
	// Open a file using a template
	auto tpl = std::string("fm-").append(getExecutableName()).append("-{}.tmp");
	_file.createTemporary(std::filesystem::temp_directory_path().append(tpl));
	_temp = true;
}

void FileMapper::initialize(const std::string& filepath, size_t ofs, size_t sz)
{
	_file.initialize(filepath);
	// Only set when a size was passed.
	if (sz)
	{
		setView(ofs, sz);
	}
}

void FileMapper::reset()
{
	if (_counter)
	{
		// Make the Unlock() function really unlock by setting the counter to one.
		_counter = 1;
		//
		unlock();
	}
	// Reset some class members.
	_size = _offset = _realOffset = 0;
}

bool FileMapper::isLocked() const
{
	return _counter != 0;
}

void FileMapper::createView(size_t sz)
{
	_file.allocate(sz);
	setView(0, sz);
}

void FileMapper::setView(size_t ofs, size_t sz)
{
	if (_counter || _ptr)
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Cannot set a locked instance!");
	}
	// Check if offset and size are in range of the file.
	if ((ssize_t) (ofs + sz) > _file.getStatus(true).st_size)
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Offset + Size are out of range!");
	}
	// Assign the new values.
	_offset = ofs;
	_size = sz;
	// Get the page aligned offset.
	_realOffset = ofs & ~(::getpagesize() - 1);
}

size_t FileMapper::getSize() const
{
	return _size;
}

void* FileMapper::doLock(bool readonly)
{
	// Check if already locked.
	if (!_counter++)
	{
		// When still locked throw an exception.
		if (_ptr)
		{
			throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Tried locking locked instance!");
		}
		// Set flags to read.
		int flags = PROT_READ;
		// Add flag for writing when the file descriptor was opened writable.
		if ((_file.getFlags() & (O_WRONLY | O_RDWR)) && !readonly)
		{
			flags |= PROT_WRITE;
		}
		// Get the pointer to the memory map with the corrected size because of the alignment.
		_ptr = (char*) (::mmap(nullptr, _size + (_offset - _realOffset), flags, MAP_SHARED, _file.getDescriptor(), _realOffset));
		// Check for failure.
		if (_ptr == MAP_FAILED)
		{
			_ptr = nullptr;
			throw ExceptionSystemCall("mmap", errno, typeid(*this).name(), __FUNCTION__);
		}
	}
	// Return the previous or current locked pointer.
	return _ptr + (_offset - _realOffset);
}

void FileMapper::unlock(bool synchronise)
{
	// Decrement the counter and check for zero.
	if (!--_counter)
	{
		// Unlock the pointer.
		if (::munmap(_ptr, _size + (_offset - _realOffset)) == -1)
		{
			throw ExceptionSystemCall("munmap", errno, typeid(*this).name(), __FUNCTION__);
		}
		// Reset the pointer.
		_ptr = nullptr;
	}
	// When not unmapped sync the data.
	else if (synchronise)
	{
		// Sync asynchronously when required.
		sync(true);
	}
}

void FileMapper::sync(bool async)
{
	if (::msync(_ptr, _size + (_offset - _realOffset), async ? MS_ASYNC : MS_SYNC) == -1)
	{
		throw ExceptionSystemCall("msync", errno, typeid(*this).name(), __FUNCTION__);
	}
}

bool FileMapper::mapView()
{
	return doLock(false) != nullptr;
}

bool FileMapper::unmapView()
{
	unlock(true);
	return true;
}

void* FileMapper::getPtr()
{
	return _ptr + (_offset - _realOffset);
}

}// namespace sf::lnx
