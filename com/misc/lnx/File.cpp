#include <sys/mman.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include "../gen/dbgutils.h"
#include "../lnx/lnx_utils.h"
#include "../gen/TDynamicBuffer.h"
#include "../gen/Exception.h"

#include "File.h"

namespace sf::lnx
{

std::ostream& operator<<(std::ostream& os, const File& file)
{
	return os << file.getStatText();
}

File::File()
	:_mode(0)
	 , _flags(0)
	 , _descriptor(-1)
	 , _errorNo(0)
{
	::memset(&_stat, 0, sizeof(_stat));
}

File::File(const std::filesystem::path& path, int flags, mode_t mode)
	:_mode(0)
	 , _flags(0)
	 , _descriptor(-1)
	 , _errorNo(0)
{
	::memset(&_stat, 0, sizeof(_stat));
	initialize(path, flags, mode);
}

File::~File()
{
	// Just close the file on destruction.
	close();
}

int File::setErrorNo(int error_no)
{
	return _errorNo = error_no;
}

void File::open(bool reopen)
{
	// When reopen is issued close the file first.
	if (reopen)
	{
		close();
	}
	// When still open bailout.
	if (isOpen())
	{
		return;
	}
	// Try opening.
	_descriptor = ::open(_path.c_str(), _flags, _mode);
	if (_descriptor == -1)
	{
		setErrorNo((int) errno);
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"File (%d, %d) '%s' failed!\n%s", _flags, _mode, _path.c_str(), strerror(_errorNo));
	}
	// Retrieve stats on opening.
	getStat(_stat);
}

void File::initialize(const std::filesystem::path& path, int flags, mode_t mode)
{
	// Close the file before opening another one.
	close();
	// Just assign the data members.
	_path = path;
	_flags = flags;
	_mode = mode;
	// Retrieve the Stat structure when the file exists.
	getStatus(true);
}

void File::createTemporary(const std::string& name_tpl, int flags)
{
	// Set the minimal flags needed for mkostemps().
	_flags = flags | O_RDWR | O_CREAT;
	// Copy string to manipulate.
	std::string tpl(name_tpl);
	// Close the file before opening another one.
	close();
	// Token to replace.
	std::string token("{}");
	// Get the location of the token.
	auto pos = tpl.find_last_of(token);
	// When not found throw an exception.
	if (pos == std::string::npos)
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"Template is missing token '%s' is not open!", token.c_str());
	}
	// Replace the token for the needed characters.
	tpl.replace(pos - 1, token.length(), "XXXXXX");
	// Determine the suffix length.
	auto suffix_len = name_tpl.length() - pos - 1;
	// Create an open file descriptor.
	_descriptor = mkostemps(const_cast<char*>(tpl.c_str()), suffix_len, flags);
	// Check for an error.
	if (_descriptor == -1)
	{
		setErrorNo((int) errno);
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"mkostemps('%s', ...) failed!\n%s", tpl.c_str(), strerror(_errorNo));
	}
	// Assign the path using the file descriptor.
	_path = file_fd_path(_descriptor);
	// Adjust the stats.
	getStatus(true);

}

void File::open(const std::string& path, int flags, mode_t mode)
{
	// Call the ini function which closes an open file.
	initialize(path, flags, mode);
	// Calls open function to reopen makeing sure the current file is closed first.
	open(false);
}

void File::allocate(size_t sz)
{
	if (posix_fallocate(_descriptor, 0, sz))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"posix_fallocate() failed!\n%s", strerror(_errorNo));
	}
}

bool File::close(bool exceptions)
{
	// To close the file must be open.
	if (!isOpen())
	{
		return true;
	}
	// Check for failure.
	if (::close(_descriptor) == -1)
	{
		setErrorNo((int) errno);
		if (exceptions)
		{
			throw Exception().Function(typeid(*this).name(), __FUNCTION__,
				"Closing file '%s' failed!\n%s", _path.c_str(), strerror(_errorNo));
		}
		else
		{
			_RTTI_NOTIFY(DO_DEFAULT, "Closing file '" << _path << "' failed!\n" << strerror(_errorNo))
		}
		// Reset the descriptor anyway.
		_descriptor = -1;
		// Signal failure.
		return false;
	}
	// Reset the descriptor.
	_descriptor = -1;
	// Signal success.
	return true;
}

void File::write(const void* buf, size_t sz, size_t* written)
{
	// Check if the file is open.
	if (!isOpen())
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"File '%s' is not open!", _path.c_str());
	}
	// Write the buffer to file.
	ssize_t wr = ::write(_descriptor, buf, sz);
	// Check for an error.
	if (wr == -1)
	{
		setErrorNo((int) errno);
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"Writting to '%s' failed!\n%s", _path.c_str(), strerror(_errorNo));
	}
		// Check if all was written
	else if ((size_t) wr != sz)
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"Writing all data (%llu/%llu) to file '%s' failed!", _path.c_str(), wr, sz);
	}
	// Assign the written return value when passed.
	if (written)
	{
		*written = wr;
	}
}

void File::write(const File& file, size_t pos, size_t sz, size_t buf_sz)
{
	// Create buffer top copy chunks of data.
	DynamicBuffer buf;
	// Set the reserved size for the max allowed chunk of data.
	buf.reserve(buf_sz ? buf_sz : 1024 * 16);
	//
	for (size_t ws = 0; ws < sz; ws += buf.size())
	{
		//RTTI_NOTIFY(DO_DEF, "pos: " << (pos + ws) << " size: " << std::min(buf.reserved(), sz - ws));
		// Read the chunk of data from file.
		file.read(buf, pos + ws, std::min(buf.reserved(), sz - ws));
		// When zero we try to read beyond the end of the file.
		if (!buf.size())
		{
			break;
		}
		// Write the chunk of data to file.
		write(buf.data(), buf.size());
	}
}

void File::read(DynamicBuffer buf, size_t pos, size_t sz) const
{
	// Make the buffer have enough space to accomodate the the requested
	// size and for a possible the terminating nul character.
	buf.reserve(sz);
	// Resize the buffer to what was read.
	buf.resize(read(buf.data(), pos, sz));
}

ssize_t File::read(void* buf, size_t pos, size_t sz) const
{
	// Read from the current position.
	ssize_t ret = ::pread(_descriptor, buf, sz, pos);
	if (ret == -1)
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"Reading from file '%s' at position (%llu) length (%llu) failed!", _path.c_str(), pos, sz);
	}
	return ret;
}

void File::rename(const std::string& path, bool assign)
{
	// Close the file when open.
	close();
	// Check if the file exists.
	if (!::access(_path.c_str(), F_OK))
	{
		// Read from the current position.
		if (::rename(_path.c_str(), path.c_str()) == -1)
		{
			throw Exception().Function(typeid(*this).name(), __FUNCTION__,
				"Renaming file '%s' to '%s' failed!\n%s", _path.c_str(), path.c_str(), strerror(errno));
		}
	}
	// Update the Path member using the new file name.
	if (assign)
	{
		_path = path;
	}
}

bool File::exists()
{
	return std::filesystem::exists(_path);
}

void File::unlink()
{
	if (exists())
	{
		if (::unlink(_path.c_str()) == 1)
		{
			throw ExceptionSystemCall("unlink", errno, typeid(*this).name(), __FUNCTION__);
		}
	}
}

void File::truncate(size_t length)
{
	// Truncate can be performed on the open or closed file.
	if (isOpen())
	{
		if (::ftruncate(_descriptor, length) == -1)
		{
			throw ExceptionSystemCall("truncate", errno, typeid(*this).name(), __FUNCTION__);
		}
	}
	else
	{
		if (::truncate(_path.c_str(), length) == -1)
		{
			throw ExceptionSystemCall("truncate", errno, typeid(*this).name(), __FUNCTION__);
		}

	}
	// Adjust the stats.
	getStatus(true);
}

void File::remove()
{
	// Close an open file first.
	close();
	// Check if the file exists.
	if (!::access(_path.c_str(), F_OK))
	{
		// Remove the file an throw an exception on failure.
		if (::unlink(_path.c_str()) == -1)
		{
			throw Exception().Function(typeid(*this).name(), __FUNCTION__,
				"Removing file '%s' failed!\n%s", _path.c_str(), strerror(_errorNo));
		}
	}
	// Clear the stat
	memset(&_stat, 0, sizeof(_stat));
}

void File::synchronise()
{
	// Check if the file is open.
	if (isOpen())
	{
		if (::fsync(_descriptor) == -1)
		{
			throw Exception().Function(typeid(*this).name(), __FUNCTION__,
				"Syncing file '%s' failed!\n%s", _path.c_str(), strerror(_errorNo));
		}
	}
}

void File::getStat(stat_type& stat) const
{
	if (!isOpen() && _path.empty())
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"File '%s' has not been opened yet!", _path.c_str(), strerror(_errorNo));
	}
	// When open use the file descriptor function.
	if (isOpen())
	{
		// Do the stat function on the descriptor.
		if (::fstat(_descriptor, &stat) == -1)
		{
			const_cast<File*>(this)->_errorNo = errno;
			throw Exception().Function(typeid(*this).name(), __FUNCTION__,
				"Statting file '%s' on descriptor failed!\n%s", _path.c_str(), strerror(_errorNo));
		}
	}
		// Do the stat function on the the filename.
		// Check if the file exists.
	else if (!::access(_path.c_str(), F_OK))
	{
		if (::stat(_path.c_str(), &stat) == -1)
		{
			const_cast<File*>(this)->_errorNo = errno;
			throw Exception().Function(typeid(*this).name(), __FUNCTION__,
				"Statting file '%s' on path failed!\n%s", _path.c_str(), strerror(_errorNo));
		}
	}
	else
	{
		::memset(&stat, 0, sizeof(stat));
	}
}

std::string File::getStatText() const
{
	std::ostringstream os(std::ostringstream::out);
	//
	os << "File type: ";
	switch (_stat.st_mode & S_IFMT)
	{
		case S_IFBLK:
			os << "block device" << std::endl;
			break;
		case S_IFCHR:
			os << "character device" << std::endl;
			break;
		case S_IFDIR:
			os << "directory" << std::endl;
			break;
		case S_IFIFO:
			os << "FIFO/pipe" << std::endl;
			break;
		case S_IFLNK:
			os << "symlink" << std::endl;
			break;
		case S_IFREG:
			os << "regular file" << std::endl;
			break;
		case S_IFSOCK:
			os << "socket" << std::endl;
			break;
		default:
			os << "unknown?" << std::endl;
			break;
	}
	//
	os << "I-node number: " << _stat.st_ino << std::endl;
	os << "Mode: " << _stat.st_mode << " (octal)" << std::endl;
	os << "Link count: " << _stat.st_nlink << std::endl;
	os << "Ownership: uid=" << _stat.st_uid << ", gid=" << _stat.st_gid << std::endl;
	os << "Preferred I/O block size: " << _stat.st_blksize << " bytes" << std::endl;
	os << "File size: " << _stat.st_size << " bytes" << std::endl;
	os << "Blocks allocated: " << _stat.st_blocks << std::endl;
	os << "Last status change: " << ctime(&_stat.st_ctime);
	os << "Last file access: " << ctime(&_stat.st_atime);
	os << "Last file modification: " << ctime(&_stat.st_mtime);
	//
	return os.str();
}

}
