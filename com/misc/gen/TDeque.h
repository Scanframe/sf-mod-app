#pragma once

#include <deque>
#include <climits>
#include <iostream>
#include <sstream>
#include <iterator>

namespace sf
{

/**
 * @brief Counted deque having additional methods and operators for ease of usage.
 *
 * This template class extends the std::deque template with easier to use methods for
 * adding finding entries and an array operator.
 *
 * @tparam T Type contained by the deque.
 */
template<typename T>
class TDeque :public std::deque<T>
{
	public:
		/**
		 * @brief Base type of this template .
		 */
		typedef typename std::deque<T> base_type;
		/**
		 * @brief Size type of this template.
		 */
		typedef typename base_type::size_type size_type;
		/**
		 * @brief Value type contained by this deque template.
		 */
		typedef typename base_type::value_type value_type;

		/**
		 * @brief Value returned by various member functions when they fail.
		 */
		static const size_t npos = static_cast<size_type>(-1);

		/**
		 * @brief Default constructor.
		 */
		TDeque() = default;

		/**
		 * @brief Initializing constructor using an iterator.
		 */
		template<typename InputIterator>
		TDeque(InputIterator first, InputIterator last)
			:base_type(first, last) {}

		/**
		 * @brief Initializing constructor using list like:<br>
		 * `TDeque que{1,2,3,4,5,6,7}`
		 */
		TDeque(std::initializer_list<value_type> list)
			:base_type(list) {}

		/**
		 * @brief Copy constructor for base type.
		 */
		explicit TDeque(const base_type& sv)
			:base_type(sv) {}

		/**
		 * @brief Initializing constructor.
		 *
		 * @param sz Size of the deque.
		 */
		explicit TDeque(size_type sz)
			:base_type(sz) {}

		/**
		 * @brief Adds item at the end of the deque.
		 *
		 * @return Start index of the inserted entry.
		 */
		size_type add(const T&);

		/**
		 * @brief Adds the deques items at the end of the deque.
		 *
		 * @return Start index of the inserted entries.
		 */
		size_type add(const TDeque<T>&);

		/**
		 * @brief Adds an item at index position.
		 *
		 * @param t Reference of instance.
		 * @param index Position where to add/insert.
		 * @return True on success.
		 */
		bool addAt(const T& t, size_type index);

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
		 * @brief Removes all entries from the deque.
		 */
		void flush()
		{
			base_type::erase(base_type::begin(), base_type::end());
		}

		/**
		 * @brief Removes specific range of entries from the deque.
		 *
		 * @param stop
		 * @param start
		 */
		void flush(size_type stop, size_type start = 0)
		{
			base_type::erase(base_type::begin() + start,
				base_type::end() + ((stop >= base_type::size()) ? (base_type::size() - 1) : stop));
		}

		/**
		 * @brief Finds an entry by instance in the deque.
		 *
		 * @return Index of the found item and 'npos' when not found.
		 */
		size_type find(const T&) const;

		/**
		 * @brief Returns the amount of entries in the deque.
		 *
		 * @return Entry count.
		 */
		[[nodiscard]] size_type count() const
		{
			return base_type::size();
		}

		/**
		 * @brief Gets entry from index position.
		 *
		 * @param i  Index position
		 * @return Instance at position.
		 */
		T& get(size_type i)
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

		/**
		 * @brief Array operator
		 *
		 * Array operator needs reimplementation using std::deque::at() which does a range check
		 * in contrast to the std::deque::operator[] functions.
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
		 * Array operator needs reimplementation using std::deque::at() which does a range check
		 * in contrast to the std::deque::operator[] functions.
		 * @param i Index position
		 * @return Template type
		 */
		const T& operator[](size_type i) const
		{
			return base_type::at(i);
		}

#pragma clang diagnostic pop

		/**
		 * Writes the content to an output stream.
		 * @param os Output stream.
		 * @param inc_hex Include hex notation when an integer type.
		 * @return The passed output stream.
		 */
		std::ostream& write(std::ostream& os, bool inc_hex) const;
};

template<typename T>
std::ostream& TDeque<T>::write(std::ostream& os, bool inc_hex) const
{
	os << '{';
	for (const auto& x: *this)
	{
		os << std::dec << x;
		if (inc_hex && std::numeric_limits<T>::is_integer)
		{
			os << " (" << std::hex << "0x" << x << ')';
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
std::ostream& operator<<(std::ostream& os, TDeque<T> const& v)
{
	return v.write(os, true);
}

template<typename T>
typename TDeque<T>::size_type TDeque<T>::add(const T& t)
{
	// Insert item at the end using iterator.
	return std::distance(base_type::begin(), base_type::insert(base_type::end(), t));
}

template<typename T>
typename TDeque<T>::size_type TDeque<T>::add(const TDeque<T>& tv)
{
	// Insert item at the end.
	return std::distance(base_type::begin(), base_type::insert(base_type::end(), tv.begin(), tv.end()));
}

template<typename T>
bool TDeque<T>::addAt(const T& t, size_type index)
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
bool TDeque<T>::detachAt(size_type index)
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
bool TDeque<T>::detach(const T& t)
{
	for (unsigned i = 0; i < base_type::size(); i++)
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
typename TDeque<T>::size_type TDeque<T>::find(const T& t) const
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

} // namespace
