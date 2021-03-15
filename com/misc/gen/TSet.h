#pragma once

namespace sf
{

/**
 * @brief
 * Template class for managing bit maks preferably when bits are
 * defined as enumerate values.
 */
template<class T>
struct TSet
{
	/**
	 * Default constructor.
	 */
	TSet() :Bits(0) {}

	/**
	 * Initialize constructor.
	 */
	explicit TSet(int bits) :Bits(bits) {}

	/**
	 * Copy constructor.
	 */
	TSet(const TSet& set) :Bits(set.Bits) {}

	/**
	 * Assign operator.
	 */
	TSet& operator=(const TSet& set)
	{
		Bits = set.Bits;
		return *this;
	}

	/**
	 * Returns true when the bit was Set.
	 */
	bool Has(T bit) const
	{
		return (Bits & (1L << bit)) > 0;
	}

	/**
	 * Sets bits in the mask.
	 */
	TSet& Set(T bit)
	{
		Bits |= (1L << bit);
		return *this;
	}

	/**
	 * Unsets bits in the mask.
	 */
	TSet& Unset(T bit)
	{
		Bits &= ~(1L << bit);
		return *this;
	}

	/**
	 * Toggles a bit in the mask.
	 */
	TSet& Toggle(T bit) {return Has(bit) ? Unset(bit) : Set(bit);}

	/**
	 * Compare equal operator.
	 */
	bool operator==(const TSet& set) const {return Bits == set.Bits;}

	/**
	 * Compare unequal operator.
	 */
	bool operator!=(const TSet& set) const {return Bits != set.Bits;}

	/**
	 * Operators and functions that are also available in the VCL 'Set' template.
	 * Returns true if bit is in the bit mask.
	 */
	bool Contains(T bit) const {return Has(bit);}

	/**
	 * Operator for adding bits to the mask.
	 */
	TSet& operator<<(T bit)
	{
		Set(bit);
		return *this;
	}

	/**
	 * Operator for removing bits from the mask.
	 */
	TSet& operator>>(T bit)
	{
		Unset(bit);
		return *this;
	}

	/**
	 * Holds the actual bits.
	 */
	int Bits;
};

}
