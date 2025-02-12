#pragma once

namespace sf
{

/**
 * @brief Envelope-letter type of smart pointers.
 *
 * In this implementation the envelope acts as a smart pointer to a reference-counted internal letter.
 * This allows a lot of flexibility & safety in working with a single object (associated with the letter)
 * being assigned to numerous envelopes in many scopes. An 'A' version is provided for use with arrays.
 * Use like:
 * <code>
 *  TEnvelope<T> e1 = new T(x,y,x);  // e1 now owns a T in a letter
 *  e1->Func();                      // invokes Func() on the new T
 *  Func(*e1);                       // passing a T& or a T this way
 *  TEnvelope<T> e2 = e1;            // e2 safely shares the letter with e1
 *
 *  TAEnvelope<T> e1 = new T[99];    // e1 now owns T[] in a letter
 *  e1[i].Func();                    // invokes Func() on a T element
 *  Func(e1[i]);                     // passing a T& or a T this way
 *  TAEnvelope<T> e2 = e1;           // e2 safely shares the letter with e1
 * </code>
*/
template<typename T>
class TEnvelope
{
	public:
		/**
		 * @brief Constructor for the envelope's letter object.
		 * @param letter Letter to wrap in the envelope.
		 */
		explicit TEnvelope(T* letter)
			: _letter(new TLetter(letter))
		{}

		/**
		 * @brief Copy constructor.
		 */
		TEnvelope(const TEnvelope& src)
			: _letter(src._letter)
		{
			_letter->addRef();
		}

		/**
		 * @brief Move constructor.
		 */
		TEnvelope(const TEnvelope&& src) noexcept
			: _letter(src._letter)
		{}

		~TEnvelope()
		{
			_letter->release();
		}

		TEnvelope& operator=(const TEnvelope& src);

		TEnvelope& operator=(T* letter);

		T* operator->()
		{
			return _letter->Object;
		}

		T& operator*()
		{
			return *_letter->Object;
		}

		T* operator()()
		{
			return _letter->Object;
		}

		explicit operator T*()
		{
			return _letter->Object;
		}

		[[nodiscard]] int referencedCount() const
		{
			return _letter ? _letter->_refCount : 0;
		}

	private:
		struct TLetter
		{
				explicit TLetter(T* object)
					: Object(object)
					, _refCount(1)
				{}

				~TLetter()
				{
					delete Object;
				}

				void addRef()
				{
					_refCount++;
				}

				void release()
				{
					if (--_refCount == 0)
					{
						delete this;
					}
				}

				T* Object;
				int _refCount;
		};

		TLetter* _letter;
};

//
template<typename T>
TEnvelope<T>& TEnvelope<T>::operator=(const TEnvelope<T>& src)
{
	if (this != &src)
	{
		_letter->release();
		_letter = src._letter;
		_letter->addRef();
	}
	return *this;
}

template<typename T>
TEnvelope<T>& TEnvelope<T>::operator=(T* object)
{
	_letter->release();
	_letter = new TLetter(object);// Assumes non-null! Use with new
	return *this;
}

/**
* @brief class TAEnvelope
*/
template<typename T>
class TAEnvelope
{
	public:
		explicit TAEnvelope(T array[])
			: _letter(new TLetter(array))
		{}

		TAEnvelope(const TAEnvelope& src)
			: _letter(src._letter)
		{
			_letter->addRef();
		}

		~TAEnvelope()
		{
			_letter->release();
		}

		TAEnvelope& operator=(const TAEnvelope& src);

		TAEnvelope& operator=(T array[]);

		T& operator[](int i)
		{
			return _letter->_array[i];
		}

		T* operator*()
		{
			return _letter->_array;
		}

	private:
		struct TLetter
		{
				explicit TLetter(T array[])
					: _array(array)
					, _refCount(1)
				{}

				~TLetter()
				{
					delete[] _array;
				}

				void addRef()
				{
					_refCount++;
				}

				void release()
				{
					if (--_refCount == 0)
					{
						delete this;
					}
				}

				T* _array;
				int _refCount;
		};

		TLetter* _letter;
};

template<typename T>
TAEnvelope<T>& TAEnvelope<T>::operator=(const TAEnvelope<T>& src)
{
	if (this != &src)
	{
		_letter->release();
		_letter = src._letter;
		_letter->addRef();
	}
	return *this;
}

template<typename T>
TAEnvelope<T>& TAEnvelope<T>::operator=(T arr[])
{
	_letter->release();
	// Assumes non-null! Use with new
	_letter = new TLetter(arr);
	return *this;
}

}// namespace sf
