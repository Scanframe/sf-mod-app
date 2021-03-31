#pragma once

namespace sf
{

/**
 * @brief Fifo template for classes and structures.
 */
template<class T>
class TFifoClass
{
	public:
		/**
		 * @brief Default constructor which needs Set() to validate object
		 */
		TFifoClass()
		{
			Init();
		}

		/**
		 * @brief Constructor that will setup the object immediately
		 */
		explicit TFifoClass(int size)
		{
			Init();
			Set(size);
		}

		/**
		 * @brief Destructor.
		 */
		~TFifoClass();

		/*
		 * @brief Put item in buffer if successful it returns '1' else '0'
		 */
		int Put(const T& item);

		/**
		 * @brief Put item of zero in fifo
		 */
		inline
		int Put()
		{
			return Put(Zero);
		}

		/**
		 * @brief Put multiple items if possible else it returns '0' and no items are inserted at all
		 */
		int Put(const T* item, int count);

		/**
		 * @brief Returns next item without removing it.
		 */
		const T& Peek() const;

		/**
		 * @brief Gives latest item put into the fifo.
		 */
		const T& Latest() const;

		/**
		 * @brief Read item at head of buffer.
		 */
		T Get();

		/**
		 * @brief Same as Read(void) this time '0' is return when buffer is empty.
		 */
		int Get(T& item);

		/**
		 * @brief Returns current buffer data size.
		 */
		int Size() const;

		/**
		 * @brief Returns maximum containable size.
		 */
		int SizeMax() const
		{
			return (Valid) ? (BufSize - 1) : 0;
		}

		/**
		 * @brief Returns remaining available size.
		 */
		int SizeRemain() const
		{
			return Valid ? (SizeMax() - Size()) : 0;
		}

		/**
		 * @brief Returns 1 if object is valid else 0
		 */
		int IsValid() const
		{
			return Valid;
		}

		/**
		 * @brief Returns '1' if object is full else 0
		 */
		int IsFull() const
		{
			return SizeRemain() ? 0 : 1;
		}

		/**
		 * @brief Resets instance to the initial state.
		 */
		void Clear()
		{
			Head = Tail = 0;
		}

		/**
		 * @brief InitializeBase object after default construction or to increase size
		 *
		 * @return 1 if valid else 0.
		 */
		int Set(int size);

		/**
		 * @brief Array operator offset from head of buffer.
		 */
		T& operator[](int pos);

		/**
		 * @brief Const array operator offset from head of buffer.
		 */
		const T& operator[](int pos) const;

		/**
		 * @brief Access function for private member.
		 */
		int GetTail() const;

		/**
		 * @brief Access function for private member.
		 */
		int GetHead() const;

		/**
		 * @brief Access function for private member.
		 */
		const T* GetBuffer(int pos = 0) const;

	private:
		int Tail{};
		int Head{};
		T* Buffer;

		void Init();

		int Valid{};
		int BufSize{};
		T Zero;
};

template<class T>
TFifoClass<T>::~TFifoClass()
{
	if (Valid)
	{
		delete[] Buffer;
	}
}

template<class T>
void TFifoClass<T>::Init()
{
	Valid = 0;
	BufSize = 0;
	Head = Tail = 0;
	Buffer = nullptr;
	memset(&Zero, 0, sizeof(T));
}

template<class T>
int TFifoClass<T>::Set(int size)
{
	if (size < 1)
	{
		return 0;
	}
	BufSize = size + 1;
	Head = Tail = 0;
	if (Buffer)
	{
		delete[] Buffer;
	}
	Buffer = new T[BufSize];
	memset(Buffer, 0, sizeof(T) * BufSize);
	Valid = Buffer ? 1 : 0;
	return Valid;
}

template<class T>
int TFifoClass<T>::Put(const T& item)
{
	if ((((Tail + 1) % BufSize)) == Head)
	{
		return 0;
	}
	Buffer[Tail++] = item;
	Tail %= BufSize;
	return 1;
}

template<class T>
int TFifoClass<T>::Put(const T* item, int count)
{
	if (SizeRemain() < count)
	{
		return 0;
	}
	for (int i = 0; i < count; Put(item[i++])) {}
	return 1;
}

template<class T>
const T& TFifoClass<T>::Peek() const
{
	if (Head == Tail)
	{
		return Zero;
	}
	return Buffer[Head];
}

template<class T>
const T& TFifoClass<T>::Latest() const
{
	if (Head == Tail)
	{
		return Zero;
	}
	return Buffer[(((Tail + BufSize - 1) % BufSize))];
}

template<class T>
T TFifoClass<T>::Get()
{
	if (Tail == Head)
	{
		return Zero;
	}
	T tmp = Buffer[Head++];
	Head %= BufSize;
	return tmp;
}

template<class T>
int TFifoClass<T>::Get(T& item)
{
	if (Tail == Head)
	{
		item = Zero;
		return 0;
	}
	item = Buffer[Head++];
	Head %= BufSize;
	return 1;
}

template<class T>
int TFifoClass<T>::Size() const
{
	return (Tail < Head) ? (BufSize + (Tail - Head)) : (Tail - Head);
}

template<class T>
T& TFifoClass<T>::operator[](int pos)
{
	return Buffer[(pos + Head) % BufSize];
}

template<class T>
const T& TFifoClass<T>::operator[](int pos) const
{
	return Buffer[(pos + Head) % BufSize];
}

template<class T>
const T* TFifoClass<T>::GetBuffer(int pos) const
{
	return &Buffer[pos];
}

template<class T>
int TFifoClass<T>::GetTail() const
{
	return Tail;
}

template<class T>
int TFifoClass<T>::GetHead() const
{
	return Head;
}

} // namespace
