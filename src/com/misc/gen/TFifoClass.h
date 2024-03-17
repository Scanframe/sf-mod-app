#pragma once

namespace sf
{

/**
 * @brief Fifo template for classes and structure pointers.
 *
 * Provides an array operator to iterate through the fifo.
 * Allows peeking for the next without popping the item.
 */
template<class T>
class TFifoClass
{
	public:
		/**
		 * Type used for size and position.
		 */
		typedef int size_type;

		/**
		 * @brief Default constructor which needs Set() to validate object
		 */
		TFifoClass()
		{
			initialize();
		}

		/**
		 * @brief Constructor that will setup the object immediately
		 */
		explicit TFifoClass(size_type size)
		{
			initialize();
			set(size);
		}

		/**
		 * @brief Destructor.
		 */
		~TFifoClass();

		/**
		 * @brief Pushes item into buffer.
		 * @param item Item pushed.
		 * @return True when successful (not full).
		 */
		bool push(const T& item);

		/**
		 * @brief Push item of zero in fifo
		 */
		bool push();

		/**
		 * @brief Pushes multiple items if possible else it returns '0' and no items are inserted at all
		 */
		bool push(const T* item, size_type count);

		/**
		 * @brief Returns next item without removing it.
		 */
		const T& peek() const;

		/**
		 * @brief Gives latest item pushed into the fifo.
		 */
		const T& latest() const;

		/**
		 * @brief Read item at head of buffer and removes it.
		 */
		T pop();

		/**
		 * @brief Same as pop(void) but returns 'false' when the buffer is empty.
		 */
		bool pop(T& item);

		/**
		 * @brief Returns if current contained buffer data size is zero.
		 */
		[[nodiscard]] bool empty() const;

		/**
		 * @brief Returns current buffer contained data size.
		 */
		[[nodiscard]] size_type size() const;

		/**
		 * @brief Returns maximum containable size.
		 */
		[[nodiscard]] size_type sizeMax() const
		{
			return (_valid) ? (_bufSize - 1) : 0;
		}

		/**
		 * @brief Returns remaining available size.
		 */
		[[nodiscard]] size_type sizeRemain() const
		{
			return _valid ? (sizeMax() - size()) : 0;
		}

		/**
		 * @brief Returns 1 if object is valid else 0
		 */
		[[nodiscard]] bool isValid() const
		{
			return _valid;
		}

		/**
		 * @brief Returns '1' if object is full else 0
		 */
		[[nodiscard]] bool isFull() const
		{
			return sizeRemain() ? 0 : 1;
		}

		/**
		 * @brief Resets instance to the initial state.
		 */
		void clear()
		{
			_head = _tail = 0;
		}

		/**
		 * @brief InitializeBase object after default construction or to increase size
		 *
		 * @return 1 if valid else 0.
		 */
		bool set(size_type size);

		/**
		 * @brief Array operator offset from head of buffer.
		 */
		T& operator[](size_type pos);

		/**
		 * @brief Const array operator offset from head of buffer.
		 */
		const T& operator[](size_type pos) const;

		/**
		 * @brief Access function for private member.
		 */
		[[nodiscard]] size_type getTail() const;

		/**
		 * @brief Access function for private member.
		 */
		[[nodiscard]] size_type getHead() const;

		/**
		 * @brief Access function for private member.
		 */
		const T* getBuffer(size_type pos = 0) const;

	private:
		void initialize();

		size_type _tail{0};
		size_type _head{0};
		T* _buffer{nullptr};
		size_type _valid{0};
		size_type _bufSize{0};
		T _zero;
};

/**
 * @cond Doxygen_ignore
 * Doxygen 1.8.17 breaks on the functions having 'typename TFifoClass<T>::size_type' in it.
 */

template<class T>
TFifoClass<T>::~TFifoClass()
{
	if (_valid)
	{
		delete[] _buffer;
	}
}

template<class T>
void TFifoClass<T>::initialize()
{
	_valid = 0;
	_bufSize = 0;
	_head = 0;
	_tail = 0;
	_buffer = nullptr;
	memset(&_zero, 0, sizeof(T));
}

template<class T>
bool TFifoClass<T>::set(TFifoClass<T>::size_type size)
{
	if (size < 1)
	{
		return false;
	}
	_bufSize = size + 1;
	_head = _tail = 0;
	if (_buffer)
	{
		delete[] _buffer;
	}
	_buffer = new T[_bufSize];
	memset(_buffer, 0, sizeof(T) * _bufSize);
	_valid = true;
	return true;
}

template<class T>
bool TFifoClass<T>::push(const T& item)
{
	if ((((_tail + 1) % _bufSize)) == _head)
	{
		return false;
	}
	_buffer[_tail++] = item;
	_tail %= _bufSize;
	return true;
}

template<class T>
bool TFifoClass<T>::push(const T* item, TFifoClass<T>::size_type count)
{
	if (sizeRemain() < count)
	{
		return true;
	}
	for (size_type i = 0; i < count; push(item[i++])) {}
	return false;
}

template<class T>
bool TFifoClass<T>::push()
{
	return push(_zero);
}

template<class T>
const T& TFifoClass<T>::peek() const
{
	if (_head == _tail)
	{
		return _zero;
	}
	return _buffer[_head];
}

template<class T>
const T& TFifoClass<T>::latest() const
{
	if (_head == _tail)
	{
		return _zero;
	}
	return _buffer[(((_tail + _bufSize - 1) % _bufSize))];
}

template<class T>
T TFifoClass<T>::pop()
{
	if (_tail == _head)
	{
		return _zero;
	}
	T tmp = _buffer[_head++];
	_head %= _bufSize;
	return tmp;
}

template<class T>
bool TFifoClass<T>::pop(T& item)
{
	if (_tail == _head)
	{
		item = _zero;
		return false;
	}
	item = _buffer[_head++];
	_head %= _bufSize;
	return true;
}

template<class T>
bool TFifoClass<T>::empty() const
{
	return size() == 0;
}

template<class T>
typename TFifoClass<T>::size_type TFifoClass<T>::size() const
{
	return (_tail < _head) ? (_bufSize + (_tail - _head)) : (_tail - _head);
}

template<class T>
T& TFifoClass<T>::operator[](TFifoClass<T>::size_type pos)
{
	return _buffer[(pos + _head) % _bufSize];
}

template<class T>
const T& TFifoClass<T>::operator[](TFifoClass<T>::size_type pos) const
{
	return _buffer[(pos + _head) % _bufSize];
}

template<class T>
inline
const T* TFifoClass<T>::getBuffer(TFifoClass<T>::size_type pos) const
{
	return &_buffer[pos];
}

template<class T>
inline
typename TFifoClass<T>::size_type TFifoClass<T>::getTail() const
{
	return _tail;
}

template<class T>
inline
typename TFifoClass<T>::size_type TFifoClass<T>::getHead() const
{
	return _head;
}

/** @endcond Doxygen_ignore */
}
