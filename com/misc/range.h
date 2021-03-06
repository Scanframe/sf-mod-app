#pragma once

#include "global.h"
#include <iostream>
#include "mcvector.h"

namespace sf
{

// Forward definition
class _MISC_CLASS TRange;

// Set byte alignment to 1 byte.
#pragma pack(push, 1)

/**
 * Struct that contains the data members of class 'TRange'.
 */
typedef struct tagRANGE
{
	/**
	 * Integer type used for start and stop.
	 */
	typedef int32_t size_type;
	/**
	 * Integer type used for the ID.
	 */
	typedef int32_t id_type;
	/**
	 * Start position of the range or interval.
	 */
	size_type Start;
	/**
	 * Stop position of the range or interval.
	 */
	size_type Stop;
	/**
	 * Identifier of this range.
	 */
	id_type Id;
} RANGE;
// Restore the pack option.
#pragma pack(pop)

/**
 * Class to manage integer ranges.
 */
class TRange :private RANGE
{
	public:
		/**
		 * Type to create lists of ranges.
		 */
		typedef mcvector <TRange> TVector;
		/**
		 * Iteration type for lists of ranges.
		 */
		typedef TVector::iter_type TIterator;

		/**
		 * Default constructor
		 */
		TRange();

		/**
		 * Copy constructor
		 * @param r
		 */
		TRange(const TRange& r);

		/**
		 * Base class constructor
		 */
		explicit TRange(const RANGE& r);

		/**
		 * Constructor
		 */
		TRange(size_type start, size_type stop, size_type id = 0);

		/**
		 * Swaps start and stop if the order is wrong
		 */
		void Normalize();

		/**
		 * Initialize data members
		 * @param start
		 * @param stop
		 * @param id
		 * @return
		 */
		TRange& Set(size_type start, size_type stop, size_type id = 0);

		/**
		 * Initialize instance with other instance
		 */
		TRange& Set(const TRange& r);

		/**
		 * Initialize instance with other instance
		 */
		TRange& Set(const RANGE& r);

		/**
		 * Copies the current instance to RANGE base struct.
		 */
		const TRange& CopyTo(RANGE& dest) const;

		/**
		 * const function to access the start of the range.
		 */
		[[nodiscard]] size_type GetStart() const;

		/**
		 * Const function to access the stop of the range.
		 */
		[[nodiscard]] size_type GetStop() const;

		/**
		 * clears the range to an empty state.
		 */
		void Clear();

		/**
		 * Returns the size of the range minimum is 1.
		 */
		[[nodiscard]] size_type GetSize() const;

		/**
		 * Return true if the range is empty.
		 */
		[[nodiscard]] bool IsEmpty() const;

		/**
		 * Returns if the passed range have some overlap.
		 * @param r
		 * @return
		 */
		[[nodiscard]] bool IsOverlapped(const TRange& r) const;

		/**
		 * Returns true if the passed range is part of this range.
		 */
		[[nodiscard]] bool IsPartOfThis(const TRange& r) const;

		/**
		 * Returns true if this range is part of the passed range.
		 */
		[[nodiscard]] bool IsPartOfOther(const TRange& r) const;

		/**
		 * Returns if the passed range an extension of this one.
		 * @param r
		 * @return
		 */
		[[nodiscard]] bool IsExtension(const TRange& r) const;

		/**
		 * Returns true if the passed range can be added to this range so that
		 * @param r
		 * @return One range
		 */
		[[nodiscard]] bool IsMergeable(const TRange& r) const;

		/**
		 * Check if idx is within this range
		 */
		[[nodiscard]] bool IsInRange(size_type idx) const;

		/**
		 * Boolean operator for testing content of this instance
		 */
		explicit operator bool() const;

		/**
		 * Base function for comparing ranges
		 *    '-' token of 'this' range
		 *    '=' token of the 'other' range
		 *     0 = same range                           |------------|
		 *                                              |============|
		 *
		 *     1 = other range is part of this range    |--------------|
		 *                                                 |=======|
		 *
		 *    -1 = this range is part of other range       |-------|
		 *                                              |==============|
		 *
		 *     2 = this range start beyond other start      |---------|
		 *         this range stop beyond other stop    |=========|
		 *
		 *    -2 = this range start before other start  |---------|
		 *         this range stop before other stop        |=========|
		 *
		 *     3 = this range extends other                    |-----|
		 *         this range beyond other range        |=====|
		 *
		 *    -3 = this range extends other             |-----|
		 *         this range before other range               |=====|
		 *
		 *     4 = ranges do not touch                           |-----|
		 *         this range beyond other range        |=====|
		 *
		 *    -4 = ranges do not touch                  |-----|
		 *         this range before other range                 |=====|
		 *
		 *     5 = this range is empty                    ||
		 *                                             |=====|
		 *
		 *    -5 = other range is empty                |-----|
		 *                                                ||
		 */
		[[nodiscard]] int Compare(const TRange& other) const;

		/**
		 * Returns the owner id of this range.
		 */
		[[nodiscard]] size_type GetId() const;

		/**
		 * Set the owner id of this range.
		 */
		TRange& SetId(size_type id);

		/**
		 * Assignment operator allow derived classes to copy their data members.
		 */
		TRange& operator=(const TRange& r);

		/**
		 * Assignment operator allow derived classes to copy their data members.
		 */
		TRange& operator=(const RANGE& r);

		/**
		 * Tests if the range start and stop members are the same.
		 */
		bool operator==(const TRange& r) const;

		/**
		 * Tests if the range start and stop members are not the same.
		 */
		bool operator!=(const TRange& r) const;

		/**
		 * Operator used for sorting
		 * Depends only on the start position first and then the stop position.
		 * Empty ranges are always larger then non empty ones to be able to
		 * move empty ones to the end of a vector.
		 */
		int operator<(const TRange& r) const;

		/**
		 * And operator which is a subset of both range where elements
		 * of the one also exists in the other range.
		 */
		TRange operator&(const TRange& r) const;

		TRange operator&=(const TRange& r);

		/**
		 * Returns a new range which is a sub set of the two ranges
		 */
		TRange operator-(const TRange& r) const;

		/**
		 * Returns a new range which is a sub set of the two ranges
		 */
		TRange& operator-=(const TRange& r);

		/**
		 * Returns a new range which is a super set of the two ranges
		 * the two ranges must at least extend each other in order
		 * to perform this operation otherwise the result is a range of (MAX_LONG,LONG_MIN)
		 */
		TRange operator+(const TRange& r) const;

		/**
		 * Returns a new range which is a super set of the two ranges
		 * the two ranges must at least extend each other in order
		 * to perform this operation otherwise the result is a range of (MAX_LONG,LONG_MIN)
		 */
		TRange& operator+=(const TRange& r);

		/**
		 * Shifts the range using the passed offset.
		 */
		TRange& OffsetBy(size_type ofs);

		/**
		 * Shifts the range using the passed offset.
		 */
		[[nodiscard]] TRange Offset(size_type ofs) const;

		/**
		 * Not allowed operators.
		 */
		bool operator>(const TRange& r) const = delete;

		bool operator>(const TRange& r) = delete;

		bool operator<=(const TRange& r) const = delete;

		bool operator<=(const TRange& r) = delete;

		bool operator>=(const TRange& r) const = delete;

		bool operator>=(const TRange& r) = delete;

		friend std::ostream& operator<<(std::ostream& os, const TRange& r);

		friend std::istream& operator>>(std::istream& is, TRange& r);

};

inline
std::ostream& operator<<(std::ostream& os, const TRange& r)
{
	return os << '(' << r.Start << ',' << r.Stop << ',' << r.Id << ')';
}

inline
std::istream& operator>>(std::istream& is, TRange& r)
{
	char c;
	return is >> c >> r.Start >> c >> r.Stop >> c >> r.Id >> c;
}

inline
TRange::TRange()
	:tagRANGE()
{
	Clear();
}

inline
TRange::TRange(const TRange& r)
	:tagRANGE(r)
{
	Set(r);
}

inline
TRange::TRange(const RANGE& r)
	:tagRANGE()
{
	Set(r);
}

inline
TRange::TRange(size_type start, size_type stop, id_type id)
	:tagRANGE()
{
	Set(start, stop, id);
}

inline
TRange::size_type TRange::GetStart() const
{
	return Start;
}

inline
TRange::size_type TRange::GetStop() const
{
	return Stop;
}

inline
TRange::id_type TRange::GetId() const
{
	return Id;
}

inline
TRange& TRange::SetId(id_type id)
{
	Id = id;
	return *this;
}

inline
void TRange::Clear()
{
	Start = 0;
	Stop = 0;
	Id = 0;
}

inline
TRange& TRange::Set(const TRange& r)
{
	*(tagRANGE*) this = *(tagRANGE*) &r;
	return *this;
}

inline
TRange::size_type TRange::GetSize() const
{
	return Stop - Start;
}

inline
bool TRange::IsEmpty() const
{
	return (Stop == Start);
}

inline
bool TRange::IsMergeable(const TRange& r) const
{
	return abs(Compare(r)) != 4;
}

inline
bool TRange::IsOverlapped(const TRange& r) const
{
	return abs(Compare(r)) <= 2;
}

inline
bool TRange::IsExtension(const TRange& r) const
{
	return abs(Compare(r)) == 3;
}

inline
bool TRange::IsPartOfOther(const TRange& r) const
{
	int res = Compare(r);
	return (res == -1 || res == 0);
}

inline
bool TRange::IsPartOfThis(const TRange& r) const
{
	int res = Compare(r);
	return (res == 1 || res == 0);
}

inline
TRange& TRange::operator=(const TRange& r)
{
	Set(r);
	return *this;
}

inline
TRange& TRange::operator=(const RANGE& r)
{
	Set(r);
	return *this;
}

inline
TRange::operator bool() const
{
	return IsEmpty();
}

inline
bool TRange::operator==(const TRange& r) const
{
	return Compare(r) == 0;
}

inline
bool TRange::operator!=(const TRange& r) const
{
	return Compare(r) != 0;
}

inline
int TRange::operator<(const TRange& r) const
{
	return IsEmpty() ? 0 : (r.IsEmpty() ? 1 : ((Start == r.Start) ? Stop < r.Stop : Start < r.Start));
}

inline
TRange& TRange::operator-=(const TRange& r)
{
	return Set(*this - r);
}

inline
TRange& TRange::operator+=(const TRange& r)
{
	return Set(*this + r);
}

inline
TRange TRange::operator&=(const TRange& r)
{
	return Set(*this & r);
}

inline
TRange& TRange::OffsetBy(size_type ofs)
{
	Start += ofs;
	Stop += ofs;
	return *this;
}

inline
TRange TRange::Offset(size_type ofs) const
{
	return TRange(*this).OffsetBy(ofs);
}

inline
bool TRange::IsInRange(size_type idx) const
{
	return idx >= Start && idx < Stop;
}

inline
const TRange& TRange::CopyTo(RANGE& dest) const
{
	dest = *this;
	return *this;
}

} // namespace
