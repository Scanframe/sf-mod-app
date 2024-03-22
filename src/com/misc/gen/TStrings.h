#pragma once

#include "TVector.h"

namespace sf
{

/**
 * @brief Counted vector of strings.
 */
template<typename T, typename CharType>
class TStrings : public TVector<T>
{
	public:
		/**
		 * @brief Base type of this template .
		 */
		typedef TVector<T> base_type;
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
		static constexpr size_t npos = base_type::npos;

		/**
		 * @brief Default constructor.
		 */
		TStrings() = default;

		/**
		 * @brief Initializing constructor using an iterator.
		 */
		template<typename InputIterator>
		TStrings(InputIterator first, InputIterator last)
			: base_type(first, last)
		{}

		/**
		 * @brief Initializer list constructor.
		 */
		explicit TStrings(std::initializer_list<value_type> list)
			: base_type(list)
		{}

		/**
		 * @brief Initializer list constructor.
		 */
		explicit TStrings(const base_type& list)
			: base_type(list)
		{}

		/**
		 * @brief Initializing constructor.
		 *
		 * @param sz Size of the vector.
		 */
		explicit TStrings(size_type sz)
			: base_type(sz)
		{}

		/**
		 * @brief Joins all entries using the passed glue.
		 *
		 * @param glue Separation string.
		 * @return Glued string.
		 */
		T join(const T& glue)
		{
			T rv;
			// Iterate through the vector.
			auto it = TVector<T>::begin();
			while (it < TVector<T>::end())
			{
				rv.append(*it++);
				// When not the end entry add the glue.
				if (it < TVector<T>::end())
				{
					rv.append(glue);
				}
			}
			return rv;
		}

		/**
		 * @brief Splits all entries using the passed separator.
		 *
		 * @param str Source string.
		 * @param separator Separation string.
		 * @return Separated string.
		 */
		TStrings<T, CharType>& explode(T str, const T& separator)
		{
			size_t ofs = 0, found = str.find_first_of(separator, ofs);
			if (found != std::string::npos)
			{
				do
				{
					base_type::push_back(str.substr(ofs, found - ofs));
					ofs = found + 1;
					if (ofs == T::npos)
					{
						break;
					}
					found = str.find_first_of(separator, ofs);
				} while (found != T::npos);
			}
			if (str.length() > ofs)
			{
				base_type::push_back(str.substr(ofs, str.length()));
			}
			return *this;
		}

		/**
		 * @brief Appends all split entries. Splits' the passed string using a separator and text delimiter.
		 * @param s Source string to split.
		 * @param sep Separator to split on which could be a comma character (',').
		 * @param delimiter Optional text delimiter which could be a double quote character ('"')
		 * @return Itself.
		 */
		TStrings<T, CharType>& split(const T& s, CharType sep, CharType delimiter = 0)
		{

			bool d = false;
			size_type p = 0, l = 0, i = 0;
			for (; i < s.length(); i++)
			{
				// When the character is a separator
				if (s[i] == sep && !d)
				{
					// First field is empty.
					if (i == 0)
					{
						base_type::append({});
					}
					else
					{
						p += i - l;
						base_type::append(s.substr(p, l));
						l = 0;
						p = 0;
					}
				}
				else
				{
					// if text delimiter was given.
					if (delimiter && s[i] == delimiter)
					{
						// Toggle delimiter flag when flag is encountered.
						d = !d;
						// Skip
						if (d)
						{
							p--;
						}
					}
					else
					{
						// Count the number of characters for the field.
						l++;
					}
				}
			}
			// Check for characters after last separator;
			if (l)
			{
				p += i - l;
				base_type::append(s.substr(p, l));
			}
			return *this;
		}
};

}// namespace sf
