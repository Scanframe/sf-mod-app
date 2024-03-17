#include <iostream>
#include <QException>
#include <QFileInfo>
#include <QDir>
#include "FileMapper.h"
#include "gen/Exception.h"

// TODO: This class is not fool proof yet.

namespace sf::qt
{

FileMapper::FileMapper()
	:_file(nullptr)
{
}

FileMapper::FileMapper(const QString& filepath, qint64 ofs, qint64 sz)
{
	initialize(filepath, ofs, sz);
}

FileMapper::~FileMapper()
{
	(void)this;
}

void FileMapper::initialize()
{
	_file.setFileTemplate(QDir::temp().filePath("temp-file-test-XXXXXX.tmp"));
	_file.setAutoRemove(true);
}


void FileMapper::initialize(const QString& filepath, qint64 ofs, qint64 sz)
{
	_file.setFileName(filepath);
	_file.setAutoRemove(false);
	setView(ofs, sz);
}

void FileMapper::setView(qint64 ofs, qint64 sz)
{
	if (_ptr)
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Cannot set a locked instance!");
	}
	// Check if offset and size are in range of the file.
	if ((ofs + sz) > QFileInfo(_file).size())
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Offset + Size are out of range!");
	}
	// Assign the new values.
	_offset = ofs;
	_size = sz;
}

QFile& FileMapper::getFile()
{
	return _file;
}

void* FileMapper::getPtr()
{
	return _ptr;
}

void FileMapper::createView(size_t sz)
{
	if (_ptr)
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__, "Cannot set a mapped instance!");
	}
	// Assign the size.
	_size = static_cast<qint64>(sz);
	// Open then file.
	if (!_file.open())
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"open '%s': (%i) %s", _file.fileName().toUtf8().data(), _file.error(), _file.errorString().toUtf8().data());
	}
	// Resize the file.
	if (!_file.resize(_size))
	{
		throw Exception().Function(typeid(*this).name(), __FUNCTION__,
			"resize '%s': (%i) %s", _file.fileName().toUtf8().data(), _file.error(), _file.errorString().toUtf8().data());
	}
}

bool FileMapper::mapView()
{
	if (_ptr == nullptr)
	{
		_ptr = _file.map(0, _size, QFile::MemoryMapFlag::NoOptions);
		if (!_ptr && _file.error() != QFile::NoError)
		{
			std::clog << _file.errorString().toStdString() << std::endl;
		}
	}
	return _ptr != nullptr;
}

bool FileMapper::unmapView()
{
	if (!_ptr)
	{
		return _file.unmap(_ptr);
	}
	return true;
}

}