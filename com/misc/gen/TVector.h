#pragma once

#	include <vector>
#include <climits>
#include <iostream>

namespace sf
{

template<class T>
class TIterator;

/**
 * Counted vector having function names compatible with Borland C++ templates.
 * @tparam T Type contained by the vector.
 */
template<typename T>
class TVector :public std::vector<T>
{
	public:
		/**
		 * Base types for this template.
		 */
		typedef typename std::vector<T> base_t;
		typedef typename base_t::size_type size_type;
		typedef typename base_t::value_type value_type;
		typedef TIterator<value_type> iter_type;

		/**
		 * Value returned by various member functions when they fail.
		 */
		static const size_t npos = static_cast<size_type>(-1);

		/**
		 * Default constructor.
		 */
		TVector() = default;

		/**
		 * Initializing constructor using an iterator.
		 * @tparam InputIterator
		 * @param first
		 * @param last
		 */
		template<typename InputIterator>
		TVector(InputIterator first, InputIterator last)
			:base_t(first, last) {}

		/**
		 * Initializing constructor using list like:<br>
		 * <code>TVector vect{1,2,3,4,5,6,7}</code>
		 * @param list
		 */
		TVector(std::initializer_list<value_type> list)
			:base_t(list) {}

		/**
		 * Copy constructor for base type..
		 * @param sv
		 */
		explicit TVector(const base_t& sv)
			:base_t(sv) {}

		/**
		 * Initializing constructor.
		 * @param sz Size of the vector.
		 */
		explicit TVector(size_type sz)
			:base_t(sz) {}

		/**
		 * Adds item at the end of the vector.
		 */
		void add(const T&);

		/**
		 * Adds the vectors items at the end of the vector.
		 */
		void add(const TVector<T>&);

		/**
		 * Adds an item at index position.
		 * @param t Reference of instance.
		 * @param index Position where to add/insert.
		 * @return True on success.
		 */
		bool addAt(const T& t, size_type index);

		/**
		 * Removes specific item from the list by instance.
		 * @param t Reference of instance to detach.
		 * @return True on success.
		 */
		bool detach(const T& t);

		/**
		 * Removes specific item from the list by index.
		 * @param index Index of the item.
		 * @return True on success.
		 */
		bool detach(size_type index);

		/**
		 * Returns true when empty false otherwise.
		 * @return True when empty.
		 */
		[[nodiscard]] bool isEmpty() const
		{
			return base_t::empty();
		}

		/**
		 * Removes all entries from the vector.
		 */
		void flush()
		{
			base_t::erase(base_t::begin(), base_t::end());
		}

		/**
		 * Removes specific range of entries from the vector.
		 * @param stop
		 * @param start
		 */
		void flush(size_type stop, size_type start = 0)
		{
			base_t::erase(base_t::begin() + start,
				base_t::end() + ((stop >= base_t::size()) ? (base_t::size() - 1) : stop));
		}

		/**
		 * Finds an entry by instance in the vector.
		 * Returns (size_type)-1 when not found.
		 * @return Index of the fount item.
		 */
		size_type find(const T&) const;

		/**
		 * Returns the amount of entries in the vector.
		 * @return Entry count.
		 */
		[[nodiscard]] size_type count() const
		{
			return base_t::size();
		}

		/**
		 * Gets entry from index position.
		 * @param i  Index position
		 * @return Instance at position.
		 */
		T& get(size_type i)
		{
			return base_t::at(i);
		}

		/**
		 * Const version of getting entry from index position.
		 * @param i  Index position
		 * @return Instance at position.
		 */
		const T& get(size_type i) const
		{
			return base_t::at(i);
		}

		/**
		 * Returns the base type.
		 * @return
		 */
		base_t getBase()
		{
			return this;
		}

		/**
		 * Returns the constant const base type.
		 * @return Base type
		 */
		base_t getBase() const
		{
			return this;
		}

		/*
		 * Array operator needs reimplementation using std::vector::at() which does a range check
		 * in contrast to the std::vector::operator[] functions.
		 */
		#pragma clang diagnostic push
		#pragma ide diagnostic ignored "HidingNonVirtualFunction"

		/**
		 * Array operator
		 * @param i Index position
		 * @return Template type
		 */
		T& operator[](size_type i)
		{
			return base_t::at(i);
		}

		/**
		 * Const array operator.
		 * @param i Index position
		 * @return Template type
		 */
		const T& operator[](size_type i) const
		{
			return base_t::at(i);
		}

		#pragma clang diagnostic pop

};

/**
 *
 * @tparam T
 * @param os Output stream
 * @param v Value of the entry.
 * @return
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, TVector<T> const& v)
{
	os << "{";
	for (const auto& x: v)
	{
		os << x;
		// Check if this is the last entry in the list using pointer comparison instead of value.
		if (&(*(v.end() - 1)) != &x)
		{
			os << ", ";
		}
	}
	return os << "}";
}

/**
 * Counted vector having function names compatible with Borland C++ templates.
 * @tparam T Type contained by the TVector.
 */
template<typename T>
class TIterator
{
	public:
		typedef std::vector<T> base_t;

		explicit TIterator(const base_t& v)
		{
			_vector = const_cast<base_t*>(&v);
			restart(0, v.size());
		}

		TIterator(const base_t& v, unsigned start, unsigned stop)
		{
			_vector = &v;
			restart(start, stop);
		}

		operator int() const // NOLINT(google-explicit-constructor)
		{
			return _cur < _upper;
		}

		const T& current() const
		{
			return (*_vector)[_cur];
		}

		T& current()
		{
			return (*_vector)[_cur];
		}

		const T& operator++(int) // NOLINT(cert-dcl21-cpp)
		{
			const T& temp = current();
			_cur++;
			return temp;
		}

		const T& operator++()
		{
			_cur++;
			return current();
		}

		void restart()
		{
			restart(_lower, _upper);
		}

		void restart(unsigned start, unsigned stop)
		{
			_cur = _lower = start;
			_upper = stop;
		}

	private:
		base_t* _vector;
		unsigned _cur{};
		unsigned _lower{};
		unsigned _upper{};
};

template<typename T>
void TVector<T>::add(const T& t)
{
	// Insert item at the end.
	base_t::insert(base_t::end(), t);
}

template<typename T>
void TVector<T>::add(const TVector<T>& tv)
{
	// Insert item at the end.
	base_t::insert(base_t::end(), tv.begin(), tv.end());
}

template<typename T>
bool TVector<T>::addAt(const T& t, size_type index)
{
	// Check if index is in range.
	if (index > base_t::size())
	{
		return false;
	}
	if (index == base_t::size())
	{
		base_t::insert(base_t::end(), t);
	}
	else
	{
		base_t::insert(base_t::begin() + index, t);
	}
	return true;
}

template<typename T>
bool TVector<T>::detach(size_type index)
{
	// Check if index is in range.
	if (index >= base_t::size())
	{
		return false;
	}
	base_t::erase(base_t::begin() + index, base_t::begin() + index + 1);
	return true;
}

template<typename T>
bool TVector<T>::detach(const T& t)
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

template<typename T>
typename TVector<T>::size_type TVector<T>::find(const T& t) const
{
	for (size_type i = 0; i < base_t::size(); i++)
	{
		if (base_t::at(i) == t)
		{
			return i;
		}
	}
	return npos;
}

} // namespace
