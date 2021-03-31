#pragma once

#include <climits>
#include "TSet.h"

namespace sf
{

/**
 * @brief Template for creating large memory inexpensive bitmasks or bit sets.
 *
 * @tparam _size Amount of bits needed.
 */
template<size_t _size>
class TBitSet
{
	public:
		/**
		 * @brief Default constructor.
		 */
		TBitSet();

		/**
		 * @brief Tests if a bit has been Set.
		 */
		bool Has(int bit);

		/**
		 * @brief Sets a single bit.
		 */
		void Set(int bit);

		/**
		 * @brief Sets a single bit.
		 */
		void Reset(int bit);

		/**
		 * @brief Clears all bits.
		 */
		void Clear();

		/**
		 * @brief Returns true if one of the bits has been Set.
		 */
		bool IsClear();

	private:
		/**
		 * @brief Holds the actual data.
		 */
		int _mask[_size / sizeof(int) + 1]{};
};

template<size_t Size>
inline
TBitSet<Size>::TBitSet()
	:_mask({0})
{
	(void) Size;
	Clear();
}

template<size_t Size>
inline
bool TBitSet<Size>::Has(int bit)
{
	if (bit < Size && bit >= 0)
	{
		return (_mask[bit / (sizeof(int) * CHAR_BIT)] & (1 << (bit % (sizeof(int) * CHAR_BIT))));
	}
	return false;
}

template<size_t Size>
inline
void TBitSet<Size>::Set(int bit)
{
	if (bit < Size && bit >= 0)
	{
		_mask[bit / (sizeof(int) * CHAR_BIT)] |= (1 << (bit % (sizeof(int) * CHAR_BIT)));
	}
}

template<size_t Size>
inline
void TBitSet<Size>::Reset(int bit)
{
	if (bit < Size && bit >= 0)
	{
		_mask[bit / (sizeof(int) * CHAR_BIT)] &= ~(1 << (bit % (sizeof(int) * CHAR_BIT)));
	}
}

template<size_t Size>
inline
void TBitSet<Size>::Clear()
{
	(void) Size;
	memset(&_mask, 0, sizeof(_mask));
}

template<size_t Size>
bool TBitSet<Size>::IsClear()
{
	(void) Size;
	for (size_t i = 0; i < sizeof(_mask) / sizeof(int); i++)
	{
		if (_mask[i])
		{
			return false;
		}
	}
	return true;
}

}
