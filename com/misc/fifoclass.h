#ifndef MISC_FIFOCLASS_H
#define MISC_FIFOCLASS_H

#include <cstring>

namespace sf
{

/**
 * Fifo template for classes and structures.
 */
template <class T>
class TFifoClass
{
	public:
		/**
		 * Default constructor which needs Set() to validate object
		 */
		TFifoClass()
		{
			Init();
		}

		/**
		 * Constructor that will setup the object immediately
		 */
		explicit TFifoClass(int size)
		{
			Init();
			Set(size);
		}

		/**
		 * Destructor.
		 */
		~TFifoClass();
		/*
		 * Put item in buffer if successful it returns '1' else '0'
		 */
		int Put(const T& item);

		/**
		 * Put item of zero in fifo
		 */
		inline
		int Put()
		{
			return Put(Zero);
		}

		/**
		 * Put multiple items if possible else it returns '0'
		 * and no items are inserted at all
		 */
		int Put(const T* item, int count);

		/**
		 * returns next item without removing it.
		 */
		const T& Peek() const;

		/**
		 * Gives latest item put into the fifo.
		 */
		const T& Latest() const;

		/**
		 * Read item at head of buffer.
		 */
		T Get();

		/**
		 * Same as Read(void) this time '0' is return when buffer is empty.
		 */
		int Get(T& item);

		/**
		 * Returns current buffer data size.
		 */
		int Size() const;

		/**
		 * Returns maximum containable size.
		 */
		int SizeMax() const
		{
			return (Valid) ? (BufSize - 1) : 0;
		}

		/**
		 * Returns remaining available size.
		 */
		int SizeRemain() const
		{
			return Valid ? (SizeMax() - Size()) : 0;
		}

		/**
		 * Returns 1 if object is valid else 0
		 */
		int IsValid() const
		{
			return Valid;
		}

		/**
		 * Returns '1' if object is full else 0
		 */
		int IsFull() const
		{
			return SizeRemain() ? 0 : 1;
		}

		/**
		 * Reset Buffer
		 */
		void Clear()
		{
			Head = Tail = 0;
		}

		/**
		 * Initialize object after default construction or to increase size
		 * Returns 1 if valid else 0.
		 */
		int Set(int size);

		/**
		 * Array operator offset from head of buffer.
		 */
		T& operator[](int pos);
		const T& operator[](int pos) const;

		/**
		 * Access function for private member.
		 */
		inline
		int GetTail() const
		{
			return Tail;
		}

		/**
		 * Access function for private member.
		 */
		inline
		int GetHead() const
		{
			return Head;
		}

		/**
		 * Access function for private member.
		 */
		inline
		const T* GetBuffer(int pos = 0) const
		{
			return &Buffer[pos];
		}

	private:
		int Tail{};
		int Head{};
		T* Buffer;
		void Init();
		int Valid{};
		int BufSize{};
		T Zero;
};

template <class T>
TFifoClass<T>::~TFifoClass()
{
	if (Valid)
	{
		delete[] Buffer;
	}
}

template <class T>
void TFifoClass<T>::Init()
{
	Valid = 0;
	BufSize = 0;
	Head = Tail = 0;
	Buffer = NULL;
	memset(&Zero, 0, sizeof(T));
}

template <class T>
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

template <class T>
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

template <class T>
int TFifoClass<T>::Put(const T* item, int count)
{
	if (SizeRemain() < count)
	{
		return 0;
	}
	for (int i = 0; i < count; Put(item[i++]))
	{}
	return 1;
}

template <class T>
const T& TFifoClass<T>::Peek() const
{
	if (Head == Tail)
	{
		return Zero;
	}
	return Buffer[Head];
}

template <class T>
const T& TFifoClass<T>::Latest() const
{
	if (Head == Tail)
	{
		return Zero;
	}
	return Buffer[(((Tail + BufSize - 1) % BufSize))];
}

template <class T>
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

template <class T>
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

template <class T>
int TFifoClass<T>::Size() const
{
	return (Tail < Head) ? (BufSize + (Tail - Head)) : (Tail - Head);
}

template <class T>
T& TFifoClass<T>::operator[](int pos)
{
	return Buffer[(pos + Head) % BufSize];
}

template <class T>
const T& TFifoClass<T>::operator[](int pos) const
{
	return Buffer[(pos + Head) % BufSize];
}

} // namespace sf

#endif // MISC_FIFOCLASS_H
