#pragma once

#include "TDynamicBuffer.h"

namespace sf
{

/**
 * @brief A dynamic buffer as typed array.
 *
 * has range check on any index.
 *
 * @see ::sf::TDynamicBuffer
 */
template<class T, class Alloc = Allocator>
class TDynamicArray
{
	public:
		/**
		 * @brief Default constructor.
		 */
		TDynamicArray() = default;

		/**
		 * @brief Dynamic array constructor.
		 */
		explicit TDynamicArray(const TDynamicBuffer<Alloc>& buf)
			: _buffer(buf)
		{}

		/**
		 * @brief Initialization constructor.
		 */
		explicit TDynamicArray(size_t sz)
			: _buffer(sizeof(T) * sz)
		{}

		/**
		 * @brief Returns the size of the array.
		 */
		[[nodiscard]] size_t count() const
		{
			return _buffer.size() / sizeof(T);
		}

		/**
		 * @brief Returns the size of the array.
		 */
		[[nodiscard]] size_t size() const
		{
			return _buffer.size() / sizeof(T);
		}

		/**
		 * @brief Returns a typed pointer to the start of the array.
		 */
		void* data()
		{
			return _buffer.data();
		}

		/**
		 * @brief Returns reference of the specified index.
		 */
		T& operator[](size_t i)
		{
			return *static_cast<T*>(_buffer.data(sizeof(T) * i));
		}

		/**
		 * @brief Returns a typed pointer to the start of the array.
		 */
		explicit operator T*()
		{
			return static_cast<T*>(_buffer.data());
		}

		/**
		 * @brief Returns a typed pointer to the start of the array.
		 */
		explicit operator T*() const
		{
			return static_cast<T*>(_buffer.data());
		}

		/**
		 * @brief Returns a typed pointer to the start of the array.
 		 */
		operator const void*() const// NOLINT(google-explicit-constructor)
		{
			return static_cast<void*>(_buffer.data());
		}

		/**
		 * @brief Returns a typed pointer to the start of the array.
 		 */
		operator void*()// NOLINT(google-explicit-constructor)
		{
			return static_cast<void*>(_buffer.data());
		}

		/**
		 *
		 * @brief Resizes the array but does leave the data in tact.
		 * @see #::sf::TDynamicBuffer::resize()
		 */
		void resize(size_t sz, bool shrink = false)
		{
			_buffer.resize(sizeof(T) * sz, shrink);
		}

		/**
		 * @brief Grows the array to the specified size but keeps the current data intact.
		 */
		void grow(size_t sz)
		{
			_buffer.grow(sizeof(T) * sz);
		}

		/**
		 * @brief Zeroes all data.
		 */
		void zero()
		{
			_buffer.zero();
		}

		/**
		 * @brief Cast operator the TDynamic buffer.
		 */
		explicit operator TDynamicBuffer<Alloc>()
		{
			return _buffer;
		}

		/**
		 * @brief Gets access to the underlying buffer.
		 */
		TDynamicBuffer<Alloc>& getBuffer()
		{
			return _buffer;
		}

	protected:
		/**
		 * @brief Holds the underlying storage buffer.
		 */
		TDynamicBuffer<Alloc> _buffer;
};

}// namespace sf
