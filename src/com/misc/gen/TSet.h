#pragma once

namespace sf
{

/**
 * @brief Template class for managing bit maks preferably when bits are defined as enumerate values.
 */
template<typename T = int, typename S = int>
struct TSet
{
		/**
		 * Default constructor.
		 */
		TSet()
			: _bits(0)
		{}

		/**
		 * @brief Initialize constructor.
		 */
		explicit TSet(S bits)
			: _bits(bits)
		{}

		/**
		 * @brief Copy constructor.
		 */
		TSet(const TSet& set)
			: _bits(set._bits)
		{}

		/**
		 * @brief assign operator.
		 */
		TSet& operator=(const TSet& set)
		{
			_bits = set._bits;
			return *this;
		}

		/**
		 * @brief Returns true when the bit was Set.
		 */
		bool has(T bit) const
		{
			return (_bits & (1ull << bit)) > 0;
		}

		/**
		 * @brief Sets bits in the mask.
		 */
		TSet& set(T bit)
		{
			_bits |= (1ull << bit);
			return *this;
		}

		/**
		 * @brief Unsets bits in the mask.
		 */
		TSet& unset(T bit)
		{
			_bits &= ~(1ull << bit);
			return *this;
		}

		/**
		 * @brief Toggles a bit in the mask.
		 */
		TSet& toggle(T bit) { return has(bit) ? Unset(bit) : Set(bit); }

		/**
		 * @brief Compare equal operator.
		 */
		bool operator==(const TSet& set) const { return _bits == set._bits; }

		/**
		 * @brief Compare unequal operator.
		 */
		bool operator!=(const TSet& set) const { return _bits != set._bits; }

		/**
		 * @brief Operators and functions that are also available in the VCL 'Set' template.
		 * @return True when bit is in the bit mask.
		 */
		bool contains(T bit) const { return has(bit); }

		/**
		 * @brief Operator for adding bits to the mask.
		 */
		TSet& operator<<(T bit)
		{
			return set(bit);
		}

		/**
		 * @brief Operator for removing bits from the mask.
		 */
		TSet& operator>>(T bit)
		{
			return unset(bit);
		}

		/**
		 * @brief Holds the actual bits.
		 */
		S _bits;
};

}// namespace sf
