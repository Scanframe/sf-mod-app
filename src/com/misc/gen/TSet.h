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
	TSet() :Bits(0) {}

	/**
	 * @brief initialize constructor.
	 */
	explicit TSet(S bits) :Bits(bits) {}

	/**
	 * @brief Copy constructor.
	 */
	TSet(const TSet& set) :Bits(set.Bits) {}

	/**
	 * @brief Assign operator.
	 */
	TSet& operator=(const TSet& set)
	{
		Bits = set.Bits;
		return *this;
	}

	/**
	 * @brief Returns true when the bit was Set.
	 */
	bool Has(T bit) const
	{
		return (Bits & (1ull << bit)) > 0;
	}

	/**
	 * @brief Sets bits in the mask.
	 */
	TSet& Set(T bit)
	{
		Bits |= (1ull << bit);
		return *this;
	}

	/**
	 * @brief Unsets bits in the mask.
	 */
	TSet& Unset(T bit)
	{
		Bits &= ~(1ull << bit);
		return *this;
	}

	/**
	 * @brief Toggles a bit in the mask.
	 */
	TSet& Toggle(T bit) {return Has(bit) ? Unset(bit) : Set(bit);}

	/**
	 * @brief Compare equal operator.
	 */
	bool operator==(const TSet& set) const {return Bits == set.Bits;}

	/**
	 * @brief Compare unequal operator.
	 */
	bool operator!=(const TSet& set) const {return Bits != set.Bits;}

	/**
	 * @brief Operators and functions that are also available in the VCL 'Set' template.
	 * @return True when bit is in the bit mask.
	 */
	bool Contains(T bit) const {return Has(bit);}

	/**
	 * @brief Operator for adding bits to the mask.
	 */
	TSet& operator<<(T bit)
	{
		Set(bit);
		return *this;
	}

	/**
	 * @brief Operator for removing bits from the mask.
	 */
	TSet& operator>>(T bit)
	{
		Unset(bit);
		return *this;
	}

	/**
	 * @brief Holds the actual bits.
	 */
	S Bits;
};

}
