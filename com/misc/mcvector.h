#ifndef MCVECTOR_H
#define MCVECTOR_H

#	include <vector>
#include <limits.h>

//
template <class T>
class mcvector : public std::vector<T>
{
	public:
		// Base type for this temnplate.
		typedef std::vector<T> base_t;

		// Default constructor.
		mcvector() {}

		// Initializing constructor.
		template <typename _InputIterator>
		mcvector(_InputIterator first, _InputIterator last)
			:base_t(first, last) {}

		//
		mcvector(const base_t& sv)
			: base_t(sv) {}

		// Initializing constructor.
		mcvector(size_t sz)
			: base_t(sz) {}

		// Adds item at the end of the vector.
		bool Add(const T&);
		// Adds an item at index.
		bool AddAt(const T&, size_t index);

		// Removes specific item from the list by instance.
		bool Detach(const T& t);
		// Removes specific item from the list by index
		bool Detach(size_t index);

		// Returns non zero when empty.
		int IsEmpty() const
		{
			return base_t::empty();
		}

		// Removes all entries from the vector.
		void Flush()
		{
			base_t::erase(base_t::begin(), base_t::end());
		}

		// Removes specific range of entries from the vector.
		void Flush(size_t stop, size_t start = 0)
		{
			base_t::erase(base_t::begin() + start,
				base_t::end() + ((stop >= base_t::size()) ? (base_t::size() - 1) : stop));
		}

		// Finds an entry by instance in the vector.
		// Returns (size_t)-1 when not found.
		size_t Find(const T&) const;

		// Returns the amount of entries in the vector.
		size_t Count() const
		{
			return base_t::size();
		}

		size_t Limit() const
		{
			return base_t::size();
		}

		// Resizes (adds or truncates) the vector.
		void Resize(size_t n)
		{
			base_t::resize(n);
		}

		// Reserves space in the vector.
		void Reserve(size_t n)
		{
			base_t::reserve(n);
		}

		// Returns the pointer to the allocated array for this vector.
		const T* Data() const
		{
			// Looks strange but is necessary for overloaded operator to be
			// called on the iterator.
			return &(*base_t::begin());
		}

		// Returns the pointer to the allocated array for this vector.
		T* Data()
		{
			// Looks strange but is necessary for overloaded operator to be
			// called on the iterator.
			return &(*base_t::begin());
		}

		// Array operators for easy access.
		T& operator[](size_t i)
		{
			return base_t::at(i);
		}

		const T& operator[](size_t i) const
		{
			return base_t::at(i);
		}

		// Get function when the vector is passed as a pointer.
		T& Get(unsigned i)
		{
			return base_t::at(i);
		}

		const T& Get(size_t i) const
		{
			return base_t::at(i);
		}

		// Returns the base type.
		base_t GetBase()
		{
			return this;
		}

		// Returns the constant base type.
		const base_t GetBase() const
		{
			return this;
		}
};

template <class T>
class mciterator
{
	public:
		typedef std::vector<T> base_t;

		mciterator(const base_t& v)
		{
			Vect = const_cast<base_t*>(&v);
			Restart(0, v.size());
		}

		mciterator(const base_t& v, unsigned start, unsigned stop)
		{
			Vect = &v;
			Restart(start, stop);
		}

		operator int() const
		{
			return Cur < Upper;
		}

		const T& Current() const
		{
			return (*Vect)[Cur];
		}

		T& Current()
		{
			return (*Vect)[Cur];
		}

		const T& operator++(int)
		{
			const T& temp = Current();
			Cur++;
			return temp;
		}

		const T& operator++()
		{
			Cur++;
			return Current();
		}

		void Restart()
		{
			Restart(Lower, Upper);
		}

		void Restart(unsigned start, unsigned stop)
		{
			Cur = Lower = start;
			Upper = stop;
		}

	private:
		base_t* Vect;
		unsigned Cur;
		unsigned Lower, Upper;
};

template <class T>
bool mcvector<T>::Add(const T& t)
{
	// Insert item at the end.
	base_t::insert(base_t::end(), t);
	return true;
}

template <class T>
bool mcvector<T>::AddAt(const T& t, size_t loc)
{
	if (loc > base_t::size())
	{
		return true;
	}
	if (loc == base_t::size())
	{
		base_t::insert(base_t::end(), t);
	}
	else
	{
		base_t::insert(base_t::begin() + loc, t);
	}
	return true;
}

template <class T>
bool mcvector<T>::Detach(size_t loc)
{
	if (loc >= base_t::size())
	{
		return false;
	}
	base_t::erase(base_t::begin() + loc, base_t::begin() + loc + 1);
	return true;
}

template <class T>
bool mcvector<T>::Detach(const T& t)
{
	for (unsigned i = 0; i < base_t::size(); i++)
	{
		if (base_t::at(i) == t)
		{
			base_t::erase(base_t::begin() + i, base_t::begin() + i + 1);
			return true;
		}
	}
	return false;
}

template <class T>
size_t mcvector<T>::Find(const T& t) const
{
	for (size_t i = 0; i < base_t::size(); i++)
	{
		if (base_t::at(i) == t)
		{
			return i;
		}
	}
	return (size_t) -1;
}

#endif // MCVECTOR_H_
