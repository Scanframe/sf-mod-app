#pragma once

#include <cassert>
#include <cinttypes>
#include <climits>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <vector>

namespace sf
{

template<class T>
class TIterator;

/**
 * @brief Counted vector having additional methods and operators for ease of usage.
 *
 * This template class extends the std::vector template with easier to use methods for
 * adding finding entries and an array operator.
 *
 * @tparam T Type contained by the vector.
 */
template<typename T>
class TVector : public std::vector<T>
{
	public:
		/**
		 * @brief Base type of this template .
		 */
		typedef typename std::vector<T> base_type;
		/**
		 * @brief Size type of this template.
		 */
		typedef typename base_type::size_type size_type;
		/**
		 * @brief Value type contained by this vector template.
		 */
		typedef typename base_type::value_type value_type;
		/**
		 * @brief Iteration type of the template.
		 */
		typedef TIterator<value_type> iter_type;
		/**
		 * @brief Iteration const type of the template.
		 */
		typedef const iter_type const_iter_type;

		/**
		 * @brief Value returned by various member functions when they fail.
		 */
		static const size_t npos = static_cast<size_type>(-1);

		/**
		 * @brief Default constructor.
		 */
		TVector() = default;

		/**
		 * @brief Copy constructor.
		 */
		TVector(const TVector& v)
			: base_type(v)
		{}

		/**
		 * @brief Move constructor.
		 */
		TVector(TVector&&) noexcept = default;

		/**
		 * @brief Assignment operator.
		 */
		TVector& operator=(const TVector& v) noexcept
		{
			*((base_type*) this) = (base_type) v;
			return *this;
		}

		/**
		 * @brief Assignment move operator.
		 */
		TVector& operator=(TVector&& v) noexcept
		{
			*((base_type*) this) = (base_type) v;
			return *this;
		}

		/**
		 * @brief Initializing constructor using an iterator.
		 */
		template<typename InputIterator>
		TVector(InputIterator first, InputIterator last)
			: base_type(first, last)
		{}

		/**
		 * @brief Initializing constructor using list like:<br>
		 * `TVector vect{1,2,3,4,5,6,7}`
		 */
		TVector(std::initializer_list<value_type> list)
			: base_type(list)
		{}

		/**
		 * @brief Copy constructor for base type.
		 */
		explicit TVector(const base_type& sv)
			: base_type(sv)
		{}

		/**
		 * @brief Initializing constructor.
		 *
		 * @param sz Size of the vector.
		 */
		explicit TVector(size_type sz)
			: base_type(sz)
		{}

		/**
		 * @brief Adds item at the end of the vector.
		 *
		 * @return Start index of the inserted entry.
		 *
		 */
		size_type add(const T& t);

		/**
		 * @brief Adds item at the end of the vector.
		 *
		 * @return Start index of the inserted entry.
		 *
		 */
		size_type add(T&& t);

		/**
		 * @brief Appends an entry to the vectors items at the end of the vector but returns itself.
		 *
		 * @return Start index of the inserted entries.
		 */
		TVector<T>& append(const T& t);

		/**
		 * @brief Appends an entry to the vectors items at the end of the vector but returns itself.
		 *
		 * @return Start index of the inserted entries.
		 */
		TVector<T>& append(T&& t);

		/**
		 * @brief Adds the vectors items at the end of the vector.
		 *
		 * @return Start index of the inserted entries.
		 */
		size_type add(const TVector<T>&);

		/**
		 * @brief Adds an item at index position.
		 *
		 * @param t Reference of instance.
		 * @param index Position where to add/insert.
		 * @return True on success.
		 */
		bool addAt(const T& t, size_type index);

		/**
		 * @brief Adds an item at index position.
		 *
		 * @param t Reference of instance.
		 * @param index Position where to add/insert.
		 * @return True on success.
		 */
		bool addAt(T&& t, size_type index);

		/**
		 * @brief Removes specific item from the list by instance.
		 *
		 * Uses the compare operator from type T to find it.
		 * @param t Reference of instance to detach.
		 * @return True on success.
		 */
		bool detach(const T& t);

		/**
		 * @brief Removes specific item from the list by index.
		 *
		 * @param index Index of the item.
		 * @return True on success.
		 */
		bool detachAt(size_type index);

		/**
		 * @brief Returns true when empty false otherwise.
		 *
		 * @return True when empty.
		 */
		[[nodiscard]] bool isEmpty() const
		{
			return base_type::empty();
		}

		/**
		 * @brief Removes all entries from the vector.
		 */
		void flush()
		{
			base_type::erase(base_type::begin(), base_type::end());
		}

		/**
		 * @brief Removes specific range of entries from the vector.
		 *
		 * @param stop
		 * @param start
		 */
		void flush(size_type stop, size_type start = 0)
		{
			base_type::erase(base_type::begin() + start, base_type::end() + ((stop >= base_type::size()) ? (base_type::size() - 1) : stop));
		}

		/**
		 * @brief Finds an entry by instance in the vector.
		 *
		 * @return Index of the found item and 'npos' when not found.
		 */
		size_type find(const T&) const;

		/**
		 * @brief Returns the amount of entries in the vector.
		 *
		 * @return Entry count.
		 */
		[[nodiscard]] size_type count() const
		{
			return base_type::size();
		}

		/**
		 * @brief Gets the first element of the vector.
		 * @return Reference of the typed value.
		 */
		inline T& first()
		{
			assert(!base_type::empty());
			return *base_type::begin();
		}

		/**
		 * @brief Gets the first element of the vector.
		 * @return Const reference of the typed value.
		 */
		inline const T& first() const noexcept
		{
			static_assert(!base_type::empty());
			return *base_type::begin();
		}

		/**
		 * @brief Gets the last element of the vector.
		 * @return Const reference of the typed value.
		 */
		inline T& last()
		{
			assert(!base_type::empty());
			return *(base_type::end() - 1);
		}

		/**
		 * @brief Gets the last element of the vector.
		 * @return Const reference of the typed value.
		 */
		inline const T& last() const noexcept
		{
			assert(!base_type::empty());
			return *(base_type::end() - 1);
		}

		/**
		 * @brief Checks if the first element is of the passed value.
		 */
		inline bool startsWith(T t) const
		{
			return !base_type::empty() && first() == t;
		}

		/**
		 * @brief Checks if the last element is of the passed value.
		 */
		inline bool endsWith(T t) const
		{
			return !base_type::empty() && last() == t;
		}

		/**
		 * @brief Gets entry from index position.
		 *
		 * @param i  Index position
		 * @return Instance at position.
		 */
		inline T& get(size_type i)
		{
			return base_type::at(i);
		}

		/**
		 * @brief Const version of getting entry from index position.
		 *
		 * @param i  Index position
		 * @return Instance at position.
		 */
		const T& get(size_type i) const
		{
			return base_type::at(i);
		}

		/**
		 * @brief Returns the base type to access it methods explicitly.
		 *
		 * @return Base type
		 */
		base_type getBase()
		{
			return this;
		}

		/**
		 * @brief Returns the constant const base type.
		 *
		 * @return Base type
		 */
		base_type getBase() const
		{
			return this;
		}

		/**
		 * @brief Array operator
		 *
		 * Array operator needs reimplementation using std::vector::at() which does a range check
		 * in contrast to the std::vector::operator[] functions.
		 * @param i Index position
		 * @return Template type
		 */
		T& operator[](size_type i)
		{
			return base_type::at(i);
		}

		/**
		 * @brief Const array operator.
		 *
		 * Array operator needs reimplementation using std::vector::at() which does a range check
		 * in contrast to the std::vector::operator[] functions.
		 * @param i Index position
		 * @return Template type
		 */
		const T& operator[](size_type i) const
		{
			return base_type::at(i);
		}

		/**
		 * Writes the content to an output stream.
		 * @param os Output stream.
		 * @param inc_hex Include hex notation when an integer type.
		 * @return The passed output stream.
		 */
		std::ostream& write(std::ostream& os, bool inc_hex) const;
};

template<typename T>
std::ostream& TVector<T>::write(std::ostream& os, bool inc_hex) const
{
	os << '{';
	for (const auto& x: *this)
	{
		os << std::dec << x;
		if (inc_hex && std::is_integral<T>::value)
		{
			uint64_t _x{0};
			for (int i = 0; i < sizeof(T); i++)
			{
				((uint8_t*) &_x)[i] = ((uint8_t*) &x)[i];
			}
			os << " (" << std::hex << "0x" << _x << ')';
		}
		// Check if this is the last entry in the list using pointer comparison instead of value.
		if (&(*(base_type::end() - 1)) != &x)
		{
			os << ", ";
		}
	}
	return os << std::dec << '}';
}

/**
 * @brief Output stream operator.
 *
 * @tparam T Type of the vector.
 * @param os Output stream
 * @param v Value of the entry.
 * @return Stream
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, TVector<T> const& v)
{
	return v.write(os, true);
}

/**
 * @brief Counted vector having function names compatible with Borland C++ templates.
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
			restart(0, _vector->size());
		}

		TIterator(const base_t& v, size_t start, size_t stop)
		{
			_vector = &v;
			restart(start, stop);
		}

		operator int() const
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

		const T& operator++(int)
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
typename TVector<T>::size_type TVector<T>::add(const T& t)
{
	// Insert item at the end.
	return std::distance(base_type::begin(), base_type::insert(base_type::end(), t));
}

template<typename T>
typename TVector<T>::size_type TVector<T>::add(T&& t)
{
	// Insert item at the end.
	return std::distance(base_type::begin(), base_type::insert(base_type::end(), t));
}

template<typename T>
TVector<T>& TVector<T>::append(const T& t)
{
	// Insert item at the end.
	base_type::push_back(t);
	return *this;
}

template<typename T>
TVector<T>& TVector<T>::append(T&& t)
{
	// Insert item at the end.
	base_type::push_back(std::move(t));
	return *this;
}

template<typename T>
typename TVector<T>::size_type TVector<T>::add(const TVector<T>& tv)
{
	// Insert item at the end.
	return std::distance(base_type::begin(), base_type::insert(base_type::end(), tv.begin(), tv.end()));
}

template<typename T>
bool TVector<T>::addAt(const T& t, size_type index)
{
	// Check if index is in range.
	if (index > base_type::size())
	{
		return false;
	}
	if (index == base_type::size())
	{
		base_type::insert(base_type::end(), t);
	}
	else
	{
		base_type::insert(base_type::begin() + index, t);
	}
	return true;
}

template<typename T>
bool TVector<T>::addAt(T&& t, size_type index)
{
	// Check if index is in range.
	if (index > base_type::size())
	{
		return false;
	}
	if (index == base_type::size())
	{
		base_type::insert(base_type::end(), t);
	}
	else
	{
		base_type::insert(base_type::begin() + index, t);
	}
	return true;
}

template<typename T>
bool TVector<T>::detachAt(size_type index)
{
	// Check if index is in range.
	if (index >= base_type::size())
	{
		return false;
	}
	base_type::erase(base_type::begin() + index, base_type::begin() + index + 1);
	return true;
}

template<typename T>
bool TVector<T>::detach(const T& t)
{
	for (size_type i = 0; i < base_type::size(); i++)
	{
		if (base_type::at(i) == t)
		{
			base_type::erase(base_type::begin() + i, base_type::begin() + i + 1);
			return true;
		}
	}
	return false;
}

template<typename T>
typename TVector<T>::size_type TVector<T>::find(const T& t) const
{
	for (size_type i = 0; i < base_type::size(); i++)
	{
		if (base_type::at(i) == t)
		{
			return i;
		}
	}
	return npos;
}

}// namespace sf
