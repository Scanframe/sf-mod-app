/*
_type defining of virtual allocation template class for
dynamic buffering class.
 Implements template TDynamicBuffer and TDynamicArray
*/

#pragma once

#include <malloc.h>
#include <cassert>

#include "dbgutils.h"
#include "../global.h"

namespace sf
{

/**
 * Memory allocator class.
 */
struct Allocator
{
	/**
	 * Dummy type for allocators which need handles.
	 */
	typedef int handle_t;

	/**
	 * Allocates the memory for a dynamic buffer.
	 */
	static void* allocmem(handle_t&, size_t sz)
	{
		auto rv = ::malloc(sz);
		assert(rv);
		return rv;
	}

	/**
	 * Reallocates the memory for a dynamic buffer.
	 */
	static void* reallocmem(handle_t&, void* p, size_t sz)
	{
		auto rv = ::realloc(p, sz);
		assert(rv);
		return rv;
	}

	/**
	 * Frees the memory for a dynamic buffer.
	 */
	static void freemem(handle_t, void*& p)
	{
		::free(p);
		p = nullptr;
	}
};

template <class Alloc>
class TDynamicBuffer
{
	public:
		/**
		 * @brief Default constructor.
		 */
		TDynamicBuffer();
		/**
		 * @brief Copy constructor.
		 */
		TDynamicBuffer(const TDynamicBuffer& db);
		/**
		 * @brief Initializing constructor.
		 */
		explicit TDynamicBuffer(size_t sz);
		/**
		 *  @brief Destructor
		 */
		~TDynamicBuffer();
		/**
		 * @brief Reserves the array but does leave the data in tact.
		 */
		void reserve(size_t sz, bool shrink = false);
		/**
		 * @brief Resizes the array but does leave the data in tact.
		 *
		 * @param sz When 0 and shrink is true the underlying buffer is freed.
		 * @param shrink
		 */
		void resize(size_t sz, bool shrink = false);
		/**
		 * @brief Sets an virtual offset to the buffer.
		 */
		void offset(size_t ofs);
		/**
		 * @brief Returns the current offset.
		 */
		[[nodiscard]] size_t offset() const;
		/**
		 * @brief Returns the byte size of the buffer.
		 */
		size_t size();
		/**
		 * @brief Returns the byte size of the buffer.
		 */
		[[nodiscard]] size_t size() const;
		/**
		 * Returns the reserved size of this instance.
		 */
		[[nodiscard]] size_t reserved() const;
		/**
		 * Returns a pointer to the start of the buffer.
		 */
		void* data();

		template<typename T>
		T* ptr(size_t offset = 0)
		{
			// Compensate for the offset.
			offset += _reference->_offset;
			return (T*)((char*) _reference->_data + offset);
		}

		/**
		 * Returns a const pointer to the start of the buffer.
		 */
		[[nodiscard]] const void* data() const;
		/**
		 * Returns a character pointer to the start of the buffer.
		 */
		char* c_str();
		/**
		 * Returns a const character pointer to the start of the buffer.
		 */
		[[nodiscard]] const char* c_str() const;
		/**
		 * Grows the array to the specified size but keeps the current data intact.
		 */
		void grow(size_t sz);
		/**
		 * Zeros all current memory size or all reserved.
		 */
		void zero(bool reserved = false);
		/**
		 * Sets all memory to the given character.
		 * When reserved is true not only the current size but all of it.
		 */
		void set(int c = 0, bool reserved = false);
		/**
		 * Returns a void pointer with a byte offset from the start.
		 */
		void* data(size_t offset);
		/**
		 * Returns a const void pointer with a byte offset from the start.
		 */
		[[nodiscard]] const void* data(size_t offset) const;
		/**
		 * Cast operator to pointer.
		 */
		explicit operator void*();
		/**
		 * Const cast operators to pointer.
		 */
		explicit operator const char*() const;
		/**
		 * Cast operator to pointer.
		 */
		explicit operator char*();
		/**
		 * Const cast operators to pointers.
		 */
		explicit operator const void*() const;
		/**
		 * Character operator to access a single byte element.
		 */
		char& operator[](size_t i);
		/**
		 * Const character operator to access a single byte element.
		 */
		char operator[](size_t i) const;
		/**
		 * Assignment operators.
		 */
		TDynamicBuffer& operator=(const TDynamicBuffer& db);

	private:
		// Structure which to keep track of the buffer when passed between instances.
		struct Reference
		{
			typename Alloc::handle_t Handle;
			// Determines if a destructor frees the memory.
			unsigned _usage;
			// Reported size of the buffer.
			size_t _size;
			// Actual allocated size of memory.
			size_t _allocated;
			// Byte offset to the data.
			size_t _offset;
			// Pointer to the allocated chunk of memory.
			void* _data;
		} * _reference;
};

template <class Alloc>
inline
size_t TDynamicBuffer<Alloc>::size()
{
	return _reference->_size - _reference->_offset;
}

template <class Alloc>
inline
size_t TDynamicBuffer<Alloc>::size() const
{
	return _reference->_size - _reference->_offset;
}

template <class Alloc>
inline
void* TDynamicBuffer<Alloc>::data()
{
	return static_cast<char*>(_reference->_data) + _reference->_offset;
}

template <class Alloc>
inline
const void* TDynamicBuffer<Alloc>::data() const
{
	return static_cast<char*>(_reference->_data) + _reference->_offset;
}

template <class Alloc>
inline
char* TDynamicBuffer<Alloc>::c_str()
{
	return static_cast<char*>(_reference->_data) + _reference->_offset;
}

template <class Alloc>
inline
const char* TDynamicBuffer<Alloc>::c_str() const
{
	return static_cast<char*>(_reference->_data) + _reference->_offset;
}

template <class Alloc>
inline
void* TDynamicBuffer<Alloc>::data(size_t offset)
{
	// Compensate for the offset.
	offset += _reference->_offset;
	return &(static_cast<char*>(_reference->_data))[offset];
}

template <class Alloc>
inline
const void* TDynamicBuffer<Alloc>::data(size_t offset) const
{
	// Compensate for the offset.
	offset += _reference->_offset;
	return &(static_cast<char*>(_reference->_data))[offset];
}

template <class Alloc>
inline
char TDynamicBuffer<Alloc>::operator[](size_t i) const
{
	// Compensate for the offset.
	i += _reference->_offset;
	if (i > _reference->_size)
	{
		throw std::out_of_range("Index out of range!");
	}
	// Make sure that no data is accessed beyond its real size.
	return (static_cast<char*>(_reference->_data))[i];
}

template <class Alloc>
inline
char& TDynamicBuffer<Alloc>::operator[](size_t i)
{
	if (i > _reference->_size)
	{
		throw std::out_of_range("Index out of range!");
	}
	// Make sure that no dta is accessed beyond its real size.
	return (static_cast<char*>(_reference->_data))[i];
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>& TDynamicBuffer<Alloc>::operator=(const TDynamicBuffer<Alloc>& db)
{
	// Prevent self copying.
	if (this == &db)
	{
		return *this;
	}
	// Avoid assigning same instance.
	if (_reference == db._reference)
	{
		return *this;
	}
	// Decrease usage count.
	_reference->_usage--;
	// If the usage is zero the reference and data buffer in it must be deleted.
	if (_reference->_usage == 0)
	{
		Alloc::freemem(_reference->Handle, _reference->_data);
		delete _reference;
	}
	// Assign reference of passed instance.
	_reference = db._reference;
	// Increment usage count.
	_reference->_usage++;
	// Return myself.
	return *this;
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>::operator void*()
{
	return _reference->_data;
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>::operator const void*() const
{
	return _reference->_data;
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>::operator char*()
{
	return _reference->_data;
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>::operator const char*() const
{
	return _reference->_data;
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>::TDynamicBuffer()
{
	_reference = new Reference;
	_reference->_size = 0;
	_reference->_allocated = 0;
	_reference->_offset = 0;
	_reference->_data = nullptr;
	_reference->_usage = 1;
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>::TDynamicBuffer(const TDynamicBuffer& db)
{
	_reference = db._reference;
	_reference->_usage++;
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>::TDynamicBuffer(size_t sz)
{
	_reference = new Reference;
	_reference->_size = 0;
	_reference->_allocated = 0;
	_reference->_offset = 0;
	_reference->_data = nullptr;
	_reference->_usage = 1;
	resize(sz);
}

template <class Alloc>
inline
TDynamicBuffer<Alloc>::~TDynamicBuffer()
{
	_reference->_usage--;
	// If the usage is zero the reference and data buffer in it must be deleted.
	if (_reference->_usage == 0)
	{
		if (_reference->_data)
		{
			Alloc::freemem(_reference->Handle, _reference->_data);
		}
		delete _reference;
	}
}

template <class Alloc>
inline
void TDynamicBuffer<Alloc>::reserve(size_t sz, bool shrink)
{
	if (sz == 0)
	{
		if (shrink)
		{
			_reference->_allocated = 0;
			_reference->_offset = 0;
			if (_reference->_data)
			{
				Alloc::freemem(_reference->Handle, _reference->_data);
			}
		}
	}
	else
	{
		// Compensate for the offset.
		sz += _reference->_offset;
		// Only allocate memory when the requested size is larger or
		// if the buffer size is allowed to shrink and the new size is smaller.
		if ((sz > _reference->_allocated) || (sz < _reference->_allocated && shrink))
		{ // Determine what is to be called.
			if (_reference->_data)
			{
				_reference->_data = Alloc::reallocmem(_reference->Handle, _reference->_data, sz);
			}
			else
			{
				_reference->_data = Alloc::allocmem(_reference->Handle, sz);
			}
			// Adjust the real size member.
			_reference->_allocated = sz;
		}
	}
}

template <class Alloc>
inline
void TDynamicBuffer<Alloc>::offset(size_t ofs)
{
	// Assign the new offset.
	_reference->_offset = ofs;
}

template <class Alloc>
inline
size_t TDynamicBuffer<Alloc>::offset() const
{
	return _reference->_offset;
}

template <class Alloc>
inline
void TDynamicBuffer<Alloc>::resize(size_t sz, bool shrink)
{
	// When resizing to zero the offset can ben cleared too.
	if (sz)
	{
		sz += _reference->_offset;
	}
	else
	{
		_reference->_offset = 0;
	}
	// Make sure enough space is there.
	reserve(sz, shrink);
	// Assign the new virtual size of this buffer.
	_reference->_size = sz;
}

template <class Alloc>
void TDynamicBuffer<Alloc>::grow(size_t sz)
{
	// Compensate with the offset.
	sz += _reference->_offset;
	// Only when there is a change.
	if (sz != _reference->_size)
	{
		// Check if the current allocated space is sufficient.
		if (sz <= _reference->_allocated)
		{
			// Assign the new virtual size.
			_reference->_size = sz;
		}
		else
		{
			// Make a copy of the reference.
			Reference ref = *_reference;
			// Reset the reference to the initial values except the usage member.
			_reference->_size = 0;
			_reference->_allocated = 0;
			_reference->_offset = 0;
			_reference->_data = nullptr;
			// Make resize fill the ref again and also allocate the new larger
			// block of memory.
			resize(sz - _reference->_offset);
			// Copy the old data to the new allocated memory.
			memcpy(_reference->_data, ref._data, ref._size);
			// Delete the previous allocated memory.
			if (ref._data)
			{
				Alloc::freemem(ref.Handle, ref._data);
			}
		}
	}
}

template <class Alloc>
inline
void TDynamicBuffer<Alloc>::set(int c, bool reserved)
{
	memset(_reference->_data, c, reserved ? _reference->_allocated : _reference->_size);
}

template <class Alloc>
inline
void TDynamicBuffer<Alloc>::zero(bool reserved)
{
	set(0, reserved);
}

template <class Alloc>
inline
size_t TDynamicBuffer<Alloc>::reserved() const
{
	return _reference->_allocated - _reference->_offset;
}

/**
 * Actual specialized dynamic buffer type.
 */
typedef TDynamicBuffer<Allocator> DynamicBuffer;

} // namespace
