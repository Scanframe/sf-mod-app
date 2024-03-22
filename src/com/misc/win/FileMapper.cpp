#include "FileMapper.h"
#include "../gen/Exception.h"
#include "../gen/dbgutils.h"
#include "../gen/gen_utils.h"
#include <cstring>
#include <windows.h>

namespace sf::win
{

/**
 * Private data for Windows implementation.
 */
struct FileMapperPrivate
{
		// Open map view as read only.
		bool FlagReadOnlyView{false};
		//
		bool FlagClient{false};
		// Stores the file handle created by 'CreateFile' or '0xFFFFFFFF' to indicate page file mapping.
		HANDLE FileHandle{nullptr};
		// Contains the file name when
		std::string FileName;
		// Contains the handle of the mapped file region.
		HANDLE MapHandle{nullptr};
		// Contains the
		std::string MapName;
		// Contains the size of the map passed when 'CreateMap' was called.
		DWORD MapSize{0};
		// Points to the data when MapView was called.
		void* Data{nullptr};
};

std::string GetLastErrorString(DWORD error = 0)
{
	// Retrieve the last error when error is zero.
	if (!error)
	{
		error = ::GetLastError();
	}
	// Pointer to be filled in by the calling function
	LPSTR buffer = nullptr;
	// Function allocating the buffer which is freed by calling ::LocalFree().
	DWORD msglen = ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,// DWORD  dwFlags,  // source and processing options
																	0,// LPCVOID  lpSource,  // address of  message source
																	error,// DWORD  dwMessageId,  // requested message identifier
																	MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),// DWORD  dwLanguageId,  // language identifier for requested message
																	(LPSTR) &buffer,// LPTSTR  lpBuffer,  // address of message buffer
																	0,// DWORD  Size,  // maximum size of message buffer
																	0// va_list *  Arguments   // address of array of message inserts
	);
	// Form a string containing also the error code.
	char buf[65];
	// String return value to copy buffer into.
	std::string rv;
	rv += '(';
	rv += sf::itoa(error, buf, 10);
	rv += ')';
	rv += ' ';
	// Fill the returning string on success.
	if (msglen)
	{// remove the newline character at the end of the buffer if it exists.
		if (buffer[msglen - 1] == '\n')
		{
			buffer[msglen - 1] = '\0';
		}
		// Append this buffer to the existing return value.
		rv.append(buffer);
		// Free the buffer allocated by the FormatMessage function.
		::LocalFree(buffer);
	}
	// Return the string value.
	return rv;
}

FileMapper::FileMapper()
	: _data(new FileMapperPrivate())
{}

FileMapper::~FileMapper()
{
	closeFile();
	delete _data;
}

bool FileMapper::closeFile()
{
	// Close the map handle before closing the file handle.
	bool rv = closeMap();
	// Check if a file was opened or not before closing it.
	if (_data->FileHandle && _data->FileHandle != (HANDLE) 0xFFFFFFFFFFFFFFFF)
	{// Check for an error.
		if (!::CloseHandle(_data->FileHandle))
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "CloseHandle On File Failed!" << GetLastErrorString())
			rv = false;
		}
		// Assign zero so this handle cannot be closed again.
		_data->FileHandle = nullptr;
		// Clear the data members.
		_data->MapName.clear();
		_data->FileName.clear();
	}
	return rv;
}

void FileMapper::initialize()
{
	if (!_data->FileHandle)
	{
		// Assigning 0xFFFFFFFFFFFFFFFF makes function CreateFileMapping use the systems page file as backup.
		_data->FileHandle = (HANDLE) 0xFFFFFFFFFFFFFFFF;
	}
	else
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "PageFile file handle already created!")
	}
}

bool FileMapper::createMapFile(const char* filename)
{
	// Do not allow this function to be called when a file handle already exists.
	if (!_data->FileHandle)
	{
		// Assign the file name to the data member.
		_data->FileName = filename;
		// Create the file
		_data->FileHandle = CreateFileA(
			_data->FileName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		// Check for an error.
		if (_data->FileHandle == INVALID_HANDLE_VALUE)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "CreateFile(\"" << filename << "\") Failed!" << GetLastErrorString())
			_data->FileHandle = nullptr;
			return false;
		}
		// Assign false to indicate that this instance created the file map.
		_data->FlagClient = false;
		return true;
	}
	return false;
}

void FileMapper::createView(size_t sz)
{
	if (!createMap(stringf("FM%08X", rand()).c_str(), sz, true, false))// NOLINT(cert-msc50-cpp)
	{
		throw Exception("CreateMap(...) failed!");
	}
}

bool FileMapper::createMap(const char* map_name, size_t map_size, bool unique, bool readonly)
{
	// Do not allow calling of this function when a map is already opened.
	if (_data->MapHandle == nullptr)
	{
		_data->MapSize = map_size;
		_data->MapName = map_name;
		_data->MapHandle = ::CreateFileMappingA(
			_data->FileHandle,
			NULL,
			(readonly ? PAGE_READONLY : PAGE_READWRITE),
			0,
			_data->MapSize,
			_data->MapName.c_str()
		);

		// Check for a valid file handle
		if (_data->MapHandle == nullptr)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "CreateFileMapping Failed! " << GetLastErrorString())
		}
		else
		{// Check if map name already exists
			if (unique && ::GetLastError() == ERROR_ALREADY_EXISTS)
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, "CreateFileMapping Failed! " << GetLastErrorString(ERROR_ALREADY_EXISTS))
				// Check for error on closing this unwanted handle.
				if (!::CloseHandle(_data->MapHandle))
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "CloseHandle Of Mapping Failed! " << GetLastErrorString())
				}
				_data->MapHandle = nullptr;
			}
			else
			{
				_data->FlagReadOnlyView = false;
				return true;
			}
		}
	}
	return false;
}

bool FileMapper::openMap(const char* map_name, unsigned long map_size, bool readonly)
{
	// Only map when handle is not valid yet.
	if (_data->MapHandle == nullptr)
	{
		// Assign the map size to the data member.
		_data->MapSize = map_size;
		// Assign the passed name to the data member.
		_data->MapName = map_name;
		// Makes MapView call with readonly flags.
		_data->FlagReadOnlyView = readonly;
		// Open the existing map with map name and size.
		_data->MapHandle = ::OpenFileMappingA(FILE_MAP_READ | (readonly ? 0 : FILE_MAP_WRITE), TRUE, _data->MapName.c_str());
		// Check for error.
		if (_data->MapHandle == nullptr)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "OpenFileMapping(\"" << _data->MapName << "\") Failed! " << GetLastErrorString())
			return false;
		}

		// Assign true to indicate that this instance is client of the file mapping.
		_data->FlagClient = true;
		return true;
	}
	return false;
}

bool FileMapper::closeMap()
{
	// Unmap view before closing the map.
	bool rv = unmapView();
	// Only map when handle is not valid yet.
	if (_data->MapHandle != nullptr)
	{
		// Check for error.
		if (!::CloseHandle(_data->MapHandle))
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "CloseHandle Of Mapping Failed! " << GetLastErrorString())
			rv = false;
		}
	}
	// Clear the handle value.
	_data->MapHandle = nullptr;
	// Clear the data member that holds the size of the mapped region.
	_data->MapSize = 0;
	// Clear the map name.
	_data->MapName.clear();
	return rv;
}

bool FileMapper::mapView()
{
	// Can only be mapped when the data is not mapped yet.
	if (_data->Data == nullptr)
	{
		// When the MapHandle is valid this function is allowed.
		if (!_data->MapHandle)
		{
			return false;
		}
		// Map the view into memory.
		_data->Data = ::MapViewOfFile(
			_data->MapHandle,
			(_data->FlagReadOnlyView ? FILE_MAP_READ : (FILE_MAP_WRITE | FILE_MAP_READ)),
			0,
			0,
			_data->MapSize
		);
		// Check if an error occurred.
		if (_data->Data == nullptr)
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "MapViewOfFile Failed! " << GetLastErrorString())
			return false;
		}
	}
	return true;
}

bool FileMapper::unmapView()
{
	// Can only be unmapped when mapped was called before
	if (_data->Data != nullptr)
	{
		// Unmap the view into memory.
		if (!::UnmapViewOfFile(_data->Data))
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "UnmapViewOfFile Failed!" << GetLastErrorString())
			return false;
		}
		// Assign null to the data member to make unmapping impossible twice
		// for this instance when successful.
		_data->Data = nullptr;
	}
	return true;
}

bool FileMapper::flushView()
{
	if (!::FlushViewOfFile(_data->Data, _data->MapSize))
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "FlushViewOfFile Failed!" << GetLastErrorString())
		return false;
	}
	return true;
}

std::string FileMapper::getFileName() const
{
	return _data->FileName;
}

std::string FileMapper::getMapName() const
{
	return _data->MapName;
}

size_t FileMapper::getMapSize() const
{
	return _data->MapSize;
}

void* FileMapper::getPtr()
{
	return _data->Data;
}

const void* FileMapper::getPtr() const
{
	return _data->Data;
}

bool FileMapper::isMapOpen() const
{
	return _data->MapHandle != nullptr;
}

bool FileMapper::isFileOpen() const
{
	return _data->FileHandle != nullptr;
}

}// namespace sf::win