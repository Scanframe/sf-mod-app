#pragmna once

namespace sf
{

/**
* @brief A pair of smart pointer template classes. Provides basic conversion
* operator to T*, as well as dereference (*), and 0-checking (!).
* These classes assume that they alone are responsible for deleting the
* object or array unless Relinquish() is called.
*/
template<typename T>
class TPointerBase
{
	public:
		T& operator*() { return *P; }

		explicit operator T*() { return P; }

		int operator!() const { return P == 0; }

		T* Relinquish()
		{
			T* p = P;
			P = 0;
			return p;
		}

	protected:
		explicit TPointerBase(T* pointer)
			: P(pointer)
		{}

		TPointerBase()
			: P(0)
		{}

		T* P;

	private:
		// Prohibit use of new
		void* operator new(size_t) noexcept { return nullptr; }

		// Delete only sets pointer to null.
		void operator delete(void* p)
		{
			static_cast<TPointerBase<T>*>(p)->P = nullptr;
		}
};

/**
* @brief Pointer to a single object. Provides member access operator ->
*/
template<typename T>
class TPointer : public TPointerBase<T>
{
	private:
		typedef TPointerBase<T> TBase;

	public:
		TPointer()
			: TBase()
		{}

		explicit TPointer(T* pointer)
			: TBase(pointer)
		{}

		~TPointer() { delete TBase::P; }

		TPointer<T>& operator=(T* src)
		{
			delete TBase::P;
			TBase::P = src;
			return *this;
		}

		// Could throw exception if P==0
		T* operator->()
		{
			return TBase::P;
		}
};

/**
* @brief Pointer to an array of type T. Provides an array subscript operator and uses array delete[]
*/
template<typename T>
class TAPointer : public TPointerBase<T>
{
	private:
		typedef TPointerBase<T> TBase;

	public:
		TAPointer()
			: TBase()
		{}

		explicit TAPointer(T array[])
			: TBase(array)
		{}

		~TAPointer()
		{
			delete[] TBase::P;
		}

		TAPointer<T>& operator=(T src[])
		{
			delete[] TBase::P;
			TBase::P = src;
			return *this;
		}

		T& operator[](int i)
		{
			// Could throw exception if P==0
			return TBase::P[i];
		}
};

}// namespace sf
