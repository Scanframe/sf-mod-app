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
		bool has(int bit);

		/**
		 * @brief Sets a single bit.
		 */
		void set(int bit);

		/**
		 * @brief Resets a single bit.
		 */
		void reset(int bit);

		/**
		 * @brief Clears all bits.
		 */
		void clear();

		/**
		 * @brief Returns true if one of the bits has been Set.
		 */
		bool isClear();

	private:
		/**
		 * @brief Holds the actual data.
		 */
		int _mask[_size / sizeof(int) + 1]{};
};

template<size_t Size>
inline
TBitSet<Size>::TBitSet()
{
	(void) Size;
	clear();
}

template<size_t Size>
inline
bool TBitSet<Size>::has(int bit)
{
	if (bit < Size && bit >= 0)
	{
		return (_mask[bit / (sizeof(int) * CHAR_BIT)] & (1 << (bit % (sizeof(int) * CHAR_BIT))));
	}
	return false;
}

template<size_t Size>
inline
void TBitSet<Size>::set(int bit)
{
	if (bit < Size && bit >= 0)
	{
		_mask[bit / (sizeof(int) * CHAR_BIT)] |= (1 << (bit % (sizeof(int) * CHAR_BIT)));
	}
}

template<size_t Size>
inline
void TBitSet<Size>::reset(int bit)
{
	if (bit < Size && bit >= 0)
	{
		_mask[bit / (sizeof(int) * CHAR_BIT)] &= ~(1 << (bit % (sizeof(int) * CHAR_BIT)));
	}
}

template<size_t Size>
inline
void TBitSet<Size>::clear()
{
	(void) Size;
	memset(&_mask, 0, sizeof(_mask));
}

template<size_t Size>
bool TBitSet<Size>::isClear()
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
